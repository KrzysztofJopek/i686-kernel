#include "mm.h"
#include "log.h"
#include "kerndefs.h"
#include "vm.h"

//TODO pages are not cleared

struct ram_region{
	void* start;
	void* end;
};
struct ram_frame{
	uint32_t addr:20;
	uint32_t used:1;
	uint32_t _reserved:11; };

#define MAX_RAM_REGIONS 8
static struct ram_region ram_regions[MAX_RAM_REGIONS];
#define MAX_RAM_FRAMES 10000
static struct ram_frame ram_frames[MAX_RAM_FRAMES];
static uint32_t curr_ram_frames = 0;

static struct ram_frame* find_free_frame()
{
	for(uint32_t i=0; i<curr_ram_frames; i++){
		if(!ram_frames[i].used){
			return ram_frames+i;
		}
	}
	return NULL;
}

static struct ram_frame* find_frame(uint32_t rpa)
{
	for(uint32_t i=0; i<curr_ram_frames; i++){
		if(!ram_frames[i].addr == rpa){
			return ram_frames+i;
		}
	}
	return NULL;
}

uint32_t alloc_frame()
{
	struct ram_frame* rp;
	if((rp = find_free_frame())){
		rp->used = 1;
		return rp->addr;
	}
	return 0;
}

void free_frame(uint32_t p)
{
	if(p & 0xfff){
		LOG_ERR("free_frame not a frame addr");
		panic();
	}
	struct ram_frame* rp = find_frame(p);
	if(!rp){
		LOG_ERR("free_frame can't find frame");
		panic();
	}
	rp->used = 0;
}


/*
 * looping over memory maps provided by multiboot and add RAM to ram_regions array
 XXX
 TODO this function is awful, rewrite it
 XXX
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
				LOG_DBG("found usable RAM");
			}
			else{
				LOG_WRN("found usable RAM, but MAX_RAM_REGIONS is too small");
			}
			num_ram++;
		}
		else{
			LOG_DBG("found not usable memory");
		}
		mem_entry = (multiboot_memory_map_t*)((uint32_t)mem_entry + mem_entry->size + sizeof(mem_entry->size));
	}

#define PHY_KERN_END		((void*)0x400000)
#define PHY_KERN_END_PAGES	(PHY_KERN_END - PAGE_SIZE*4) // 4 pages for kern_pgdir, kern_base, kern_heap, res_page in vm.c
	for(uint8_t reg=0; reg<MAX_RAM_REGIONS; reg++){
		void* ptr = ram_regions[reg].start;
		while(ptr+4096 < ram_regions[reg].end){
			if(ptr < PHY_KERN_END_PAGES){
				ptr += PAGE_SIZE;
				continue;
			}
			ram_frames[curr_ram_frames++].addr = A2F(ptr);
			if(curr_ram_frames == MAX_RAM_FRAMES){
				goto end;
			}
			ptr += PAGE_SIZE;
		}
	}
end:
	return;
}

