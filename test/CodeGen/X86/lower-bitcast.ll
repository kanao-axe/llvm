; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc < %s -mtriple=x86_64-unknown -mcpu=core2 -mattr=+sse2 | FileCheck %s
; RUN: llc < %s -mtriple=x86_64-unknown -mcpu=core2 -mattr=+sse2 -x86-experimental-vector-widening-legalization | FileCheck %s --check-prefix=CHECK-WIDE

; FIXME: Ideally we should be able to fold the entire body of @test1 into a
; single paddd instruction. At the moment we produce the sequence
; pshufd+paddq+pshufd. This is fixed with the widening legalization.

define double @test1(double %A) {
; CHECK-LABEL: test1:
; CHECK:       # %bb.0:
; CHECK-NEXT:    shufps {{.*#+}} xmm0 = xmm0[0,1,1,3]
; CHECK-NEXT:    paddd {{.*}}(%rip), %xmm0
; CHECK-NEXT:    pshufd {{.*#+}} xmm0 = xmm0[0,2,2,3]
; CHECK-NEXT:    retq
;
; CHECK-WIDE-LABEL: test1:
; CHECK-WIDE:       # %bb.0:
; CHECK-WIDE-NEXT:    paddd {{.*}}(%rip), %xmm0
; CHECK-WIDE-NEXT:    retq
  %1 = bitcast double %A to <2 x i32>
  %add = add <2 x i32> %1, <i32 3, i32 5>
  %2 = bitcast <2 x i32> %add to double
  ret double %2
}

define double @test2(double %A, double %B) {
; CHECK-LABEL: test2:
; CHECK:       # %bb.0:
; CHECK-NEXT:    paddd %xmm1, %xmm0
; CHECK-NEXT:    retq
;
; CHECK-WIDE-LABEL: test2:
; CHECK-WIDE:       # %bb.0:
; CHECK-WIDE-NEXT:    paddd %xmm1, %xmm0
; CHECK-WIDE-NEXT:    retq
  %1 = bitcast double %A to <2 x i32>
  %2 = bitcast double %B to <2 x i32>
  %add = add <2 x i32> %1, %2
  %3 = bitcast <2 x i32> %add to double
  ret double %3
}

define i64 @test3(i64 %A) {
; CHECK-LABEL: test3:
; CHECK:       # %bb.0:
; CHECK-NEXT:    movq %rdi, %xmm0
; CHECK-NEXT:    addps {{.*}}(%rip), %xmm0
; CHECK-NEXT:    movq %xmm0, %rax
; CHECK-NEXT:    retq
;
; CHECK-WIDE-LABEL: test3:
; CHECK-WIDE:       # %bb.0:
; CHECK-WIDE-NEXT:    movq %rdi, %xmm0
; CHECK-WIDE-NEXT:    addps {{.*}}(%rip), %xmm0
; CHECK-WIDE-NEXT:    movq %xmm0, %rax
; CHECK-WIDE-NEXT:    retq
  %1 = bitcast i64 %A to <2 x float>
  %add = fadd <2 x float> %1, <float 3.0, float 5.0>
  %2 = bitcast <2 x float> %add to i64
  ret i64 %2
}

; FIXME: Ideally we should be able to fold the entire body of @test4 into a
; single paddd instruction. This is fixed with the widening legalization.

define i64 @test4(i64 %A) {
; CHECK-LABEL: test4:
; CHECK:       # %bb.0:
; CHECK-NEXT:    movq %rdi, %xmm0
; CHECK-NEXT:    pshufd {{.*#+}} xmm0 = xmm0[0,1,1,3]
; CHECK-NEXT:    paddd {{.*}}(%rip), %xmm0
; CHECK-NEXT:    pshufd {{.*#+}} xmm0 = xmm0[0,2,2,3]
; CHECK-NEXT:    movq %xmm0, %rax
; CHECK-NEXT:    retq
;
; CHECK-WIDE-LABEL: test4:
; CHECK-WIDE:       # %bb.0:
; CHECK-WIDE-NEXT:    movq %rdi, %xmm0
; CHECK-WIDE-NEXT:    paddd {{.*}}(%rip), %xmm0
; CHECK-WIDE-NEXT:    movq %xmm0, %rax
; CHECK-WIDE-NEXT:    retq
  %1 = bitcast i64 %A to <2 x i32>
  %add = add <2 x i32> %1, <i32 3, i32 5>
  %2 = bitcast <2 x i32> %add to i64
  ret i64 %2
}

define double @test5(double %A) {
; CHECK-LABEL: test5:
; CHECK:       # %bb.0:
; CHECK-NEXT:    addps {{.*}}(%rip), %xmm0
; CHECK-NEXT:    retq
;
; CHECK-WIDE-LABEL: test5:
; CHECK-WIDE:       # %bb.0:
; CHECK-WIDE-NEXT:    addps {{.*}}(%rip), %xmm0
; CHECK-WIDE-NEXT:    retq
  %1 = bitcast double %A to <2 x float>
  %add = fadd <2 x float> %1, <float 3.0, float 5.0>
  %2 = bitcast <2 x float> %add to double
  ret double %2
}

; FIXME: Ideally we should be able to fold the entire body of @test6 into a
; single paddw instruction. This is fixed with the widening legalization.

define double @test6(double %A) {
; CHECK-LABEL: test6:
; CHECK:       # %bb.0:
; CHECK-NEXT:    punpcklwd {{.*#+}} xmm0 = xmm0[0,0,1,1,2,2,3,3]
; CHECK-NEXT:    paddw {{.*}}(%rip), %xmm0
; CHECK-NEXT:    pshufb {{.*#+}} xmm0 = xmm0[0,1,4,5,8,9,12,13,8,9,12,13,12,13,14,15]
; CHECK-NEXT:    retq
;
; CHECK-WIDE-LABEL: test6:
; CHECK-WIDE:       # %bb.0:
; CHECK-WIDE-NEXT:    paddw {{.*}}(%rip), %xmm0
; CHECK-WIDE-NEXT:    retq
  %1 = bitcast double %A to <4 x i16>
  %add = add <4 x i16> %1, <i16 3, i16 4, i16 5, i16 6>
  %2 = bitcast <4 x i16> %add to double
  ret double %2
}

define double @test7(double %A, double %B) {
; CHECK-LABEL: test7:
; CHECK:       # %bb.0:
; CHECK-NEXT:    paddw %xmm1, %xmm0
; CHECK-NEXT:    retq
;
; CHECK-WIDE-LABEL: test7:
; CHECK-WIDE:       # %bb.0:
; CHECK-WIDE-NEXT:    paddw %xmm1, %xmm0
; CHECK-WIDE-NEXT:    retq
  %1 = bitcast double %A to <4 x i16>
  %2 = bitcast double %B to <4 x i16>
  %add = add <4 x i16> %1, %2
  %3 = bitcast <4 x i16> %add to double
  ret double %3
}

; FIXME: Ideally we should be able to fold the entire body of @test8 into a
; single paddb instruction. At the moment we produce the sequence
; pshufd+paddw+pshufd. This is fixed with the widening legalization.

define double @test8(double %A) {
; CHECK-LABEL: test8:
; CHECK:       # %bb.0:
; CHECK-NEXT:    punpcklbw {{.*#+}} xmm0 = xmm0[0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7]
; CHECK-NEXT:    paddb {{.*}}(%rip), %xmm0
; CHECK-NEXT:    pshufb {{.*#+}} xmm0 = xmm0[0,2,4,6,8,10,12,14,u,u,u,u,u,u,u,u]
; CHECK-NEXT:    retq
;
; CHECK-WIDE-LABEL: test8:
; CHECK-WIDE:       # %bb.0:
; CHECK-WIDE-NEXT:    paddb {{.*}}(%rip), %xmm0
; CHECK-WIDE-NEXT:    retq
  %1 = bitcast double %A to <8 x i8>
  %add = add <8 x i8> %1, <i8 3, i8 4, i8 5, i8 6, i8 7, i8 8, i8 9, i8 10>
  %2 = bitcast <8 x i8> %add to double
  ret double %2
}

define double @test9(double %A, double %B) {
; CHECK-LABEL: test9:
; CHECK:       # %bb.0:
; CHECK-NEXT:    paddb %xmm1, %xmm0
; CHECK-NEXT:    retq
;
; CHECK-WIDE-LABEL: test9:
; CHECK-WIDE:       # %bb.0:
; CHECK-WIDE-NEXT:    paddb %xmm1, %xmm0
; CHECK-WIDE-NEXT:    retq
  %1 = bitcast double %A to <8 x i8>
  %2 = bitcast double %B to <8 x i8>
  %add = add <8 x i8> %1, %2
  %3 = bitcast <8 x i8> %add to double
  ret double %3
}
