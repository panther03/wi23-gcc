/* Definitions for decoding wi23 opcodes.
   Copyright (C) 2009-2023 Free Software Foundation, Inc.
   Contributed by Julien de Castelnau
   Based off moxie.h

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */

/* WI-23 has three instruction formats: I-format, J-format, R-format, Other. Each has different variations

  R-format:

    Some use a destination, source, target, w/ FN   (WI23_RF_FN_DST)
    Some use a destination, source, target, w/o FN  (WI23_RF_DST)
    Some use a destination, source, w/o FN          (WI23_RF_DS)

  I-format:

    Some use a destination, source, 16-bit imm      (WI23_IF_DSI)
    Some use a source, 16-bit imm                   (WI23_IF_SI)
    Some use only 16-bit imm                        (WI23_IF_I)

  J-format:

    26-bit displacement                             (WI23_JF_D26)

  Other:

    No arguments                                    (WI23_OTHER)    
 */

// R-format
#define WI23_RF_I_FN_DST 0x100 // (Integer) Destination Source Target with Function code
#define WI23_RF_I_DST    0x101 // (Integer) Destination Source Target
#define WI23_RF_F_FN_DST 0x110 // (Float) Destination Source Target with Function code
#define WI23_RF_F_DST    0x111 // (Float) Destination Source Target
#define WI23_RF_DS       0x102 // Destination Source


// I-format
#define WI23_IF_DSI_Z  0x200 // Destination Source Immediate, zero-extended
#define WI23_IF_DSI_S  0x201 // Destination Source Immediate, sign-extended
#define WI23_IF_DSI_5  0x202 // Destination Source Immediate lowest 5 bits (for shift)
#define WI23_IF_SI_Z   0x203 // Source Immediate, zero-extended
#define WI23_IF_SI_S   0x204 // Source Immediate, sign-extended
#define WI23_IF_SI_PC  0x205 // Source Immediate, pc-relative (sign extended)
#define WI23_IF_I      0x206 // Immediate

// J-format
#define WI23_JF_D26    0x300 // 26-bit displacement

// Other
#define WI23_OTHER     0x400

// Illegal instruction
#define WI23_ILLEGAL   0xFFF

// helper macros from riscv.h
// eXTract field
// x = input value
// s = start bit of field (numbered from 0)
// n = length of field
#define XT(x, s, n)  (((x) >> (s)) & ((1 << (n)) - 1))

// eXTract field Signed
// x = input value
// s = start bit of field (numbered from 0)
// n = length of field
#define XTS(x, s, n) (XT(x, s, n) | ((-(XT(x, (s + n - 1), 1))) << (n)))

typedef struct wi23_opc_info_t
{
  short         opcode;
  unsigned      itype;
  const char *  name;
} wi23_opc_info_t;

typedef struct wi23_fnc_info_t
{
  short         fncode;
  const char *  name;
} wi23_fnc_info_t;

extern const wi23_opc_info_t wi23_opc_info[64];
extern const wi23_fnc_info_t wi23_arith_fnc[4];
extern const wi23_fnc_info_t wi23_shift_fnc[4];
extern const wi23_fnc_info_t wi23_float_fnc[4];
