//===-- StatisticQueue.h-----------------------------------------*- C++ -*-===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm/IR/Function.h"
#include <condition_variable>
#include <queue>

#ifndef KLEE_STATISTICQUEUE_H
#define KLEE_STATISTICQUEUE_H

namespace klee {

class StatisticQueue {

private:
  std::mutex mutex;
  std::condition_variable cond_var;
  std::queue<std::unordered_map<const llvm::Function *,
                                std::unordered_map<std::string, int>>>
      StatQueue;

public:
  void
  push(const std::unordered_map<const llvm::Function *,
                                std::unordered_map<std::string, int>> &value);

  std::unordered_map<const llvm::Function *,
                     std::unordered_map<std::string, int>>
  pop();
  bool empty();
};

} // namespace klee

#endif