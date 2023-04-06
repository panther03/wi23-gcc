OUTPUT_FORMAT("elf32-wi23")
OUTPUT_ARCH(wi23)

SEARCH_DIR(.)

RAM_SIZE = 16 * 1024; /* 16 KB */

MEMORY
{
  flash     (rx)   : ORIGIN = 0, LENGTH = 32K
  ram       (rw!x) : ORIGIN = 0x800000, LENGTH = RAM_SIZE
}
SECTIONS
{
  .text :
  {
    *(.text*)
    *(.strings)
    *(._pm*)
    *(.init)
    *(.fini)
     _etext = . ;
  }  > flash
  .tors :
  {
    ___ctors = . ;
    *(.ctors)
    ___ctors_end = . ;
    ___dtors = . ;
    *(.dtors)
    ___dtors_end = . ;
  } > ram
  .data          : AT (ADDR (.text) + SIZEOF (.text))
  {
    *(.data)
    *(.rodata)
    *(.rodata*)
     _edata = . ;
  }  > ram
  .bss  SIZEOF(.data) + ADDR(.data) :
  {
     _bss_start = . ;
    *(.bss)
    *(COMMON)
     _end = . ;
  }  > ram
   __data_load_start = LOADADDR(.data);
   __data_load_end = __data_load_start + SIZEOF(.data);
  .stab 0 (NOLOAD) :
  {
    *(.stab)
  }
  .stabstr 0 (NOLOAD) :
  {
    *(.stabstr)
  }
}