#include "print.h"
#include "int.h"

void kmain()
{
#ifdef _DEBUG_
	asm volatile ("1: jmp 1b");
#endif

	setup_int();
	clear();
	while(1);
}
