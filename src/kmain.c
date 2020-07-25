#include "print.h"
#include "int.h"
#include "fd.h"
#include "log.h"

void kmain()
{
#ifdef _DEBUG_
	asm volatile ("1: jmp 1b");
#endif

	clear();
	setup_int();
	setup_fd();
	LOG("INT and FD init done");
	while(1);
}
