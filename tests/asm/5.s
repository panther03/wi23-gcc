lbi r0, 500
lbi r1, 0x6969
loop:
addi r0, r0, -1
bnez r0, loop
st r0, r1, 0
lbi r0, 500
j loop