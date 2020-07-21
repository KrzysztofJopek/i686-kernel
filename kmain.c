#include "print.h"

void kmain()
{
#ifdef _DEBUG_
	asm volatile ("1: jmp 1b");
#endif
	print("JOPEK");
	return;
}
