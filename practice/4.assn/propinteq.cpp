#include "llvm/IR/PassManager.h"
#include "llvm/IR/PatternMatch.h"
#include "llvm/IR/Module.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;
using namespace std;
using namespace llvm::PatternMatch;

namespace {
class PropagateIntegerEquality : public PassInfoMixin<PropagateIntegerEquality> {
  /* Substitute all uses of Value Y in BasicBlock BB with Value X */
  void substitute(BasicBlock &BB, Value *X, Value *Y) {
    for (auto &I : BB) {
      unsigned n = I.getNumOperands();
      for(auto i=0; i<n; i++) {
        if(Y == I.getOperand(i))
          I.setOperand(i, X);
      }
    }
  }
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
    for (auto I = F.begin(), E = F.end(); I != E; ++I) {
      DominatorTree &DT = FAM.getResult<DominatorTreeAnalysis>(F);
      BasicBlock &BB = *I;
      Instruction *end_br = BB.getTerminator();
      Value *Tr, *Fa, *Cnd;
      // If a block is ended with conditional branch
      // Cnd has the instruction which generates that condition
      if (match(end_br, m_Br(m_Value(Cnd), m_Value(Tr), m_Value(Fa)))) {
        Value *X, *Y;
        ICmpInst::Predicate Pred;
        // Condition is "icmp eq iN x y" which is optimizable
        if (match(Cnd, m_ICmp(Pred, m_Value(X), m_Value(Y))) &&
        Pred == ICmpInst::ICMP_EQ) {
          // Determine priority of two operands
          // pri = 1 if X has higher priority to substitute Y
          // pri = -1 if Y has higher priority to substitute X
          Constant *C;
          int pri = 0;
          // 1. Constant int is an operand of icmp
          if (match(X, m_Constant(C)))
            pri = 1;
          else if(match(Y, m_Constant(C)))
            pri = -1;
          // 2. Two operands of icmp are not constant
          else {
            // 2-1. One of operands is given as an argument
            for (Argument &arg : F.args()) {
              if (&arg == X) {
                pri = 1;
                break;
              } else if(&arg == Y) {
                pri = -1;
                break;
              }
            }
            // 2-2. Both operands are defined in preexisting instruction
            if (pri==0 && DT.dominates(dyn_cast<Instruction>(X), dyn_cast<Instruction>(Y)))
              pri = 1;
            else if (pri==0 && DT.dominates(dyn_cast<Instruction>(Y), dyn_cast<Instruction>(X)))
              pri = -1;
          }
          BasicBlockEdge BBE(&BB, dyn_cast<BasicBlock>(Tr));
          for (BasicBlock &BBSubstitute : F) {
            if(DT.dominates(BBE, &BBSubstitute))
              substitute(BBSubstitute, (pri==1) ? X : Y, (pri==-1) ? X : Y);
          }
        }
      }
    }

    return PreservedAnalyses::all();
  }
};
}

extern "C" ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "PropagateIntegerEquality", "v0.1",
    [](PassBuilder &PB) {
      PB.registerPipelineParsingCallback(
        [](StringRef Name, FunctionPassManager &FPM,
           ArrayRef<PassBuilder::PipelineElement>) {
          if (Name == "prop-int-eq") {
            FPM.addPass(PropagateIntegerEquality());
            return true;
          }
          return false;
        }
      );
    }
  };
}
