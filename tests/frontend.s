	.file	"frontend.c"
	.text
	.p2align	2
	.global	__mulsi3
	.type	__mulsi3, @function
__mulsi3:
	ld fp, sp, 0; ld ra, sp, 4; addi sp, sp, 8; jr ra;	// 10	[c=0]  returner
	.size	__mulsi3, .-__mulsi3
	.p2align	2
	.global	poll_kb
	.type	poll_kb, @function
poll_kb:
	lbi r1,$h:PS2_KEY_AWAIT; slbi r1,PS2_KEY_AWAIT; ld r1, r1, 0;	// 6	[c=8]  *movsi/5
	ld r9, r1,0	// 7	[c=4]  *movsi/3
	beqz	r9,.L3	// 8	[c=20]  *branchsi
	addi r2, r0, 0	// 32	[c=4]  *movsi/1
	st r2, r1,0	// 11	[c=4]  *movsi/2
	lbi r1,$h:PS2_KEY; slbi r1,PS2_KEY; ld r1, r1, 0;	// 12	[c=8]  *movsi/5
	ld r9, r1,0	// 13	[c=4]  *movsi/3
.L3:
	ld fp, sp, 0; ld ra, sp, 4; addi sp, sp, 8; jr ra;	// 35	[c=0]  returner
	.size	poll_kb, .-poll_kb
	.p2align	2
	.global	vga_print
	.type	vga_print, @function
vga_print:
	srli r4,r1,4	// 9	[c=8]  ashlsi3/0
	srli r1,r1,6	// 10	[c=8]  ashlsi3/0
	add r1,r4,r1	// 11	[c=4]  addsi3/1
	add r1,r1,r0	// 12	[c=4]  addsi3/1
	srli r1,r1,2	// 13	[c=8]  ashlsi3/0
	lbi r4,$h:VGA_TEXT_BUFFER; slbi r4,VGA_TEXT_BUFFER; ld r4, r4, 0;	// 14	[c=8]  *movsi/5
	add r4,r4,r1	// 15	[c=4]  addsi3/1
	ld r5, r3,0	// 64	[c=4]  *movqi/3
	srli r1,r5,24	// 17	[c=8]  ashlsi3/0
	beqz	r1,.L8	// 19	[c=20]  *branchsi
	addi r3,r3,2	// 23	[c=8]  addsi3/0
	addi r5,r4,4	// 24	[c=8]  addsi3/0
.L10:
	srli r1,r1,24	// 27	[c=8]  lshrsi3/0
	st r1, r5,0	// 28	[c=4]  *movsi/2
	ld r1, r2,0	// 29	[c=4]  *movsi/3
	st r1, r4,0	// 30	[c=4]  *movsi/2
	ld r1, r3,0	// 31	[c=4]  *movqi/3
	addi r4,r4,8	// 32	[c=8]  addsi3/0
	addi r2,r2,8	// 33	[c=8]  addsi3/0
	addi r3,r3,2	// 34	[c=8]  addsi3/0
	addi r5,r5,8	// 35	[c=8]  addsi3/0
	srli r1,r1,24	// 37	[c=8]  ashlsi3/0
	bnez	r1,.L10	// 39	[c=20]  *branchsi
.L8:
	ld fp, sp, 0; ld ra, sp, 4; addi sp, sp, 8; jr ra;	// 67	[c=0]  returner
	.size	vga_print, .-vga_print
	.p2align	2
	.global	vga_print_plain
	.type	vga_print_plain, @function
vga_print_plain:
	srli r3,r1,4	// 8	[c=8]  ashlsi3/0
	srli r1,r1,6	// 9	[c=8]  ashlsi3/0
	add r1,r3,r1	// 10	[c=4]  addsi3/1
	add r1,r1,r0	// 11	[c=4]  addsi3/1
	srli r1,r1,2	// 12	[c=8]  ashlsi3/0
	lbi r3,$h:VGA_TEXT_BUFFER; slbi r3,VGA_TEXT_BUFFER; ld r3, r3, 0;	// 13	[c=8]  *movsi/5
	add r3,r3,r1	// 14	[c=4]  addsi3/1
	ld r4, r2,0	// 59	[c=4]  *movqi/3
	srli r1,r4,24	// 16	[c=8]  ashlsi3/0
	beqz	r1,.L17	// 18	[c=20]  *branchsi
	addi r2,r2,2	// 21	[c=8]  addsi3/0
	addi r4,r3,4	// 22	[c=8]  addsi3/0
	lbi  r5,15	// 27	[c=4]  *movsi/6
