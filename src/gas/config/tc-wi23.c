/* tc-wi23.c -- Assemble code for wi23
   Copyright (C) 2009-2023 Free Software Foundation, Inc.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to
   the Free Software Foundation, 51 Franklin Street - Fifth Floor,
   Boston, MA 02110-1301, USA.  */

#include "as.h"
#include "safe-ctype.h"
#include "opcode/wi23.h"
#include "elf/wi23.h"

const char comment_chars[]        = "//";
const char line_separator_chars[] = ";";
const char line_comment_chars[]   = "//";

static int pending_reloc;
static htab_t opcode_hash_table = NULL;
static htab_t fncode_hash_table = NULL;
static htab_t regnames_hash_table = NULL;

#define VERY_BAD_CHECK_FOR_S_FREG(opcode) ((opcode->opcode == 0x20) || (opcode->opcode == 0x22) || (opcode->opcode == 0x24))
#define VERY_BAD_CHECK_FOR_D_FREG(opcode) ((opcode->opcode == 0x21) || (opcode->opcode == 0x23))

// Borrowed from tc-riscv.c

/* All WI-23 registers belong to one of these classes.  */
enum reg_class
{
  RCLASS_GPR,
  RCLASS_FPR,
  RCLASS_MAX
};

#define ENCODE_REG_HASH(cls, n) \
  ((void *)(uintptr_t)((n) * RCLASS_MAX + (cls) + 1))
#define DECODE_REG_CLASS(hash) (((uintptr_t)(hash) - 1) % RCLASS_MAX)
#define DECODE_REG_NUM(hash) (((uintptr_t)(hash) - 1) / RCLASS_MAX)

const pseudo_typeS md_pseudo_table[] =
{
  {0, 0, 0}
};

const char FLT_CHARS[] = "rRsSfFdDxXpP";
const char EXP_CHARS[] = "eE";

static valueT md_chars_to_number (char * buf, int n);

/* Byte order.  */
extern int target_big_endian;

void
md_operand (expressionS *op __attribute__((unused)))
{
  /* Empty for now. */
}

/* This function is called once, at assembler startup time.  It sets
   up the hash table with all the opcodes in it, and also initializes
   some aliases for compatibility with other assemblers.  */

void
md_begin (void)
{
  int count;

  ///////////////////
  // OPCODE TABLE //
  /////////////////

  opcode_hash_table = str_htab_create ();
  const wi23_opc_info_t *opcode;

  /* Insert names into hash table.  */
  for (count = 0, opcode = wi23_opc_info; count++ < 63; opcode++) {
    str_hash_insert (opcode_hash_table, opcode->name, opcode, 0);
  }

  ///////////////////
  // FNCODE TABLE //
  /////////////////
  // This exists to handle function codes in instructions
  // Because we can't know the full encoding of that instruction,
  // an instruction with function codes will fail lookup in the opcode table, and succeed here

  fncode_hash_table = str_htab_create ();
  const wi23_fnc_info_t *fncode;

  /* Insert names into hash table.  */
  for (count = 0, fncode = wi23_shift_fnc; count++ < 3; fncode++)
    str_hash_insert (fncode_hash_table, fncode->name, fncode, 0);
  for (count = 0, fncode = wi23_arith_fnc; count++ < 3; fncode++)
    str_hash_insert (fncode_hash_table, fncode->name, fncode, 0);
  for (count = 0, fncode = wi23_float_fnc; count++ < 3; fncode++)
    str_hash_insert (fncode_hash_table, fncode->name, fncode, 0);

  //////////////////////////
  // REGISTER NAME TABLE //
  ////////////////////////

  regnames_hash_table = str_htab_create ();
  void* hash;

  /* Insert names into hash table.  */
  for (count = 0; count < 32; count++) {
    hash = ENCODE_REG_HASH (RCLASS_GPR, count);
    str_hash_insert (regnames_hash_table, ireg_names[count], hash, 0);
  }
  for (count = 0; count < 32; count++) {
    hash = ENCODE_REG_HASH (RCLASS_FPR, count);
    str_hash_insert (regnames_hash_table, freg_names[count], hash, 0);
  }

  bfd_set_arch_mach (stdoutput, TARGET_ARCH, 0);

}

/* Parse an expression and then restore the input line pointer.  */

