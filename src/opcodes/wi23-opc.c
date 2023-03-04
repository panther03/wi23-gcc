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
    { 0x00, "add" },
    { 0x01, "sub" },
    { 0x02, "xor" },
    { 0x03, "andn" }
  };

const wi23_fnc_info_t wi23_shift_fnc[4] =
  {
    { 0x00, "rol" },
    { 0x01, "sll" },
    { 0x02, "ror" },
    { 0x03, "srl" }
  };

const wi23_fnc_info_t wi23_float_fnc[4] =
  {
    { 0x00, "fadd" },
    { 0x01, "fsub" },
    { 0x02, "fmul" },
    { 0x03, "fdiv" }
  };