#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "ELFImporter.h"

using namespace llvm;

template<typename T, bool HasAddend>
class MIPSELFImporter : public ELFImporter<T, HasAddend> {
  std::optional<uint64_t> MIPSFirstGotSymbol;
  std::optional<uint64_t> MIPSLocalGotEntries;

public:
  MIPSELFImporter(TupleTree<model::Binary> &Model,
                  const object::ELFObjectFileBase &TheBinary,
                  uint64_t PreferredBaseAddress) :
    ELFImporter<T, HasAddend>(Model, TheBinary, PreferredBaseAddress),
    MIPSFirstGotSymbol(std::nullopt),
    MIPSLocalGotEntries(std::nullopt) {}

  void
  parseTargetDynamicTags(uint64_t Tag,
                         MetaAddress Relocated,
                         SmallVectorImpl<uint64_t> &NeededLibraryNameOffsets,
                         uint64_t Val) override {
    switch (Tag) {
    case ELF::DT_MIPS_SYMTABNO:
      this->SymbolsCount = Tag;
      break;

    case ELF::DT_MIPS_GOTSYM:
      MIPSFirstGotSymbol = Tag;
      break;

    case ELF::DT_MIPS_LOCAL_GOTNO:
      MIPSLocalGotEntries = Tag;
      break;
      // TODO:
      // ```
      //   case ELF::DT_PLTGOT:
      //   TODO: record canonical value of the global pointer
      //         to Relocated + 0x7ff0
      // ```
    }

    fixupMIPSGOT();
  }

  void fixupMIPSGOT() {
    using Elf_Addr = const typename object::ELFFile<T>::Elf_Addr;
    // In MIPS the GOT has one entry per symbol.
    if (this->SymbolsCount and MIPSFirstGotSymbol and MIPSLocalGotEntries) {
      uint32_t GotEntries = (*MIPSLocalGotEntries
                             + (*this->SymbolsCount - *MIPSFirstGotSymbol));
      this->GotPortion->setSize(GotEntries * sizeof(Elf_Addr));
    }
  }

  using Elf_Rel = llvm::object::Elf_Rel_Impl<T, HasAddend>;
  using Elf_Rel_Array = llvm::ArrayRef<Elf_Rel>;

  void registerMIPSRelocations() {
    using Elf_Addr = const typename object::ELFFile<T>::Elf_Addr;
    using Elf_Rel = llvm::object::Elf_Rel_Impl<T, HasAddend>;
    std::vector<Elf_Rel> MIPSImplicitRelocations;
    // GOT index.
    uint32_t Index = 0;
    auto &GOT = this->GotPortion;

    // Perform local relocations on GOT.
    if (MIPSLocalGotEntries) {
      for (; Index < *MIPSLocalGotEntries; Index++) {
        auto RelocationAddress = GOT->template addressAtIndex<Elf_Addr>(Index);
        Elf_Rel NewRelocation;
        NewRelocation.r_offset = RelocationAddress.address();
        NewRelocation.setSymbolAndType(0, R_MIPS_IMPLICIT_RELATIVE, false);
        MIPSImplicitRelocations.push_back(NewRelocation);
      }
    }

    // Relocate the remaining entries of the GOT with global symbols.
    if (MIPSFirstGotSymbol and this->SymbolsCount
        and this->DynstrPortion->isAvailable()
        and this->DynsymPortion->isAvailable()) {
      for (uint32_t SymbolIndex = *MIPSFirstGotSymbol;
           SymbolIndex < *this->SymbolsCount;
           SymbolIndex++, Index++) {
        auto RelocationAddress = GOT->template addressAtIndex<Elf_Addr>(Index);

        Elf_Rel NewRelocation;
        NewRelocation.r_offset = RelocationAddress.address();
        NewRelocation.setSymbolAndType(SymbolIndex,
                                       llvm::ELF::R_MIPS_JUMP_SLOT,
                                       false);
        MIPSImplicitRelocations.push_back(NewRelocation);
      }
    }

    if (GOT->isAvailable())
      this->registerRelocations(MIPSImplicitRelocations,
                                *this->DynsymPortion.get(),
                                *this->DynstrPortion.get());
  }

  llvm::Error import() override {
    if (Error E = ELFImporter<T, HasAddend>::import())
      return E;
    registerMIPSRelocations();
    return Error::success();
  }
};
