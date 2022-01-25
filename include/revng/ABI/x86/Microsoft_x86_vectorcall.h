#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Model/ABI.h"
#include "revng/Model/Register.h"

namespace abi {

template<model::ABI::Values ABI>
struct Trait;

template<>
struct Trait<model::ABI::Microsoft_x86_vectorcall> {
  static constexpr auto ABI = model::ABI::Microsoft_x86_vectorcall;

  static constexpr bool ArgumentsArePositionBased = false;
  static constexpr bool OnlyStartDoubleArgumentsFromAnEvenRegister = false;
  static constexpr bool ArgumentsCanBeSplitBetweenRegistersAndStack = false;
  static constexpr bool UsePointerToCopyForStackArguments = false;

  static constexpr size_t MaximumGPRsPerAggregateArgument = 0;
  static constexpr size_t MaximumGPRsPerAggregateReturnValue = 0;
  static constexpr size_t MaximumGPRsPerScalarArgument = 1;
  static constexpr size_t MaximumGPRsPerScalarReturnValue = 1;

  static constexpr std::array GeneralPurposeArgumentRegisters = {
    model::Register::ecx_x86,
    model::Register::edx_x86
  };
  static constexpr std::array GeneralPurposeReturnValueRegisters = {
    model::Register::eax_x86
  };

  static constexpr std::array VectorArgumentRegisters = {
    model::Register::xmm0_x86, model::Register::xmm1_x86,
    model::Register::xmm2_x86, model::Register::xmm3_x86,
    model::Register::xmm4_x86, model::Register::xmm5_x86
  };
  static constexpr std::array VectorReturnValueRegisters = {
    model::Register::xmm0_x86
  };

  static constexpr std::array CalleeSavedRegisters = {
    model::Register::ebx_x86, model::Register::ebp_x86,
    model::Register::esp_x86, model::Register::edi_x86,
    model::Register::esi_x86, model::Register::xmm6_x86,
    model::Register::xmm7_x86
  };

  static constexpr auto ReturnValueLocationRegister = model::Register::eax_x86;
};

} // namespace abi