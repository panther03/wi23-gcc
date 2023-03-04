/* wi23-specific support for 32-bit ELF.
   Copyright 2009, 2010, 2012 Free Software Foundation, Inc.

   Copied from elf32-moxie.c, elf32-fr30.c which is..
   Copyright 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007
   Free Software Foundation, Inc.

   This file is part of BFD, the Binary File Descriptor library.

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
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"
#include "elf-bfd.h"
//#include "elf/wi23.h"

#define TARGET_BIG_SYM          bfd_elf32_wi23_vec
#define TARGET_BIG_NAME		"elf32-wi23"

#define ELF_ARCH		bfd_arch_moxie
#define ELF_MACHINE_CODE	EM_MOXIE
#define ELF_MAXPAGESIZE		0x1


#define bfd_elf32_bfd_reloc_type_lookup		bfd_default_reloc_type_lookup
#define bfd_elf32_bfd_reloc_name_lookup		_bfd_norelocs_bfd_reloc_name_lookup
#define elf_info_to_howto			_bfd_elf_no_info_to_howto

#include "elf32-target.h"
