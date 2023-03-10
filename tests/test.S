// calculate sum of 1 .. (switches) and put it in leds
.text
lbi r1, 0xC000
slbi r1, 0x0000

_loop:
// Should also be relocation
// beqz r2, _function_in_other_file
// Should be relocation here
ld r2, r0, _moretest
j _done

_fac:
addi r2, r2, -1
ld r5, r0, _test
fcvti f0, r5
bnez r2, _loop
j _fac
j _done

add r3, r3, r2
st r3, r4, 0
j _fac

_done: 
ld r3, r4, 0
st r3, r1, 0
// j _function_in_other_file

.data
_test:
.float 0f-314841971
_moretest:
.ascii "HI!!"