.L19:
	srli r1,r1,24	// 25	[c=8]  lshrsi3/0
	st r1, r4,0	// 26	[c=4]  *movsi/2
	st r5, r3,0	// 28	[c=4]  *movsi/2
	ld r1, r2,0	// 29	[c=4]  *movqi/3
	addi r3,r3,8	// 30	[c=8]  addsi3/0
	addi r2,r2,2	// 31	[c=8]  addsi3/0
	addi r4,r4,8	// 32	[c=8]  addsi3/0
	srli r1,r1,24	// 34	[c=8]  ashlsi3/0
	bnez	r1,.L19	// 36	[c=20]  *branchsi
.L17:
	ld fp, sp, 0; ld ra, sp, 4; addi sp, sp, 8; jr ra;	// 62	[c=0]  returner
	.size	vga_print_plain, .-vga_print_plain
	.section	.rodata.str1.4,"aMS",@progbits,1
	.p2align	2
.LC0:
	.string	"Hello World!"
	.p2align	2
.LC1:
	.string	"SPACEBAR Pressed!"
	.section	.text.startup,"ax",@progbits
	.p2align	2
	.global	main
	.type	main, @function
main:
	slbi  r2,45240	// 7	[c=4]  *movsi/6
	lbi  r2,5791
	addi r1, r0, 0	// 8	[c=4]  *movsi/1
	addi r0, r1, 0	// 9	[c=4]  *movsi/0
	jal 0; stu ra, sp, -4; stu fp, sp, -4; jal vga_print_plain;	// 10	[c=0]  *call/0
	lbi r6,$h:PS2_KEY_AWAIT; slbi r6,PS2_KEY_AWAIT; ld r6, r6, 0;	// 11	[c=8]  *movsi/5
	lbi r7,$h:VGA_TEXT_BUFFER; slbi r7,VGA_TEXT_BUFFER; ld r7, r7, 0;	// 12	[c=8]  *movsi/5
	addi r9,r7,320	// 13	[c=8]  addsi3/0
	addi r7,r7,324	// 53	[c=8]  addsi3/0
	addi r10, r0, 0	// 18	[c=4]  *movsi/1
	slbi  r8,0	// 54	[c=4]  *movsi/6
	lbi  r8,21248
	lbi  r5,15	// 29	[c=4]  *movsi/6
.L29:
	ld r1, r6,0	// 15	[c=4]  *movsi/3
	beqz	r1,.L27	// 16	[c=20]  *branchsi
	st r10, r6,0	// 19	[c=4]  *movsi/2
.L27:
	addi r4, r9, 0	// 22	[c=4]  *movsi/0
	slbi  r3,20552	// 49	[c=16]  *movsi/6
	lbi  r3,5794
	addi r2, r7, 0	// 50	[c=4]  *movsi/0
	addi r1, r8, 0	// 61	[c=4]  *movsi/0
.L28:
	srli r1,r1,24	// 27	[c=8]  lshrsi3/0
	st r1, r2,0	// 28	[c=4]  *movsi/2
	st r5, r4,0	// 30	[c=4]  *movsi/2
	ld r1, r3,0	// 31	[c=4]  *movqi/3
	addi r4,r4,8	// 32	[c=8]  addsi3/0
	addi r3,r3,2	// 33	[c=8]  addsi3/0
	addi r2,r2,8	// 34	[c=8]  addsi3/0
	srli r1,r1,24	// 36	[c=8]  ashlsi3/0
	beqz	r1,.L29	// 38	[c=20]  *branchsi
	srli r1,r1,24	// 78	[c=8]  lshrsi3/0
	st r1, r2,0	// 79	[c=4]  *movsi/2
	st r5, r4,0	// 80	[c=4]  *movsi/2
	ld r1, r3,0	// 81	[c=4]  *movqi/3
	addi r4,r4,8	// 82	[c=8]  addsi3/0
	addi r3,r3,2	// 83	[c=8]  addsi3/0
	addi r2,r2,8	// 84	[c=8]  addsi3/0
	srli r1,r1,24	// 85	[c=8]  ashlsi3/0
	bnez	r1,.L28	// 86	[c=20]  *branchsi
	j .L29		// 90	[c=4]  jump
	.size	main, .-main
	.global	PS2_KEY_AWAIT
	.data
	.p2align	2
	.type	PS2_KEY_AWAIT, @object
	.size	PS2_KEY_AWAIT, 4
PS2_KEY_AWAIT:
	.long	45060
	.global	PS2_KEY
	.p2align	2
	.type	PS2_KEY, @object
	.size	PS2_KEY, 4
PS2_KEY:
	.long	45056
	.global	VGA_TEXT_BUFFER
	.section	.bss,"aw",@nobits
	.p2align	2
	.type	VGA_TEXT_BUFFER, @object
	.size	VGA_TEXT_BUFFER, 4
VGA_TEXT_BUFFER:
	.zero	4
	.ident	"GCC: (GNU) 12.2.0"
