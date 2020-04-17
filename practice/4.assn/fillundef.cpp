#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include<vector>

using namespace llvm;
using namespace std;

// This example shows how to visit uses of an instruction, and selectively
// replace use with UndefValue.
// After it is run, all non-constant operands in "undef_zone" block will be
// replaced with undef.

namespace {
class FillUndef : public PassInfoMixin<FillUndef> {
  void replaceSpecificUsesWithUndef(Value *V) {
    // As we are changing use list, `for (auto &U : V->uses())` cannot be
    // used.
    // Otherwise, the iterator is invalidated at U.set(), so incrementing the
    // iterator will crash.
    for (auto itr = V->use_begin(), end = V->use_end(); itr != end;) {
      Use &U = *itr++;
      User *Usr = U.getUser();
      Instruction *UsrI = dyn_cast<Instruction>(Usr);
      if (UsrI) {
        BasicBlock *BB = UsrI->getParent();
        if (BB->getName() == "undef_zone")
          U.set(UndefValue::get(V->getType()));
      }
    }
  }
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
    for (Argument &Arg : F.args())
      replaceSpecificUsesWithUndef(&Arg);

    for (auto &BB : F)
      for (auto &I : BB)
        replaceSpecificUsesWithUndef(&I);
    return PreservedAnalyses::all();
  }
};
}

extern "C" ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "FillUndef", "v0.1",
    [](PassBuilder &PB) {
      PB.registerPipelineParsingCallback(
        [](StringRef Name, FunctionPassManager &FPM,
           ArrayRef<PassBuilder::PipelineElement>) {
          if (Name == "fill-undef") {
            FPM.addPass(FillUndef());
            return true;
          }
          return false;
        }
      );
    }
  };
}
