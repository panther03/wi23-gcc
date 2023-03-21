	.file	"test.c"
	.option nopic
	.attribute arch, "rv64i2p0_m2p0_a2p0_f2p0_d2p0_c2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.comm	myint,4,4
	.comm	myshort,2,2
	.comm	mydouble,8,8
	.align	1
	.globl	test
	.type	test, @function
test:
	addi	sp,sp,-32
	sd	s0,24(sp)
	addi	s0,sp,32
	sw	zero,-20(s0)
	lw	a5,-20(s0)
	sext.w	a5,a5
	bnez	a5,.L2
	lw	a5,-20(s0)
	addiw	a5,a5,1
	sw	a5,-20(s0)
.L2:
	lw	a5,-20(s0)
	mv	a0,a5
	ld	s0,24(sp)
	addi	sp,sp,32
	jr	ra
	.size	test, .-test
	.align	1
	.globl	foo
	.type	foo, @function
foo:
	addi	sp,sp,-48
	sd	ra,40(sp)
	sd	s0,32(sp)
	sd	s1,24(sp)
	addi	s0,sp,48
	sd	a0,-40(s0)
	mv	a5,a1
	sw	a5,-44(s0)
	ld	a5,-40(s0)
	addi	a5,a5,4
	lw	a4,0(a5)
	ld	a5,-40(s0)
	addi	a5,a5,8
	lw	a5,0(a5)
	addw	a5,a4,a5
	sext.w	a4,a5
	ld	a5,-40(s0)
	addi	a5,a5,12
	lw	a5,0(a5)
	addw	a5,a4,a5
	sext.w	a5,a5
	lw	a4,-44(s0)
	addw	a5,a4,a5
	sext.w	s1,a5
	ld	a5,-40(s0)
	addi	a5,a5,12
	lw	a4,0(a5)
	lw	a5,-44(s0)
	subw	a5,a4,a5
	sext.w	a5,a5
	mv	a0,a5
	call	sumUpTo
	mv	a5,a0
	addw	a5,s1,a5
	sext.w	a5,a5
	mv	a0,a5
	ld	ra,40(sp)
	ld	s0,32(sp)
	ld	s1,24(sp)
	addi	sp,sp,48
	jr	ra
	.size	foo, .-foo
	.align	1
	.globl	main
	.type	main, @function
main:
	addi	sp,sp,-32
	sd	ra,24(sp)
	sd	s0,16(sp)
	addi	s0,sp,32
	li	a5,1
	sw	a5,-32(s0)
	li	a5,2
	sw	a5,-28(s0)
	li	a5,3
	sw	a5,-24(s0)
	addi	a5,s0,-32
	li	a1,222
	mv	a0,a5
	call	foo
	mv	a5,a0
	mv	a0,a5
	ld	ra,24(sp)
	ld	s0,16(sp)
	addi	sp,sp,32
	jr	ra
	.size	main, .-main
	.ident	"GCC: () 9.3.0"
