; Write your own check here.
; Feel free to add arguments to @f, so its signature becomes @f(i32 %x, ...).
; But, this file should contain one function @f() only.
; FileCheck syntax: https://llvm.org/docs/CommandGuide/FileCheck.html

define i32 @f(i32 %x) {
; CHECK-LABEL:	define i32 @f(i32 %x)
; CHECK-NEXT:	  [[REM:%.*]] = urem i32 [[X:%.*]], 10
; CHECK-NEXT:	  [[COND:%.*]] = icmp eq i32 [[REM]], 0
; CHECK-NEXT:	  br i1 [[COND]], label [[BB_EXIT1:%.*]], label [[BB_TRUNC:%.*]]
; CHECK:	bb_trunc:
; CHECK-NEXT:	  [[Y:%.*]] = sub i32 [[X]], [[REM]]
; CHECK-NEXT:	  br label [[BB_EXIT2:%.*]]
; CHECK:	bb_exit1:
; CHECK-NEXT:	  ret i32 [[X]]
; CHECK:	bb_exit2:
; CHECK-NEXT:	  ret i32 [[Y]]
;
  %rem = urem i32 %x, 10
  %cond = icmp eq i32 %rem, 0
  br i1 %cond, label %bb_exit1, label %bb_trunc
bb_trunc:
  %y = sub i32 %x, %rem
  br label %bb_exit2
bb_exit1:
  ret i32 %x
bb_exit2:
  ret i32 %y
}
