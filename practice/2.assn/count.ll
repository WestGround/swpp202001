define i32 @count_even(i32* %arr, i32 %N) {
   entry:
	br label %iter
   iter:
	%i = phi i32 [0, %entry], [%i1, %even], [%i2, %odd]
	%cnt = phi i32 [0, %entry], [%cnt1, %even], [%cnt, %odd]
	%cmp = icmp ult i32 %i, %N 
	br i1 %cmp, label %true, label %end
   true:
   	%ep = getelementptr inbounds i32, i32* %arr, i32 %i
	%elem = load i32, i32* %ep
   	%rem =  urem i32 %elem, 2
	%0 = icmp eq i32 %rem, 0
   	br i1 %0, label %even, label %odd
   even:
   	%i1 = add i32 1, %i
	%cnt1 = add i32 1, %cnt
        br label %iter
   odd:
        %i2 = add i32 1, %i
        br label %iter
   end:
        ret i32 %cnt
}
