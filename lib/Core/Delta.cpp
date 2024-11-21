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

const double NANOSECONDS_PER_SECOND = 1000000000.0;

std::vector<nlohmann::json> Delta::SerializeDelMap(
    const std::unordered_map<const llvm::Function *,
                             std::unordered_map<std::string, int>> &DelMap,
    std::string UID) {

  std::vector<nlohmann::json> jsonArray;

  for (const auto &funPair : DelMap) {
    auto funName = funPair.first->getName();
    const auto &metricsMap = funPair.second;

    for (const auto &metricPair : metricsMap) {
      const std::string &metricName = metricPair.first;
      uint64_t prev;
      auto count = metricPair.second;
      for (const auto &jsonObject : newPrevMap) {
        if (jsonObject["params"]["funName"] == funName &&
            jsonObject["name"] == metricName) {
          prev = jsonObject["params"]["value"];
          break;
        }
      }

      if (count - prev != 0) {
        if (metricName == "SolverTime") {
          double solverCount =
              static_cast<double>(count) / NANOSECONDS_PER_SECOND;
          solverCount = static_cast<double>(round(solverCount * 100)) / 100;
          jsonArray.push_back({{"guid", UID},
                               {"name", metricName},
                               {"params",
                                {{"funName", funName},
                                 {"type", "double"},
                                 {"value", solverCount},
                                 {"transitive", false}}}});
        } else {
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
  }

  newPrevMap = jsonArray;

  return jsonArray;
}

std::unordered_map<const llvm::Function *, std::unordered_map<std::string, int>>
Delta::getCurrentMetric(
    std::unordered_map<CallPathNode *, StatisticRecord *> StatMap) {

  std::unordered_map<const llvm::Function *,
                     std::unordered_map<std::string, int>>
      DelMap;

  std::lock_guard<std::mutex> lock(StatisticMapMutex);
  for (const auto &pair : StatMap) {
    CallPathNode *cpn = pair.first;
    DelMap[cpn->function]["Instructions"] +=
        pair.second->getValue(stats::instructions);
    DelMap[cpn->function]["SolverTime"] +=
        pair.second->getValue(stats::solverTime);
    DelMap[cpn->function]["Forks"] += pair.second->getValue(stats::forks);
  }

  return DelMap;
}

void Delta::initPrevDelta(
    std::unordered_map<CallPathNode *, StatisticRecord *> StatMap) {

  for (const auto &pair : StatMap) {
    previousMap[pair.first->function]["Instructions"] =
        pair.second->getValue(stats::instructions);
    previousMap[pair.first->function]["Forks"] =
        pair.second->getValue(stats::forks);
    previousMap[pair.first->function]["SolverTime"] =
        pair.second->getValue(stats::solverTime);
  }
}
