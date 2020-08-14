#include "vm.h"
#include <stdint.h>
#include "log.h"
#include "mm.h"

struct page_dir_ent{
	uint32_t P : 1;
	uint32_t R : 1;
	uint32_t U : 1;
	uint32_t W : 1;
	uint32_t D : 1;
	uint32_t A : 1;
	uint32_t f0 : 1;
	uint32_t S : 1;
	uint32_t G : 1;
	uint32_t res : 3;
	uint32_t addr : 20;
};

struct page_tab_ent{
	uint32_t P : 1;
	uint32_t R : 1;
	uint32_t U : 1;
	uint32_t W : 1;
	uint32_t C : 1;
	uint32_t A : 1;
	uint32_t D : 1;
	uint32_t f0 : 1;
	uint32_t G : 1;
	uint32_t res : 3;
	uint32_t addr : 20;
};

static struct page_tab_ent kern_heap[KERN_HEAP_PAGES];
static void create_kern_heap_tab()
{
	for(int i=0; i<KERN_HEAP_PAGES; i++){
		uint32_t frame_addr = alloc_frame();
		if(!frame_addr){
			//TODO panic
			LOG("Can't alloc heap");
		}
		kern_heap[i].addr = frame_addr;
		kern_heap[i].P = 1;
		kern_heap[i].R = 1;
	}
}

static struct page_tab_ent kern_base[1024];
static void create_kern_base_tab()
{
	for(int i=0; i<1024; i++){
		kern_base[i].addr = (i*4096);
		kern_base[i].P = 1;
		kern_base[i].R = 1;
	}
}

#define KERN_START	0xC0000000
#define KERN_POS	(KERN_START>>22)
static struct page_dir_ent kern_pgdir[1024];
void setup_vm()
{
	create_kern_base_tab();
	kern_pgdir[KERN_POS].addr = (uint32_t)kern_base >>12;
	create_kern_heap_tab();
	kern_pgdir[KERN_POS+1].addr = (uint32_t)kern_heap >>12;
	asm volatile (
			"mov cr3, %0;"
			:
			: "r" ((uint32_t)kern_pgdir)
		     );
}


