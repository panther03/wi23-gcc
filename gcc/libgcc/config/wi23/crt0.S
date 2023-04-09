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
	# Initialize DATA by copying from program memory
	//slbi    r0,$h:_data_load_start
	lbi     r0,__data_load_start
	//slbi    r1,$h:_data_load_end
	lbi     r1,__data_load_end
	addi r2, r0, 0
	j     .dscopy
	.dsloop:
        # Copy PM[r2] to RAM[r2]
        ldcr    r3,r2,0
        st      r3,r2,0
        addi   r2,r2,4
	.dscopy:
		sltu r0,r2,r1
		bnez r0,.dsloop
	
	jal main;
	j _exit;

// Stop the processor once we've quit the program.
.globl	__exit
__exit:
_exit:
	halt