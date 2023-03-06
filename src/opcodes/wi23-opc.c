/* wi23-opc.c -- Definitions for wi23 opcodes.
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
   along with this file; see the file COPYING.  If not, write to the
   Free Software Foundation, 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "sysdep.h"
#include "opcode/wi23.h"

const char * ireg_names[32] =
  { "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
    "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15",
    "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23",
    "r24", "r25", "r26", "gp", "fp", "sp", "ra", "csr" };

const char * freg_names[32] =
  { "f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7",
    "f8", "f9", "f10", "f11", "f12", "f13", "f14", "f15",
    "f16", "f17", "f18", "f19", "f20", "f21", "f22", "f23",
    "f24", "f25", "f26", "f27", "f28", "f29", "f30", "fcsr" };

const wi23_opc_info_t wi23_opc_info[64] =
  {
    { 0x00, WI23_OTHER,       "halt" },
    { 0x01, WI23_OTHER,       "nop" },
    { 0x02, WI23_IF_I,        "irq" },
    { 0x03, WI23_IF_DSI_S,    "ldcr" },

    { 0x04, WI23_JF_D26,      "j" },
    { 0x05, WI23_IF_SI_S,     "jr" },
    { 0x06, WI23_JF_D26,      "jal" },
    { 0x07, WI23_IF_SI_S,     "jalr" },

    { 0x08, WI23_IF_DSI_S,    "addi" },
    { 0x09, WI23_IF_DSI_S,    "subi" },
    { 0x0A, WI23_IF_DSI_Z,    "xori" },
    { 0x0B, WI23_IF_DSI_Z,    "andni" },

    { 0x0C, WI23_IF_SI_PC,    "beqz" },
    { 0x0D, WI23_IF_SI_PC,    "bnez" },
    { 0x0E, WI23_IF_SI_PC,    "bltz" },
    { 0x0F, WI23_IF_SI_PC,    "bgez" },

    { 0x10, WI23_IF_DSI_S,    "st" },
    { 0x11, WI23_IF_DSI_S,    "ld" },
    { 0x12, WI23_IF_SI_Z,     "slbi" },
    { 0x13, WI23_IF_DSI_S,    "stu" },

    { 0x14, WI23_IF_DSI_5,    "roli" },
    { 0x15, WI23_IF_DSI_5,    "slli" },
    { 0x16, WI23_IF_DSI_5,    "rori" },
    { 0x17, WI23_IF_DSI_5,    "srli" },

    { 0x18, WI23_IF_SI_S,     "lbi" },
    { 0x19, WI23_RF_DS,       "btr" },
    { 0x1A, WI23_RF_I_FN_DST,   "" }, // shift
    { 0x1B, WI23_RF_I_FN_DST,   "" }, // arithmetic

    { 0x1C, WI23_RF_I_DST,      "seq" },
    { 0x1D, WI23_RF_I_DST,      "slt" },
    { 0x1E, WI23_RF_I_DST,      "sle" },
    { 0x1F, WI23_RF_I_DST,      "sco" },

    { 0x20, WI23_RF_DS,       "icvtf" },
    { 0x21, WI23_RF_DS,       "fcvti" },
    { 0x22, WI23_RF_DS,       "imovf" },
    { 0x23, WI23_RF_DS,       "fmovi" },
    { 0x24, WI23_RF_DS,       "fclass" },

    { 0x25, WI23_ILLEGAL,     "illegal" },
    { 0x26, WI23_ILLEGAL,     "illegal" },
    { 0x27, WI23_ILLEGAL,     "illegal" },
    { 0x28, WI23_ILLEGAL,     "illegal" },
    { 0x29, WI23_ILLEGAL,     "illegal" },
    { 0x2A, WI23_ILLEGAL,     "illegal" },
    { 0x2B, WI23_ILLEGAL,     "illegal" },
    { 0x2C, WI23_ILLEGAL,     "illegal" },
    { 0x2D, WI23_ILLEGAL,     "illegal" },
    { 0x2E, WI23_ILLEGAL,     "illegal" },
    { 0x2F, WI23_ILLEGAL,     "illegal" },

    { 0x30, WI23_IF_DSI_S,    "fst" },
    { 0x31, WI23_IF_DSI_S,    "fld" },

    { 0x32, WI23_ILLEGAL,     "illegal" },
    { 0x33, WI23_ILLEGAL,     "illegal" },
    { 0x34, WI23_ILLEGAL,     "illegal" },
    { 0x35, WI23_ILLEGAL,     "illegal" },
    { 0x36, WI23_ILLEGAL,     "illegal" },
    { 0x37, WI23_ILLEGAL,     "illegal" },
    { 0x38, WI23_ILLEGAL,     "illegal" },
    { 0x39, WI23_ILLEGAL,     "illegal" },
    { 0x3A, WI23_ILLEGAL,     "illegal" },

    { 0x3B, WI23_RF_F_FN_DST,   "" }, // float arithmetic
    { 0x3C, WI23_RF_F_DST,      "feq" },
    { 0x3E, WI23_RF_F_DST,      "fle" },
    { 0x3F, WI23_RF_F_DST,      "flt" }
  };

const wi23_fnc_info_t wi23_arith_fnc[4] =
  {
    { 0x00, 0x1B, "add" },
    { 0x01, 0x1B, "sub" },
    { 0x02, 0x1B, "xor" },
    { 0x03, 0x1B, "andn" }
  };

const wi23_fnc_info_t wi23_shift_fnc[4] =
  {
    { 0x00, 0x1A, "rol" },
    { 0x01, 0x1A, "sll" },
    { 0x02, 0X1A, "ror" },
    { 0x03, 0x1A, "srl" }
  };

const wi23_fnc_info_t wi23_float_fnc[4] =
  {
    { 0x00, 0x3A, "fadd" },
    { 0x01, 0x3A, "fsub" },
    { 0x02, 0X3A, "fmul" },
    { 0x03, 0x3A, "fdiv" }
  };