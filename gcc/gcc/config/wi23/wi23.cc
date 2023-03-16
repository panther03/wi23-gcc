/* Target Code for wi23
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

#define IN_TARGET_CODE 1

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "backend.h"
#include "target.h"
#include "rtl.h"
#include "tree.h"
#include "stringpool.h"
#include "attribs.h"
#include "df.h"
#include "regs.h"
#include "memmodel.h"
#include "emit-rtl.h"
#include "diagnostic-core.h"
#include "output.h"
#include "stor-layout.h"
#include "varasm.h"
#include "calls.h"
#include "expr.h"
#include "builtins.h"

/* This file should be included last.  */
// ?????????? ^
#include "target-def.h"

#define LOSE_AND_RETURN(msgid, x)		\
  do						\
    {						\
      wi23_operand_lossage (msgid, x);		\
      return;					\
    } while (0)



/* Worker function for TARGET_RETURN_IN_MEMORY.  */

static bool
wi23_return_in_memory (const_tree type, const_tree fntype ATTRIBUTE_UNUSED)
{
  const HOST_WIDE_INT size = int_size_in_bytes (type);
  return (size == -1 || size > 2 * UNITS_PER_WORD);
}

/* Define how to find the value returned by a function.
   VALTYPE is the data type of the value (as a tree).
   If the precise function being called is known, FUNC is its
   FUNCTION_DECL; otherwise, FUNC is 0.  

   We always return values in register r9 for wi23.  */

static rtx
wi23_function_value (const_tree valtype, 
		      const_tree fntype_or_decl ATTRIBUTE_UNUSED,
		      bool outgoing ATTRIBUTE_UNUSED)
{
  return gen_rtx_REG (TYPE_MODE (valtype), RET_VAL_REGNUM);
}

/* Define how to find the value returned by a library function.

   We always return values in register r9 for wi23.  */

static rtx
wi23_libcall_value (machine_mode mode,
                     const_rtx fun ATTRIBUTE_UNUSED)
{
  return gen_rtx_REG (mode, RET_VAL_REGNUM);
}

/* Handle TARGET_FUNCTION_VALUE_REGNO_P.

   We always return values in register r9 for wi23.  */

static bool
wi23_function_value_regno_p (const unsigned int regno)
{
  return (regno == RET_VAL_REGNUM);
}

/* Emit an error message when we're in an asm, and a fatal error for
   "normal" insns.  Formatted output isn't easily implemented, since we
   use output_operand_lossage to output the actual message and handle the
   categorization of the error.  */

static void
wi23_operand_lossage (const char *msgid, rtx op)
{
  debug_rtx (op);
  output_operand_lossage ("%s", msgid);
}

/* The PRINT_OPERAND_ADDRESS worker.  */

void
wi23_print_operand_address (FILE * file, machine_mode, rtx x)
{
  switch (GET_CODE (x))
    {
    case REG:
      fprintf (file, "%s,0", reg_names[REGNO (x)]);
      break;

    case PLUS:
      switch (GET_CODE (XEXP (x, 1)))
        {
        case CONST_INT:
          fprintf (file, "%s,%ld",
                   reg_names[REGNO (XEXP (x, 0))], INTVAL (XEXP (x, 1)));
          break;
        case SYMBOL_REF:
          // TODO no idea if this is the syntax wi23-gas wants, probably not
          fprintf (file, "%s,", reg_names[REGNO (XEXP (x, 0))]);
          output_addr_const (file, XEXP (x, 1));
          break;
        case CONST:
          {
            rtx plus = XEXP (XEXP (x, 1), 0);
            if (GET_CODE (XEXP (plus, 0)) == SYMBOL_REF
                && CONST_INT_P (XEXP (plus, 1)))
              {
                output_addr_const (file, XEXP (plus, 0));
                fprintf (file, "%s, %ld+", reg_names[REGNO (XEXP (x, 0))], INTVAL (XEXP (plus, 1)));
              }
            else
              abort ();
          }
          break;
        default:
          abort ();
        }
      break;

    default:
      output_addr_const (file, x);
      break;
    }
}

