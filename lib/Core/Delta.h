//===-- Delta.h--------------------------------------------------*- C++ -*-===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef KLEE_GETDELTA_H
#define KLEE_GETDELTA_H

#include "CallPathManager.h"
#include "nlohmann/json.hpp"
#include "llvm/IR/Function.h"
#include <map>
#include <string>

#include <klee/Statistics/Statistics.h>

namespace klee {

class Delta {
public:
  std::unordered_map<const llvm::Function *,
                     std::unordered_map<std::string, int>>
      previousMap;
  std::unordered_map<const llvm::Function *, std::map<std::string, int>> Delta;

public:
  std::vector<nlohmann::json> SerializeDelMap(
      std::unordered_map<const llvm::Function *,
                         std::unordered_map<std::string, int>> &DelMap,
      std::string UID);

  std::unordered_map<const llvm::Function *,
                     std::unordered_map<std::string, int>>
      getCurrentMetric(std::unordered_map<CallPathNode *, StatisticRecord *>);

  void
  initPrevDelta(std::unordered_map<CallPathNode *, StatisticRecord *> StatMap);
};
} // namespace klee

#endif /* KLEE_DELTA_H */
