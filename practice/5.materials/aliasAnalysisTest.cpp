#include "llvm/Analysis/BasicAliasAnalysis.h"
#include "llvm/AsmParser/Parser.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/SourceMgr.h"
#include "gtest/gtest.h"

using namespace llvm;
using namespace std;

class MyAliasAnalysisTest : public testing::Test {
private:
  // Parses LLVM IR Assembly & returns a created module.
  // Note that the result is contained in unique_ptr; It is one of C++'s smart
  // pointer classes. if the unique_ptr goes out of a scope without being moved
  // to somewhere else, its content is deleted.
  // See also: https://thispointer.com/c11-unique_ptr-tutorial-and-examples/
  unique_ptr<Module> parseModule(StringRef Assembly) {
    SMDiagnostic Error;
    unique_ptr<Module> M = parseAssemblyString(Assembly, Error, Context);

    string errMsg;
    raw_string_ostream os(errMsg);
    Error.print("", os);
    EXPECT_TRUE(M) << os.str();

    return M;
  }

protected:
  // LLVMContext object maintains all other objects that are created during
  // parsing or optimizing programs. For example, when you created a new i16
  // type instance, it is registered at this Context instance.
  LLVMContext Context;
  // The LLVM IR module that is parsed.
  unique_ptr<Module> M;
  // @test() function inside M.
  Function *testF;
  // A mapping from register names to instructions inside @test().
  map<string, Instruction *> Instrs;

  // Parse the IR assembly by calling parseModule function.
  void parseAssembly(StringRef Assembly) {
    M = parseModule(Assembly);
    ASSERT_TRUE(M);

    Function *F = M->getFunction("test");
    ASSERT_TRUE(F) << "Test must have a function @test";
    if (!F)
      return;

    for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
      if (I->hasName())
        Instrs[I->getName().str()] = &*I;
    }
    testF = F;
  }

  // Create an empty function & store it into testF.
  void createEmptyTestFunction() {
    M = unique_ptr<Module>(new Module("MyModule", Context));
    FunctionType *FTy =
      FunctionType::get(Type::getVoidTy(Context), {}, false);
    testF = Function::Create(FTy, Function::ExternalLinkage, "test", *M);
  }


  // Run basic alias analysis on two values A and B, and return its result.
  AliasResult getAliasResult(const Value *A, const Value *B,
                             BasicAA::Result &Res) {
    AAQueryInfo EmptyInfo;
    return Res.alias(MemoryLocation(A, 4), // 4-bytes access..!
                     MemoryLocation(B, 4), // 4-bytes access..!
                     EmptyInfo);
  }

  // Run basic alias analysis on two registers %a and %b, and return its result.
  AliasResult getAliasResult(const string &a, const string &b,
                             BasicAA::Result &Res) {
    AAQueryInfo EmptyInfo;
    EXPECT_TRUE(Instrs.count(a) != 0) << "There is no register " << a;
    EXPECT_TRUE(Instrs.count(b) != 0) << "There is no register " << b;
    return Res.alias(MemoryLocation(Instrs[a], 4), // 4-bytes access..!
                     MemoryLocation(Instrs[b], 4), // 4-bytes access..!
                     EmptyInfo);
  }
};


// TEST_F(C, F): Add a test that is a member function F of C.
// Note that, this macro actually creates a dummy class that inherits
// MyAliasAnalysisTest; that is why member functions are declared as protected.
//
// TestUsingParseAssembly creates a test by writing the input program as a
// string literal & running unit tests.
TEST_F(MyAliasAnalysisTest, TestUsingParseAssembly) {
  // Let's parse this IR assembly, and create mappings from register names
  // to instruction objects!
  parseAssembly(R"myasm(
    define void @test(i1 %c) {
      %p = alloca i32
      %q = alloca i32
      ; %p, %q never alias! (they have different memory addresses)
      %p2 = getelementptr i32, i32* %p, i32 1
      ; %p, %p2 never alias again.
      %p3 = getelementptr i32, i32* %p, i32 0
      ; %p, %p3 must alias (they have the same address).

      %r = select i1 %c, i32* %p, i32* %q
      ; r and %p may alias (it is unknown in compile time)
      ret void
    }
  )myasm");

  // Mappings created!
  Instruction *p = this->Instrs["p"];
  Instruction *p2 = this->Instrs["p2"];
  outs() << "----- MyAliasAnalysisTest.TestUsingParseAssembly -----" << "\n";
  outs() << "p: " << *p << "\n";
  outs() << "p2: " << *p2 << "\n";

  // Let's build a passbuilder & get alias analysis result..
  FunctionAnalysisManager FAM;
  PassBuilder PB;
  PB.registerFunctionAnalyses(FAM);
  // Do you remember how we created DominatorTree object in assignment 3?
  // The BasicAA::Result object (BasicAA stands for 'Basic Alias Analysis') is
  // created in a similar way..!
  auto AARes = FAM.getResult<BasicAA>(*this->testF);

  EXPECT_EQ(getAliasResult("p", "q", AARes), AliasResult::NoAlias);
  EXPECT_EQ(getAliasResult("p", "p2", AARes), AliasResult::NoAlias);
  EXPECT_EQ(getAliasResult("p", "p3", AARes), AliasResult::MustAlias);
  // MayAlias is the least accurate result.
  // In principle, alias analysis can always return 'MayAlias'.
  // It is theoretically correct, but it is never useful in practice.
  EXPECT_EQ(getAliasResult("p", "r", AARes), AliasResult::MayAlias);
}

// TestUsingIRBuilder creates a test by constructing an input program using
// IRBuilder & running unit tests.
TEST_F(MyAliasAnalysisTest, TestUsingIRBuilder) {
  // Create an empty IR function & store it into this->testF.
  createEmptyTestFunction();

  // Create an entry basic blcok.
  BasicBlock *Entry = BasicBlock::Create(Context, "entry", testF);
  IRBuilder<> EntryBuilder(Entry);
  auto *p = EntryBuilder.CreateAlloca(Type::getInt32PtrTy(Context), nullptr, "p");
  auto *q = EntryBuilder.CreateAlloca(Type::getInt32PtrTy(Context), nullptr, "q");
  auto *p2 = EntryBuilder.CreateGEP(p, ConstantInt::get(Type::getInt32Ty(Context), 1));
  auto *p3 = EntryBuilder.CreateGEP(p, ConstantInt::get(Type::getInt32Ty(Context), 0));
  // What about terminator inst..?
  // Create an exit basic block.
  BasicBlock *Exit = BasicBlock::Create(Context, "exit", testF);
  EntryBuilder.CreateBr(Exit);
  IRBuilder<>(Exit).CreateRetVoid(); // In-line creation of `ret void`

  outs() << "----- MyAliasAnalysisTest.TestUsingIRBuilder -----" << "\n";
  outs() << "Created a test function: " << *this->testF << "\n";

  // Again, let's build a passbuilder & get alias analysis result..
  FunctionAnalysisManager FAM;
  PassBuilder PB;
  PB.registerFunctionAnalyses(FAM);
  // Do you remember how DominatorTree object was created in the assignment 3?
  // The BasicAA object (which stands for 'Basic Alias Analysis') is created
  // in a similar way..!
  auto AARes = FAM.getResult<BasicAA>(*this->testF);

  EXPECT_EQ(getAliasResult(p, q, AARes), AliasResult::NoAlias);
  EXPECT_EQ(getAliasResult(p, p2, AARes), AliasResult::NoAlias);
  EXPECT_EQ(getAliasResult(p, p3, AARes), AliasResult::MustAlias);
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
