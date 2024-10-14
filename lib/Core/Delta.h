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

#include "nlohmann/json.hpp"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LLVMContext.h"
#include <map>
#include <string>

#include <klee/Statistics/Statistics.h>

namespace klee {

/// TimingSolver - A simple class that collects delta statistics
class Delta {
public:
  std::map<const llvm::Function *, StatisticRecord> previousMap;
  std::map<const llvm::Function *, std::map<std::string, int>> Delta;

public:
  std::map<const llvm::Function *, std::map<std::string, int>>
  CalculateDelta(std::map<const llvm::Function *, StatisticRecord> StatMap);

  std::vector<nlohmann::json> SerializeDelMap(
      std::map<const llvm::Function *, std::map<std::string, int>> &DelMap);

  std::map<const llvm::Function *, std::map<std::string, int>>
  getCurrentMetric(std::map<const llvm::Function *, StatisticRecord> StatMap);

  void initPrevDelta(std::map<const llvm::Function *, StatisticRecord> StatMap);
};
} // namespace klee

#endif /* KLEE_DELTA_H */
