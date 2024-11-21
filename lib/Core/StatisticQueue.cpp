//===-- StatisticQueue.cpp---------------------------------------*- C++ -*-===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "StatisticQueue.h"

using namespace klee;

void StatisticQueue::push(
    const std::unordered_map<const llvm::Function *,
                             std::unordered_map<std::string, int>> &value) {
  std::unique_lock<std::mutex> lock(mutex);
  StatQueue.push(value);
  cond_var.notify_one();
}

std::unordered_map<const llvm::Function *, std::unordered_map<std::string, int>>
StatisticQueue::pop() {
  std::unique_lock<std::mutex> lock(mutex);
  cond_var.wait(lock, [this] { return !StatQueue.empty(); });
  std::unordered_map<const llvm::Function *,
                     std::unordered_map<std::string, int>>
      value = std::move(StatQueue.front());
  StatQueue.pop();
  return value;
}

bool StatisticQueue::empty() {

  std::unique_lock<std::mutex> lock(mutex);
  return StatQueue.empty();
}