static char *
parse_exp_save_ilp (char *s, expressionS *op)
{
  char *save = input_line_pointer;

  input_line_pointer = s;
  expression (op);
  s = input_line_pointer;
  input_line_pointer = save;
  return s;
}

static unsigned int
parse_register_operand (char **ptr, enum reg_class class)
{
  void* reg;
  char *s = *ptr;
  char s_end_temp = 0;

  // Iterate through the register string until we have found the end of it (space, end of line or comma).
  for (; *s && !is_end_of_line[*s & 0xff] && *s != ' ' && *s != ','; s++);
  // Did s increase at all? if not, we did not get a register
  if (s == *ptr)
  {
    as_bad (_("expecting register"));
    ignore_rest_of_line ();
    return -1;
  }
  // We have some sort of string. 
  // Let's save this end character so we can restore it later
  s_end_temp = *s;
  // Now null terminate the string so we can search it in the hash table
  *s = 0;

  reg = str_hash_find (regnames_hash_table, *ptr);
  if (reg == NULL) {
    as_bad (_("illegal register `%s'"), *ptr);
    ignore_rest_of_line ();
    return -1;
  } else if (DECODE_REG_CLASS(reg) != class) {
    if (class == RCLASS_FPR) {
      as_bad (_("reg class mismatch, expected float got `%s'"), *ptr);
    } else if (class == RCLASS_GPR) {
      as_bad (_("reg class mismatch, expected integer got `%s'"), *ptr);
    } else {
      as_bad (_("something went wrong, unrecognized register class passed to parse_register_operand"));
    }
    ignore_rest_of_line ();
    return -1;
  }

  // We have a valid register.
  // Restore the termination character, set ptr to the end of the register string.
  *s = s_end_temp;
  *ptr = s;
  
  return DECODE_REG_NUM (reg);
}

/* This is the guts of the machine-dependent assembler.  STR points to
   a machine dependent instruction.  This function is supposed to emit
   the frags/bytes it assembles to.  */

