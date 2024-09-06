//===-- Delta.cpp -----------------------------------------------*- C++ -*-===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "Delta.h"
#include <iostream>

using namespace klee;

namespace klee {



std::map<std::string, int>
Delta::CalculateDelta(std::map<std::string, int> statisticMap) {

  for (const auto &pair : statisticMap) {
    deltaMap[pair.first] = pair.second - previousMap[pair.first];
  }

  previousMap = statisticMap; 

  return deltaMap;
}
} // namespace klee