static void
wi23_print_operand (FILE *file, rtx x, int code)
{
  rtx operand = x;

  /* New code entries should just be added to the switch below.  If
     handling is finished, just return.  If handling was just a
     modification of the operand, the modified operand should be put in
     "operand", and then do a break to let default handling
     (zero-modifier) output the operand.  */

  switch (code)
    {
    case 0:
      /* No code, print as usual.  */
      break;

    default:
      LOSE_AND_RETURN ("invalid operand modifier letter", x);
    }

  /* Print an operand as without a modifier letter.  */
  switch (GET_CODE (operand))
    {
    case REG:
      if (REGNO (operand) > FCSR_REGNUM)
	internal_error ("internal error: bad register: %d", REGNO (operand));
      fprintf (file, "%s", reg_names[REGNO (operand)]);
      return;

    case MEM:
      output_address (GET_MODE (XEXP (operand, 0)), XEXP (operand, 0));
      return;

    default:
      /* No need to handle all strange variants, let output_addr_const
	 do it for us.  */
      if (CONSTANT_P (operand))
	{
	  output_addr_const (file, operand);
	  return;
	}

      LOSE_AND_RETURN ("unexpected operand", x);
    }
}

/* Per-function machine data.  */
struct GTY(()) machine_function
 {
   /* Number of bytes saved on the stack for callee saved registers.  */
   int callee_saved_reg_size;

   /* Number of bytes saved on the stack for local variables.  */
   int local_vars_size;

   /* The sum of 2 sizes: locals vars and padding byte for saving the
    * registers.  Used in expand_prologue () and expand_epilogue().  */
   int size_for_adjusting_sp;
 };

/* Zero initialization is OK for all current fields.  */

static struct machine_function *
wi23_init_machine_status (void)
{
  return ggc_cleared_alloc<machine_function> ();
}

/* The TARGET_OPTION_OVERRIDE worker.  */
static void
wi23_option_override (void)
{
  /* Set the per-function-data initializer.  */
  init_machine_status = wi23_init_machine_status;
}

/* Compute the size of the local area and the size to be adjusted by the
 * prologue and epilogue.  */

static void
wi23_compute_frame (void)
{
  /* For aligning the local variables.  */
  int stack_alignment = STACK_BOUNDARY / BITS_PER_UNIT;
  int padding_locals;
  int regno;

  /* Padding needed for each element of the frame.  */
  cfun->machine->local_vars_size = get_frame_size ();

  /* Align to the stack alignment.  */
  padding_locals = cfun->machine->local_vars_size % stack_alignment;
  if (padding_locals)
    padding_locals = stack_alignment - padding_locals;

  cfun->machine->local_vars_size += padding_locals;

  cfun->machine->callee_saved_reg_size = 0;

  /* Save callee-saved registers.  */
  for (regno = 0; regno < FIRST_PSEUDO_REGISTER; regno++)
    if (df_regs_ever_live_p (regno) && !call_used_or_fixed_reg_p (regno))
      cfun->machine->callee_saved_reg_size += 4;

  cfun->machine->size_for_adjusting_sp =
    crtl->args.pretend_args_size
    + cfun->machine->local_vars_size
    + (ACCUMULATE_OUTGOING_ARGS
       ? (HOST_WIDE_INT) crtl->outgoing_args_size : 0);
}

void
wi23_expand_prologue (void)
{
  int regno;
  rtx insn;

  wi23_compute_frame ();

  if (flag_stack_usage_info)
    current_function_static_stack_size = cfun->machine->size_for_adjusting_sp;

  /* Save callee-saved registers.  */
  for (regno = 0; regno < FIRST_PSEUDO_REGISTER; regno++)
    {
      if (df_regs_ever_live_p (regno)
	  && !call_used_or_fixed_reg_p (regno))
	{
	  insn = emit_insn (gen_movsi_push (gen_rtx_REG (Pmode, regno)));
	  RTX_FRAME_RELATED_P (insn) = 1;
	}
    }

  if (cfun->machine->size_for_adjusting_sp > 0)
  {
    int i = cfun->machine->size_for_adjusting_sp; 
    if (i <= (1 << 15)) {
      insn = emit_insn (gen_addsi3 (stack_pointer_rtx, 
        stack_pointer_rtx, 
        GEN_INT (-i)));
      RTX_FRAME_RELATED_P (insn) = 1;
    } else {
      abort();
    }
  }
}

