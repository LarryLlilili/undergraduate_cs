	.data
		os_size: .word 0x12345678
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
	li $6, 0x50000
	#la $8, os_size
	#lw $6, ($8)         #length

	jal 0x8007b1a8     #call read function
	jal 0xa08002bc 
