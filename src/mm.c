#include "mm.h"
#include "log.h"

struct ram_region{
	void* start;
	void* end;
};

#define MAX_RAM_REGIONS 8
static struct ram_region ram_regions[MAX_RAM_REGIONS];
/*
 * check if addr is in RAM region
 * return ram_region number starting from 1 if exist or 0 if doesn't exist
 */
uint8_t get_ram_region(void* ptr)
{
	for(uint8_t i=0; i<MAX_RAM_REGIONS; i++){
		if(ptr >= ram_regions[i].start && ptr <= ram_regions[i].end){
			return i+1; // return number of region
		}
	}
	return 0;
}

/*
 * looping over memory maps provided by multiboot and add RAM to ram_regions array
 */
void setup_mem(multiboot_info_t* m_info)
{
	multiboot_memory_map_t* mem_entry = (multiboot_memory_map_t*)m_info->mmap_addr;
	uint8_t num_ram = 0;
	while(mem_entry < (multiboot_memory_map_t*)(m_info->mmap_addr + m_info->mmap_length)){
		if(mem_entry->type == 1){
			if(num_ram < MAX_RAM_REGIONS){
				ram_regions[num_ram].start = (void*)((uint32_t)mem_entry->addr);
				ram_regions[num_ram].end = (void*)((uint32_t)mem_entry->addr + (uint32_t)mem_entry->len - 1);
				LOG("found usable RAM\n");
			}
			else{
				LOG("WARNING: found usable RAM, but MAX_RAM_REGIONS is too small\n");
			}
			num_ram++;
		}
		else{
			LOG("found not usable memory\n");
		}
		mem_entry = (multiboot_memory_map_t*)((uint32_t)mem_entry + mem_entry->size + sizeof(mem_entry->size));
	}

}

