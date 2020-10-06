#include "kerndefs.h"
#include "proc.h"

void panic()
{
	for(;;){
		asm volatile ("hlt;");
	}
}

struct proc* currproc;
