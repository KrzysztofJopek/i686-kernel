#include "mm.h"
#include "log.h"
#include <stdint.h>

void setup_mem(multiboot_info_t* m_info)
{
	multiboot_memory_map_t* mem_entry = (multiboot_memory_map_t*)m_info->mmap_addr;
	while(mem_entry < (multiboot_memory_map_t*)(m_info->mmap_addr + m_info->mmap_length)){
		if(mem_entry->type == 1){
			LOG("found usable RAM\n");
		}
		else{
			LOG("found not usable memory\n");
		}
		mem_entry = (multiboot_memory_map_t*)((uint32_t)mem_entry + mem_entry->size + sizeof(mem_entry->size));
	}

}

