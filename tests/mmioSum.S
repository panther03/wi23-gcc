// calculate sum of 1 .. (switches) and put it in leds
.text
lbi r1, 0xC0
slbi r1, 0x00
lbi r4, 0x00
slbi r4, 0x00
_loop:
ld r2, r1, 1
st r2, r4, 0 // set r3 to r2
// now r2 contains the state of the switches
// calculate the sum
beqz r2, _done
_fac:
addi r2, r2, -1
ld r5, r0, _test
fcvti f0, r5
bnez r2, _loop
j _fac
j _done
ld r3, r4
nop
add r3, r3, r2
st r3, r4, 0
bnez r2, .fac
_done: 
ld r3, r4
st r3, r1, 0
j _loop

.data
_test:
.float 0f-314841971
_moretest:
.ascii "HI!!"