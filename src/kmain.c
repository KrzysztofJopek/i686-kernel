#include "print.h"
#include "int.h"
#include "fd.h"
#include "log.h"
#include "mm.h"
#include "vm.h"
#include "halloc.h"
#include "proc.h"
#include "ustar.h"


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
	if(m_info->mods_count != 0){
		LOG("Found multiboot module, loading USTAR filesystem");
		multiboot_module_t* list = (multiboot_module_t*)m_info->mods_addr;
		ustar_load((uint8_t*)list->mod_start, (uint8_t*)list->mod_end);
		int fd = open("us/test.txt");
		if(fd < 0){
			LOG("Can't open test.txt: %d", fd);
			goto sched;
		}
		char txt[11];
		int res = read(fd, txt, 10);
		if(res <= 0){
			LOG("Can't read test.txt: %d", res);
			goto sched;
		}
		txt[res] = '\0';
		LOG("Init done, txt: %s", txt);
	}
sched:
	scheduler();
	while(1);
}
