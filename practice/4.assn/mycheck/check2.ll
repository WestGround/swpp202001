; Write your own check here.
; Feel free to add arguments to @f, so its signature becomes @f(i32 %x, ...).
; But, this file should contain one function @f() only.
; FileCheck syntax: https://llvm.org/docs/CommandGuide/FileCheck.html

define i32 @f(i32 %x) {
; CHECK-LABEL:	define i32 @f(i32 %x)
; CHECK:	entry:
; CHECK-NEXT:	  br label [[LOOP:%.*]]
; CHECK:	loop:
; CHECK-NEXT:	  [[RES:%.*]] = phi i32 [ [[X:%.*]], [[ENTRY:%.*]] ], [ [[RESHALF:%.*]], [[LOOP]] ]
; CHECK-NEXT:	  [[REM:%.*]] = urem i32 [[RES]], 2
; CHECK-NEXT:	  [[TMP:%.*]] = sub i32 [[RES]], [[REM]]
; CHECK-NEXT:	  [[RESHALF]] = udiv i32 [[TMP]], 2
; CHECK-NEXT:	  [[COND:%.*]] = icmp eq i32 [[REM]], 0
; CHECK-NEXT:	  br i1 [[COND]], label [[LOOP]], label [[BBEXIT:%.*]]
; CHECK:	bb_exit:
; CHECK-NEXT:	  ret i32 [[RES]]
entry:
  br label %loop
loop:
  %res = phi i32 [%x, %entry], [%reshalf, %loop]
  %rem = urem i32 %res, 2
  %tmp = sub i32 %res, %rem
  %reshalf = udiv i32 %tmp, 2
  %cond = icmp eq i32 %rem, 0
  br i1 %cond, label %loop, label %bb_exit
bb_exit:
  ret i32 %res
}
