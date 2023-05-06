# ECE 554 Spring 2023 "WI-23" Toolchain

This is the repo for the GNU-based toolchain port for the WI-23 ISA in our ECE 554 group project. 

# Repo structure

## High-level

* `gcc/` contains the gcc source tree. For our interests, this includes the C compiler backend itself, and the implementation of libgcc.

* `src/` contains the combined binutils-gdb source tree. This is essentially "everything else" in the toolchain. The linker, assembler, and simulator are all in this repository.

* `tests/` contain some C unit tests cases that we used in the process of testing the compiler/simulator etc. These are different than the unit test cases for the processor but some are shared.

There are some other files that we used just for running the appropriate build commands for the toolchain, and some documentation I used while developing it instead of flooding JIRA tickets.
These aren't actually needed for the toolchain itself though.

## Compiler files

The main directory of interest is in `gcc/gcc/conifg/wi23` (from the root of this repo). This is where the backend files for the WI-23 ISA in GCC live. In particular, the following files in this directory play important roles:

* `wi23.md`: This is a file writen in GCC's custom machine description language that looks like a Lisp dialect. At a high-level, these files basically have rules that describe "patterns" (`define_insn`) that GCC matches instructions in the intermediate representation to. Based on the pattern matched in the rule, a specific assembly instruction is printed. For instance, take this rule we have for floating-point adds:
```
(define_insn "addsf3"
  [(set (match_operand:SF 0 "register_operand" "=f")
          (plus:SF
           (match_operand:SF 1 "register_operand" "f")
           (match_operand:SF 2 "register_operand" "f")))]
  ""
  "fadd %0, %1, %2")
```
This tells GCC "given an instruction that adds two operands in floating-point registers (f) with a float type (SF), and writes it to a floating-point class register (=f), generate this `fadd` assembly line and use the names of those regsiters in the following order."

There are also `define_expand`s, which can transform the incoming stream of RTL instructions so that they *do* match an existing pattern that can be handled by a `define_insn`.

* `wi23.cc`: This C++ file handles a variety of different tasks in the backend, so it is hard to encapsulate. The important things it describes are how to generate code for the function prologues/epilogues, how to pass arguments in functions, and how to index those arguments relative to the stack frame.

* `wi23.h`: This header file defines some important fundamental properties of the ISA/calling convention, like what is the word size, how many registers are there, which of the registers are reserved and which can be safely overridden in a function call, etc.

If you are curious to learn more, [GCC's internals documentation](https://gcc.gnu.org/onlinedocs/gccint/Machine-Desc.html) is the best resource. 

## Remaining toolchain

This will just call out a list of various files in the `src/` directory that are of interest.

* `include/opcode/wi23.h`: A header file that is reused by many of the different tools to parse the instruction formats of the ISA.
* `opcodes/wi23-opc.c`: The description of the instructions and their formats in the ISA based on the data defined in the header `wi23.h`.
* `opcodes/wi23-dis.c`: A "disassembler" that allows you to see a disassembled output of the binary instruction stream from a file. This allows you to run tools like `objdump` on an ELF executable and see some form of assembly decompilation for it.
* `gas/config/tc-wi23.c`: The main assembler file. It uses the GNU assembler as essentially a library, so it does the main work of parsing the assembly file text stream and generating the appropriate instruction words, but the handling of generating the executable output (ELF) is part of GNU BFD, and handling parsing complicated assembler macros like constant expressions which the assembler supports is part of the assembler library.
* `sim/wi23/interp.c`: The simulator source.
* `bfd/elf32-wi23.c`: A description of the ELF executable format for WI-23. This includes info about the specific types of relocations mainly. These are used to describe whenever you have say function calls or global variables that are defined outside of a given file (so you have a relocatable object file.)

Note the absence of a linker source file. The linker is essentially achieved "for free" once the BFD backend for your given ISA is implemented. It's mostly a matter of just turning on the compile flag for it.
