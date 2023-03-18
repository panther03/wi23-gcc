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
	.global	foo
	.type	foo, @function
j main
foo:
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
	add r1,r2,r1
	addi r9, r1, 0
	ld fp, sp, 0; ld ra, sp, 4; addi sp, sp, 8; jr ra;
	.size	foo, .-foo
	.p2align	2
	.global	main
	.type	main, @function
main:
	addi sp,sp,-12
	lbi  r1,1
	st r1, fp,-12
	lbi  r1,2
	st r1, fp,-8
	lbi  r1,3
	st r1, fp,-4
	addi r2,fp,-12
	lbi  r1,222
	addi r0, r2, 0
	jal 0; stu ra, sp, -4; stu fp, sp, -4; jal foo;
	addi r1, r9, 0
	addi r9, r1, 0
	ld fp, sp, 0; ld ra, sp, 4; addi sp, sp, 8; jr ra;
	halt
	.size	main, .-main
	.ident	"GCC: (GNU) 12.2.0"
