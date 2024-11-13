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

std::vector<nlohmann::json> Delta::SerializeDelMap(
    std::unordered_map<const llvm::Function *,
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
      std::string type_of_var = "int";
      for (const auto &jsonObject : newPrevMap) {
        if (jsonObject["params"]["funName"] == funName &&
            jsonObject["name"] == metricName) {
          prev = jsonObject["params"]["value"];
          break;
        }
      }

      if (count - prev != 0) {
        if (metricName == "SolverTime") {
          type_of_var = "double";
          double solverCount = static_cast<double>(count) / (double)1000000000;
          solverCount = static_cast<double>(round(solverCount * 100)) / 100;
          jsonArray.push_back({{"guid", UID},
                               {"name", metricName},
                               {"params",
                                {{"funName", funName},
                                 {"type", type_of_var},
                                 {"value", solverCount},
                                 {"transitive", false}}}});
        } else {
          jsonArray.push_back({{"guid", UID},
                               {"name", metricName},
                               {"params",
                                {{"funName", funName},
                                 {"type", type_of_var},
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

  for (const auto &pair : StatMap) {
    CallPathNode *cpn = pair.first;
    if (DelMap.count(cpn->function) > 0) {
      DelMap[cpn->function]["Instructions"] +=
          pair.second->getValue(stats::instructions);
      DelMap[cpn->function]["SolverTime"] +=
          pair.second->getValue(stats::solverTime);
      DelMap[cpn->function]["Forks"] += pair.second->getValue(stats::forks);
    } else {
      DelMap[cpn->function]["Instructions"] =
          pair.second->getValue(stats::instructions);
      DelMap[cpn->function]["SolverTime"] +=
          pair.second->getValue(stats::solverTime);
      DelMap[cpn->function]["Forks"] = pair.second->getValue(stats::forks);
    }
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

} // namespace klee
