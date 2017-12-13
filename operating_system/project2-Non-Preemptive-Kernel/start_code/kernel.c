/*
   kernel.c
   the start of kernel
   */

#include "common.h"
#include "kernel.h"
#include "scheduler.h"
#include "th.h"
#include "util.h"
#include "queue.h"
#include "tasks.c"

volatile pcb_t *current_running;
queue R_queue, B_queue;
queue_t ready_queue, blocked_queue;
pcb_t *ready_arr[NUM_TASKS];
pcb_t *blocked_arr[NUM_TASKS];
pcb_t pcbs[NUM_TASKS];
context context_save[NUM_TASKS];
/*
   this function is the entry point for the kernel
   It must be the first function in the file
   */

#define PORT3f8 0xbfe48000

 void printnum(unsigned long long n)
 {
   int i,j;
   unsigned char a[40];
   unsigned long port = PORT3f8;
   i=10000;
   while(i--);

   i = 0;
   do {
   a[i] = n % 16;
   n = n / 16;
   i++;
   }while(n);

  for (j=i-1;j>=0;j--) {
   if (a[j]>=10) {
      *(unsigned char*)port = 'a' + a[j] - 10;
    }else{
	*(unsigned char*)port = '0' + a[j];
   }
  }
  printstr("\r\n");
}

void _stat(void){

	/* some scheduler queue initialize */
	/* need student add */
	ready_queue=&R_queue;
	ready_queue->pcbs=ready_arr;
	queue_init(ready_queue);
	ready_queue->capacity=NUM_TASKS;

	blocked_queue=&B_queue;
	blocked_queue->pcbs=blocked_arr;
	queue_init(blocked_queue);
	blocked_queue->capacity=NUM_TASKS;
	
	clear_screen(0, 0, 30, 24);

	/* Initialize the PCBs and the ready queue */
	/* need student add */
	int i;
	for(i=0;i<NUM_TASKS;++i){
		pcbs[i].state=PROCESS_READY;
		pcbs[i].type=task[i]->task_type;
		pcbs[i].context=&(context_save[i]);
		pcbs[i].context->ra=task[i]->entry_point;
		pcbs[i].context->sp=STACK_MAX-i*STACK_SIZE;
		queue_push(ready_queue,&(pcbs[i]));
	};
	/*Schedule the first task */
	scheduler_count = 0;
	scheduler_entry();

	/*We shouldn't ever get here */
	ASSERT(0);
}
