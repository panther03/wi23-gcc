/* Target Definitions for wi23.
   Copyright (C) 2008-2022 Free Software Foundation, Inc.
   Contributed by Anthony Green.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 3, or (at your
   option) any later version.

   GCC is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

#ifndef GCC_WI23_H
#define GCC_WI23_H

#undef  STARTFILE_SPEC
#define STARTFILE_SPEC "%{!mno-crt0:crt0%O%s} crti.o%s crtbegin.o%s"

/* Provide an ENDFILE_SPEC appropriate for svr4.  Here we tack on our own
   magical crtend.o file (see crtstuff.c) which provides part of the
   support for getting C++ file-scope static object constructed before
   entering `main', followed by the normal svr3/svr4 "finalizer" file,
   which is either `gcrtn.o' or `crtn.o'.  */

#undef  ENDFILE_SPEC
#define ENDFILE_SPEC "crtend.o%s crtn.o%s"

/* Provide a LIB_SPEC appropriate for svr4.  Here we tack on the default
   standard C library (unless we are building a shared library) and
   the simulator BSP code.  */

#undef LIB_SPEC
#define LIB_SPEC "%{!shared:%{!symbolic:-lc}}"

#undef  LINK_SPEC
#define LINK_SPEC "%{h*} %{v:-V} %{!mel:-EB} %{mel:-EL}\
		   %{static:-Bstatic} %{shared:-shared} %{symbolic:-Bsymbolic}"

#ifndef MULTILIB_DEFAULTS
#define MULTILIB_DEFAULTS { "meb" }
#endif

/* Layout of Source Language Data Types */

#define INT_TYPE_SIZE 32
#define SHORT_TYPE_SIZE 16
#define LONG_TYPE_SIZE 32
#define LONG_LONG_TYPE_SIZE 64

#define FLOAT_TYPE_SIZE 32
#define DOUBLE_TYPE_SIZE 64
#define LONG_DOUBLE_TYPE_SIZE 64

#define DEFAULT_SIGNED_CHAR 0

#undef  SIZE_TYPE
#define SIZE_TYPE "unsigned int"

#undef  PTRDIFF_TYPE
#define PTRDIFF_TYPE "int"

#undef  WCHAR_TYPE
#define WCHAR_TYPE "unsigned int"

#undef  WCHAR_TYPE_SIZE
#define WCHAR_TYPE_SIZE 32

/* Registers...

   General-purpose

   r0   - always 0
   r1   - argument 0
   r2   - argument 1
   r3   - argument 2
   r4   - argument 3
   r5   - argument 4
   r6   - argument 5
   r7   - argument 6
   r8   - argument 7
   r9   - return value
   r10  - temporary
   r11  - temporary
   r12  - temporary
   r13  - temporary
   r14  - temporary
   r15  - temporary
   r16  - temporary
   r17  - temporary
   r18  - temporary   
   r19  - callee saved
   r20  - callee saved
   r21  - callee saved
   r22  - callee saved
   r23  - callee saved
   r24  - callee saved
   r25  - callee saved
   r26  - callee saved
   gp   - global pointer (tbd) probably just make it another callee saved
   fp   - frame pointer
   sp   - stack pointer
   ra   - return address
   csr  - status register

   Floating-point

   f0   - reserved (???)
   f1   - argument 0
   f2   - argument 1
   f3   - argument 2
   f4   - argument 3
   f5   - argument 4
   f6   - argument 5
   f7   - argument 6
   f8   - argument 7
   f9   - return value
   f10  - temporary
   f11  - temporary
   f12  - temporary
   f13  - temporary
   f14  - temporary
   f15  - temporary
   f16  - temporary
   f17  - temporary
   f18  - temporary   
   f19  - callee saved
   f20  - callee saved
   f21  - callee saved
   f22  - callee saved
   f23  - callee saved
   f24  - callee saved
   f25  - callee saved
   f26  - callee saved
   f27  - callee saved
   f28  - callee saved
   f29  - callee saved
   f30  - callee saved
   fcsr - floating point status register
   
*/

#define REGISTER_NAMES   { \
   /* general-purpose regs */ \
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", \
    "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15", \
    "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23", \
    "r24", "r25", "r26", "gp", "fp", "sp", "ra", "csr", \
   /* floating-point regs */ \
    "f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7", \
    "f8", "f9", "f10", "f11", "f12", "f13", "f14", "f15", \
    "f16", "f17", "f18", "f19", "f20", "f21", "f22", "f23", \
    "f24", "f25", "f26", "f27", "f28", "f29", "f30", "fcsr", \
   /* fake regs   still not entirely clear on what these do lol */ \
    "?ap", "?fp" };

