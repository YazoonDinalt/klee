//===-- Delta.cpp -----------------------------------------------*- C++ -*-===//
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

std::map<const llvm::Function *, std::map<std::string, int>>
Delta::CalculateDelta(
    std::map<const llvm::Function *, StatisticRecord> StatMap) {

  std::map<const llvm::Function *, std::map<std::string, int>> DelMap;

  for (const auto &pair : StatMap) {
    DelMap[pair.first]["Instructions"] =
        pair.second.getValue(stats::instructions) -
        previousMap[pair.first].getValue(stats::instructions);
    DelMap[pair.first]["Forks"] =
        pair.second.getValue(stats::forks) -
        previousMap[pair.first].getValue(stats::forks);
  }

  previousMap = StatMap;

  // SerializeDelMap(DelMap);

  return DelMap;
}

std::vector<nlohmann::json> Delta::SerializeDelMap(
    const std::map<std::string, std::map<std::string, int>> &DelMap) {

  std::vector<nlohmann::json> jsonArray;

  for (const auto &funPair : DelMap) {
    const std::string &funName = funPair.first;
    const auto &metricsMap = funPair.second;

    for (const auto &metricPair : metricsMap) {
      const std::string &metricName = metricPair.first;
      int count = metricPair.second;

      if (count != 0) {
        jsonArray.push_back({{"name", metricName},
                             {"params",
                              {{"funName", funName},
                               {"type", "int"},
                               {"value", count},
                               {"transitive", false}}}});
      }
    }
  }

  // for (const auto &jsonObj : jsonArray) {
  //   std::cout << jsonObj.dump(4)
  //             << std::endl; // 4 - количество пробелов для отступа
  // }
  // std::cout << "\n\n\n" << std::endl;
  return jsonArray;
}

void Delta::initPrevDelta(
    std::map<const llvm::Function *, StatisticRecord> StatMap) {
  previousMap = StatMap;
}

} // namespace klee
