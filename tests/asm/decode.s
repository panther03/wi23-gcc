halt
nop
irq 0x42
alabel:
ldcr r0, r0, 0x42
j alabel
jr r0, alabel
jal alabel
jalr r0, alabel
addi r0, r0, -1
subi r0, r0, 1
xori r0, r0, 0x42
andni r0, r0, 0x42
beqz r1,alabel
bnez r1,alabel
bltz r1,alabel
bgez r1,alabel
st r0, sp, 4
ld r0, ra, 4
slbi r0, 0x42
stu r0, sp, 8
roli r0, r0, 10
slli r0, r0, 10
rori r0, r0, 10
srli r0, r0, 10
lbi r0, 0x42
andi r0, r0, 0x42
add r0, r0, r1
sub r0, r1, r2
xor r3, r4, r5
andn r5, r6, r7 
rol r0, r0, r1
sll r0, r1, r2
ror r3, r4, r5
srl r5, r6, r7 
and r0, r1, r3
or  r1, r0, r3
slt r0, r1, r3
sltu r0, r1, r3
sle r0, r1, r3
sleu r0, r1, r3
seq r0, r1, r3
icvtf r0, f1
fcvti f0, r1
imovf r0, f1
fmovi f0, r1
fclass r0, f1
xorsi r1, r0, -1
fst f1, sp, 4
fld f1, sp, 4
stb r0, sp, 3
ldb r0, sp, 3
sth r0, sp, 3
ldh r0, sp, 3
srai r0, r1, 4
ori r0, r1, 0xFA
sra r0, r1, r2
feq f1, f2, f3
fle f1, f2, f3
flt f1, f2, f3

push r0
pop r1
fpush f0
fpop f1
spc
ret
//ret
//call