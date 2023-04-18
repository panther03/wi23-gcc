planning to use this file to document issues that come up during development/testing which i just need to jot down

- (DONE) Haven't added the thing that copies instruction memory to data memory at startup yet (crti-hardware.S) for ft32
- (SHOULD BE DONE) Investigate what r0 is declared as - SHOULD NOT BE ZERO
- (NOPE) To indicate that calls clobber $ra you need to use a parallel condition
- (DONE) Should we make return and call simpler? Like corresponding to the actual instructions?
- (DONE) Subword insns
- (DONE) the current store absolute clobbers the store value and thus needs to be a define_expand
- (DONE) compiler needs to be aware of lbi sign extension
- Dedicated zero extend and sign extend insns?
- (DONE) Pick an ISA and re-do the stack using that ISA as the reference. 
- DMEM needs to be at least as large as IMEM, if IMEM is getting full - because the data is just copied to that address in DMEM. So possibly DMEM needs to go at the beginning in the linker script
- Code density improvement idea: R0 is always 0?
- (DONE) Fix wi23_initial_elimination_offset and wi23_regno_ok_for_base_p
- Does div.S need to have the size assembly directive? Removed it because assembler errors
- Look into why callee saved registers are being hit so hard instead of temporaries/arguments
- (DONE) FLD appears to be getting the wrong register in simulation
- halt at the end of main could be bad news?


elf32wi23.sh has the stack address