;; Constraint definitions for wi23
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
;; Constraints
;; -------------------------------------------------------------------------

(define_register_constraint "f" "FLOAT_REGS"
  "A floating-point register (if available).")

(define_constraint "A"
  "An absolute address."
  (and (match_code "mem")
       (ior (match_test "GET_CODE (XEXP (op, 0)) == SYMBOL_REF")
	    (match_test "GET_CODE (XEXP (op, 0)) == LABEL_REF")
	    (match_test "GET_CODE (XEXP (op, 0)) == CONST"))))

(define_constraint "T"
  "a symbol or smth idrk"
  (and (match_test "satisfies_constraint_i(op)")
    (ior (match_code "label_ref")
    (match_code "symbol_ref"))))

(define_memory_constraint "B"
  "An offset address."
  (and (match_code "mem")
       (match_test "(!wi23_is_mem_pm(op))")
       (match_test "(GET_CODE (XEXP (op, 0)) == PLUS)")))

(define_memory_constraint "W"
  "A register indirect memory operand."
  (and (match_code "mem")
       (match_test "!wi23_is_mem_pm(op)
        && REG_P (XEXP (op, 0))
		    && REGNO_OK_FOR_BASE_P (REGNO (XEXP (op, 0)))")))

(define_memory_constraint "e"
  "An offset address."
  (and (match_code "mem")
       (match_test "wi23_is_mem_pm(op) && (
          (GET_CODE (XEXP (op, 0)) == SYMBOL_REF) ||
          (GET_CODE (XEXP (op, 0)) == LABEL_REF) ||
          (GET_CODE (XEXP (op, 0)) == CONST_INT) ||
          (GET_CODE (XEXP (op, 0)) == CONST))"
       )))

 (define_constraint "L"
   "An unsigned 5-bit constant (for shift counts)."
   (and (match_code "const_int")
        (match_test "IMM_SHIFT (ival)")))

(define_constraint "I"
  "A signed 16-bit constant (for arithmetic instructions)."
  (and (match_code "const_int")
       (match_test "IMM16_S (ival)")))

(define_constraint "J"
  "An unsigned 16-bit constant (for logical instructions)."
  (and (match_code "const_int")
       (match_test "IMM16_Z (ival)")))

(define_constraint "M"
  "A shifted unsigned 16-bit constant suitable for slbi"
  (and (match_code "const_int")
       (match_test "IMM16_UPPER (ival)")))

(define_constraint "O"
  "The constant zero"
  (and (match_code "const_int")
       (match_test "ival == 0")))