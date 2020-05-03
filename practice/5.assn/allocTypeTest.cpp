#include "llvm/AsmParser/Parser.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/SourceMgr.h"
#include "gtest/gtest.h"
#include "allocType.h"

using namespace llvm;
using namespace std;

TEST(AllocTypeTest, MyTest) {
  // Create an IR Module.
  LLVMContext Context;
  unique_ptr<Module> M(new Module("MyTestModule", Context));
  auto *TestFTy = FunctionType::get(Type::getVoidTy(Context), {}, false);
  auto *MallocFTy = FunctionType::get(Type::getInt8PtrTy(Context),
                                      { Type::getInt64Ty(Context) }, false);
  Function *TestF = Function::Create(TestFTy, Function::ExternalLinkage,
                                     "test", *M);
  Function *MallocF = Function::Create(MallocFTy, Function::ExternalLinkage,
                                       "malloc", *M);

  BasicBlock *bbEntry = BasicBlock::Create(Context, "entry", TestF);
  IRBuilder<> entry(bbEntry);

  // arr : %arr = malloc(8)
  auto *n = ConstantInt::get(Type::getInt32Ty(Context), 8);
  auto *arr = entry.CreateCall(MallocF, {n}, "arr");

  // ep1 : %ep1 = getelementptr i8, i8* %arr, i32 0
  // e1 : %e1 = load i8, i8* %ep1
  // ep2 : %ep2 = getelementptr i8, i8* %arr, i32 1
  // e2 : %e2 = load i8, i8* %ep2
  auto *ep1 = entry.CreateGEP(arr, ConstantInt::get(Type::getInt32Ty(Context), 0), "ep1");
  auto *e1 = entry.CreateLoad(ep1, "e1");
  auto *ep2 = entry.CreateGEP(arr, ConstantInt::get(Type::getInt32Ty(Context), 1), "ep2");
  auto *e2 = entry.CreateLoad(ep2, "e2");

  // s : %sum = add i8 %e1, %e2
  // avg : %avg = udiv i8 %sum, 2
  // avg64 : %avg32 = bitcast i8 %avg to i32
  auto *s = entry.CreateAdd(e1, e2, "sum");
  auto *avg = entry.CreateUDiv(s, ConstantInt::get(Type::getInt8Ty(Context), 2), "avg");
  auto *avg32 = entry.CreateBitCast(avg, Type::getInt32Ty(Context), "avg32");

  // p : %p = alloca i32
  // st : store i32 %avg32, i32* %p
  auto *p = entry.CreateAlloca(Type::getInt32Ty(Context), ConstantInt::get(Type::getInt32Ty(Context), 1), "p");
  auto *st = entry.CreateStore(avg32, p);

  BasicBlock *bbExit = BasicBlock::Create(Context, "exit", TestF);
  entry.CreateBr(bbExit);
  IRBuilder<>(bbExit).CreateRetVoid();

  FunctionAnalysisManager FAM;
  PassBuilder PB;
  PB.registerFunctionAnalyses(FAM);
  auto AARes = FAM.getResult<BasicAA>(*TestF);

  EXPECT_EQ(getBlockType(n), UNKNOWN);
  EXPECT_EQ(getBlockType(arr), HEAP);
  EXPECT_EQ(getBlockType(ep1), HEAP);
  EXPECT_EQ(getBlockType(avg32), UNKNOWN);
  EXPECT_EQ(getBlockType(p), STACK);
  EXPECT_NE(getBlockType(st), HEAP);
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
