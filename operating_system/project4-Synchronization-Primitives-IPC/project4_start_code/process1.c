#include "common.h"
#include "syslib.h"
#include "util.h"
#include "printf.h"

void proc1(void){
//	printstr("lock init\n");
//	usr_lock_init(1);
	printstr("lock acquire\n");
	usr_lock_acquire(0);
	printstr("proc1 yield proc2\n");
	yield();
	usr_lock_release(0);
	exit();
}
