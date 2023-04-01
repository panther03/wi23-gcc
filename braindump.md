planning to use this file to document issues that come up during development/testing which i just need to jot down

- Haven't added the thing that copies instruction memory to data memory at startup yet (crti-hardware.S) for ft32
- Investigate what r0 is declared as - SHOULD NOT BE ZERO
- To indicate that calls clobber $ra you need to use a parallel condition
- Should we make return and call simpler? Like corresponding to the actual instructions?