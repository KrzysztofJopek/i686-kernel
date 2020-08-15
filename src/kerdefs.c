#include "kerndefs.h"

void panic()
{
	for(;;){
		asm volatile ("hlt;");
	}
}
