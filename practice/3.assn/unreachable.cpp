#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include <string>
#include <vector>
using namespace llvm;

namespace {
class MyUnreachablePass : public PassInfoMixin<MyUnreachablePass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
    std::vector<BasicBlock*> reached_blocks;
    std::vector<BasicBlock*> stack;
    std::vector<std::string> unreached_blocks;
    auto I = F.begin();
    BasicBlock &init = *I;

    stack.push_back(&init);
    while(!stack.empty()) {
      BasicBlock* BB = stack.back();
      bool found = false;
      stack.pop_back();
      for(unsigned i=0; i<reached_blocks.size(); i++)
        if(BB == reached_blocks.at(i)) {
          found = true;
          break;
        }

      if(!found)
        reached_blocks.push_back(BB); 
      else
        continue;

      Instruction *inst = BB->getTerminator();
      for(unsigned i=0; i<(inst->getNumSuccessors()); i++)
        stack.push_back(inst->getSuccessor(i));
    }

    for (auto I= F.begin(); I != F.end(); ++I) {
      BasicBlock &BB = *I;
      bool reached = false;
      std::string str = (BB.getName()).str();
      for(unsigned i=0; i<reached_blocks.size(); i++) {
        if(str == (reached_blocks.at(i)->getName()).str()) {
          reached = true;
          break;
        }
      }

      if(!reached) {
        std::vector<std::string>::iterator i;
        for(i=unreached_blocks.begin(); i!=unreached_blocks.end(); i++)
          if((*i) > str)
            break;
        unreached_blocks.insert(i, str);
      }
    }

    for(auto i=unreached_blocks.begin(); i!=unreached_blocks.end(); i++)
      outs() << (*i) << "\n";

    return PreservedAnalyses::all();
    }
};
}

extern "C" ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "MyUnreachablePass", "v0.1",
    [](PassBuilder &PB) {
      PB.registerPipelineParsingCallback(
        [](StringRef Name, FunctionPassManager &FPM,
           ArrayRef<PassBuilder::PipelineElement>) {
          if (Name == "my-unreachable") {
            FPM.addPass(MyUnreachablePass());
            return true;
          }
          return false;
        }
      );
    }
  };
}
