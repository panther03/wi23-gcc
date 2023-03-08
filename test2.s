// calculate sum of 1 .. (switches) and put it in leds
.text
lbi r1, 0xC0
slbi r1, 0x00
lbi r4, 0x00
slbi r4, 0x00
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
.data
test:
.float 0f-314841971.693993751E-40 
moretest:
.ascii "HI!!!!!"
