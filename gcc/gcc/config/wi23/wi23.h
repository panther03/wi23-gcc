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
#define STARTFILE_SPEC "crt0.o%s"

/* Provide an ENDFILE_SPEC appropriate for svr4.  Here we tack on our own
   magical crtend.o file (see crtstuff.c) which provides part of the
   support for getting C++ file-scope static object constructed before
   entering `main', followed by the normal svr3/svr4 "finalizer" file,
   which is either `gcrtn.o' or `crtn.o'.  */

#undef  ENDFILE_SPEC
#define ENDFILE_SPEC ""

/* Provide a LIB_SPEC appropriate for svr4.  Here we tack on the default
   standard C library (unless we are building a shared library) and
   the simulator BSP code.  */

#undef LIB_SPEC
#define LIB_SPEC ""

#undef  LINK_SPEC
#define LINK_SPEC "%{h*} %{v:-V}\
		   %{static:-Bstatic} %{shared:-shared} %{symbolic:-Bsymbolic}"

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

   r0   - argument 0
   r1   - argument 1
   r2   - argument 2
   r3   - argument 3
   r4   - argument 4
   r5   - argument 5
   r6   - argument 6
   r7   - argument 7
   r8   - argument 8
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
   r27  - callee saved
   fp   - frame pointer
   sp   - stack pointer
   ra   - return address
   tmp  - scratch register

   Floating-point

   f0   - argument 0
   f1   - argument 1
   f2   - argument 2
   f3   - argument 3
   f4   - argument 4
   f5   - argument 5
   f6   - argument 6
   f7   - argument 7
   f8   - argument 8
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
   ft1  - scratch register
   ft2  - scratch register
   
*/

#define REGISTER_NAMES   { \
   /* general-purpose regs */ \
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", \
    "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15", \
    "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23", \
    "r24", "r25", "r26", "r27", "fp", "sp", "ra", "tmp", \
   /* floating-point regs */ \
    "f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7", \
    "f8", "f9", "f10", "f11", "f12", "f13", "f14", "f15", \
    "f16", "f17", "f18", "f19", "f20", "f21", "f22", "f23", \
    "f24", "f25", "f26", "f27", "f28", "f29", "ft1", "ft2", \
   /* fake regs   still not entirely clear on what these do lol */ \
    "?ap", "?fp" };

#define WI23_R0     0
#define WI23_R1     1
#define WI23_R2     2
#define WI23_R3     3
#define WI23_R4     4
#define WI23_R5     5
#define WI23_R6     6
#define WI23_R7     7
#define WI23_R8     8
#define WI23_R9     9
#define WI23_R10    10
#define WI23_R11    11
#define WI23_R12    12
#define WI23_R13    13
#define WI23_R14    14
#define WI23_R15    15
#define WI23_R16    16
#define WI23_R17    17
#define WI23_R18    18
#define WI23_R19    19
#define WI23_R20    20
#define WI23_R21    21
#define WI23_R22    22
#define WI23_R23    23
#define WI23_R24    24
#define WI23_R25    25
#define WI23_R26    26
#define WI23_R27    27
#define WI23_FP     28
#define WI23_SP     29
#define WI23_RA     30
#define WI23_TMP    31

#define WI23_F0     32
#define WI23_F1     33
#define WI23_F2     34
#define WI23_F3     35
#define WI23_F4     36
#define WI23_F5     37
#define WI23_F6     38
#define WI23_F7     39
#define WI23_F8     40
#define WI23_F9     41
#define WI23_F10    42
#define WI23_F11    43
#define WI23_F12    44
#define WI23_F13    45
#define WI23_F14    46
#define WI23_F15    47
#define WI23_F16    48
#define WI23_F17    49
#define WI23_F18    50
#define WI23_F19    51
#define WI23_F20    52
#define WI23_F21    53
#define WI23_F22    54
#define WI23_F23    55
#define WI23_F24    56
#define WI23_F25    57
#define WI23_F26    58
#define WI23_F27    59
#define WI23_F28    60
#define WI23_F29    61
#define WI23_FT1    62
#define WI23_FT2    63
#define WI23_QFP    64
#define WI23_QAP    65

