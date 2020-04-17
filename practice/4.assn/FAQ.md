### FAQs for Assignment 4

Q. When shouldn't the code be optimized?

A.

(1) If `icmp ne x y` is false, x == y should not be propagated to the false branch.

(2) If it is comparing pointers, the equality should not be used.

(3) Even if all predecessors are having equivalent conditions, it shouldn't be optimized.

ex)
```
bb1:
  br (icmp eq x, y), bb_here, bb_else
bb2:
  br (icmp eq x, y), bb_here, bb_else
bb_here
  // In this block, x == y always holds. However, finding this is not trivial,
  // so don't optimize in assn 4.
```


Q. printdom.cpp is not printing `Edge (bb1, bb2) dominates bb2!`. What is the reason?

A. It is simply because printdom.cpp is written to print dominance relation between
(entry, bbN) and bbM, not for all possible edges.


Q. What is the definition of dominance relation between instructions?

A. Imagine an control flow graph with each instruction becoming 
a node, and an edge defined between two nodes if the latter instruction is
executed after the former one is executed.
Then, dominance relation can be defined on the control flow graph.


Q. What is a constant expression?

A. It is a constant which is not simply 0, 1, .. but an expression.
For example, https://godbolt.org/z/wiNY44 .
