define void @f(i1 %cond, i1 %cond2) {
entry:
  br i1 %cond, label %bb1, label %bb3
bb2:
  br label %bbexit
bb1:
  br i1 %cond2, label %bb2, label %bbexit
bb3:
  br label %bbexit
bbexit:
  ret void
}