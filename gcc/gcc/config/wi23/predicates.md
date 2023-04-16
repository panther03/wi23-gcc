;; Predicate definitions for Moxie
;; Copyright (C) 2009-2022 Free Software Foundation, Inc.
;; Contributed by Anthony Green <green@moxielogic.com>

;; This file is part of GCC.

;; GCC is free software; you can redistribute it and/or modify it
;; under the terms of the GNU General Public License as published
;; by the Free Software Foundation; either version 3, or (at your
;; option) any later version.

;; GCC is distributed in the hope that it will be useful, but WITHOUT
;; ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
;; or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
;; License for more details.

;; You should have received a copy of the GNU General Public License
;; along with GCC; see the file COPYING3.  If not see
;; <http://www.gnu.org/licenses/>.

;; -------------------------------------------------------------------------
;; Predicates
;; -------------------------------------------------------------------------

;; Nonzero if OP can be source of a simple move operation.

(define_predicate "wi23_general_movsrc_operand"
  (match_code "mem,const_int,reg,subreg,symbol_ref,label_ref,const")
{
  /* Any (MEM LABEL_REF) is OK.  That is a pc-relative load.  */
  if (MEM_P (op) && GET_CODE (XEXP (op, 0)) == LABEL_REF)
    return 1;

  if (MEM_P (op)
      && GET_CODE (XEXP (op, 0)) == PLUS
      && GET_CODE (XEXP (XEXP (op, 0), 0)) == REG
      && GET_CODE (XEXP (XEXP (op, 0), 1)) == CONST_INT
      && IMM16_S (INTVAL (XEXP (XEXP (op, 0), 1))))
    return 1;

  return general_operand (op, mode);
})

(define_predicate "const0_operand"
  (and (match_code "const_int,const_wide_int")
       (match_test "op == CONST0_RTX (mode)")))

(define_predicate "reg_or_0_operand"
  (ior (match_operand 0 "register_operand")
       (match_operand 0 "const0_operand")))

(define_predicate "reg_or_u16_operand"
  (if_then_else (match_code "const_int")
    (match_test "IMM16_Z (INTVAL(op))")
    (match_operand 0 "register_operand")))

(define_predicate "reg_or_s16_operand"
  (if_then_else (match_code "const_int")
    (match_test "IMM16_S (INTVAL(op))")
    (match_operand 0 "register_operand")))

(define_predicate "shft_reg_or_u5_operand"
  (if_then_else (match_code "const_int")
    (match_test "IMM_SHIFT (INTVAL(op))")
    (match_operand 0 "register_operand")))

(define_predicate "wi23_call_insn_operand"
  (ior (match_code "symbol_ref")
       (match_operand 0 "register_operand")
       (and (match_test "satisfies_constraint_B(op)")
       (match_test "satisfies_constraint_W(op)"))))