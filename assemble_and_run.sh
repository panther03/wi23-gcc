#!/bin/sh

# Check if the argument is given
if [ -z "$1" ]
  then
    echo "Usage: $0 <test.S>"
    exit 1
fi

echo "\n\n"
echo "Assembling $1"
echo "===================="
# Assemble the assembly code to create object file
wi23-elf-as "$1" -o test.o

echo "\n\n"
echo "Linking $1"
echo "===================="
# Link object file to create binary file
wi23-elf-ld -o test.elf test.o

echo "\n\n"
echo "Running $1"
echo "===================="
# Run the binary in the simulator
wi23-elf-run -t test.elf