void
md_assemble (char *str)
{
  char *op_start;
  char *op_end;

  wi23_opc_info_t *opcode;
  wi23_fnc_info_t *fncode = NULL;
  unsigned int itype = 0;
  char *p;
  char pend;

  unsigned int iword = 0;

  int nlen = 0;

  /* Drop leading whitespace.  */
  while (*str == ' ')
    str++;

  /* Find the op code end.  */
  op_start = str;
  for (op_end = str;
       *op_end && !is_end_of_line[*op_end & 0xff] && *op_end != ' ';
       op_end++)
    nlen++;

  pend = *op_end;
  *op_end = 0;

  if (nlen == 0)
    as_bad (_("can't find opcode "));
  opcode = (wi23_opc_info_t *) str_hash_find (opcode_hash_table, op_start);

  if (opcode == NULL)
  {
    // Attempt to look up in the function code table instead
    fncode = (wi23_fnc_info_t *) str_hash_find (fncode_hash_table, op_start);

    if (fncode == NULL) {
      as_bad (_("unknown opcode %s"), op_start);
      return;
    }
    itype = fncode->itype;
  } else {
    itype = opcode->itype;
  }  

  p = frag_more (4);

  printf("insn: type = %x, name = %s\n", itype, op_start);

  *op_end = pend;

  switch (itype)
  {
    /////////////////////////////
    // R-type - function code //
    ///////////////////////////
    case WI23_RF_F_FN_DST:
    case WI23_RF_I_FN_DST:
      if (fncode != NULL) {
        iword = (fncode->opcode << 26) | (fncode->fncode);
      } else {
        as_bad (_("bad news\n"));
      }
      
      
      // Skip whitespace after opcode
      while (ISSPACE (*op_end)) op_end++;

      {
        int dst, src, trg;
        dst = parse_register_operand (&op_end, (itype == WI23_RF_F_FN_DST) ? RCLASS_FPR : RCLASS_GPR);
        if (*op_end != ',')
          as_warn (_("expecting comma delimited register operands"));
        op_end++;
        src = parse_register_operand (&op_end, (itype == WI23_RF_F_FN_DST) ? RCLASS_FPR : RCLASS_GPR);
        if (*op_end != ',')
          as_warn (_("expecting comma delimited register operands"));
        op_end++;
        trg = parse_register_operand (&op_end, (itype == WI23_RF_F_FN_DST) ? RCLASS_FPR : RCLASS_GPR);
        iword |= (dst << 11) + (trg << 16) + (src << 21);
      }
      break;
    ////////////////////////////////
    // R-type - no function code //
    //////////////////////////////
    case WI23_RF_F_DST:
    case WI23_RF_I_DST:
      iword = (opcode->opcode << 26);
      
      // Skip whitespace after opcode
      while (ISSPACE (*op_end)) op_end++;

      {
        int dst, src, trg;
        dst = parse_register_operand (&op_end, (itype == WI23_RF_F_DST) ? RCLASS_FPR : RCLASS_GPR);
        if (*op_end != ',')
          as_warn (_("expecting comma delimited register operands"));
        op_end++;
        src = parse_register_operand (&op_end, (itype == WI23_RF_F_DST) ? RCLASS_FPR : RCLASS_GPR);
        if (*op_end != ',')
          as_warn (_("expecting comma delimited register operands"));
        op_end++;
        trg = parse_register_operand (&op_end, (itype == WI23_RF_F_DST) ? RCLASS_FPR : RCLASS_GPR);
        iword |= (dst << 11) + (trg << 16) + (src << 21);
      }
      break;
    /////////////////////////
    // R-type - no target //
    ///////////////////////
    case WI23_RF_DS:
      iword = (opcode->opcode << 26);
      
      // Skip whitespace after opcode
      while (ISSPACE (*op_end)) op_end++;

      {
        int dst, src;
        dst = parse_register_operand (&op_end, VERY_BAD_CHECK_FOR_D_FREG(opcode) ? RCLASS_FPR : RCLASS_GPR);
        if (*op_end != ',')
          as_warn (_("expecting comma delimited register operands"));
        op_end++;
        src = parse_register_operand (&op_end, VERY_BAD_CHECK_FOR_S_FREG(opcode) ? RCLASS_FPR : RCLASS_GPR);
        iword |= (dst << 11) + (src << 21);
        printf("IWORD !!! %x", iword);
      }
      break;
    //////////////////////////////////////////////
    // I-type - Destination, Source, Immediate //
    ////////////////////////////////////////////
    case WI23_IF_DSI_Z:
    case WI23_IF_DSI_S:
    case WI23_IF_DSI_5:
      iword = (opcode->opcode << 26);
      // Skip whitespace after opcode
      while (ISSPACE (*op_end)) op_end++;

      {
        expressionS arg;
        int dst, src;

        dst = parse_register_operand (&op_end, ((opcode->opcode == 0x30) || (opcode->opcode == 0x31)) ? RCLASS_FPR : RCLASS_GPR);
        if (*op_end != ',')
          as_warn (_("expecting comma delimited register operands"));
        op_end++;
        src = parse_register_operand (&op_end, RCLASS_GPR);

        while (ISSPACE (*op_end)) op_end++;
        if (*op_end != ',')
          as_warn (_("expecting comma delimited register operands"));
        op_end++;

        op_end = parse_exp_save_ilp (op_end, &arg);
        fix_new_exp (frag_now,
		     (p - frag_now->fr_literal + 2),
		     2,
		     &arg,
		     0,
		     BFD_RELOC_16);

        iword |= (dst << 16) + (src << 21);
      }
      break;

    /////////////////////////////////
    // I-type - Source, Immediate //
    ///////////////////////////////
    case WI23_IF_SI_Z:
    case WI23_IF_SI_S:
      iword = (opcode->opcode << 26);
      // Skip whitespace after opcode
      while (ISSPACE (*op_end)) op_end++;

      {
        expressionS arg;
        int src;

        src = parse_register_operand (&op_end, RCLASS_GPR);

        while (ISSPACE (*op_end)) op_end++;
        if (*op_end != ',')
          as_warn (_("expecting comma delimited register operands"));
        op_end++;

        op_end = parse_exp_save_ilp (op_end, &arg);
        fix_new_exp (frag_now,
          (p - frag_now->fr_literal + 2),
          2,
          &arg,
          0,
          BFD_RELOC_16);

        iword |= (src << 21);
      }
      break;

    /////////////////////////////////////////////
    // I-type - Source, Immediate PC-relative //
    ///////////////////////////////////////////
    case WI23_IF_SI_PC:
      iword = (opcode->opcode << 26);
      // Skip whitespace after opcode
      while (ISSPACE (*op_end)) op_end++;

      {
        expressionS arg;
        int src;

        src = parse_register_operand (&op_end, RCLASS_GPR);
        
        while (ISSPACE (*op_end)) op_end++;
        if (*op_end != ',')
          as_warn (_("expecting comma delimited register operands"));
        op_end++;

        op_end = parse_exp_save_ilp (op_end, &arg);
        fix_new_exp (frag_now,
          (p - frag_now->fr_literal + 2),
          2,
          &arg,
          true,
          BFD_RELOC_16_PCREL);

        iword |= (src << 21);
      }
      break;

    //////////////////////////////
    // I-type - Immediate Only //
    ////////////////////////////
    case WI23_IF_I:
      iword = (opcode->opcode << 26);
      // Skip whitespace after opcode
      while (ISSPACE (*op_end)) op_end++;

      {
        expressionS arg;

        op_end = parse_exp_save_ilp (op_end, &arg);
        fix_new_exp (frag_now,
          (p - frag_now->fr_literal + 2),
          2,
          &arg,
          0,
          BFD_RELOC_16);

      }
      break;

    ///////////////////////////////////
    // J-type - 26-bit displacement //
    /////////////////////////////////
    case WI23_JF_D26:
      iword = (opcode->opcode << 26);
      // Skip whitespace after opcode
      while (ISSPACE (*op_end)) op_end++;

      {
        expressionS arg;

        op_end = parse_exp_save_ilp (op_end, &arg);
        fix_new_exp (frag_now,
          (p - frag_now->fr_literal),
          4,
          &arg,
          true,
          BFD_RELOC_WI23_PCREL26_S);

      }
      break;

    ////////////
    // Other //
    //////////
    case WI23_OTHER:
    iword = (opcode->opcode << 26);;
    break;
    default:
      abort ();
  }

  md_number_to_chars (p, iword, 4);
  dwarf2_emit_insn (4);

  printf("iword: iword = %x\n", iword);

  while (ISSPACE (*op_end))
    op_end++;

  if (*op_end != 0)
    as_warn (_("extra stuff on line ignored"));

  if (pending_reloc)
    as_bad (_("Something forgot to clean up\n"));
}

