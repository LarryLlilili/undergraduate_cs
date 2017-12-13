	.text
	.globl main

main:
	# check the offset of main
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	li $4, 0xa0800200  #start 
	li $5, 0x200      #offset
	li $6, 0xffff       #length

	jal 0x8007b1a8     #call read function
	jal 0xa080026c 
1:	b 1b
