# calculate sum of 1 .. (switches) and put it in leds
.text
lui $1, 0xC0
li $1, 0x00
lui $4, 0x00
li $4, 0x00
_loop:
lw $2, 1($1)
sw $2, 0($4) # set r3 to r2
# now r2 contains the state of the switches
# calculate the sum
beq $2,$0, _done
_fac:
addi $2, $2, -1
lw $5, _test
#fcvti $0, $5
bne $2,$0, _loop
j _fac
j _done
ld $3, 0($4)
nop
add $3, $3, $2
sw $3, 0($4)
bne $2,$0, _fac
_done: 
lw $3, 0($4)
sw $3, 0($1)
j _loop

.data
_test:
.float 0f-314841971
_moretest:
.ascii "HI!!"
