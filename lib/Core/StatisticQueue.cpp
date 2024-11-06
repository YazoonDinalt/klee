#include "StatisticQueue.h"
#include "llvm/IR/Function.h"
#include <condition_variable>
#include <mutex>
#include <queue>
#include <unordered_map>

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
      value = StatQueue.front();
  StatQueue.pop();
  return value;
}

bool StatisticQueue::empty() {

  std::unique_lock<std::mutex> lock(mutex);
  return StatQueue.empty();
}
