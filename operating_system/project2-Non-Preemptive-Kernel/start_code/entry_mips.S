#include "regs.h"
	.text
	.align	2
	.globl  get_timer	
	.ent	get_timer
	.type	get_timer, @function
get_timer:
	mfc0 $2,$9             #move from coprocessor0 register to general register $2 and $9

        nop
        j       ra             #return address
        nop
	.align	2
	.globl  kernel_entry	
	.ent	kernel_entry   #function start
	.type	kernel_entry, @function
kernel_entry:
	addiu sp, sp, -24  #stack pointer
	sw ra, 0(sp)       #memory[GPR[sp]]<--GPR[ra]
	bnez $4, 1f        #if GPR[4]!=0 jump to 1f 
	nop

	jal do_yield
	nop
	beqz $0,2f        #if GPR[0]==0 jump to 2f
	nop

1:
	jal do_exit
	nop
2:
	lw ra, 0(sp)       #GPR[ra]<--memory[GPR[sp]]
	addiu sp, sp, 24
	jr ra              #PC<--GPR[ra]
	nop

	.align	2
	.globl  scheduler_entry	
	.ent	scheduler_entry
	.type	scheduler_entry, @function
scheduler_entry:
	# call scheduler, which will set new current process
	# need student add
	jal scheduler
	nop
	
	la t0, current_running  #obtain the address of current_running
	lw t0, (t0)             #get the start address of pcb
	lw t0, 8(t0)            #get the start address of struct context
	lw sp, (t0)		#start recover registers
	lw ra, 4(t0)		
	lw s0, 8(t0)
	lw s1, 12(t0)
	lw s2, 16(t0)
	lw s3, 20(t0)
	lw s4, 24(t0)
	lw s5, 28(t0)
	lw s6, 32(t0)
	lw s7, 36(t0)
	lw s8, 40(t0)

	jr ra
	nop
	
	
	
	.align	2
	.globl  save_pcb	
	.ent	save_pcb
	.type	save_pcb, @function
save_pcb:
	# save the pcb of the currently running process
	# need student add  
	la t0, current_running	#get the address of current_running
	lw t0, (t0)		#get the start address of pcb
	lw t0, 8(t0)		#get the start address of struct context
	lw t1,16(sp) 		#get ra
	addiu sp,sp,24		
	sw sp, (t0)		#store sp
	addiu sp,sp,-24		
	sw t1, 4(t0)		#store ra
	sw s0, 8(t0)
	sw s1, 12(t0)
	sw s2, 16(t0)
	sw s3, 20(t0)
	sw s4, 24(t0)
	sw s5, 28(t0)
	sw s6, 32(t0)
	sw s7, 36(t0)
	sw s8, 40(t0)
	jr ra
	nop
