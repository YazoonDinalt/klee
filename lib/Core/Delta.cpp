//===-- Delta.cpp------------------------------------------------*- C++ -*-===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "Delta.h"
#include "CoreStats.h"

using namespace klee;

namespace klee {

std::unordered_map<const llvm::Function *, std::unordered_map<std::string, int>>
Delta::CalculateDelta(
    std::unordered_map<const llvm::Function *, StatisticRecord *> StatMap) {

  std::unordered_map<const llvm::Function *,
                     std::unordered_map<std::string, int>>
      DelMap;

  for (const auto &pair : StatMap) {
    DelMap[pair.first]["Instructions"] =
        pair.second->getValue(stats::instructions) -
        previousMap[pair.first]->getValue(stats::instructions);
    DelMap[pair.first]["Forks"] =
        pair.second->getValue(stats::forks) -
        previousMap[pair.first]->getValue(stats::forks);
  }

  previousMap = StatMap;

  return DelMap;
}

std::vector<nlohmann::json> Delta::SerializeDelMap(
    std::unordered_map<const llvm::Function *,
                       std::unordered_map<std::string, int>> &DelMap, std::string UID) {

  std::vector<nlohmann::json> jsonArray;

  for (const auto &funPair : DelMap) {
    auto funName = funPair.first->getName();
    const auto &metricsMap = funPair.second;

    for (const auto &metricPair : metricsMap) {
      const std::string &metricName = metricPair.first;
      uint64_t prev = 0;
      // if (metricName == "Instructions") {
      //   std::optional<int> prev =
      //       previousMap[funPair.first]->getValue(stats::instructions);
      //   if (!prev)
      //     prev = 0;
      // } else if (metricName == "Forks") {
      //   std::optional<int> prev =
      //       previousMap[funPair.first]->getValue(stats::forks);
      //   if (!prev)
      //     prev = 0;
      // }

      int count = metricPair.second;

      if (count - prev != 0) {
        jsonArray.push_back({{"guid", UID},
                             {"name", metricName},
                             {"params",
                              {{"funName", funName},
                               {"type", "int"},
                               {"value", count},
                               {"transitive", false}}}});
      }
    }
  }

  return jsonArray;
}

std::unordered_map<const llvm::Function *, std::unordered_map<std::string, int>>
Delta::getCurrentMetric(
    std::unordered_map<const llvm::Function *, StatisticRecord *> StatMap) {

  std::unordered_map<const llvm::Function *,
                     std::unordered_map<std::string, int>>
      DelMap;

  for (const auto &pair : StatMap) {
    DelMap[pair.first]["Instructions"] =
        pair.second->getValue(stats::instructions);
    DelMap[pair.first]["Forks"] = pair.second->getValue(stats::forks);
  }

  return DelMap;
}

void Delta::initPrevDelta(
    std::unordered_map<const llvm::Function *, StatisticRecord *> StatMap) {
  previousMap = StatMap;
}

} // namespace klee
