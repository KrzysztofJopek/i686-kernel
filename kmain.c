#include "print.h"
#include "int.h"
#include "fd.h"

void kmain()
{
#ifdef _DEBUG_
	asm volatile ("1: jmp 1b");
#endif

	clear();
	setup_int();
	setup_fd();
	log("INT INIT DONE");
	while(1);
}