#define FIRST_PSEUDO_REGISTER 66

enum reg_class
{
  NO_REGS,
  GENERAL_REGS,
  FLOAT_REGS,
  ALL_REGS,
  LIM_REG_CLASSES
};


#define REG_CLASS_CONTENTS \
{ { 0x00000000, 0x00000000, 0x00000000 }, /* empty */ \
  { 0xFFFFFFFF, 0x00000000, 0x00000003 }, /* general */ \
  { 0x00000000, 0xFFFFFFFF, 0x00000000 }, /* float */ \
  { 0xFFFFFFFF, 0xFFFFFFFF, 0x00000003 }, /* all */ \
}

#define N_REG_CLASSES LIM_REG_CLASSES

#define REG_CLASS_NAMES {\
    "NO_REGS", \
    "GENERAL_REGS", \
    "FLOAT_REGS", \
    "ALL_REGS" }

#define FIXED_REGISTERS		\
{ /* general */ \
  1, 0, 0, 0, 0, 0, 0, 0,	\
  0, 0, 0, 0, 0, 0, 0, 0,	\
  0, 0, 0, 0, 0, 0, 0, 0,	\
  0, 0, 0, 0, 1, 1, 1, 1,	\
  /* float */ \
  0, 0, 0, 0, 0, 0, 0, 0,	\
  0, 0, 0, 0, 0, 0, 0, 0,	\
  0, 0, 0, 0, 0, 0, 0, 0,	\
  0, 0, 0, 0, 0, 0, 0, 1,	\
  /* fake */ \
  1, 1}

#define CALL_USED_REGISTERS		\
{ \
  1, 1, 1, 1, 1, 1, 1, 1,	/* general */ \
  1, 1, 1, 1, 1, 1, 1, 1,	\
  1, 1, 1, 1, 0, 0, 0, 0,	\
  0, 0, 0, 0, 1, 1, 1, 1,	\
  1, 1, 1, 1, 1, 1, 1, 1,	/* float */ \
  1, 1, 1, 1, 1, 1, 1, 1,	\
  1, 1, 1, 1, 0, 0, 0, 0,	\
  0, 0, 0, 0, 0, 0, 0, 1,	\
  1, 1 /* fake */ }

/* A C expression whose value is a register class containing hard
   register REGNO.  */
#define REGNO_REG_CLASS(R) ((R > CSR_REGNUM && R <= FCSR_REGNUM) ? FLOAT_REGS : GENERAL_REGS)

// immediate size checks, borrowed from nios2.h
#define IMM16_S(X) ((unsigned HOST_WIDE_INT)(X) + 0x8000 < 0x10000)
#define IMM16_Z(X) ((X) >= 0 && (X) < 0x10000)
#define IMM16_UPPER(X) (((X) & 0xffff) == 0)
#define IMM_SHIFT(X) ((X) >= 0 && (X) <= 31)

/* The Overall Framework of an Assembler File */

#undef  ASM_SPEC
#define ASM_SPEC "%{!mel:-EB} %{mel:-EL}"
#define ASM_COMMENT_START "//"
#define ASM_APP_ON ""
#define ASM_APP_OFF ""

#define FILE_ASM_OP     "\t.file\n"

/* Switch to the text or data segment.  */
#define TEXT_SECTION_ASM_OP  "\t.text"
#define DATA_SECTION_ASM_OP  "\t.data"

/* Assembler Commands for Alignment */

#define ASM_OUTPUT_ALIGN(STREAM,POWER) \
	fprintf (STREAM, "\t.p2align\t%d\n", POWER);

/* Output and Generation of Labels */

#define GLOBAL_ASM_OP "\t.global\t"

/* Passing Arguments in Registers */

/* A C type for declaring a variable that is used as the first
   argument of `FUNCTION_ARG' and other related values.  */
//#define CUMULATIVE_ARGS unsigned int

/* If defined, the maximum amount of space required for outgoing arguments
   will be computed and placed into the variable
   `current_function_outgoing_args_size'.  No space will be pushed
   onto the stack for each call; instead, the function prologue should
   increase the stack frame size by this amount.  */
#define ACCUMULATE_OUTGOING_ARGS 1

typedef struct {
  /* Number of integer registers used so far, up to MAX_ARGS_IN_REGISTERS. */
  unsigned int num_gprs;

  /* Number of floating-point registers used so far, likewise.  */
  unsigned int num_fprs;
} CUMULATIVE_ARGS;

/* A C statement (sans semicolon) for initializing the variable CUM
   for the state at the beginning of the argument list.  
   No fucking clue what this actually does, just copied from riscv */
