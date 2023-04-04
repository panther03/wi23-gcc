.globl	__start
	.weak	_start
	.text
	.type	__start,@function
__start:
_start:
	lbi	sp, $h:_stack // set the top of stack
	slbi sp, _stack	
	addi fp, sp, 0 // copy frame pointer to stack pointer.

	// TODO: necessary to zero out BSS?
    // Will be zeroed in FPGA, not necessarily in simulator?

    // TODO: copy initialized data over to data memory here
	
	jal main;
	j _exit;

// Stop the processor once we've quit the program.
.globl	__exit
__exit:
_exit:
	halt