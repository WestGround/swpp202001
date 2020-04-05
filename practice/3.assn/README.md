### Assignment 3

To submit your code:

```
./package.sh
# Now, upload submit.tar.gz to ETL
```

#### 1. polygon.cpp

You are going to fix an algorithm that evaluates the area of a polygon.

The area of a polygon can be calculated by accumulating the result of cross
products of two adjacent points.
Let's assume that a polygon consists of vertices (x1, y1), (x2, y2), .., (xn, yn).
For example, (0, 0), (1, 0), (1, 1), (0, 1) will describe a square with lower-left
point located at (0, 0) and upper-right point located at (1, 1).

If the points are placed in counter clockwise order (반시계방향), the area is
equivalent to 0.5 * (cross((x1, y1), (x2, y2)) + cross((x2, y2), (x3, y3)) +
.. + cross((xn, yn), (x1, y1))).
Here, cross((xa, ya), (xb, yb)) is xa * yb - ya * xb.
If the points are in clockwise order, the result is negation of the summation.

For example, consider the square:

- cross((0,0), (1,0)) = 0
- cross((1,0), (1,1)) = 1
- cross((1,1), (0,1)) = 1
- cross((0,1), (0,0)) = 0

So adding all of those & multiplying 0.5 results in 1, which is exactly the area. :)

`polygon.cpp` will already have this algorithm, but it raises signed overflow in
certain inputs. Please fix it so

(1) UB Sanitizer does not raise failure

(2) It prints the correct area.

##### Input Conditions

Each coordinate (x, y) will fit in 32bit signed integer range
(-2147483648 ~ 2147483647).
The calculated area will fit in 64bit unsigned integer range (`uint64_t`).
It is guaranteed that

(1) The polygon is convex

(2) The calculated area will not have non-integral value (e.g. the area cannot be 0.5)

(3) *the points are given in counter clockwise order*.

##### How to check

```
./polygon.sh all <clang directory(e.g: ~/my-llvm-release/bin)>
# If you want to compile only..
./polygon.sh build <clang directory(e.g: ~/my-llvm-release/bin)>
# If you want to run only..
./polygon.sh run
```

##### Hints

- Understand what is the difference between `int`, `int64_t`, and `uint64_t`


#### 2. unreachable.cpp

Update `unreachable.cpp` so it enumerates unreachable blocks from the given IR.
For example,

```
define void @f(i1 %cond) {
entry:
  br i1 %cond, label %bb1, label %bb2
bb1:
  br label %bb2
bb2:
  ret void
bb_unreachable:
  ret void
}
```

The block `bb_unreachable` cannot be executed whatever `%cond` is.
Your pass should print the name of the blocks that are unreachable, per one line.
If there are more than one unreachable blocks, please print them in
lexicographical order (사전순).
It is assumed that names of basic blocks are unique, and all branch conditions
are given from a function argument (such as `%cond` in the above example).

NOTE: you should report blocks that are syntactically unreachable only.
For example,

```
entry:
if (flag) {
  if (!flag) {
  BB1: // logically unreachable, but syntactically reachable
  }
}
```

In this case, you should not report BB1, because there exists branch instructions from the entry to BB1 syntactically.


##### How to check

```
./unreachable.sh all <clang directory(e.g: ~/my-llvm-release/bin)>
# If you want to compile only..
./unreachable.sh build <clang directory(e.g: ~/my-llvm-release/bin)>
# If you want to run only..
./unreachable.sh run <clang directory(e.g: ~/my-llvm-release/bin)>
```

##### Setting VSCode

Setting VSCode to successfully find header files will greatly help doing your
assignment.

In order to do this, you will need to update your includePath
( https://stackoverflow.com/a/48015407 ).

If you're using master branch built with llvmscript, please add these two paths
to the includePath:

```
..(your path)../my-llvm-project/llvm/include
..(your path)../my-llvm-release/include
```

If you're using the prebuilt binary, please add this:

```
..(your path)../clang+llvm-9.0.0-x86_64-<...>/include
```
