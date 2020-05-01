## Assignment 5

### 1. Write GoogleTest for getBlockType

`AllocType getBlockType(const Value *V)` is a simple function written by TA
that returns whether `V` is pointing to the stack area or heap area.
For example, if `V` is an `alloca` instruction, `getBlockType(V)` returns
`STACK`.
If `V` is a `call` instruction to "malloc", `getBlockType(V)` returns `HEAP`.
If it is not known in compile time, it returns `UNKNOWN`.

Please write a Google unit test for `getBlockType` using `IRBuilder` and
`EXPECT_*`.

1. Please don't modify `getBlockType(V)`.

2. Fill the `@test` function using IRBuilder. The `@test` should contain at least:

    - One call instruction to malloc

    - One alloca instruction

    - One bitcast instruction (which should cast a pointer to a different
    pointer type in this example, e.g. `%y = bitcast i8* %x to i32*`)

    - One getelementptr instruction

3. Write 4 tests using `EXPECT_*` (e.g. `EXPECT_EQ`) that checks whether
`getBlockType(V)` is working as expected. There should be at least:

    - One EXPECT_* check to AllocType::HEAP

    - One EXPECT_* check to AllocType::STACK

    - One EXPECT_* check to AllocType::UNKNOWN


TAs will check whether your allocTypeTest.cpp compiles & the written code at
TEST(AllocTypeTest, MyTest) satisfies the conditions.

To submit your code:

```
./package.sh
# Now, upload submit.tar.gz to ETL
```

To test your code:

```
./run.sh all (your dir)/llvm-10.0-releaseassert/bin
```


### 2. Code Review

You will receive two students' propinteq.cpp via mail. Please read the
code and leave your review as C++ comments.
Please submit the code with reviews to ETL.

Your review should be written in English.
You can leave your C++ code in the comments if you think it is helpful
for writing a good feedback.
You don't need to leave your names because TAs already know who reviews the
code.

The contents of a comment depends on its location.
It is not strictly enforced, but please try to follow these rules if possible:

1. A comment that is left at the top of the source code contains a high-level
feedback on overall algorithm or structure of the whole code.

2. A comment that is left before a function definition contains a high-level
feedback to the specific function or relevant functions.

3. A comment that is left before/after a specific statement or code block
gives a low-level comment to the specific part.

TAs will read your comments by seeing diff from the original code as well as
reading the original program.
The diff should be less than 50 lines (meaning that your comments should be
less than 50 lines), and should follow 80-column rules
(one line should be less than 80 letters in ASCII).
If there are too many things to review, leave comments on what seem to be the
most important ones first (e.g. high-level feedback, correctness of algorithms).

If the code looks great, just leave 'Looks good to me' / 'Looks great to me' /
'LGTM' or any statement that is similar to this at the top of the code
as a comment. :)

Here are a few things that you can do:

- Mention whether you could easy follow the code or not (if not, say why).

- Understand the algorithm and leave counterexamples if exist.

- Check whether C++/LLVM libraries are correctly used.

- If you know C++/LLVM library that can be used to neaten the code, suggest it.

- If the code have a problem in performance, explain how it can be addressed.

- Comment about formatting / naming issues if you want. You should follow the
naming rule from LLVM / the formatting rules from clang-format.
