#include "vm.h"
#include "log.h"
#include "mm.h"
#include "strutils.h"

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

static void set_cr3(void* addr)
{
	asm volatile (
			"mov cr3, %0;"
			:
			: "r" ((uint32_t)addr)
		     );
}

static struct page_tab_ent* kern_heap;
static void create_kern_heap_tab()
{
	kern_heap = (struct page_tab_ent*)(P2V(F2A(alloc_frame())));
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
	kern_base = (struct page_tab_ent*)(P2V(F2A(alloc_frame())));
	if(!kern_base){
		LOG_ERR("Can't kern_base tab");
		panic();
	}
	for(int i=0; i<PAGE_ENTRY; i++){
		kern_base[i].addr = i;
		kern_base[i].P = 1;
		kern_base[i].R = 1;
	}
}


static struct page_dir_ent* res_page;
#define KERN_POS (KERN_ADDR >> 22)
static struct page_dir_ent* kern_pgdir;
void setup_vm()
{
	kern_pgdir = (struct page_dir_ent*)P2V(F2A(alloc_frame()));
	if(!kern_pgdir){
		LOG_ERR("Can't alloc heap");
		panic();
	}
	create_kern_base_tab();
	kern_pgdir[KERN_POS].addr = A2F(V2P(kern_base));
	kern_pgdir[KERN_POS].P = 1;
	kern_pgdir[KERN_POS].R = 1;
	create_kern_heap_tab();
	kern_pgdir[KERN_POS+1].addr = A2F(V2P(kern_heap));
	kern_pgdir[KERN_POS+1].P = 1;
	kern_pgdir[KERN_POS+1].R = 1;
	set_cr3(V2P(kern_pgdir));
}

void* alloc_page()
{
	void* page = P2V(F2A(alloc_frame()));
	if(!page){
		LOG_ERR("OOM");
		panic();
	}
	uint32_t dir_pos = (uint32_t)page >> 22 ;
	uint32_t tab_pos = ((uint32_t)page << 10) >> 22 ;
	if(kern_pgdir[dir_pos].P == 0){
		kern_pgdir[dir_pos].addr = A2F(res_page);
		kern_pgdir[dir_pos].P = 1;
		kern_pgdir[dir_pos].R = 1;
		res_page = P2V(F2A(alloc_frame()));
		if(!res_page){
			LOG_ERR("res_page OOM");
			panic();
		}
		if((uint32_t)res_page >> 22 != dir_pos){
			LOG_ERR("res_page BUG, can't handle it this way");
			panic();
		}
	}
	struct page_tab_ent* tab = P2V(F2A(kern_pgdir[dir_pos].addr));
	tab[tab_pos].addr = A2F(V2P(page));
	tab[tab_pos].P = 1;
	tab[tab_pos].R = 1;
	return page;
}

void free_page(void* addr)
{
	uint32_t dir_pos = (uint32_t)addr >> 22 ;
	uint32_t tab_pos = ((uint32_t)addr << 10) >> 22 ;
	struct page_tab_ent* tab = P2V(F2A(kern_pgdir[dir_pos].addr));
	tab[tab_pos].addr = 0;
	tab[tab_pos].P = 0;
	tab[tab_pos].R = 0;
	free_frame(A2F(addr));
}

/*
 * Copy kernel space to user process,
 * setup one tab of pages starting at 0x0;
 * TODO - 0x0 page should generate SEGINT
 */
void setup_user(void* pgdir)
{
	memcpy(pgdir, kern_pgdir, PAGE_SIZE);

	struct page_tab_ent* user_space = (struct page_tab_ent*)alloc_page();
	if(!user_space){
		LOG_ERR("Can't user_space tab");
		panic();
	}
	for(int i=0; i<PAGE_ENTRY; i++){
		user_space[i].addr = i;
		user_space[i].P = 1;
		user_space[i].R = 1;
		user_space[i].U = 1;
	}

	
	struct page_dir_ent* pg = pgdir;
	pg[0].addr = A2F(V2P(user_space));
	pg[0].P = 1;
	pg[0].R = 1;
	pg[0].U = 1;
}

void setup_code(void* pgdir, void* code, size_t size)
{
	set_cr3(V2P(pgdir));
	memcpy((void*)0x10, code, size);
	set_cr3(V2P(kern_pgdir));
}

void set_kpgdir()
{
	set_cr3(V2P(kern_pgdir));
}

void set_upgdir(void* pgdir)
{
	set_cr3(V2P(pgdir));
}

//pg contains kernel space
void copy_from_user(void* to, void* from, unsigned len)
{
	void* pg = currproc->pgdir;
	set_cr3(V2P(pg));

	memcpy(to, from, len);

	set_cr3(V2P(kern_pgdir));
}

void copy_to_user(void* to, void* from, unsigned len)
{
	void* pg = currproc->pgdir;
	set_cr3(V2P(pg));

	memcpy(to, from, len);

	set_cr3(V2P(kern_pgdir));
}
