## Assignment 4

NOTE: For assignment 4, you will need to use **LLVM 10.0 binary that is
compiled by yourself** because prebuilt binary does not contain FileCheck.
Please download ../4.materials/llvm-10.0.json, and clone/build LLVM 10.0 using
https://github.com/aqjune/llvmscript.

To submit your code:

```
./package.sh
# Now, upload submit.tar.gz to ETL
```

Also, after this assignment, you will have peer-to-peer **code review** time
with your submission to assn 4. So, please do your best to make your code
elegant & succinct!


### Helpful resources for this homework

This directory has three sample programs that will be helpful for doing your assignment:

- printdom.cpp shows how to check dominance relation between two basic blocks
as well as **an edge** and a block.

- instmatch.cpp shows how to use matchers to detect instructions of interest.

- fillundef.cpp shows how to visit uses of an instruction and replace interested
use with something else selectively.

To see how the sample programs work, please run:

```
./sample.sh all (your dir)/llvm-10.0-releaseassert/bin
```

Also, don't forget to revisit `myopt.cpp` used in practice session 4 as well.


### 1. propinteq.cpp

Write a pass that optimizes a program by propagating equality of integer values.
To be specific, whenever `icmp eq iN x, y` is used by a branch condition, please
replace x with y (or y with x) for all uses of x that are
(1) executed after the branch is taken
(2) dominated by the conditional branch.

```
  %cond = icmp eq i32 %x, %y
  br i1 %cond, label %BB_true, label %BB_false
BB_true:
  call void @f(i32 %x, i32 %y) // becomes @f(i32 %x, i32 %x)
  br label %BB_end
BB_false:
  call void @f(i32 %x, i32 %y) // shouldn't be optimized
  br label %BB_end
BB_end:
  call void @f(i32 %x, i32 %y) // shouldn't be optimized
```

To check your implementation, please use `run.sh`.
This will run 11 FileChecks in `data` directory.
Note that they are not full tests; more tests can be added by TA when grading
your homework.

```
./run.sh all (your dir)/llvm-10.0-releaseassert/bin
# To build propinteq.cpp..
./run.sh build (your dir)/llvm-10.0-releaseassert/bin
# To see the output of input.ll..
./run.sh run (your dir)/llvm-10.0-releaseassert/bin
# To run tests..
./run.sh test (your dir)/llvm-10.0-releaseassert/bin
```

#### Correctness of Optimization

Here is the list of conditions for correct replacement of two values.

1. If two instructions %x and %y are compared, and %x dominates %y,
replace %y with %x.

```
; pseudocode
entry:
  %x = ...
  br %BB1
BB1:
  %y = ...
  %cond = icmp eq i32 %x, %y
  br %cond, %BB_true, %BB_false
BB_true:
  %a = add i32 %x, %y // should be %a = add %x, %x
```

2. If two arguments %x and %y are compared, replace the one which appeared later
in the function argument list with the other one which appeared earlier.

```
define void @f(i32 %y, i32 %x) {
  %cond = icmp eq i32 %x, %y
  br %cond, %BB_true, %BB_false
BB_true:
  %a = add i32 %x, %y // should be %a = add %y, %y because the function
                      // signature is f(y, x)
}
```

3. If an argument is compared with an instruction, replace the instruction
with an argument.

```
define void @f(i32 %a) {
  %b = ...
  %cond = icmp eq i32 %a, %b
  br %cond, %BB_true, %BB_false
BB_true:
  %a = add i32 %a, %b // should be %a = add %a, %a
```

After your optimization is run, there should be no more replacable values.
It is guaranteed that a comparison with constant value is never given to a
branch condition.

#### Hints

- Read the three sample programs + `myopt.cpp` in practice session 4 &
see what kind of valuable code snippets do they have.
Especially, the dominance check with an edge and a block!

- Be familiar with DominatorTree class. Understand why it is a **tree**.
Using autocompletion & online document, see what kind of methods it supports.

- If you're not sure whether you optimized your example correctly in general
(regardless of integer equality propagation), you can use Alive2
(http://volta.cs.utah.edu:8080 , repo : https://github.com/aliveToolkit/alive2).
:) If you're interested in how to use this, feel free to ask us.


### 2. Writing FileCheck Tests

Please write your own 3 FileCheck tests at mycheck/checkN.ll .

Your FileCheck tests will be run on other students' submissions, and will be
scored by measuring its effectiveness.
We'll calculate the total score based on two standards:

1. How many times did it catch other's miscompilation?

2. Is your FileCheck test valid on your submission & our TA's solution?

To see more details about FileCheck syntax, please visit
https://llvm.org/docs/CommandGuide/FileCheck.html .


#### Constraints

The test files should satisfy following constraints:

1. They are valid IR functions satisfying SSA. There is one function per file.

2. A branch condition should be either a constant or register.
If the condition is a register, it should only compare
integer registers (no vectors, pointers, int constants, etc).
If branch condition is a constant, it shouldn't be constant expression.

```
  br i1 true, ..      // okay

  %cond = icmp eq %x, %y
  br i1 %cond, ..     // okay

  %cond2 = icmp eq %x, 1
  br i1 %cond2, ..    // not okay
```

3. A terminator instruction should be either `br` or `ret`.

4. They should not have unreachable blocks.

Apr. 20: We have one more constraint to guarantee that there is only one
possible output.

5. If an `icmp` instruction is used by a conditional branch `br`, the conditional
branch and the `icmp` should be in the same basic block.

```
BB:
  %cond = icmp eq %x, %y
  br label %BB2
BB2:
  br i1 %cond, ..    // not okay

BB3:
  %cond3 = icmp eq %x, %y
  br li1 %cond3, ..  // okay
```

Note that all checks that are going to be used to grade your program will
satisfy these constraints.

#### Hints

- Please see how tests are written in data/ directory.

- Carefully write your tests so it does not miss checks. For example,
using `; CHECK :` instead of `; CHECK:` will cause FileCheck miss the check.
To check this, you can run `FileCheck` with `-v`.

- You can automatize FileCheck test generation using
`llvm/utils/update_test_checks.py`. If you're interested in
this, see FileCheck tests in `llvm/test/` and see how they're used!
