/* Disassemble wi23 instructions.
   Copyright (C) 2009-2023 Free Software Foundation, Inc.

   This file is part of the GNU opcodes library.

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   It is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "sysdep.h"
#include <stdio.h>

#define STATIC_TABLE
#define DEFINE_TABLE

#include "opcode/wi23.h"
#include "disassemble.h"

static fprintf_ftype fpr;
static void *stream;

/* Macros to extract operands from the instruction word.  */
#define OP_RS(i)   (XT(i, 21, 5))
#define OP_RD_I(i) (XT(i, 16, 5))
#define OP_RT_R(i) (XT(i, 16, 5))
#define OP_RD_R(i) (XT(i, 11, 5))
#define IMM_SX(i)  (XTS(i, 0, 16))
#define IMM_ZX(i)  (XT(i, 0, 16))
#define D26(i)     (XTS(i, 0, 26))

static const char * ireg_names[32] =
  { "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
    "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15",
    "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23",
    "r24", "r25", "r26", "gp", "fp", "sp", "ra", "csr" };

static const char * freg_names[32] =
  { "f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7",
    "f8", "f9", "f10", "f11", "f12", "f13", "f14", "f15",
    "f16", "f17", "f18", "f19", "f20", "f21", "f22", "f23",
    "f24", "f25", "f26", "f27", "f28", "f29", "f30", "fcsr" };

int
print_insn_wi23 (bfd_vma addr, struct disassemble_info * info)
{
  int status;
  stream = info->stream;
  const wi23_opc_info_t* opcode;
  const wi23_fnc_info_t* fncode;
  bfd_byte buffer[4];
  unsigned int iword;
  unsigned imm_us = 0;
  signed imm_s = 0;
  unsigned disp = 0;

  const char* rs;
  const char* rd;
  fpr = info->fprintf_func;

  if ((status = info->read_memory_func (addr, buffer, 4, info)))
    goto fail;

  iword = bfd_getb32 (buffer);

  // Extract
  opcode = &wi23_opc_info[XT(iword, 26, 6)];
  switch (opcode->itype)
	{

  ///////////////
  // R-Format //
  //////////////

	case WI23_RF_F_FN_DST:
    fncode = &wi23_float_fnc[XT(iword, 0, 2)];
    fpr (stream, "%s\t%s,%s,%s",
      fncode->name,
      freg_names[OP_RD_R(iword)],
      freg_names[OP_RS(iword)],
      freg_names[OP_RT_R(iword)]);
	  break;
  case WI23_RF_I_FN_DST:
    if (opcode->opcode == 0x1A) {
      fncode = &wi23_shift_fnc[XT(iword, 0, 2)];
    } else { // 0x1B
      fncode = &wi23_arith_fnc[XT(iword, 0, 2)]; break; // 1B
    }
    fpr (stream, "%s\t%s,%s,%s",
      fncode->name,
      ireg_names[OP_RD_R(iword)],
      ireg_names[OP_RS(iword)],
      ireg_names[OP_RT_R(iword)]);
	  break;
  case WI23_RF_F_DST:
    fpr (stream, "%s\t%s,%s,%s",
      opcode->name,
      freg_names[OP_RD_R(iword)],
      freg_names[OP_RS(iword)],
      freg_names[OP_RT_R(iword)]);
	  break;
  case WI23_RF_I_DST:
    fpr (stream, "%s\t%s,%s,%s",
      opcode->name,
      ireg_names[OP_RD_R(iword)],
      ireg_names[OP_RS(iword)],
      ireg_names[OP_RT_R(iword)]);
	  break;
  case WI23_RF_DS:
    // too many cases to encode directly
    
    switch (opcode->opcode) {
      case 0x20: // icvtf
      case 0x22: // imovf
      case 0x24: // fclass
        rs = freg_names[OP_RS(iword)];
        rd = ireg_names[OP_RD_R(iword)];
        break;
      case 0x21: // fcvti
      case 0x23: // fmovi
        rs = ireg_names[OP_RS(iword)];
        rd = freg_names[OP_RD_R(iword)];
        break;
      default: // btr
        rs = ireg_names[OP_RS(iword)];
        rd = ireg_names[OP_RD_R(iword)];
        break;
    }
    fpr (stream, "%s\t%s,%s",
      opcode->name,
      rd,
      rs);
	  break;

  ///////////////
  // I-Format //
  //////////////

	case WI23_IF_DSI_Z:
    imm_us = IMM_ZX(iword);
    fpr (stream, "%s\t%s, %s, %x", opcode->name,
	       ireg_names[OP_RD_I(iword)],
	       ireg_names[OP_RS(iword)],
         imm_us);
	  break;
	case WI23_IF_DSI_S:
    if (opcode->opcode == 0x30 || opcode->opcode == 0x31) {
      rd = freg_names[OP_RD_I(iword)];
    } else {
      rd = ireg_names[OP_RD_I(iword)];
    }
    imm_s = IMM_SX(iword);
    fpr (stream, "%s\t%s, %s, %d", opcode->name,
	       rd,
	       ireg_names[OP_RS(iword)],
         imm_s);
	  break;
  case WI23_IF_DSI_5:
    imm_us = XT(iword, 0, 5);
    fpr (stream, "%s\t%s, %s, %d", opcode->name,
	       ireg_names[OP_RD_I(iword)],
	       ireg_names[OP_RS(iword)],
         imm_us);
	  break;
  case WI23_IF_SI_Z:
    imm_us = IMM_ZX(iword);
    fpr (stream, "%s\t%s, %x", opcode->name,
	       ireg_names[OP_RS(iword)],
         imm_us);
	  break;
  case WI23_IF_SI_S:
    imm_s = IMM_SX(iword);
    fpr (stream, "%s\t%s, %d", opcode->name,
	       ireg_names[OP_RS(iword)],
         imm_s);
	  break;
  case WI23_IF_SI_PC:
    imm_us = IMM_ZX(iword);
    fpr (stream, "%s\t%s, ", opcode->name, ireg_names[OP_RS(iword)]);
    info->print_address_func (addr + imm_us + 4, info);
	  break;
  case WI23_IF_I:
    imm_us = IMM_ZX(iword);
    fpr (stream, "%s\t%x", opcode->name, imm_us);
	  break;

  ///////////////
  // J-Format //
  //////////////

  case WI23_JF_D26:
    disp = IMM_ZX(iword);
    fpr (stream, "%s\t", opcode->name);
    info->print_address_func (addr + disp + 4, info);
    break;

  /////////////////////////////////////////
  // Other & illegal (handled the same) //
  ///////////////////////////////////////

  case WI23_OTHER:
  case WI23_ILLEGAL: // ILEGAL
    fpr (stream, "%s", opcode->name);
    break;
	default:
	  abort();

	}

  return 4;

 fail:
  info->memory_error_func (status, addr, info);
  return -1;
}
