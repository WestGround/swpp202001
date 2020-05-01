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

  // Please fill @test() using IRBuilder to TestF.
  // The created program should contain at least:
  //   - One call instruction to malloc
  //     (hint: 'CreateCall(MallocF, { (size argument) }, "p")').
  //   - One alloca instruction
  //   - One bitcast
  //   - One getelementptr

  // Please write 4 EXPECT_* calls.
  // ex) EXPECT_EQ(getBlockType(p), AllocType::HEAP);
  // There should be at least:
  //   - One EXPECT_* check to AllocType::HEAP
  //   - One EXPECT_* check to AllocType::STACK
  //   - One EXPECT_* check to AllocType::UNKNOWN

  // A dummy EXPECT_EQ.
  // Please delete this line after you finish your implementation.
  EXPECT_EQ(1234, 5678);
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
