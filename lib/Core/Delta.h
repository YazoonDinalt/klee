//===-- Delta.h--- ----------------------------------------------*- C++ -*-===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef KLEE_GETDELTA_H
#define KLEE_GETDELTA_H

#include <map>
#include <string>

namespace klee {

/// TimingSolver - A simple class that collects delta statistics
class Delta {
public:
  std::map<std::string, int> deltaMap;
  std::map<std::string, int> previousMap{{"Instructions", 0}, {"Forks", 0}};
  ;

public:
  void initPrevious();
  std::map<std::string, int>
  CalculateDelta(std::map<std::string, int> statisticMap);
};
} // namespace klee

#endif /* KLEE_DELTA_H */
