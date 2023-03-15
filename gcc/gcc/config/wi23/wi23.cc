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


int
wi23_initial_elimination_offset (int from, int to)
{
  int ret = 0;
  
  abort ();

  return ret;
} 

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
  if (i & 0xFFFF0000 == 0)
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
    0 // crtl->args.pretend_args_size
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

struct gcc_target targetm = TARGET_INITIALIZER;

//#include "gt-wi23.h"
