// A setjmp.c for wi23
//   Copyright (C) 2009, 2019  Anthony Green
//
//   The authors hereby grant permission to use, copy, modify, distribute,
//   and license this software and its documentation for any purpose, provided
//   that existing copyright notices are retained in all copies and that this
//   notice is included verbatim in any distributions. No written agreement,
//   license, or royalty fee is required for any of the authorized uses.
//   Modifications to this software may be copyrighted by their authors
//   and need not follow the licensing terms described here, provided that
//   the new terms are clearly indicated on the first page of each file where
//   they apply. 

// UNIMPLEMENTED

        .text
        .global setjmp
        .type   setjmp,@function
setjmp:
	halt
.Lend1:
	.size	setjmp,.Lend1-setjmp

	.global	longjmp
	.type	longjmp,@function
longjmp:
	halt
.Lend2:
	.size	longjmp,.Lend2-longjmp
