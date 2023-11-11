//===-- TargetCalculator.cpp ---------- -----------------------------------===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "TargetCalculator.h"

#include "ExecutionState.h"
#include "ObjectManager.h"

#include "klee/Module/CodeGraphInfo.h"
#include "klee/Module/KInstruction.h"
#include "klee/Module/Target.h"
#include "klee/Module/TargetHash.h"

#include <set>
#include <vector>

using namespace llvm;
using namespace klee;

llvm::cl::opt<TrackCoverageBy> TrackCoverage(
    "track-coverage", cl::desc("Specifiy the track coverage mode."),
    cl::values(clEnumValN(TrackCoverageBy::None, "none", "Not track coverage."),
               clEnumValN(TrackCoverageBy::Blocks, "blocks",
                          "Track only covered block."),
               clEnumValN(TrackCoverageBy::Branches, "branches",
                          "Track only covered conditional branches."),
               clEnumValN(TrackCoverageBy::All, "all", "Track all.")),
    cl::init(TrackCoverageBy::None), cl::cat(ExecCat));

void TargetCalculator::update(const ExecutionState &state) {
  if (state.getPrevPC() == state.getPrevPC()->parent->getLastInstruction() &&
      !fullyCoveredFunctions.count(state.getPrevPC()->parent->parent)) {
    auto &fBranches = getCoverageTargets(state.getPrevPC()->parent->parent);

    if (!coveredFunctionsInBranches.count(state.getPrevPC()->parent->parent)) {
      if (fBranches.count(state.getPrevPC()->parent) != 0) {
        if (!coveredBranches[state.getPrevPC()->parent->parent].count(
                state.getPrevPC()->parent)) {
          state.coverNew();
          coveredBranches[state.getPrevPC()->parent->parent]
                         [state.getPrevPC()->parent];
        }
        if (!fBranches.at(state.getPrevPC()->parent).empty()) {
          unsigned index = 0;
          for (auto succ : successors(state.getPrevPCBlock()->basicBlock)) {
            if (succ == state.getPCBlock()->basicBlock) {
              if (!coveredBranches[state.getPrevPC()->parent->parent]
                                  [state.getPrevPC()->parent]
                                      .count(index)) {
                state.coverNew();
                coveredBranches[state.getPrevPC()->parent->parent]
                               [state.getPrevPC()->parent]
                                   .insert(index);
              }
              break;
            }
            ++index;
          }
        }
      }
      if (getCoverageTargets(state.getPrevPC()->parent->parent) ==
          coveredBranches[state.getPrevPC()->parent->parent]) {
        coveredFunctionsInBranches.insert(state.getPrevPC()->parent->parent);
      }
    }
    if (!fullyCoveredFunctions.count(state.getPrevPC()->parent->parent) &&
        coveredFunctionsInBranches.count(state.getPrevPC()->parent->parent)) {
      bool covered = true;
      std::set<KFunction *> fnsTaken;
      std::deque<KFunction *> fns;
      fns.push_back(state.getPrevPC()->parent->parent);

      while (!fns.empty() && covered) {
        KFunction *currKF = fns.front();
        fnsTaken.insert(currKF);
        for (auto &kcallBlock : currKF->kCallBlocks) {
          if (kcallBlock->calledFunctions.size() == 1) {
            auto calledFunction = *kcallBlock->calledFunctions.begin();
            KFunction *calledKFunction =
                state.getPrevPC()
                    ->parent->parent->parent->functionMap[calledFunction];
            if (calledKFunction->numInstructions != 0 &&
                coveredFunctionsInBranches.count(calledKFunction) == 0 &&
                !getCoverageTargets(calledKFunction).empty()) {
              covered = false;
              break;
            }
            if (!fnsTaken.count(calledKFunction) &&
                fullyCoveredFunctions.count(calledKFunction) == 0 &&
                calledKFunction->numInstructions != 0) {
              fns.push_back(calledKFunction);
            }
          }
        }
        fns.pop_front();
      }

      if (covered) {
        fullyCoveredFunctions.insert(state.getPrevPC()->parent->parent);
      }
    }
  }
}

void TargetCalculator::update(
    ExecutionState *current, const std::vector<ExecutionState *> &addedStates,
    const std::vector<ExecutionState *> &removedStates) {
  if (current && (std::find(removedStates.begin(), removedStates.end(),
                            current) == removedStates.end())) {
    localStates.insert(current);
  }
  for (const auto state : addedStates) {
    localStates.insert(state);
  }
  for (const auto state : removedStates) {
    localStates.insert(state);
  }
  for (auto state : localStates) {
    KFunction *kf = state->getPrevPCBlock()->parent;
    KModule *km = kf->parent;
    if (state->getPrevPC()->inst->isTerminator() &&
        km->inMainModule(*kf->function)) {
      update(*state);
    }
  }
  localStates.clear();
}

