#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/EarlyFunctionAnalysis/FunctionEdgeType.h"

/* TUPLE-TREE-YAML
name: FunctionEdgeType
doc: Type of edge on the CFG
type: enum
members:
  - name: DirectBranch
    doc: Branch due to function-local CFG (a regular branch)
  - name: FunctionCall
    doc: A function call for which the cache was able to produce a summary
  - name: Return
    doc: A proper function return
  - name: BrokenReturn
    doc: |
      A branch returning to the return address, but leaving the stack in an
      unexpected situation
  - name: LongJmp
    doc: A branch representing a longjmp or similar constructs
  - name: Killer
    doc: A killer basic block (killer syscall or endless loop)
  - name: Unreachable
    doc: The basic block ends with an unreachable instruction
TUPLE-TREE-YAML */

#include "revng/Yield/Generated/Early/FunctionEdgeType.h"

// TODO: we need to handle noreturn function calls

namespace yield::FunctionEdgeType {

inline bool isCall(Values V) {
  switch (V) {
  case Count:
    revng_abort();
    break;

  case FunctionCall:
    return true;

  case Invalid:
  case DirectBranch:
  case Return:
  case BrokenReturn:
  case LongJmp:
  case Killer:
  case Unreachable:
    return false;
  }
}

inline Values from(efa::FunctionEdgeType::Values V) {
  switch (V) {
  case efa::FunctionEdgeType::Count:
    return Count;
  case efa::FunctionEdgeType::FunctionCall:
    return FunctionCall;
  case efa::FunctionEdgeType::Invalid:
    return Invalid;
  case efa::FunctionEdgeType::DirectBranch:
    return DirectBranch;
  case efa::FunctionEdgeType::Return:
    return Return;
  case efa::FunctionEdgeType::BrokenReturn:
    return BrokenReturn;
  case efa::FunctionEdgeType::LongJmp:
    return LongJmp;
  case efa::FunctionEdgeType::Killer:
    return Killer;
  case efa::FunctionEdgeType::Unreachable:
    return Unreachable;
  }

  revng_abort();
}

} // namespace yield::FunctionEdgeType

#include "revng/Yield/Generated/Late/FunctionEdgeType.h"
