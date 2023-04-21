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
#include "tm.h"
#include "rtl.h"
#include "regs.h"
#include "insn-config.h"
#include "insn-attr.h"
#include "recog.h"
#include "output.h"
#include "alias.h"
#include "tree.h"
#include "stringpool.h"
#include "attribs.h"
#include "varasm.h"
#include "stor-layout.h"
#include "calls.h"
#include "function.h"
#include "explow.h"
#include "memmodel.h"
#include "emit-rtl.h"
#include "reload.h"
#include "tm_p.h"
#include "target.h"
#include "target-def.h"
#include "basic-block.h"
#include "expr.h"
#include "optabs.h"
#include "bitmap.h"
#include "df.h"
#include "diagnostic.h"
#include "builtins.h"
#include "predict.h"
#include "tree-pass.h"
#include "opts.h"
#include "tm-constrs.h"

/* This file should be included last.  */
#include "target-def.h"

#define LOSE_AND_RETURN(msgid, x)		\
  do						\
    {						\
      wi23_operand_lossage (msgid, x);		\
      return;					\
    } while (0)

#define BITSET_P(VALUE,BIT) (((VALUE) & ((uint64_t)(1UL << (BIT)))) != 0)

/* If non-zero, this is an offset to be added to SP to redefine the CFA
   when restoring the FP register from the stack.  Only valid when generating
   the epilogue.  */

static int epilogue_cfa_sp_offset;

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

   We always return values in registers r9 & f9 for wi23.  */

static rtx
wi23_function_value (const_tree valtype, 
		      const_tree fntype_or_decl ATTRIBUTE_UNUSED,
		      bool outgoing ATTRIBUTE_UNUSED)
{
  if ((TYPE_MODE (valtype)) == SFmode) {
    return gen_rtx_REG (TYPE_MODE (valtype), FP_RET_VAL_REGNUM);
  } else {
    return gen_rtx_REG (TYPE_MODE (valtype), GP_RET_VAL_REGNUM);
  }
}

/* Define how to find the value returned by a library function.

   We always return values in registers r9 & f9 for wi23.  */

static rtx
wi23_libcall_value (machine_mode mode,
                     const_rtx fun ATTRIBUTE_UNUSED)
{
  if (mode == SFmode) {
    return gen_rtx_REG (mode, FP_RET_VAL_REGNUM);
  } else {
    return gen_rtx_REG (mode, GP_RET_VAL_REGNUM);
  }
}

/* Handle TARGET_FUNCTION_VALUE_REGNO_P.

   We always return values in registers r9 & f9 for wi23.  */

