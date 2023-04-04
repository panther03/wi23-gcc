# Run configure on first compile or when your system setup changes (for example you install a new version of a library or something)
# So, if you just make a change to a C file, you shouldn't need to reconfigure
# If things are not updating though it doesn't hurt to delete your build directory, reconfigure, remake and try again

# To configure gcc - The C compiler
../../gcc/configure --target=wi23-elf --enable-languages=c --disable-nls --disable-libssp --prefix="$WI23_HOME"
# To configure assembler, linker, simulator, etc. 
../../src/configure --target=wi23-elf --disable-gdb --prefix="$WI23_HOME" --disable-target-newlib --disable-target-libgloss

# to build the code (gcc or src) (run after configuring)
make -jN # N = number of processors on system (use cat /proc/cpuinfo to determine)

# to assemble an assembly file called test.S
wi23-elf-as test.S -o test.o
# this creates a relocatable object file called test.o. you can link it with a binary using the linker:
wi23-elf-ld -o test.elf test.o
# if everything went well you have a linekd binary called test.elf. you can now run this in the simulator with:
wi23-elf-run -t test.elf
# -t is to enable tracing, which you probably want to have. use wi23-elf-run --help to list all the options.

# Dump the contents of the obj file
wi23-elf-objdump -d test.elf

#Compile changes to the simulator
cd /home/elan/Documents/ECE554/wi23-gcc/build/src
make
make install