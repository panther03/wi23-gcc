#!/bin/bash
# Give this script a .c file as an argument and it will compile & link it with the necessary libraries to run in the simulator.
rm -rf out/*
mkdir -p out/

OBJECT_OUT="out/$(basename $1 .c).o"
ELF_OUT="out/$(basename $1 .c).elf"
ASM_OUT="$(basename $1 .c).S"

wi23-elf-gcc -x assembler-with-cpp tests/asm/crt0.s -c -o out/crt0.o 
if [[ $DEBUG ]]; then
    mkdir -p out/debugging
    cd out/debugging/
    wi23-elf-gcc "../../$1" -dap -S -o "${ASM_OUT}"    
    mv "${ASM_OUT}" ..
    cd ../../
fi

wi23-elf-gcc $1 -c -g -o "${OBJECT_OUT}"
wi23-elf-ld out/crt0.o -o "${ELF_OUT}" "${OBJECT_OUT}"
wi23-elf-objdump -dr "out/crt0.o" > out/crt0-out.log
wi23-elf-objdump -dr -S "${OBJECT_OUT}" > out/object-out.log
wi23-elf-objdump -sdr "${ELF_OUT}" > out/exec-out.log
if [[ ! $DEBUG ]]; then
    read -n 1 -p "Continue with simulation?"
    wi23-elf-run "${ELF_OUT}" > out/trace.log 2>&1
fi