#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/PatternMatch.h"

using namespace llvm;
using namespace std;
using namespace llvm::PatternMatch;

namespace {
class PropagateIntegerEquality : public PassInfoMixin<PropagateIntegerEquality> {
public:
  // This pass finds the icmp instruction in a block first, and then
  // finds branch instruction in the same block to match and optimize.
  // It is true as the br and corresponding cond are always in the same block
  // according to the assignment specification, but there is some waste of time
  // if there are multiple icmp in one block.
  // So it's better to find the branch instruction first, and then find the
  // correspoding icmp instruction inside a block
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
	  DominatorTree &DT = FAM.getResult<DominatorTreeAnalysis>(F);
	  for(auto &BB1 : F){
		  for(auto &I1 : BB1){
			  // switch deals with the only case where getOpcode() is ICmp
			  // so better to use if statement
			  /*  if(I1.getOpcode() == Instruction::ICmp) */ 
			  switch(I1.getOpcode()){
				  // better to insert white spaces appropriately
				  // and maintain consistent indentation for readability
				  case Instruction::ICmp:{
					bool eq=false;
					if(dyn_cast<ICmpInst>(&I1)!=nullptr) eq=dyn_cast<ICmpInst>(&I1)->getPredicate()==CmpInst::ICMP_EQ;
					if(!eq) continue;
					BasicBlock *BX,*BY;
					 Value *V1 = I1.getOperand(0);
					 Value *V2 = I1.getOperand(1);
					 Value *lvalue = &cast<Value>(I1);
					 // Using match() in PatternMatch.h can simplify code above
					 // by determining whether the instruction is what we want
					 // and extracting all operands at once
					 if(dyn_cast<Argument>(V1)!=nullptr && dyn_cast<Argument>(V2)==nullptr){
						 //Case 1
					 }
					 else if (dyn_cast<Argument>(V1)==nullptr && dyn_cast<Argument>(V2)!=nullptr){
						 //Case 2
						 Value *T = V1;
						 V1 = V2;
						 V2 = T;
					 }
					 else if (dyn_cast<Argument>(V1)!=nullptr && dyn_cast<Argument>(V2)!=nullptr){
						 if(dyn_cast<Argument>(V1) -> getArgNo() >  dyn_cast<Argument>(V2) -> getArgNo()){
					//		 Case 3
							 Value *T = V1;
							 V1 = V2;
							 V2 = T;
						 }
					 }
					 // Repeated appearance of dyn_cast<Argument>([V1, V2])
					 // can be simplified by assigning to variables at first
					 /*  Argument *arg1 = dyn_cast<Argument>(V1);	*
					  *  Argument *arg2 = dyn_cast<Argument>(V2);	*
					  *  if(arg1 && !arg2) // case 1				*
					  *  else if(!arg1 && arg2) //case 2			*
					  *  else if(arg1 && arg2) //case 3				*/
					 else{
						 //Case 4
						 //find V1, V2
						 bool flag=false;
						 for(auto &TB : F){
							 if(flag) break;
							 for(auto &TI : TB){
								 if(flag) break;
								 if(&cast<Value>(TI) == V1){
									 flag=true;
									 break;
								 }
								 else if(&cast<Value>(TI) == V2){
									 Value *T = V1;
									 V1=V2;
									 V2=T;
									 flag=true;
									 break;
								 }
							 }
						 }
					 }
					 for(auto &I2 : BB1){
				     // As branch instruction can be the terminator only,
					 // better to use match() directly to the terminator,
					 // not for all instructions in BB1 with for loop
					 /*  Instruction *I2 = BB1.getTerminator();	*
					  *  if(match(I2, m_Br(...))) 				*/
						 if(match(&I2,m_Br(m_Value(lvalue),m_BasicBlock(BX),m_BasicBlock(BY)))){
							 BasicBlock *BBB=BY;
							 if(eq) BBB=BX;
							 // Here, eq is always true due to the line 27,
							 // and there's no modification to eq
							 // Better to initialize BBB with BX directly
							 BasicBlockEdge BBE(&BB1,BBB);
							 for(auto itr = V2->use_begin(),end=V2->use_end();itr!=end;){
								 Use &U = *itr++;
								 User *Usr = U.getUser();
								 Instruction *UsrI = dyn_cast<Instruction>(Usr);
								 if(UsrI){
									 BasicBlock *BB = UsrI->getParent();
									 if(DT.dominates(BBE,BB)) U.set(V1);
								 }
							 }
						 }
					 }
				}
			  	default:{
					break;
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
