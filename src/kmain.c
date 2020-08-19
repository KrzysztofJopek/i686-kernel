#include "print.h"
#include "int.h"
#include "fd.h"
#include "log.h"
#include "mm.h"
#include "vm.h"
#include "halloc.h"
#include "proc.h"

void kmain(multiboot_info_t* m_info)
{
#ifdef _DEBUG_
	asm volatile ("1: jmp 1b");
#endif

	clear();
	setup_int();
	setup_fd();
	LOG("INT and FD init done");
	setup_mem(m_info);
	LOG("MM init done");
	setup_vm();
	LOG("VM init done");
	char* xd = halloc(5);
	scheduler();
	while(1);
}
