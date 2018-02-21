  # labwork.S
  # Written 2015-2017 by F Lundevall
  # Skeleton file for IS1200/IS1500 lab 1.
  # The macros PUSH and POP are in the public domain.
  # Please add your own code at the end of the file.

  #
  # Please keep the two macros PUSH and POP unchanged
  #
.macro	PUSH reg
	addi	$sp,$sp,-4
	sw	\reg,0($sp)
.endm

.macro	POP reg
	lw	\reg,0($sp)
	addi	$sp,$sp,4
.endm
  #
  # Please add your own code below this line
  #


.text
.global hexasc
.global delay
.global time2string
.global enable_interrupt

enable_interrupt:
	ei
	jr $ra
	nop

hexasc:
	andi $t0,$a0,0xf

	addi $t1,$0,9
	ble $t0,$t1,digit
	nop

	addi $v0,$t0,55	# 'A' - 10
	jr $ra
	nop

digit:	addi $v0,$t0,0x30 # '0'
	jr $ra
	nop

delay:
	addi $t0,$0,3764

while:	ble $a0,$0,ret #while $a0 > $0
	nop
	addi $a0,$a0,-1

	add $t1,$0,$0

incr:	ble $t0,$t1,while
	nop
	addi $t1,$t1,1
	j incr
	nop

ret:	jr $ra
	nop

#
#

time2string:
	PUSH $s0
	PUSH $s1
	PUSH $ra

	add $s0,$0,$a0
	add $s1,$0,$a1

	srl $a0,$s1,12
	jal hexasc
	nop
	sb $v0,0($s0)

	srl $a0,$s1,8
	jal hexasc
	nop
	sb $v0,1($s0)

	addi $t0,$0,0x3a
	sb $t0,2($s0)

	srl $a0,$s1,4
	jal hexasc
	sb $v0,3($s0)

	andi $a0,$s1,0xf
	addi $t0,$0,2
	beq $a0,$t0,two
	jal hexasc
	nop
	sb $v0,4($s0)
	sb $0,5($s0)
	j skiptwo

two:	addi $t0,$0,0x54
	addi $t1,$0,0x57
	addi $t2,$0,0x4f
	sb $t0,4($s0)
	sb $t1,5($s0)
	sb $t2,6($s0)
	sb $0,7($s0)

skiptwo:
	POP $ra
	POP $s1
	POP $s0

	jr $ra
	nop
