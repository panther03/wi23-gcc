/* Integer division routines for RISC-V.

   Copyright (C) 2016-2022 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */

#include "wi23-asm.h"

  .text
  .align 4

FUNC_BEGIN (__divsi3)
  bltz  r0, .L10
  bltz  r1, .L11
  /* Since the quotient is positive, fall into __udivdi3.  */

FUNC_BEGIN (__udivsi3)
  mov    r2, r1 // r1 = quotient
  mov    r1, r0 // r0 = divisor
  lbi    r0, -1
  beqz   r2, .L5
  lbi    r3, 1
  sleu   tmp, r1, r2
  bnez   tmp, .L2
.L1:
// WI-23 is definitely top 10 worst ISAs
  lbi tmp, 0
  sle tmp,r2,tmp
  bnez tmp, .L2
  slli  r2, r2, 1
  slli  r3, r3, 1
  sltu  tmp, r2, r1
  bnez  tmp, .L1
.L2:
  lbi r0, 0
.L3:
  sltu  tmp, r1, r2
  bnez  tmp, .L4
  sub   r1, r2, r1
  or    r0, r0, r3
.L4:
  srli  r3, r3, 1
  srli  r2, r2, 1
  bnez  r3, .L3
.L5:
  mov r9, r0
  ret
HIDDEN_DEF (__udivsi3)

FUNC_BEGIN (__umodsi3)
  /* Call __udivsi3(r0, r1), then return the remainder, which is in r1.  */
  mov  r10, ra
  jal   HIDDEN_JUMPTARGET(__udivsi3)
  mov  r9, r1
  jr   r10, 0

  /* Handle negative arguments to __divsi3.  */
.L10:
  subi   r0, r0, 0
  /* Zero is handled as a negative so that the result will not be inverted.  */
  lbi tmp, 0
  slt tmp,tmp,r1
  bnez tmp, .L12 /* Compute __udivsi3(-a0, a1), then negate the result.  */

  subi   r1, r1, 0
  j     HIDDEN_JUMPTARGET(__udivsi3)     /* Compute __udivsi3(-a0, -a1).  */
.L11:                /* Compute __udivsi3(a0, -a1), then negate the result.  */
  subi   r1, r1, 0
.L12:
  mov  r10, ra
  jal   HIDDEN_JUMPTARGET(__udivsi3)
  subi   r9, r9, 0
  jr    r10, 0

FUNC_BEGIN (__modsi3)
  mov   r10, ra
  bltz   r1, .L31
  bltz   r0, .L32
.L30:
  jal    HIDDEN_JUMPTARGET(__udivsi3)    /* The dividend is not negative.  */
  mov   r9, r1
  jr     r10, 0
.L31:
  subi    r1, r1, 0
  bgez    r0, .L30
.L32:
  subi    r0, r0, 0
  jal    HIDDEN_JUMPTARGET(__udivsi3)    /* The dividend is hella negative.  <- ???? */
  subi    r9, r1, 0
  jr     tmp, 0