/* Turn a string in input_line_pointer into a floating point constant
   of type type, and store the appropriate bytes in *LITP.  The number
   of LITTLENUMS emitted is stored in *SIZEP .  An error message is
   returned, or NULL on OK.  */

const char *
md_atof (int type, char *litP, int *sizeP)
{
  return ieee_md_atof (type, litP, sizeP, TARGET_BYTES_BIG_ENDIAN);
}

struct option md_longopts[] =
{
  { NULL,          no_argument, NULL, 0}
};

size_t md_longopts_size = sizeof (md_longopts);

const char *md_shortopts = "";

int
md_parse_option (int c ATTRIBUTE_UNUSED, const char *arg ATTRIBUTE_UNUSED)
{
  return 0;
}

void
md_show_usage (FILE *stream ATTRIBUTE_UNUSED)
{
  fprintf (stream, _("\
  -EB                     assemble for a big endian system (default)\n\
  -EL                     assemble for a little endian system\n"));
}

/* Apply a fixup to the object file.  */

void
md_apply_fix (fixS *fixP ATTRIBUTE_UNUSED,
	      valueT * valP ATTRIBUTE_UNUSED, segT seg ATTRIBUTE_UNUSED)
{
  char *buf = fixP->fx_where + fixP->fx_frag->fr_literal;
  long val = *valP;
  long newval;
  long max, min;

  max = min = 0;
  switch (fixP->fx_r_type)
    {
    case BFD_RELOC_32:
      buf[0] = val >> 24;
      buf[1] = val >> 16;
      buf[2] = val >> 8;
      buf[3] = val >> 0;
      buf += 4;
      break;
    case BFD_RELOC_16_PCREL:
    case BFD_RELOC_16:
      buf[0] = val >> 8;
      buf[1] = val >> 0;
      buf += 2;
      break;
    case BFD_RELOC_8:
      *buf++ = val;
      break;
    case BFD_RELOC_WI23_PCREL26_S:
      if (!val)
	    break;
      // not sure why -2 and not -1 tbh
      if (val < -(1<<25) || val > (1<<25)-2)
	as_bad_where (fixP->fx_file, fixP->fx_line,
                      _("pcrel too far BFD_RELOC_WI23_PCREL26_S"));
      newval = md_chars_to_number (buf, 4);
      newval |= val & 0x3ffffff;
      md_number_to_chars (buf, newval, 4);
      break;

    default:
      abort ();
    }

  if (max != 0 && (val < min || val > max))
    as_bad_where (fixP->fx_file, fixP->fx_line, _("offset out of range"));

  if (fixP->fx_addsy == NULL && fixP->fx_pcrel == 0)
    fixP->fx_done = 1;
}