#define INIT_CUMULATIVE_ARGS(CUM,FNTYPE,LIBNAME,FNDECL,N_NAMED_ARGS) \
  memset (&(CUM), 0, sizeof (CUM))

/* How Scalar Function Values Are Returned */

/* STACK AND CALLING */

/* Define this macro if pushing a word onto the stack moves the stack
   pointer to a smaller address.  */
#define STACK_GROWS_DOWNWARD 1

/* Define this if the above stack space is to be considered part of the
   space allocated by the caller.  */
#define OUTGOING_REG_PARM_STACK_SPACE(FNTYPE) 1

/* Define this if it is the responsibility of the caller to allocate
   the area reserved for arguments passed in registers.  */
#define REG_PARM_STACK_SPACE(FNDECL) 0 // FIXME: changed this from moxie??

/* Offset from the argument pointer register to the first argument's
   address.  On some machines it may depend on the data type of the
   function.  */
#define FIRST_PARM_OFFSET(F) 0

/* Define this macro to nonzero value if the addresses of local variable slots
   are at negative offsets from the frame pointer.  */
#define FRAME_GROWS_DOWNWARD 1

/* Define this macro as a C expression that is nonzero for registers that are
   used by the epilogue or the return pattern.  The stack and frame
   pointer registers are already assumed to be used as needed.  */
#define EPILOGUE_USES(R) (R == RA_REGNUM)

/* A C expression whose value is RTL representing the location of the
   incoming return address at the beginning of any function, before
   the prologue.  */
#define INCOMING_RETURN_ADDR_RTX	gen_rtx_REG (Pmode, RA_REGNUM)

// FIXME cba to implement this just going to not implement it and hope gcc doesnt explode

/*
// Describe how we implement __builtin_eh_return.  
#define EH_RETURN_DATA_REGNO(N)	((N) < 4 ? (N+2) : INVALID_REGNUM)

// Store the return handler into the call frame. 
#define EH_RETURN_HANDLER_RTX						\
  gen_frame_mem (Pmode,							\
		 plus_constant (Pmode, frame_pointer_rtx, UNITS_PER_WORD))
*/

/* Storage Layout */

#define BITS_BIG_ENDIAN 0
#define BYTES_BIG_ENDIAN 1
#define WORDS_BIG_ENDIAN 1

/* Alignment required for a function entry point, in bits.  */
#define FUNCTION_BOUNDARY 32

/* Define this macro as a C expression which is nonzero if accessing
   less than a word of memory (i.e. a `char' or a `short') is no
   faster than accessing a word of memory.  */
#define SLOW_BYTE_ACCESS 1

/* Number of storage units in a word; normally the size of a
   general-purpose register, a power of two from 1 or 8.  */
#define UNITS_PER_WORD 4

/* Define this macro to the minimum alignment enforced by hardware
   for the stack pointer on this machine.  The definition is a C
   expression for the desired alignment (measured in bits).  */
#define STACK_BOUNDARY 32

/* Normal alignment required for function parameters on the stack, in
   bits.  All stack parameters receive at least this much alignment
   regardless of data type.  */
#define PARM_BOUNDARY 32

/* Alignment of field after `int : 0' in a structure.  */
#define EMPTY_FIELD_BOUNDARY  32

/* No data type wants to be aligned rounder than this.  */
#define BIGGEST_ALIGNMENT 32

/* The best alignment to use in cases where we have a choice.  */
#define FASTEST_ALIGNMENT 32

/* Every structures size must be a multiple of 8 bits.  */
#define STRUCTURE_SIZE_BOUNDARY 8

/* Look at the fundamental type that is used for a bit-field and use 
   that to impose alignment on the enclosing structure.
   struct s {int a:8}; should have same alignment as "int", not "char".  */
#define	PCC_BITFIELD_TYPE_MATTERS	1

/* Largest integer machine mode for structures.  If undefined, the default
   is GET_MODE_SIZE(DImode).  */
#define MAX_FIXED_MODE_SIZE 32

/* Make arrays of chars word-aligned for the same reasons.  */
#define DATA_ALIGNMENT(TYPE, ALIGN)		\
  (TREE_CODE (TYPE) == ARRAY_TYPE		\
   && TYPE_MODE (TREE_TYPE (TYPE)) == SImode	\
   && (ALIGN) < FASTEST_ALIGNMENT ? FASTEST_ALIGNMENT : (ALIGN))
     
/* Set this nonzero if move instructions will actually fail to work
   when given unaligned data.  */
#define STRICT_ALIGNMENT 1

