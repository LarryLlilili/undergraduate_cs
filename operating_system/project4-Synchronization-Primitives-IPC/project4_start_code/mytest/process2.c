#include "common.h"
#include "syslib.h"
#include "util.h"
#include "printf.h"

void proc2(void){
	printstr("start process2");
	kill(2);
	usr_lock_acquire(0);
	print_str(3,2,"proc1 has release the lock before being killed\n");
	exit();

}

