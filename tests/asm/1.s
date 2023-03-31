fadd f1, f1, f1

// calculate sum of 1 .. (switches) and put it in leds
.text
lbi r1, 0xC0
slbi r1, 0x00
lbi r4, 0x00
slbi r4, 0x00
jal -1
jal 0
jal 0
jal 0
jal 0
jal 0
jal 0
jal 0
jal 0

main:
        slbi  r2,20664
        lbi  r2,-16677
        addi r1, r0, 0
        addi r0, r1, 0
        jal 0; stu ra, sp, -4; stu fp, sp, -4; jal vga_print_plain;
        lbi r6,$h:PS2_KEY_AWAIT; slbi r6,PS2_KEY_AWAIT; ld r6, r6, 0;


.loop:
ld r2, r1, 1
st r2, r4 // set r3 to r2
// now r2 contains the state of the switches
// calculate the sum
beqz r2, .done
.fac:
addi r2, r2, -1
fld f0, r0, test
ld r3, r4
add r3, r3, r2
st r3, r4, 0
bnez r2, .fac
.done: 
ld r3, r4
st r3, r1, 0
j .loop
jal 0
.data
test:
.float 0f-314841971.693993751E-40 
moretest:
.ascii "HI!!!!!"

PS2_KEY:
        .long   45056
        .global VGA_TEXT_BUFFER
        .section        .bss,"aw",@nobits
        .p2align        2
        .type   VGA_TEXT_BUFFER, @object
        .size   VGA_TEXT_BUFFER, 4
VGA_TEXT_BUFFER:
        .zero   4
        .ident  "GCC: (GNU) 12.2.0"