/* Generating Code for Profiling */
#define FUNCTION_PROFILER(FILE,LABELNO) (abort (), 0)

/* Trampolines for Nested Functions.  */
// FIXME figure out how to actually calculate this
#define TRAMPOLINE_SIZE (2 + 6 + 4 + 2 + 6)

/* Alignment required for trampolines, in bits.  */
#define TRAMPOLINE_ALIGNMENT 32

/* An alias for the machine mode for pointers.  */
#define Pmode         SImode

/* An alias for the machine mode used for memory references to
   functions being called, in `call' RTL expressions.  */
#define FUNCTION_MODE SImode

/* The register number of the stack pointer register, which must also
   be a fixed register according to `FIXED_REGISTERS'.  */
#define STACK_POINTER_REGNUM SP_REGNUM

/* The register number of the frame pointer register, which is used to
   access automatic variables in the stack frame.  */
#define FRAME_POINTER_REGNUM SFP_REGNUM

/* The register number of the arg pointer register, which is used to
   access the function's argument list.  */
#define ARG_POINTER_REGNUM AP_REGNUM

#define HARD_FRAME_POINTER_REGNUM FP_REGNUM

#define ELIMINABLE_REGS							\
{{ FRAME_POINTER_REGNUM, HARD_FRAME_POINTER_REGNUM },			\
 { ARG_POINTER_REGNUM,   HARD_FRAME_POINTER_REGNUM }}			

/* This macro returns the initial difference between the specified pair
   of registers.  */
// FIXME -- need to implement this function for wi23
#define INITIAL_ELIMINATION_OFFSET(FROM, TO, OFFSET)			\
  do {									\
    (OFFSET) = 0; /*wi23_initial_elimination_offset((FROM), (TO));*/ \
  } while (0)

/* A C expression that is nonzero if REGNO is the number of a hard
   register in which function arguments are sometimes passed.  */
#define FUNCTION_ARG_REGNO_P(N)						\
  (IN_RANGE ((N), GP_ARG_FIRST, GP_ARG_LAST)				\
   || (IN_RANGE ((N), FP_ARG_FIRST, FP_ARG_LAST)))

/* A macro whose definition is the name of the class to which a valid
   base register must belong.  A base register is one used in an
   address which is the register value plus a displacement.  */
#define BASE_REG_CLASS GENERAL_REGS

#define INDEX_REG_CLASS NO_REGS

// FIXME only used by moxie
#define HARD_REGNO_OK_FOR_BASE_P(NUM) \
  ((unsigned) (NUM) < FIRST_PSEUDO_REGISTER \
   && (REGNO_REG_CLASS(NUM) == GENERAL_REGS \
       || (NUM) == HARD_FRAME_POINTER_REGNUM))

/* A C expression which is nonzero if register number NUM is suitable
   for use as a base register in operand addresses.  */
#ifdef REG_OK_STRICT
#define REGNO_OK_FOR_BASE_P(NUM)		 \
  (HARD_REGNO_OK_FOR_BASE_P(NUM) 		 \
   || HARD_REGNO_OK_FOR_BASE_P(reg_renumber[(NUM)]))
#else
#define REGNO_OK_FOR_BASE_P(NUM)		 \
  ((NUM) >= FIRST_PSEUDO_REGISTER || HARD_REGNO_OK_FOR_BASE_P(NUM))
#endif

/* A C expression which is nonzero if register number NUM is suitable
   for use as an index register in operand addresses.  */
#define REGNO_OK_FOR_INDEX_P(NUM) FP_REGNUM

/* The maximum number of bytes that a single instruction can move
   quickly between memory and registers or between two memory
   locations.  */
#define MOVE_MAX 4

/* All load operations zero extend.  */
// FIXME: lbi sign extends?
#define LOAD_EXTEND_OP(MEM) ZERO_EXTEND

/* A number, the maximum number of registers that can appear in a
   valid memory address.  */
#define MAX_REGS_PER_ADDRESS 1

/* An alias for a machine mode name.  This is the machine mode that
   elements of a jump-table should have.  */
#define CASE_VECTOR_MODE SImode

/* Run-time Target Specification */

#define TARGET_CPU_CPP_BUILTINS() \
  { \
    builtin_define_std ("wi23");			\
    builtin_define_std ("WI23");			\
  }

#define ADDR_SPACE_PM 1

#define REGISTER_TARGET_PRAGMAS() do { \
  c_register_addr_space ("__flash", ADDR_SPACE_PM); \
} while (0)

extern int wi23_is_mem_pm(rtx o);
#endif /* GCC_WI23_h */
