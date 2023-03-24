	.file	"test.c"
	.text
	.global	myint
	.section	.bss,"aw",@nobits
	.p2align	2
	.type	myint, @object
	.size	myint, 4
myint:
	.zero	4
	.global	myshort
	.p2align	1
	.type	myshort, @object
	.size	myshort, 2
myshort:
	.zero	2
	.global	mydouble
	.p2align	2
	.type	mydouble, @object
	.size	mydouble, 8
mydouble:
	.zero	8
	.text
	.p2align	2
	.global	sumUpTo
	.type	sumUpTo, @function
sumUpTo:
	addi sp,sp,-12
	st r0, fp,-12
	addi r1, r0, 0
	st r1, fp,-4
	addi r1, r0, 0
	st r1, fp,-8
	j .L2
.L3:
	ld r2, fp,-8
	ld r1, fp,-4
	add r1,r2,r1
	st r1, fp,-8
	ld r1, fp,-4
	addi r1,r1,1
	st r1, fp,-4
.L2:
	ld r2, fp,-4
	ld r1, fp,-12
	slt r1,r2,r1
	beqz	r1,.L3
	ld r1, fp,-4
	addi r9, r1, 0
	ld fp, sp, 0; ld ra, sp, 4; addi sp, sp, 8; jr ra;
	.size	sumUpTo, .-sumUpTo
	.p2align	2
	.global	foo
	.type	foo, @function
main:
	lbi r1, %h:foo
	slbi r2, foo
	addi sp,sp,-16
	lbi  r1,1
	st r1, fp,-16
	lbi  r1,2
	st r1, fp,-12
	lbi  r1,3
	st r1, fp,-8
	lbi  r1,4
	st r1, fp,-4
	addi r2,fp,-16
	lbi  r1,222
	addi r0, r2, 0
	jal 0; stu ra, sp, -4; stu fp, sp, -4; jal foo;
	addi r2, r9, 0
	ld r1, fp,-16
	add r2,r2,r1
	ld r1, fp,-4
	andn r1,r2,r1
	addi r9, r1, 0
	ld fp, sp, 0; ld ra, sp, 4; addi sp, sp, 8; jr ra;
	.size	main, .-main
	.ident	"GCC: (GNU) 12.2.0"
.org 0x0001ffffU
foo:
	stu r20, sp, -4
	addi sp,sp,-8
	st r0, fp,-4
	st r1, fp,-8
	ld r1, fp,-4
	addi r1,r1,4
	ld r2, r1,0
	ld r1, fp,-4
	addi r1,r1,8
	ld r1, r1,0
	add r2,r2,r1
	ld r1, fp,-4
	addi r1,r1,12
	ld r1, r1,0
	add r2,r2,r1
	ld r1, fp,-8
	add r20,r2,r1
	ld r1, fp,-4
	addi r1,r1,12
	ld r2, r1,0
	ld r1, fp,-8
	sub r1,r1,r2
	addi r0, r1, 0
	jal 0; stu ra, sp, -4; stu fp, sp, -4; jal sumUpTo;
	addi r1, r9, 0
	add r1,r20,r1
	addi r9, r1, 0
	addi sp,sp,8
	ld r20, sp, 0; addi sp, sp, 4;
	ld fp, sp, 0; ld ra, sp, 4; addi sp, sp, 8; jr ra;
	.size	foo, .-foo
	.p2align	2
	.global	main
	.type	main, @function