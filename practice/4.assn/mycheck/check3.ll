; Write your own check here.
; Feel free to add arguments to @f, so its signature becomes @f(i32 %x, ...).
; But, this file should contain one function @f() only.
; FileCheck syntax: https://llvm.org/docs/CommandGuide/FileCheck.html

define i32 @f(i32 %a, i32 %b, i32 %c, i32 %d) {
; CHECK-LABEL:	define i32 @f(i32 %a, i32 %b, i32 %c, i32 %d)
; CHECK-NEXT:	  [[COND1:%.*]] = icmp eq i32 [[A:%.*]], [[B:%.*]]
; CHECK-NEXT:	  br i1 [[COND1]], label [[BB1:%.*]], label [[BBEXIT1:%.*]]
; CHECK:	bb1:
; CHECK-NEXT:	  [[AA:%.*]] = add i32 1, [[A]]
; CHECK-NEXT:	  [[BB:%.*]] = add i32 1, [[A]]
; CHECK-NEXT:	  [[COND2:%.*]] = icmp eq i32 [[C:%.*]], [[D:%.*]]
; CHECK-NEXT:	br i1 [[COND2]], label [[BB2:%.*]], label [[BBEXIT2:%.*]]
; CHECK:	  bb2:
; CHECK-NEXT:	  [[CC:%.*]] = add i32 [[C]], [[A]]
; CHECK-NEXT:	  [[COND3:%.*]] = icmp eq i32 [[C]], [[CC]]
; CHECK-NEXT:	  br i1 [[COND3]], label [[BB3:%.*]], label [[BBEXIT2]]
; CHECK:	bb3:
; CHECK-NEXT:	  [[COND4:%.*]] = icmp eq i32 [[C]], [[A]]
; CHECK-NEXT:	  br i1 [[COND4]], label [[BBEXIT1]], label [[BBEXIT2]]
; CHECK:	bb_exit1:
; CHECK-NEXT:	  [[T1:%.*]] = add i32 [[A]], [[B]]
; CHECK-NEXT:	  [[T2:%.*]] = add i32 [[T1]], [[C]]
; CHECK-NEXT:	  [[T3:%.*]] = add i32 [[T2]], [[D]]
; CHECK-NEXT:	  ret i32 [[T3]]
; CHECK:	bb_exit2:
; CHECK-NEXT:	  [[T4:%.*]] = add i32 [[A]], [[A]]
; CHECK-NEXT:	  [[T5:%.*]] = add i32 [[T4]], [[C]]
; CHECK-NEXT:	  [[T6:%.*]] = add i32 [[T5]], [[D]]
; CHECK-NEXT:	  ret i32 [[T6]]
;
  %cond1 = icmp eq i32 %a, %b
  br i1 %cond1, label %bb1, label %bb_exit1
bb1:
  %aa = add i32 1, %a
  %bb = add i32 1, %b
  %cond2 = icmp eq i32 %c, %d
  br i1 %cond2, label %bb2, label %bb_exit2
bb2:
  %cc = add i32 %c, %b
  %cond3 = icmp eq i32 %d, %cc 
  br i1 %cond3, label %bb3, label %bb_exit2
bb3:
  %cond4 = icmp eq i32 %cc, %b
  br i1 %cond4, label %bb_exit1, label %bb_exit2
bb_exit1:
  %tmp1 = add i32 %a, %b
  %tmp2 = add i32 %tmp1, %c
  %tmp3 = add i32 %tmp2, %d
  ret i32 %tmp3
bb_exit2:
  %tmp4 = add i32 %a, %b
  %tmp5 = add i32 %tmp4, %c
  %tmp6 = add i32 %tmp5, %d
  ret i32 %tmp6
}
