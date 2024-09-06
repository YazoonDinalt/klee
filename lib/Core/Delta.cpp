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

std::map<std::string, std::map<std::string, int>>
Delta::CalculateDelta(std::map<std::string, StatisticRecord> StatMap) {

  std::map<std::string, std::map<std::string, int>> DelMap;

  for (const auto &pair : StatMap) {
    auto name = pair.first;

    DelMap[pair.first]["Instructions"] =
        pair.second.getValue(stats::instructions);
    DelMap[pair.first]["Forks"] = pair.second.getValue(stats::forks);
  }

  return DelMap;
}

} // namespace klee
