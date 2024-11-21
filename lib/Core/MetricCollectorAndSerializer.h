//===--MetricCollectorAndSerializer.h-----------------------------*-C++ -*-===//
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

#include <mutex>

namespace klee {

class MetricCollectorAndSerializer {
private:
  std::vector<nlohmann::json> previousMap;
  std::mutex StatisticMapMutex;
  std::unordered_map<const llvm::Function *, std::map<std::string, int>> Delta;

  std::vector<nlohmann::json> SerializeDelMap(
      const std::unordered_map<const llvm::Function *,
                               std::unordered_map<std::string, int>> &DelMap,
      const std::string UID);

public:
  std::vector<nlohmann::json> GetJson(
      const std::unordered_map<const llvm::Function *,
                               std::unordered_map<std::string, int>> &DelMap,
      const std::string UID);

  std::unordered_map<const llvm::Function *,
                     std::unordered_map<std::string, int>>
      getCurrentMetric(std::unordered_map<CallPathNode *, StatisticRecord *>);
};
} // namespace klee

#endif /* KLEE_DELTA_H */
