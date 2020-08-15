#include "vm.h"
#include "kerndefs.h"
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

static struct page_tab_ent* kern_heap;
static void create_kern_heap_tab()
{
	kern_heap = (struct page_tab_ent*)(alloc_frame() << 12);
	if(!kern_heap){
		LOG_ERR("Can't kern_heap tab");
		panic();
	}
	for(int i=0; i<KERN_HEAP_PAGES; i++){
		uint32_t frame_addr = alloc_frame();
		if(!frame_addr){
			LOG_ERR("Can't alloc heap");
			panic();
		}
		kern_heap[i].addr = frame_addr;
		kern_heap[i].P = 1;
		kern_heap[i].R = 1;
	}
}

static struct page_tab_ent* kern_base;
static void create_kern_base_tab()
{
	kern_base = (struct page_tab_ent*)(alloc_frame() << 12);
	if(!kern_base){
		LOG_ERR("Can't kern_base tab");
		panic();
	}
	for(int i=0; i<1024; i++){
		kern_base[i].addr = i;
		kern_base[i].P = 1;
		kern_base[i].R = 1;
	}
}

#define KERN_START	0xC0000000
#define KERN_POS	(KERN_START>>22)
static struct page_dir_ent* kern_pgdir;
void setup_vm()
{
	create_kern_base_tab();
	kern_pgdir[KERN_POS].addr = (uint32_t)kern_base >>12;
	kern_pgdir[KERN_POS].P = 1;
	kern_pgdir[KERN_POS].R = 1;
	kern_pgdir[0].addr = (uint32_t)kern_base >>12;
	kern_pgdir[0].P = 1;
	kern_pgdir[0].R = 1;
	create_kern_heap_tab();
	kern_pgdir[KERN_POS+1].addr = (uint32_t)kern_heap >>12;
	kern_pgdir[KERN_POS+1].P = 1;
	kern_pgdir[KERN_POS+1].R = 1;
	asm volatile (
			"mov cr3, %0;"
			:
			: "r" ((uint32_t)kern_pgdir)
		     );
}


