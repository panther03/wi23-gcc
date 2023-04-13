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
    /opt/or1k-elf/bin/or1k-elf-gcc  -mhard-float -dap "../../$1" -S -o "or1k-${ASM_OUT}" 
    exit 1
    #wi23-elf-gcc -O2 "../../$1" -dap -S -o "${ASM_OUT}"    
    moxie-elf-gcc -O2 "../../$1" -S -o "moxie-${ASM_OUT}" 
    mv "${ASM_OUT}" ..
    mv "moxie-${ASM_OUT}" ..
    mv "or1k-${ASM_OUT}" ..
    cd ../../
fi

#-T tests/script.ld
wi23-elf-gcc -fno-builtin $1 -c -g -o "${OBJECT_OUT}"
echo "wi23-elf-gcc -fno-builtin $1 -c -g ${OBJECT_OUT}"
wi23-elf-gcc -o "${ELF_OUT}" "${OBJECT_OUT}"
wi23-elf-objdump -dr -S "${OBJECT_OUT}" > out/object-out.log
wi23-elf-objdump -sdr -S "${ELF_OUT}" > out/exec-out.log
wi23-elf-objcopy --verilog-data-width 4 "${ELF_OUT}"  -O verilog  "${HEX_OUT}"
if [[ ! $DEBUG ]]; then
    read -n 1 -p "Continue with simulation?"
    wi23-elf-run -D -t "${ELF_OUT}" > out/trace.log 2>&1
fi
