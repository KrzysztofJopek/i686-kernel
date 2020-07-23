#include "print.h"
#include "int.h"

void kmain()
{
#ifdef _DEBUG_
	asm volatile ("1: jmp 1b");
#endif

	clear();
	setup_int();
	log("INT INIT DONE");
	while(1);
}
