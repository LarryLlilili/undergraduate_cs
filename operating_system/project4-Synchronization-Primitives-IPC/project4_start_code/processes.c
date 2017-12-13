#include "common.h"
#include "syslib.h"
#include "util.h"
#include "printf.h"

void init(void)
{
	ASSERT( spawn("proc1") >= 0 );
	ASSERT( spawn("proc2") >= 0 );
	exit();
}