// Special register declarations
#define GP_ARG_FIRST        WI23_R0
#define GP_ARG_LAST         WI23_R8
#define GP_RET_VAL_REGNUM   WI23_R9
#define FP_ARG_FIRST        WI23_F0
#define FP_ARG_LAST         WI23_F8
#define FP_RET_VAL_REGNUM   WI23_F9
#define FP_OFS                32
#define LAST_REAL_REGISTER    63
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
{ \
/*  r0  r1  r2  r3  r4  r5  r6  r7 */  0, 0, 0, 0, 0, 0, 0, 0,	\
/*  r8  r9 r10 r11 r12 r13 r14 r15 */  0, 0, 0, 0, 0, 0, 0, 0,	\
/* r16 r17 r18 r19 r20 r21 r22 r23 */  0, 0, 0, 0, 0, 0, 0, 0,	\
/* r24 r25 r26 r27  fp  sp  ra tmp */  0, 0, 0, 0, 0, 1, 0, 1,	\
/*  f0  f1  f2  f3  f4  f5  f6  f7 */  0, 0, 0, 0, 0, 0, 0, 0,	\
/*  f8  f9 f10 f11 f12 f13 f14 f15 */  0, 0, 0, 0, 0, 0, 0, 0,	\
/* f16 f17 f18 f19 f20 f21 f22 f23 */  0, 0, 0, 0, 0, 0, 0, 0,	\
/* f24 f25 f26 f27 f28 f29 ft1 ft2 */  0, 0, 0, 0, 0, 0, 1, 1,	\
/* ?fp ?ap                         */  1, 1, \
}

#define CALL_REALLY_USED_REGISTERS		\
{ \
  /*  r0  r1  r2  r3  r4  r5  r6  r7 */  1, 1, 1, 1, 1, 1, 1, 1,	\
  /*  r8  r9 r10 r11 r12 r13 r14 r15 */  1, 1, 1, 1, 1, 1, 1, 1,	\
  /* r16 r17 r18 r19 r20 r21 r22 r23 */  1, 1, 1, 0, 0, 0, 0, 0,	\
  /* r24 r25 r26 r27  fp  sp  ra tmp */  0, 0, 0, 0, 0, 0, 0, 0,	\
  /*  f0  f1  f2  f3  f4  f5  f6  f7 */  1, 1, 1, 1, 1, 1, 1, 1,	\
  /*  f8  f9 f10 f11 f12 f13 f14 f15 */  1, 1, 1, 1, 1, 1, 1, 1,	\
  /* f16 f17 f18 f19 f20 f21 f22 f23 */  1, 1, 1, 0, 0, 0, 0, 0,	\
  /* f24 f25 f26 f27 f28 f29 ft1 ft2 */  0, 0, 0, 0, 0, 0, 0, 0,	\
  /* ?fp ?ap                         */  0, 0, \
}

/* Registers used as temporaries in prologue/epilogue code. */
#define WI23_PROLOGUE_TEMP_REGNUM (WI23_TMP)
#define WI23_PROLOGUE_TEMP(MODE)	\
  gen_rtx_REG (MODE, WI23_PROLOGUE_TEMP_REGNUM)

/* Registers used for exception handling.  */
#define WI23_EH_FIRST_REGNUM	(WI23_R17)
#define WI23_EH_LAST_REGNUM	(WI23_R18)
#define WI23_EH_TEMP_REGNUM	(WI23_TMP)

/* We can't copy to or from our CC register. */
#define AVOID_CCMODE_COPIES 1

/* A C expression whose value is a register class containing hard
   register REGNO.  */
#define REGNO_REG_CLASS(R) ((R >= FP_OFS && R <= LAST_REAL_REGISTER) ? FLOAT_REGS : GENERAL_REGS)

// immediate size checks, borrowed from nios2.h
#define IMM16_S(X) ((unsigned HOST_WIDE_INT)(X) + 0x8000 < 0x10000)
#define IMM16_Z(X) ((X) >= 0 && (X) < 0x10000)
#define IMM16_UPPER(X) (((X) & 0xffff) == 0)
#define IMM_SHIFT(X) ((X) >= 0 && (X) <= 31)

/* The Overall Framework of an Assembler File */

//#undef  ASM_SPEC
//#define ASM_SPEC ""
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
#define EPILOGUE_USES(R) (R == WI23_RA)