/* Put number into target byte order.  */

void
md_number_to_chars (char * ptr, valueT use, int nbytes)
{
  number_to_chars_bigendian (ptr, use, nbytes);
}

/* Convert from target byte order to host byte order.  */

static valueT
md_chars_to_number (char * buf, int n)
{
  valueT result = 0;
  unsigned char * where = (unsigned char *) buf;

  while (n--)
	{
	  result <<= 8;
	  result |= (*where++ & 255);
	}

  return result;
}

/* Generate a machine-dependent relocation.  */
arelent *
tc_gen_reloc (asection *section ATTRIBUTE_UNUSED, fixS *fixP)
{
  arelent *relP;
  bfd_reloc_code_real_type code;

  switch (fixP->fx_r_type)
    {
    case BFD_RELOC_32:
    case BFD_RELOC_16:
    case BFD_RELOC_16_PCREL:
    case BFD_RELOC_WI23_PCREL26_S:
      code = fixP->fx_r_type;
      break;
    default:
      as_bad_where (fixP->fx_file, fixP->fx_line,
		    _("Semantics error.  This type of operand can not be relocated, it must be an assembly-time constant"));
      return 0;
    }

  relP = XNEW (arelent);
  relP->sym_ptr_ptr = XNEW (asymbol *);
  *relP->sym_ptr_ptr = symbol_get_bfdsym (fixP->fx_addsy);
  relP->address = fixP->fx_frag->fr_address + fixP->fx_where;

  relP->addend = fixP->fx_offset;

  /* This is the standard place for KLUDGEs to work around bugs in
     bfd_install_relocation (first such note in the documentation
     appears with binutils-2.8).

     That function bfd_install_relocation does the wrong thing with
     putting stuff into the addend of a reloc (it should stay out) for a
     weak symbol.  The really bad thing is that it adds the
     "segment-relative offset" of the symbol into the reloc.  In this
     case, the reloc should instead be relative to the symbol with no
     other offset than the assembly code shows; and since the symbol is
     weak, any local definition should be ignored until link time (or
     thereafter).
     To wit:  weaksym+42  should be weaksym+42 in the reloc,
     not weaksym+(offset_from_segment_of_local_weaksym_definition)

     To "work around" this, we subtract the segment-relative offset of
     "known" weak symbols.  This evens out the extra offset.

     That happens for a.out but not for ELF, since for ELF,
     bfd_install_relocation uses the "special function" field of the
     howto, and does not execute the code that needs to be undone.  */

  if (OUTPUT_FLAVOR == bfd_target_aout_flavour
      && fixP->fx_addsy && S_IS_WEAK (fixP->fx_addsy)
      && ! bfd_is_und_section (S_GET_SEGMENT (fixP->fx_addsy)))
    {
      relP->addend -= S_GET_VALUE (fixP->fx_addsy);
    }

  relP->howto = bfd_reloc_type_lookup (stdoutput, code);
  if (! relP->howto)
    {
      const char *name;

      name = S_GET_NAME (fixP->fx_addsy);
      if (name == NULL)
	name = _("<unknown>");
      as_fatal (_("Cannot generate relocation type for symbol %s, code %s"),
		name, bfd_get_reloc_code_name (code));
    }

  return relP;
}

/* Decide from what point a pc-relative relocation is relative to,
   relative to the pc-relative fixup.  Er, relatively speaking.  */
long
md_pcrel_from (fixS *fixP)
{
  valueT addr = fixP->fx_where + fixP->fx_frag->fr_address;

  switch (fixP->fx_r_type)
    {
    case BFD_RELOC_16:
    case BFD_RELOC_16_PCREL:
    case BFD_RELOC_WI23_PCREL26_S:
    case BFD_RELOC_32:
      return addr + 4;
    default:
      abort ();
      return addr;
    }
}
