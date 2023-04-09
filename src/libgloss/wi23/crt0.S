// crt0.S -- startup file for wi23
// 
//
// The authors hereby grant permission to use, copy, modify, distribute,
// and license this software and its documentation for any purpose, provided
// that existing copyright notices are retained in all copies and that this
// notice is included verbatim in any distributions. No written agreement,
// license, or royalty fee is required for any of the authorized uses.
// Modifications to this software may be copyrighted by their authors
// and need not follow the licensing terms described here, provided that
// the new terms are clearly indicated on the first page of each file where
// they apply.
//

	.globl	__start
	.weak	_start
	.text
	.type	__start,@function
__start:
_start:
	lbi	sp, $h:_stack // set the top of stack
	slbi sp, _stack	
	xor	fp,fp,fp // zero fp to allow unwinders to stop

	// zero the bss area
	///ld	$r0, __bss_start__
	///xor	$r1, $r1
	///ldi.l	$r2, __bss_end__
	///sub	$r2, $r0
	///jsra	memset

	

	/* Call _init to invoke static constructors, etc.  */
	jal 0; stu ra, sp, -4; stu fp, sp, -4; jal _init;
	
	/* Call _fini at exit time for static destructors.  */
	// julien: or just dont
	//ldi.l	$r0, _fini
	//jal 0; stu ra, sp, -4; stu fp, sp, -4; jal atexit;
	
	// Set argc and argv.  These are populated by the simulator.  */
	//lda.l	$r0, 0x4
	//ldi.l	$r1, 0x8
	
	jal 0; stu ra, sp, -4; stu fp, sp, -4; jal main;
	jal 0; stu ra, sp, -4; stu fp, sp, -4; jal exit;

.Lend:
	.size	__start,(.Lend-__start)

// TODO no fucking clue if this makes sense
.globl	_exit
_exit:
	j _exit