/* A C expression whose value is RTL representing the location of the
   incoming return address at the beginning of any function, before
   the prologue.  */
#define INCOMING_RETURN_ADDR_RTX	gen_rtx_REG (Pmode, WI23_RA)

// Describe how we implement __builtin_eh_return.  
#define EH_RETURN_DATA_REGNO(N)					\
  ((N) <= (WI23_EH_LAST_REGNUM - WI23_EH_FIRST_REGNUM) ?	\
   WI23_EH_FIRST_REGNUM + (N) : INVALID_REGNUM)

/* A C expression whose value is RTL representing a location in which to store
   a stack adjustment to be applied before function return. This is used to
   unwind the stack to an exception handler's call frame. It will be assigned
   zero on code paths that return normally.  */
#define EH_RETURN_STACKADJ_RTX	gen_rtx_REG (SImode, WI23_EH_TEMP_REGNUM)

// Store the return handler into the call frame. 
#define EH_RETURN_HANDLER_RTX						\
  gen_frame_mem (Pmode,							\
		 plus_constant (Pmode, hard_frame_pointer_rtx, UNITS_PER_WORD))


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

/* Define this macro to 1 if operations between registers with
   integral mode smaller than a word are always performed on the
   entire register.  */
#define WORD_REGISTER_OPERATIONS 1

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
   && TYPE_MODE (TREE_TYPE (TYPE)) == QImode	\
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
#define STACK_POINTER_REGNUM WI23_SP

/* The register number of the frame pointer register, which is used to
   access automatic variables in the stack frame.  */
#define FRAME_POINTER_REGNUM WI23_QFP

/* The register number of the arg pointer register, which is used to
   access the function's argument list.  */
#define ARG_POINTER_REGNUM WI23_QAP

#define HARD_FRAME_POINTER_REGNUM WI23_FP

#define ELIMINABLE_REGS					\
{{ ARG_POINTER_REGNUM,   STACK_POINTER_REGNUM},		\
 { ARG_POINTER_REGNUM,   HARD_FRAME_POINTER_REGNUM},	\
 { FRAME_POINTER_REGNUM, STACK_POINTER_REGNUM},		\
 { FRAME_POINTER_REGNUM, HARD_FRAME_POINTER_REGNUM}}

/* This macro returns the initial difference between the specified pair
   of registers.  */
// FIXME -- need to implement this function for wi23
#define INITIAL_ELIMINATION_OFFSET(FROM, TO, OFFSET)			\
  do {									\
    (OFFSET) = wi23_initial_elimination_offset((FROM), (TO)); \
  } while (0)

/* A C expression that is nonzero if REGNO is the number of a hard
   register in which function arguments are sometimes passed.  */
#define FUNCTION_ARG_REGNO_P(N)						\
  (IN_RANGE ((N), GP_ARG_FIRST, GP_ARG_LAST)) \
   || (IN_RANGE ((N), FP_ARG_FIRST, FP_ARG_LAST))

/* A macro whose definition is the name of the class to which a valid
   base register must belong.  A base register is one used in an
   address which is the register value plus a displacement.  */
#define BASE_REG_CLASS GENERAL_REGS

#define INDEX_REG_CLASS NO_REGS

/* A C expression which is nonzero if register number NUM is suitable
   for use as a base register in operand addresses.  */
#define REGNO_OK_FOR_BASE_P(REGNO)	\
  wi23_regno_ok_for_base_p (REGNO, true)

/* A C expression which is nonzero if register number NUM is suitable
   for use as an index register in operand addresses.  */
#define REGNO_OK_FOR_INDEX_P(NUM) 0

/* The maximum number of bytes that a single instruction can move
   quickly between memory and registers or between two memory
   locations.  */
#define MOVE_MAX 4

/* All load operations zero extend.  */
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

/* Define these boolean macros to indicate whether or not your
   architecture has (un)conditional branches that can span all of
   memory.  */
#define HAS_LONG_COND_BRANCH false
#define HAS_LONG_UNCOND_BRANCH false

/* Define this macro if it is as good or better to call a constant
   function address than to call an address kept in a register.  */
#define NO_FUNCTION_CSE 1

extern int wi23_is_mem_pm(rtx o);
#endif /* GCC_WI23_h */