void
wi23_expand_epilogue (void)
{
  int regno;
  rtx reg;

  if (cfun->machine->callee_saved_reg_size != 0)
  {
    int i = cfun->machine->size_for_adjusting_sp; 
    if (i <= (1 << 15)) {
      emit_insn (gen_addsi3 (stack_pointer_rtx, 
        stack_pointer_rtx, 
        GEN_INT (i)));
    } else {
      abort();
    }

    for (regno = FIRST_PSEUDO_REGISTER; regno-- > 0; ) {
      if (!call_used_or_fixed_reg_p (regno)
        && df_regs_ever_live_p (regno))
      {
        rtx preg = gen_rtx_REG (Pmode, regno);
        emit_insn (gen_movsi_pop (preg));
      }
    }
  }

  emit_jump_insn (gen_returner ());
}

// TODO: Implement select_section thing from ft32 to support loading from program memory?

/* Implements the macro INITIAL_ELIMINATION_OFFSET, return the OFFSET.  */

int
wi23_initial_elimination_offset (int from, int to)
{
  int ret;
  
  if ((from) == SFP_REGNUM && (to) == FP_REGNUM)
    {
      /* Compute this since we need to use cfun->machine->local_vars_size.  */
      wi23_compute_frame ();
      ret = -cfun->machine->callee_saved_reg_size;
    }
  else if ((from) == AP_REGNUM && (to) == FP_REGNUM)
    ret = 0x00;
  else
    abort ();

  return ret;
}

/* Helper function for `wi23_legitimate_address_p'.  */

static bool
wi23_reg_ok_for_base_p (const_rtx reg, bool strict_p)
{
  int regno = REGNO (reg);

  if (strict_p)
    return HARD_REGNO_OK_FOR_BASE_P (regno)
	   || HARD_REGNO_OK_FOR_BASE_P (reg_renumber[regno]);
  else    
    return !HARD_REGISTER_NUM_P (regno)
	   || HARD_REGNO_OK_FOR_BASE_P (regno);
}

/* Worker function for TARGET_LEGITIMATE_ADDRESS_P.  */

static bool
wi23_legitimate_address_p (machine_mode mode ATTRIBUTE_UNUSED,
			    rtx x, bool strict_p,
			    addr_space_t as)
{
  // TODO add support for instruction memory
  gcc_assert (ADDR_SPACE_GENERIC_P (as));

  if (GET_CODE(x) == PLUS
      && REG_P (XEXP (x, 0))
      && wi23_reg_ok_for_base_p (XEXP (x, 0), strict_p)
      && CONST_INT_P (XEXP (x, 1))
      && IN_RANGE (INTVAL (XEXP (x, 1)), -32768, 32767))
    return true;
  if (REG_P (x) && wi23_reg_ok_for_base_p (x, strict_p))
    return true;
  if (GET_CODE (x) == SYMBOL_REF
      || GET_CODE (x) == LABEL_REF
      || GET_CODE (x) == CONST)
    return true;
  return false;
}

// TODO: deal with varargs later


// TODO add floating point reg support for both of these functions
/*static rtx
wi23_function_arg (cumulative_args_t cum_v, const function_arg_info &arg)
{
  CUMULATIVE_ARGS *cum = get_cumulative_args (cum_v);

  if (cum->num_gprs < 9)
    return gen_rtx_REG (arg.mode, *cum);
  else 
    return NULL_RTX;
}

#define WI23_FUNCTION_ARG_SIZE(MODE, TYPE)	\
  ((MODE) != BLKmode ? GET_MODE_SIZE (MODE)	\
   : (unsigned) int_size_in_bytes (TYPE))

static void
moxie_function_arg_advance (cumulative_args_t cum_v,
			    const function_arg_info &arg)
{
  CUMULATIVE_ARGS *cum = get_cumulative_args (cum_v);

  cum->num_gprs = (cum->num_gprs < RA_REGNUM
	  ? *cum + ((3 + WI23_FUNCTION_ARG_SIZE (arg.mode, arg.type)) / 4)
	  : *cum);
}
*/

/* Used by constraints.md to distinguish between GENERIC and PM
   memory addresses.  */

int
wi23_is_mem_pm (rtx o)
{
  return (MEM_P (o)
          && !ADDR_SPACE_GENERIC_P (MEM_ADDR_SPACE (o)));
}

