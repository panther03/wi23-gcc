#!/bin/bash
# Give this script a .c file as an argument and it will compile & link it with the necessary libraries to run in the simulator.
rm -rf out/*
mkdir -p out/

OBJECT_OUT="out/$(basename $1 .c).o"
ELF_OUT="out/$(basename $1 .c).elf"
HEX_OUT="out/$(basename $1 .c).hex"
ASM_OUT="$(basename $1 .c).S"

if [[ $DEBUG ]]; then
    mkdir -p out/debugging
    cd out/debugging/
    #/opt/or1k-elf/bin/or1k-elf-gcc  -mhard-float -dap "../../$1" -S -o "or1k-${ASM_OUT}" 
    wi23-elf-gcc -dap -fsingle-precision-constant -fno-builtin -O2 -fno-inline "../../$1" -S -o "${ASM_OUT}"    
    #moxie-elf-gcc -fno-inline  -O2 "../../$1" -S -o "moxie-${ASM_OUT}" 
    #/opt/riscv-elf/bin/riscv-elf-gcc "../../$1" -c -o "riscv-${ASM_OUT}"
    #/opt/riscv-elf/bin/riscv-elf-objdump -sdr "riscv-${ASM_OUT}"
    mv "${ASM_OUT}" ..
    #mv "moxie-${ASM_OUT}" ..
    #mv "riscv-${ASM_OUT}" ..
    #mv "or1k-${ASM_OUT}" ..
    cd ../../
fi

#-T tests/script.ld
#wi23-elf-gcc -O2 -fsingle-precision-constant -fno-builtin -fno-inline $1 -S -o "${ASM_OUT}" > /dev/null 2>&1
wi23-elf-gcc -fsingle-precision-constant -fno-builtin -fno-inline -O2 $1 -c -g -o "${OBJECT_OUT}"
wi23-elf-gcc -v -o "${ELF_OUT}" "${OBJECT_OUT}"
wi23-elf-objdump -sdr -S "${OBJECT_OUT}" > out/object-out.log
wi23-elf-objdump -sdr -S "${ELF_OUT}" > out/exec-out.log
wi23-elf-objcopy --verilog-data-width 4 "${ELF_OUT}"  -O verilog  "${HEX_OUT}"
if [[ ! $DEBUG ]]; then
    read -n 1 -p "Continue with simulation?"
    wi23-elf-run -t "${ELF_OUT}" > out/trace.log 2>&1
fi
