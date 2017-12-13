#include "common.h"
#include "scheduler.h"
#include "util.h"

static unsigned int sticks_th4;
static unsigned int sticks_th5;
static unsigned int time;


void thread4(void)
{
	clear_screen(0,0,3,3);
	sticks_th4=get_timer();
	do_yield();
	do_exit();
}

void thread5(void)
{
	sticks_th5=get_timer();
	time=(sticks_th5-sticks_th4)/MHZ;
	print_str(1,3,"Context switch time(ms) from thread4 to thread 5 is: ");
	printint(4,4,time);

	do_yield();
	do_exit();
}
