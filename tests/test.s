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
	.ident	"GCC: (GNU) 12.2.0"