const char *
wi23_load_immediate (rtx dst, int32_t i, bool high)
{
  char pattern[100];

  // if it fits into 16-bits, don't care if signed or not
  // FIXME: stupid way of doing this?
  if ((i & 0xFFFF0000) == 0)
  {
      if (high) sprintf (pattern, "slbi  %%0,%d", i);
      else sprintf (pattern, "lbi  %%0,%d", i);
      output_asm_insn (pattern, &dst);
  }
  else
  {
      sprintf (pattern, "lbi  %%0,%d", i >> 16);
      wi23_load_immediate (dst, i & 0xFFFF, true);
      output_asm_insn (pattern, &dst);
   }

  return "";
}







/* The Global `targetm' Variable. */

/* Initialize the GCC target structure.  */

#undef  TARGET_PROMOTE_PROTOTYPES
#define TARGET_PROMOTE_PROTOTYPES	hook_bool_const_tree_true

#undef  TARGET_RETURN_IN_MEMORY
#define TARGET_RETURN_IN_MEMORY		wi23_return_in_memory
#undef  TARGET_MUST_PASS_IN_STACK
#define TARGET_MUST_PASS_IN_STACK	must_pass_in_stack_var_size
#undef  TARGET_PASS_BY_REFERENCE
#define TARGET_PASS_BY_REFERENCE    hook_pass_by_reference_must_pass_in_stack
//#undef  TARGET_ARG_PARTIAL_BYTES
//#define TARGET_ARG_PARTIAL_BYTES        hook_bool_void_false
//#undef  TARGET_FUNCTION_ARG
//#define TARGET_FUNCTION_ARG		hook_bool_void_false
//#undef  TARGET_FUNCTION_ARG_ADVANCE
//#define TARGET_FUNCTION_ARG_ADVANCE	hook_bool_void_false

#undef TARGET_LRA_P
#define TARGET_LRA_P hook_bool_void_false

#undef  TARGET_ADDR_SPACE_LEGITIMATE_ADDRESS_P
#define TARGET_ADDR_SPACE_LEGITIMATE_ADDRESS_P	wi23_legitimate_address_p

//#undef  TARGET_SETUP_INCOMING_VARARGS
//#define TARGET_SETUP_INCOMING_VARARGS 	hook_bool_void_false

//#undef	TARGET_FIXED_CONDITION_CODE_REGS
//#define	TARGET_FIXED_CONDITION_CODE_REGS hook_bool_void_false

/* Define this to return an RTX representing the place where a
   function returns or receives a value of data type RET_TYPE, a tree
   node representing a data type.  */
#undef TARGET_FUNCTION_VALUE
#define TARGET_FUNCTION_VALUE wi23_function_value
#undef TARGET_LIBCALL_VALUE
#define TARGET_LIBCALL_VALUE wi23_libcall_value
#undef TARGET_FUNCTION_VALUE_REGNO_P
#define TARGET_FUNCTION_VALUE_REGNO_P wi23_function_value_regno_p

#undef TARGET_FRAME_POINTER_REQUIRED
#define TARGET_FRAME_POINTER_REQUIRED hook_bool_void_true

//#/undef TARGET_STATIC_CHAIN
//#define TARGET_STATIC_CHAIN hook_bool_void_false
//#undef TARGET_ASM_TRAMPOLINE_TEMPLATE
//#define TARGET_ASM_TRAMPOLINE_TEMPLATE hook_bool_void_false
//#undef TARGET_TRAMPOLINE_INIT
//#define TARGET_TRAMPOLINE_INIT hook_bool_void_false

#undef TARGET_OPTION_OVERRIDE
#define TARGET_OPTION_OVERRIDE wi23_option_override

#undef  TARGET_PRINT_OPERAND
#define TARGET_PRINT_OPERAND wi23_print_operand
#undef  TARGET_PRINT_OPERAND_ADDRESS
#define TARGET_PRINT_OPERAND_ADDRESS wi23_print_operand_address

#undef  TARGET_CONSTANT_ALIGNMENT
#define TARGET_CONSTANT_ALIGNMENT constant_alignment_word_strings

struct gcc_target targetm = TARGET_INITIALIZER;

#include "gt-wi23.h"
