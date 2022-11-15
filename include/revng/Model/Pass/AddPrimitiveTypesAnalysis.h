#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <array>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/Support/raw_ostream.h"

#include "revng/Model/Pass/AddPrimitiveTypes.h"
#include "revng/Pipeline/Contract.h"
#include "revng/Pipeline/Target.h"
#include "revng/Pipes/FileContainer.h"
#include "revng/Pipes/Kinds.h"

namespace revng::pipes {

class AddPrimitiveTypesAnalysis {
public:
  static constexpr auto Name = "AddPrimitiveTypes";
  static constexpr auto Doc = "Introduce in the model all the combinations of "
                              "`PrimitiveTypes` (`Generic`, `PointerOrNumber`, "
                              "`Number`, `Unsigned`, and `Signed`) for 8-, "
                              "16-, 32-, 64- and 128-bit sizes.";

public:
  std::vector<std::vector<pipeline::Kind *>> AcceptedKinds = {
    { &revng::kinds::Binary }
  };

public:
  void run(pipeline::Context &Context, const BinaryFileContainer &SourceBinary);
};

} // namespace revng::pipes
