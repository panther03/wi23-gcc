	.file	"test.c"
	.comm	myint,4,4
	.comm	myshort,2,2
	.comm	mydouble,8,4
	.section .text
	.align	4
.proc	foo
	.global foo
	.type	foo, @function
foo:
.LFB0:
	.cfi_startproc
	l.sw    	-4(r1),r2	 # SI store
	.cfi_offset 2, -4
	l.addi  	r2,r1,0 # addsi3
	.cfi_def_cfa_register 2
	l.addi	r1,r1,-12	# allocate frame
	l.sw    	-8(r2),r3	 # SI store
	l.sw    	-12(r2),r4	 # SI store
	l.lwz   	r3,-8(r2)	 # SI load
	l.addi  	r3,r3,4 # addsi3
	l.lwz   	r4,0(r3)	 # SI load
	l.lwz   	r3,-8(r2)	 # SI load
	l.addi  	r3,r3,8 # addsi3
	l.lwz   	r3,0(r3)	 # SI load
	l.add   	r4,r4,r3 # addsi3
	l.lwz   	r3,-8(r2)	 # SI load
	l.addi  	r3,r3,12 # addsi3
	l.lwz   	r3,0(r3)	 # SI load
	l.add   	r4,r4,r3 # addsi3
	l.lwz   	r3,-12(r2)	 # SI load
	l.add   	r3,r4,r3 # addsi3
	l.ori   	r11,r3,0	 # move reg to reg
	l.ori	r1,r2,0	# deallocate frame
	l.lwz   	r2,-4(r1)	 # SI load
	l.jr    	r9	# return_internal
	l.nop			# nop delay slot
	.cfi_endproc
.LFE0:
	.size	foo, .-foo
	.align	4
.proc	main
	.global main
	.type	main, @function
main:
.LFB1:
	.cfi_startproc
	l.sw    	-8(r1),r2	 # SI store
	.cfi_offset 2, -8
	l.addi  	r2,r1,0 # addsi3
	.cfi_def_cfa_register 2
	l.sw    	-4(r1),r9	 # SI store
	l.addi	r1,r1,-20	# allocate frame
	.cfi_offset 9, -4
	l.addi  	r3,r0,1	 # move immediate I
	l.sw    	-20(r2),r3	 # SI store
	l.addi  	r3,r0,2	 # move immediate I
	l.sw    	-16(r2),r3	 # SI store
	l.addi  	r3,r0,3	 # move immediate I
	l.sw    	-12(r2),r3	 # SI store
	l.addi  	r3,r2,-20 # addsi3
	l.addi  	r4,r0,222	 # move immediate I
	l.jal   	foo # call_value_internal
	l.nop			# nop delay slot
	l.ori   	r3,r11,0	 # move reg to reg
	l.ori   	r11,r3,0	 # move reg to reg
	l.ori	r1,r2,0	# deallocate frame
	l.lwz   	r2,-8(r1)	 # SI load
	l.lwz   	r9,-4(r1)	 # SI load
	l.jr    	r9	# return_internal
	l.nop			# nop delay slot
	.cfi_endproc
.LFE1:
	.size	main, .-main
	.ident	"GCC: (GNU) 5.2.0"