static bool
wi23_function_value_regno_p (const unsigned int regno)
{
  return (regno == GP_RET_VAL_REGNUM) || (regno == FP_RET_VAL_REGNUM);
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
    case 'C': {
      enum rtx_code c = GET_CODE (x);
      fprintf (file, "%s", GET_RTX_NAME (c));
      return;
    }
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
      if (REGNO (operand) > LAST_REAL_REGISTER)
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

struct GTY(())  wi23_frame_info {
  /* The size of the frame in bytes.  */
  HOST_WIDE_INT total_size;

  /* Bit X is set if the function saves or restores scalar register X.  */
  uint64_t mask;

  /* Offsets of register save areas from frame bottom */
  HOST_WIDE_INT reg_sp_offset;

  /* Offset of virtual frame pointer from stack pointer/frame bottom */
  HOST_WIDE_INT frame_pointer_offset;

  /* Offset of hard frame pointer from stack pointer/frame bottom */
  HOST_WIDE_INT hard_frame_pointer_offset;

  /* The offset of arg_pointer_rtx from the bottom of the frame.  */
  HOST_WIDE_INT arg_pointer_offset;
};

/* Per-function machine data.  */

struct GTY(()) machine_function
{
  /* The current frame information, calculated by wi23_compute_frame.  */
  struct wi23_frame_info frame;

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

static bool
wi23_callee_saved_regno_p (int regno)
{
  /* Check call-saved registers.  */
  if ((df_regs_ever_live_p (regno) && !call_used_or_fixed_reg_p (regno))
      || (crtl->calls_eh_return && (regno >= WI23_EH_FIRST_REGNUM
				    && regno <= WI23_EH_LAST_REGNUM)))
    {
      return true;
    }

  return false;
}

static unsigned int
wi23_stack_align (unsigned int loc)
{
  return (loc + ((STACK_BOUNDARY/BITS_PER_UNIT)-1)) & -(STACK_BOUNDARY/BITS_PER_UNIT);
}

/* WI23 stack frames grown downward.  High addresses are at the top.

	+-------------------------------+
	|                               |
	|  incoming stack arguments     |
	|                               |
	+-------------------------------+ <-- incoming stack pointer
	|                               |
	|  callee-allocated save area   |
	|  for arguments that are       |
	|  split between registers and  |
	|  the stack                    |
	|                               |
	+-------------------------------+ <-- arg_pointer_rtx
	|                               |
	|  callee-allocated save area   |
	|  for register varargs         |
	|                               |
	+-------------------------------+ <-- hard_frame_pointer_rtx;
	|                               |     stack_pointer_rtx + reg_sp_offset
	|  Scalar registers save area   |       + UNITS_PER_WORD
	|                               |
	+-------------------------------+ <-- frame_pointer_rtx (virtual)
	|                               |
	|  local variables              |
	|                               |
P +-------------------------------+
	|                               |
	|  outgoing stack arguments     |
	|                               |
	+-------------------------------+ <-- stack_pointer_rtx

   Dynamic stack allocations such as alloca insert data at point P.
   They decrease stack_pointer_rtx but leave frame_pointer_rtx and
   hard_frame_pointer_rtx unchanged.  */

/* Compute the size of the local area and the size to be adjusted by the
 * prologue and epilogue.  */

static void
wi23_compute_frame (void)
{
  struct wi23_frame_info *frame;
  HOST_WIDE_INT offset;
  unsigned int regno, num_x_saved = 0;

  frame = &cfun->machine->frame;

  memset (frame, 0, sizeof (*frame));

  /* Find out which scalar regs we need to save.  */
  num_x_saved = 0;
  for (regno = 0; regno <= LAST_REAL_REGISTER; regno++)
  {
    if (wi23_callee_saved_regno_p (regno)) {
      frame->mask |= (uint64_t)((1UL) << regno);
      num_x_saved++;
    }
  }

  /* At the bottom of the frame are any outgoing stack arguments.  */
  offset = wi23_stack_align (crtl->outgoing_args_size);

  /* Next are local stack variables.  */
  /* TODO(m): For some functions (e.g. __negdi2) we get an offset here even
     though there are no local stack variables.  */
  offset += wi23_stack_align (get_frame_size ());

  /* The virtual frame pointer points above the local variables.  */
  frame->frame_pointer_offset = offset;

  /* Next are the callee-saved scalar regs.  */
  if (frame->mask)
    {
      unsigned x_save_size = wi23_stack_align (num_x_saved * UNITS_PER_WORD);
      offset += x_save_size;
    }
  frame->reg_sp_offset = offset - UNITS_PER_WORD;

  /* The hard frame pointer points above the callee-saved GPRs.  */
  frame->hard_frame_pointer_offset = offset;

  /* Next is the callee-allocated area for pretend stack arguments.  */
  offset += wi23_stack_align (crtl->args.pretend_args_size);

  /* Arg pointer must be below pretend args, but must be above alignment
     padding.  */
  frame->arg_pointer_offset = offset - crtl->args.pretend_args_size;
  frame->total_size = offset;
}

static int
wi23_num_saved_regs (struct wi23_frame_info *frame)
{
  int num_saved = 0;
  for (int regno = 0; regno <= LAST_REAL_REGISTER; regno++)
    if (BITSET_P (frame->mask, regno))
      ++num_saved;
  return num_saved;
}

/* Emit a move from SRC to DEST.  Assume that the move expanders can
   handle all moves if !can_create_pseudo_p ().  The distinction is
   important because, unlike emit_move_insn, the move expanders know
   how to force Pmode objects into the constant pool even when the
   constant pool address is not itself legitimate.  */

static rtx
wi23_emit_move (rtx dest, rtx src)
{
  return (can_create_pseudo_p ()
	  ? emit_move_insn (dest, src)
	  : emit_move_insn_1 (dest, src));
}

/* Make the last instruction frame-related and note that it performs
   the operation described by FRAME_PATTERN.  */

static void
wi23_set_frame_expr (rtx frame_pattern)
{
  rtx insn;

  insn = get_last_insn ();
  RTX_FRAME_RELATED_P (insn) = 1;
  REG_NOTES (insn) = alloc_EXPR_LIST (REG_FRAME_RELATED_EXPR,
				      frame_pattern,
				      REG_NOTES (insn));
}

/* Return a frame-related rtx that stores REG at MEM.
   REG must be a single register.  */

static rtx
wi23_frame_set (rtx mem, rtx reg)
{
  rtx set = gen_rtx_SET (mem, reg);
  RTX_FRAME_RELATED_P (set) = 1;
  return set;
}

static void
wi23_save_reg (rtx reg, rtx mem)
{
  wi23_emit_move (mem, reg);
  wi23_set_frame_expr (wi23_frame_set (mem, reg));
}

/* Restore register REG from MEM.  */

static void
wi23_restore_reg (rtx reg, rtx mem)
{
  rtx insn = wi23_emit_move (reg, mem);
  rtx dwarf = NULL_RTX;
  dwarf = alloc_reg_note (REG_CFA_RESTORE, reg, dwarf);

  if (epilogue_cfa_sp_offset && REGNO (reg) == HARD_FRAME_POINTER_REGNUM)
    {
      rtx cfa_adjust_rtx = gen_rtx_PLUS (Pmode, stack_pointer_rtx,
					 GEN_INT (epilogue_cfa_sp_offset));
      dwarf = alloc_reg_note (REG_CFA_DEF_CFA, cfa_adjust_rtx, dwarf);
    }

  REG_NOTES (insn) = dwarf;
  RTX_FRAME_RELATED_P (insn) = 1;
}

/* Add a constant offset of any distance to a frame related pointer
   (usually the stack pointer or the frame pointer). */

static void
wi23_emit_frame_addi (rtx dst_rtx, rtx src_rtx, const int offset)
{
  rtx insn;

  if (IMM16_S (offset))
    {
      insn = gen_add3_insn (dst_rtx, src_rtx, GEN_INT (offset));
      RTX_FRAME_RELATED_P (emit_insn (insn)) = 1;
    }
  else
    {
      gcc_unreachable();
    }
}

void
wi23_expand_prologue (void)
{
  struct wi23_frame_info *frame = &cfun->machine->frame;
  HOST_WIDE_INT size = frame->total_size;
  int num_saved_regs;
  rtx insn;

  if (flag_stack_usage_info)
    current_function_static_stack_size = size;
  
  if (size > 0) {
    insn = gen_add3_insn (stack_pointer_rtx,
        stack_pointer_rtx,
        GEN_INT (-size));
    RTX_FRAME_RELATED_P (emit_insn (insn)) = 1;
  }

  /* Save the registers.  */
  num_saved_regs = wi23_num_saved_regs (frame);
  if (num_saved_regs > 0) {
    // total_size could be too big to fit in immediate. but we're not
    // going to worry about that. :)

    /* Save the scalar registers. */
    HOST_WIDE_INT offset = frame->reg_sp_offset;
    for (int regno = LAST_REAL_REGISTER; regno >= 0; regno--) {
      if (BITSET_P (frame->mask, regno))
      {
        auto m = (regno >= FP_OFS) ? E_SFmode : word_mode;
        rtx mem =
          gen_frame_mem (m, plus_constant (Pmode,
                    stack_pointer_rtx,
                    offset));
        wi23_save_reg (gen_rtx_REG (m, regno), mem);
        offset -= UNITS_PER_WORD;
      }
    }
  }

  /* Set up the frame pointer, if we're using one.  */
  if (frame_pointer_needed)
  {
    HOST_WIDE_INT offset = frame->hard_frame_pointer_offset;
    wi23_emit_frame_addi (hard_frame_pointer_rtx, stack_pointer_rtx,
            offset);
  }

}

void
wi23_expand_epilogue (void)
{
  struct wi23_frame_info *frame = &cfun->machine->frame;
  HOST_WIDE_INT size = frame->total_size;
  rtx insn;
  int num_saved_regs;

  /* Reset the epilogue cfa info before starting to emit the epilogue.  */
  epilogue_cfa_sp_offset = 0;

  /* Move past any dynamic stack allocations.  */
  // REMOVED: WI23 does not support alloca

  /* Restore the registers.  */
  num_saved_regs = wi23_num_saved_regs (frame);
  if (num_saved_regs > 0) {
    HOST_WIDE_INT offset = frame->reg_sp_offset - (num_saved_regs - 1) * UNITS_PER_WORD;
    for (int regno = 0; regno <= LAST_REAL_REGISTER; regno++) {
      if (BITSET_P (frame->mask, regno)) {
        auto m = (regno >= FP_OFS) ? E_SFmode : word_mode;
        rtx mem =
          gen_frame_mem (m, plus_constant (Pmode,
                    stack_pointer_rtx,
                    offset));
        wi23_restore_reg (gen_rtx_REG (m, regno), mem);
        offset += UNITS_PER_WORD;
      }
    }
  }

  /* Adjust stack pointer.  */
  if (size > 0) {
    if (frame_pointer_needed) {
      epilogue_cfa_sp_offset = size;
    }
    insn = emit_insn (
        gen_add3_insn (stack_pointer_rtx, stack_pointer_rtx,
          GEN_INT(size)));

    rtx dwarf = NULL_RTX;
    rtx cfa_adjust_rtx = gen_rtx_PLUS (Pmode, stack_pointer_rtx, const0_rtx);
    dwarf = alloc_reg_note (REG_CFA_DEF_CFA, cfa_adjust_rtx, dwarf);
    RTX_FRAME_RELATED_P (insn) = 1;

    REG_NOTES (insn) = dwarf;
  }

  /* If this function uses eh_return, add the final stack adjustment now.  */
  if (crtl->calls_eh_return)
  {
    emit_insn (gen_add3_insn (stack_pointer_rtx, stack_pointer_rtx,
      EH_RETURN_STACKADJ_RTX));
  }
}

int
wi23_initial_elimination_offset (int from, int to)
{
  HOST_WIDE_INT src, dest;

  wi23_compute_frame ();

  if (to == HARD_FRAME_POINTER_REGNUM)
    dest = cfun->machine->frame.hard_frame_pointer_offset;
  else if (to == STACK_POINTER_REGNUM)
    dest = 0; /* The stack pointer is the base of all offsets, hence 0.  */
  else
    gcc_unreachable ();

  if (from == FRAME_POINTER_REGNUM)
    src = cfun->machine->frame.frame_pointer_offset;
  else if (from == ARG_POINTER_REGNUM)
    src = cfun->machine->frame.arg_pointer_offset;
  else
    gcc_unreachable ();

  return src - dest;
}

/* Helper function for `wi23_legitimate_address_p'.  */

bool
wi23_regno_ok_for_base_p (int regno, bool strict_p)
{
  if (!HARD_REGISTER_NUM_P (regno))
    {
      if (!strict_p)
	return true;

      if (!reg_renumber)
	return false;

      regno = reg_renumber[regno];
    }

  /* The fake registers will be eliminated to either the stack or
     hard frame pointer, both of which are usually valid base registers.
     Reload deals with the cases where the eliminated form isn't valid.  */
  return ((regno >= WI23_R0 && regno < FP_OFS)
	  || regno == WI23_SP
	  || regno == FRAME_POINTER_REGNUM
	  || regno == ARG_POINTER_REGNUM);
}

static bool
wi23_rtx_ok_for_base_p (rtx x, bool strict_p)
{
  return REG_P (x) && wi23_regno_ok_for_base_p (REGNO (x), strict_p);
}

/* Worker function for TARGET_LEGITIMATE_ADDRESS_P.  */

static bool
wi23_legitimate_address_p (machine_mode mode ATTRIBUTE_UNUSED,
			    rtx x, bool strict_p,
			    addr_space_t as)
{
  if (GET_CODE(x) == PLUS
      && REG_P (XEXP (x, 0))
      && wi23_rtx_ok_for_base_p (XEXP (x, 0), strict_p)
      && CONST_INT_P (XEXP (x, 1))
      && IN_RANGE (INTVAL (XEXP (x, 1)), -32768, 32767)) {
    return true;
  }
  if (REG_P (x) && wi23_rtx_ok_for_base_p (x, strict_p)) {
    return true;
  } 
  if (GET_CODE (x) == SYMBOL_REF
      || GET_CODE (x) == LABEL_REF
      || GET_CODE (x) == CONST) {
    return true;
  }
    
  return false;
}

/* Make ADDR suitable for use as a call or sibcall target.  */

rtx
wi23_legitimize_call_address (rtx addr)
{
  if (!wi23_call_insn_operand (addr, VOIDmode))
    {
      return copy_addr_to_reg (addr);
    }
  return addr;
}

// TODO: deal with varargs later

// TODO add floating point reg support for both of these functions
static rtx
wi23_function_arg (cumulative_args_t cum_v, const function_arg_info &arg)
{
  CUMULATIVE_ARGS *cum = get_cumulative_args (cum_v);

  if (arg.mode == SFmode) {
    if (cum->num_fprs < 9)
      return gen_rtx_REG (arg.mode, FP_ARG_FIRST + cum->num_fprs);
    else 
      return NULL_RTX;
  } else {
    if (cum->num_gprs < 9)
      return gen_rtx_REG (arg.mode, cum->num_gprs);
    else 
      return NULL_RTX;
  }

}

static void
wi23_function_arg_advance (cumulative_args_t cum_v,
			    const function_arg_info &arg)
{
  CUMULATIVE_ARGS *cum = get_cumulative_args (cum_v);
  HOST_WIDE_INT param_size = arg.promoted_size_in_bytes() ;
  gcc_assert (param_size >= 0);

  /* Convert to words (round up).  */
  param_size = (UNITS_PER_WORD - 1 + param_size) / UNITS_PER_WORD;

  if (arg.mode == SFmode) {
    if (cum->num_fprs + param_size > 9)
      cum->num_fprs = 9;
    else
      cum->num_fprs += param_size;
  } else {
    if (cum->num_gprs + param_size > 9)
      cum->num_gprs = 9;
    else
      cum->num_gprs += param_size;
  }

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
  if ((i & 0xFFFF8000) == 0 || high)
  {
      if (high) {
        sprintf (pattern, "slbi  %%0,%d // low immediate high", i);
      } else {
        sprintf (pattern, "lbi  %%0,%d // low immediate", i);
      }
      output_asm_insn (pattern, &dst);
  }
  else
  {
      sprintf (pattern, "lbi  %%0,%d // high immediate", i >> 16);
      output_asm_insn (pattern, &dst);
      wi23_load_immediate (dst, i & 0xFFFF, true);
   }

  return "";
}

/* Jump to LABEL if (CODE OP0 OP1) holds.  */

void
wi23_expand_conditional_branch (rtx label, rtx_code code, rtx op0, rtx op1)
{
  op0 = force_reg (word_mode, op0);
  rtx condition;
  if (op1 != const0_rtx || ((op1 == const0_rtx) && ((code == GT) || (code == LE)))) {
    rtx temp;
    bool inverted = false;
    op1 = force_reg (word_mode, op1);
    temp = gen_reg_rtx (word_mode);
    if (code == GT || code == GE || code == GTU || code == GEU || code == NE) {
      code = reverse_condition(code);
      inverted = true;
    }
    emit_move_insn (temp, gen_rtx_fmt_ee (code, word_mode, op0, op1));
    op0 = temp;
    op1 = const0_rtx;
    if (inverted) {
      condition = gen_rtx_fmt_ee (EQ, VOIDmode, op0, op1);  
    } else {
      condition = gen_rtx_fmt_ee (NE, VOIDmode, op0, op1);  
    }
  } else {
    condition = gen_rtx_fmt_ee (code, VOIDmode, op0, op1);
  }
  
  emit_jump_insn (gen_rtx_SET (pc_rtx,
		     gen_rtx_IF_THEN_ELSE (VOIDmode, condition, gen_rtx_LABEL_REF (VOIDmode, label), pc_rtx)));
}

// same shit except float
void
wi23_expand_conditional_fbranch (rtx label, rtx_code code, rtx op0, rtx op1)
{
  op0 = force_reg (SFmode, op0);
  rtx condition;
  rtx temp;
  bool inverted = false;
  op1 = force_reg (SFmode, op1);
  temp = gen_reg_rtx (word_mode);
  if (code == GT || code == GE || code == NE) {
    code = reverse_condition(code);
    inverted = true;
  }
  emit_move_insn (temp, gen_rtx_fmt_ee (code, word_mode, op0, op1));
  // now for the integer branch part
  op0 = temp;
  op1 = const0_rtx;
  if (inverted) {
    condition = gen_rtx_fmt_ee (EQ, VOIDmode, op0, op1);  
  } else {
    condition = gen_rtx_fmt_ee (NE, VOIDmode, op0, op1);  
  }
  
  emit_jump_insn (gen_rtx_SET (pc_rtx,
		     gen_rtx_IF_THEN_ELSE (VOIDmode, condition, gen_rtx_LABEL_REF (VOIDmode, label), pc_rtx)));
}


static bool
wi23_hard_regno_mode_ok (unsigned int regno, machine_mode mode)
{
  
  if ((GET_MODE_CLASS (mode) != MODE_FLOAT) && (REGNO_REG_CLASS(regno) == FLOAT_REGS)) {
    return false;
  }

  return true;
}

static bool
wi23_can_change_mode_class (machine_mode from, machine_mode to,
			    reg_class_t rclass)
{
  return !reg_classes_intersect_p (FLOAT_REGS, rclass);
}

bool wi23_cannot_force_const_mem(machine_mode m, rtx r) {
  return false;
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
#undef  TARGET_FUNCTION_ARG
#define TARGET_FUNCTION_ARG		wi23_function_arg
#undef  TARGET_FUNCTION_ARG_ADVANCE
#define TARGET_FUNCTION_ARG_ADVANCE	wi23_function_arg_advance

//#undef TARGET_LRA_P
//#define TARGET_LRA_P hook_bool_void_false

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

#undef TARGET_HARD_REGNO_MODE_OK
#define TARGET_HARD_REGNO_MODE_OK wi23_hard_regno_mode_ok

//#undef TARGET_CAN_CHANGE_MODE_CLASS
//#define TARGET_CAN_CHANGE_MODE_CLASS wi23_can_change_mode_class

#undef  TARGET_CANNOT_FORCE_CONST_MEM
#define TARGET_CANNOT_FORCE_CONST_MEM wi23_cannot_force_const_mem

struct gcc_target targetm = TARGET_INITIALIZER;

#include "gt-wi23.h"
