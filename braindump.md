planning to use this file to document issues that come up during development/testing which i just need to jot down

- Haven't added the thing that copies instruction memory to data memory at startup yet (crti-hardware.S) for ft32
- (SHOULD BE DONE) Investigate what r0 is declared as - SHOULD NOT BE ZERO
- (NOPE) To indicate that calls clobber $ra you need to use a parallel condition
- (DONE) Should we make return and call simpler? Like corresponding to the actual instructions?
- (DONE) Subword insns
- (DONE) the current store absolute clobbers the store value and thus needs to be a define_expand
- (DONE) compiler needs to be aware of lbi sign extension
- Dedicated zero extend and sign extend insns?
- Pick an ISA and re-do the stack using that ISA as the reference.

elf32wi23.sh has the stack address