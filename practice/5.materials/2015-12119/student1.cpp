#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/PatternMatch.h"

using namespace llvm;
using namespace std;
using namespace llvm::PatternMatch; // Important!

namespace
{
class PropagateIntegerEquality : public PassInfoMixin<PropagateIntegerEquality>
{
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM)
  {
    // get a dominator tree
    DominatorTree &DT = FAM.getResult<DominatorTreeAnalysis>(F);
    bool OptimFlag; // Flag to indicate whether optimization should happen
    // visit every basic block in the function
    for (auto &BB : F)
    {
      OptimFlag = false; // reset optimization flag
      // get the terminating Condition of the basic block. It can be either "br" or "ret". We are interested in br only.
      BranchInst *TI = dyn_cast<BranchInst>(BB.getTerminator()); // check if the termintor is "br"
      if (!TI || !(TI->isConditional()))
        continue;                       // check if the termintor is a Conditional branch. Short circuit: TI->isConditional() will not be executed if TI is null.
      Value *Cond = TI->getCondition(); // get the branching Condition
      // checking if the branching condition is ICMP_EQ
      CmpInst *Cmp = dyn_cast<CmpInst>(Cond);
      if (!Cmp || (Cmp->getPredicate() != ICmpInst::ICMP_EQ))
      { // short circuit: no null pointer deferencing
        // outs () << "The condition was not ICMP_EQ, so it is ignored\n"; // debug
        continue;
      }

      // below code will be executed only when Cmp==ICMP_EQ
      Value *V1 = Cmp->getOperand(0);
      Value *V2 = Cmp->getOperand(1);
      // Using match() declared in PatternMatch.h can simplify codes
      // from line 26 to line 40 above.
      /*  Value *Cond, *True, *False, *V1, *V2;                              *
       *  ICmpInst::Predicate Pred;                                          *
       *  if(!match(TI, m_Br(m_Value(Cond), m_Value(True), m_Value(False)))) *
       *    continue;                                                        *
       *  if(!match(Cond, m_ICmp(Pred, m_Value(V1), m_Value(V2))) ||         *
       *  Pred != ICmpInst::ICMP_EQ)                                         *
       *    continue;                                                        */

      if (V1->getType()->isPointerTy())
      // specification says to optimize integer equality only, so better to use
      // if(!(V1->getType()->isIntegerTy()) || !(V1->getType()->isIntegerTy()))
        continue; // don't optimize if V1 and V2 are PointerType

      // outs() << "operands: " << *V1 << "  " << *V2 << "\n";
      Value *OldVal; // to be replaced
      Value *NewVal; // to replace with

      // to check whether an operand is an instruction or an argument
      Instruction *V1Inst = dyn_cast<Instruction>(V1);
      Instruction *V2Inst = dyn_cast<Instruction>(V2);
      Argument *V1Arg = dyn_cast<Argument>(V1);
      Argument *V2Arg = dyn_cast<Argument>(V2);

      //case 1: If two instructions V1 and V2 are compared, and V1 dominates V2, replace V2 with V1.
      if (V1Inst && V2Inst)
      { // if both V1 and V2 are instructions 
        if (DT.dominates(V1Inst, V2Inst))
        {
          OptimFlag = true;
          OldVal = V2;
          NewVal = V1;
        }
        if (DT.dominates(V2Inst, V1Inst))
        {
          OptimFlag = true;
          OldVal = V1;
          NewVal = V2;
        }
      }
      //case 2: If two arguments %x and %y are compared, replace the one which appeared later in the function argument list with the other one which appeared earlier.
      else if (V1Arg && V2Arg)
      {
        Function *V1Fnt = V1Arg->getParent();
        Function *V2Fnt = V2Arg->getParent();

        int V1ArgNum, V2ArgNum;
        int idx = 0;
        for (auto itr = V1Fnt->arg_begin(), end = V1Fnt->arg_end(); itr != end;)
        {
          if (itr->getName() == V1Arg->getName())
            V1ArgNum = itr->getArgNo();
          if (itr->getName() == V2Arg->getName())
            V2ArgNum = itr->getArgNo();
          itr++;
        }
       
        // Always "OptimFlag = true" in both if blocks, so take it out of block
        if (V1ArgNum > V2ArgNum)
        { // V1 appears later
          OptimFlag = true;
          OldVal = V1;
          NewVal = V2;
        }
        else if (V2ArgNum > V1ArgNum)
        { // V2 appears later
          OptimFlag = true;
          OldVal = V2;
          NewVal = V1;
        }
      }
      //case 3: If an argument is compared with an instruction, replace the instruction with an argument.
      else if (V1Inst && V2Arg)
      {
        OptimFlag = true;
        OldVal = V1;
        NewVal = V2;
      }
      else if (V1Arg && V2Inst)
      {
        OptimFlag = true;
        OldVal = V2;
        NewVal = V1;
      }

      if (!OptimFlag) // quit if there isn't anything to optimize
      {
        continue;
      }

      // now, replace OldVal with NewVal.
      BasicBlock *trueDestBlock = TI->getSuccessor(0); // destination block when the Condition is true.
      BasicBlockEdge TBE(&BB, trueDestBlock);
      // find all uses of OldVal
      for (auto itr = OldVal->use_begin(), end = OldVal->use_end(); itr != end;)
      {
        Use &U = *itr++;
        User *Usr = U.getUser();
        Instruction *UsrI = dyn_cast<Instruction>(Usr);
        if (UsrI)
        {
          BasicBlock *BB = UsrI->getParent();
          if (DT.dominates(TBE, BB))
            U.set(NewVal);
        }
      }
    }
    return PreservedAnalyses::all();
  }
};
}; // namespace

extern "C" ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo()
{
  return {
      LLVM_PLUGIN_API_VERSION, "PropagateIntegerEquality", "v0.1",
      [](PassBuilder &PB) {
        PB.registerPipelineParsingCallback(
            [](StringRef Name, FunctionPassManager &FPM,
               ArrayRef<PassBuilder::PipelineElement>) {
              if (Name == "prop-int-eq")
              {
                FPM.addPass(PropagateIntegerEquality());
                return true;
              }
              return false;
            });
      }};
}
