#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <memory>
#include <type_traits>

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Casting.h"

#include "revng/ADT/STLExtras.h"
#include "revng/Support/Assert.h"

template<typename T>
struct KeyedObjectTraits;

template<typename T>
struct concrete_types_traits;

template<typename T>
using concrete_types_traits_t = typename concrete_types_traits<T>::type;

// clang-format off
template<typename T>
concept ConcreteTypeTraitCompatible = requires {
  typename concrete_types_traits_t<T>;
} && StrictSpecializationOf<concrete_types_traits_t<T>, std::tuple>;
// clang-format on

template<typename T>
concept HasLLVMRTTI = requires(T *A) {
  { A->classof(A) } -> std::same_as<bool>;
};

template<typename T>
concept Upcastable = HasLLVMRTTI<T> and ConcreteTypeTraitCompatible<T>;

template<typename T>
using pointee = typename std::pointer_traits<std::decay_t<T>>::element_type;

template<typename T>
concept Dereferenceable = requires(T A) {
  { *A };
};

static_assert(Dereferenceable<int *>);
static_assert(not Dereferenceable<int>);

template<typename T>
concept UpcastablePointerLike = Dereferenceable<T> and Upcastable<pointee<T>>;

// clang-format off
template<typename ReturnT, typename L, UpcastablePointerLike P, std::size_t... I>
  requires(not std::is_void_v<ReturnT>)
ReturnT upcastImpl(P &&Upcastable,
                   const L &Callable,
                   const ReturnT &IfNull,
                   std::index_sequence<I...>) {
  // clang-format on
  using pointee = std::remove_reference_t<decltype(*Upcastable)>;
  using concrete_types = concrete_types_traits_t<pointee>;
  auto *Pointer = &*Upcastable;
  if (Pointer == nullptr)
    return IfNull;

  std::optional<ReturnT> Result;

  ((Result = llvm::isa<std::tuple_element_t<I, concrete_types>>(Pointer) ?
               Callable(*llvm::cast<std::tuple_element_t<I, concrete_types>>(
                 Pointer)) :
               Result),
   ...);

  revng_assert(Result);
  return *Result;
}

// clang-format off
template<typename ReturnT, typename L, UpcastablePointerLike P>
  requires(not std::is_void_v<ReturnT>)
ReturnT upcast(P &&Upcastable, const L &Callable, const ReturnT &IfNull) {
  using pointee = std::remove_reference_t<decltype(*Upcastable)>;
  using concrete_types = concrete_types_traits_t<pointee>;
  return upcastImpl(Upcastable,
                    Callable,
                    IfNull,
                    std::make_index_sequence<std::tuple_size_v<concrete_types>>());
}

template<typename L, UpcastablePointerLike P>
void upcast(P &&Upcastable, const L &Callable) {
  auto Wrapper = [&](auto &Upcasted) {
    Callable(Upcasted);
    return true;
  };
  upcast(Upcastable, Wrapper, false);
}

template<UpcastablePointerLike P, typename KeyT, typename L>
void invokeByKey(const KeyT &Key, const L &Callable) {
  auto Upcastable = KeyedObjectTraits<P>::fromKey(Key);

  upcast(Upcastable, [&Callable]<typename UpcastedT>(const UpcastedT &C) {
    Callable(static_cast<UpcastedT *>(nullptr));
  });
}

template<UpcastablePointerLike P, typename KeyT, typename L, typename ReturnT>
ReturnT invokeByKey(const KeyT &Key, const L &Callable, const ReturnT &IfNull) {
  auto Upcastable = KeyedObjectTraits<P>::fromKey(Key);

  auto ToCall = [&Callable]<typename UpcastedT>(const UpcastedT &C) {
    return Callable(static_cast<UpcastedT *>(nullptr));
  };
  return upcast(Upcastable, ToCall, IfNull);
}

/// A unique_ptr copiable thanks to LLVM RTTI
template<Upcastable T>
class UpcastablePointer {
private:
  template<Upcastable P>
  static P *clone(P *Pointer) {
    auto Dispatcher = [](auto &Upcasted) -> P * {
      using type = std::remove_reference_t<decltype(Upcasted)>;
      return new type(Upcasted);
    };
    return ::upcast(Pointer, Dispatcher, static_cast<P *>(nullptr));
  }

  template<Upcastable P>
  static void destroy(P *Pointer) {
    ::upcast(Pointer, [](auto &Upcasted) { delete &Upcasted; });
  }

public:
  template<typename L>
  void upcast(const L &Callable) {
    ::upcast(Pointer, Callable);
  }

  template<typename L>
  void upcast(const L &Callable) const {
    ::upcast(Pointer, Callable);
  }

private:
  using concrete_types = concrete_types_traits_t<T>;
  static constexpr void (*Deleter)(T *) = &destroy<T>;
  using inner_pointer = std::unique_ptr<T, decltype(Deleter)>;

public:
  using pointer = typename inner_pointer::pointer;
  using element_type = typename inner_pointer::element_type;

public:
  constexpr UpcastablePointer() noexcept : Pointer(nullptr, Deleter) {}
  constexpr UpcastablePointer(std::nullptr_t P) noexcept :
    Pointer(P, Deleter) {}
  explicit UpcastablePointer(pointer P) noexcept : Pointer(P, Deleter) {}

public:
  template<derived_from<T> Q, typename... Args>
  static UpcastablePointer<T> make(Args &&...TheArgs) {
    return UpcastablePointer<T>(new Q(std::forward<Args>(TheArgs)...));
  }

public:
  UpcastablePointer &operator=(const UpcastablePointer &Other) {
    if (&Other != this) {
      Pointer.reset(clone(Other.Pointer.get()));
    }
    return *this;
  }

  UpcastablePointer(const UpcastablePointer &Other) :
    UpcastablePointer(nullptr) {
    *this = Other;
  }

  UpcastablePointer &operator=(UpcastablePointer &&Other) {
    if (&Other != this) {
      Pointer.reset(Other.Pointer.release());
    }
    return *this;
  }

  UpcastablePointer(UpcastablePointer &&Other) noexcept :
    UpcastablePointer(nullptr) {
    *this = std::move(Other);
  }

  bool operator==(const UpcastablePointer &Other) const {
    bool Result = false;
    upcast([&](auto &Upcasted) {
      Other.upcast([&](auto &OtherUpcasted) {
        using ThisType = std::remove_cvref_t<decltype(Upcasted)>;
        using OtherType = std::remove_cvref_t<decltype(OtherUpcasted)>;
        if constexpr (std::is_same_v<ThisType, OtherType>) {
          Result = Upcasted == OtherUpcasted;
        }
      });
    });
    return Result;
  }

  auto get() const noexcept { return Pointer.get(); }
  auto &operator*() const { return *Pointer; }
  auto *operator->() const noexcept { return Pointer.operator->(); }

  void reset(pointer Other = pointer()) noexcept { Pointer.reset(Other); }

private:
  inner_pointer Pointer;
};