void TargetCalculator::update(ref<ObjectManager::Event> e) {
  if (auto statesEvent = dyn_cast<ObjectManager::States>(e)) {
    update(statesEvent->modified, statesEvent->added, statesEvent->removed);
  }
}

const std::map<KBlock *, std::set<unsigned>> &
TargetCalculator::getCoverageTargets(KFunction *kf) {
  switch (TrackCoverage) {
  case TrackCoverageBy::Blocks:
    return codeGraphInfo.getFunctionBlocks(kf);
  case TrackCoverageBy::Branches:
    return codeGraphInfo.getFunctionConditionalBranches(kf);
  case TrackCoverageBy::None:
  case TrackCoverageBy::All:
    return codeGraphInfo.getFunctionBranches(kf);

  default:
    assert(0 && "not implemented");
  }
}

bool TargetCalculator::uncoveredBlockPredicate(ExecutionState *state,
                                               KBlock *kblock) {
  bool result = false;

  auto &fBranches = getCoverageTargets(kblock->parent);

  if (fBranches.count(kblock) != 0 || isa<KCallBlock>(kblock)) {
    if (coveredBranches[kblock->parent].count(kblock) == 0) {
      result = true;
    } else {
      auto &cb = coveredBranches[kblock->parent][kblock];
      if (isa<KCallBlock>(kblock) &&
          cast<KCallBlock>(kblock)->calledFunctions.size() == 1) {
        auto calledFunction =
            *cast<KCallBlock>(kblock)->calledFunctions.begin();
        KFunction *calledKFunction =
            kblock->parent->parent->functionMap[calledFunction];
        result = fullyCoveredFunctions.count(calledKFunction) == 0 &&
                 calledKFunction->numInstructions;
      }
      if (fBranches.at(kblock) != cb) {
        result |=
            kblock->basicBlock->getTerminator()->getNumSuccessors() > cb.size();
      }
    }
  }

  return result;
}

TargetHashSet TargetCalculator::calculate(ExecutionState &state) {
  KBlock *kb = state.getPCBlock();
  KFunction *kf = kb->parent;
  kb = !isa<KCallBlock>(kb) || (kb->getLastInstruction() != state.getPC())
           ? kb
           : kf->blockMap[state.getPC()
                              ->parent->basicBlock->getTerminator()
                              ->getSuccessor(0)];
  for (auto sfi = state.stack.callStack().rbegin(),
            sfe = state.stack.callStack().rend();
       sfi != sfe; sfi++) {
    kf = sfi->kf;

    using std::placeholders::_1;
    KBlockPredicate func =
        std::bind(&TargetCalculator::uncoveredBlockPredicate, this, &state, _1);
    auto blocks = codeGraphInfo.getNearestPredicateSatisfying(kb, func, true);

    if (!blocks.empty()) {
      TargetHashSet targets;
      for (auto block : blocks) {
        auto &fBranches = getCoverageTargets(block->parent);

        if (fBranches.count(block) != 0 || isa<KCallBlock>(block)) {
          if (coveredBranches[block->parent].count(block) == 0) {
            targets.insert(ReachBlockTarget::create(block, false));
          } else {
            auto &cb = coveredBranches[block->parent][block];
            bool notCoveredFunction = false;
            if (isa<KCallBlock>(block) &&
                cast<KCallBlock>(block)->calledFunctions.size() == 1) {
              auto calledFunction =
                  *cast<KCallBlock>(block)->calledFunctions.begin();
              KFunction *calledKFunction =
                  block->parent->parent->functionMap[calledFunction];
              notCoveredFunction =
                  fullyCoveredFunctions.count(calledKFunction) == 0 &&
                  calledKFunction->numInstructions;
            }
            if (notCoveredFunction) {
              targets.insert(ReachBlockTarget::create(block, true));
            } else {
              if (fBranches.at(block) != cb) {
                for (unsigned index = 0;
                     index <
                     block->basicBlock->getTerminator()->getNumSuccessors();
                     ++index) {
                  if (!cb.count(index))
                    targets.insert(CoverBranchTarget::create(block, index));
                }
              }
            }
          }
        }
      }
      assert(!targets.empty());
      return targets;
    }

    if (sfi->caller) {
      kb = sfi->caller->parent;

      kb = !isa<KCallBlock>(kb) || (kb->getLastInstruction() != sfi->caller)
               ? kb
               : kf->blockMap[sfi->caller->parent->basicBlock->getTerminator()
                                  ->getSuccessor(0)];
    }
  }

  return {};
}

bool TargetCalculator::isCovered(KFunction *kf) const {
  return fullyCoveredFunctions.count(kf) != 0;
}
