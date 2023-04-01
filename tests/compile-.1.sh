#!/bin/sh
wi23-elf-as asm/1.s  -o 1.o
wi23-elf-ld 1.o  -o 1.elf
wi23-elf-objdump -sdr 1.o > object_out.log
wi23-elf-objdump -sdr 1.elf > exec_out.log