#include "common.h"
#include "syslib.h"
#include "util.h"

static unsigned int sticks_start;
static unsigned int sticks;
static unsigned int time;
static unsigned int count=0;

void _start(void)
{
	/* need student add */
	sticks_start=get_timer();
	yield();
	sticks=get_timer();
	++count;
	time=(sticks-sticks_start)/MHZ;
	if(count>1){
		print_str(1,6,"Switch context time(ms) from process to process is: \n");
		printint(4,7,time);
	}
}
