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
	.global	main
	.type	main, @function
main:
	lbi  r1,222
	lbi  r0,111
	jal 0
stu ra, sp, -4
stu fp, sp, -4
jal foo
	addi r1, r9, 0
	addi r9, r1, 0
	ld fp, sp, 0
ld ra, sp, 4
addi sp, sp, 8
jr ra
	.size	main, .-main
	.ident	"GCC: (GNU) 12.2.0"
