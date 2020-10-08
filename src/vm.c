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

static struct page_dir_ent* kern_pgdir;
struct page_tab_ent* get_kern_page_tab(uint32_t index)
{
	struct page_tab_ent* self_tab = ((struct page_tab_ent*)0xFFC00000) + (0x400 * 1023);
	struct page_tab_ent* dest_tab = ((struct page_tab_ent*)0xFFC00000) + (0x400 * index);
	//create tab if does not exist
	if(self_tab[index].P == 0){
		uint32_t page_tab_frame = alloc_frame();
		if(!page_tab_frame){
			LOG_ERR("oom");
			panic();
		}
		self_tab[index].addr = page_tab_frame;
		self_tab[index].P = 1;
		self_tab[index].R = 1;
		kern_pgdir[index].addr = page_tab_frame;
		kern_pgdir[index].P = 1;
		kern_pgdir[index].R = 1;
		set_kpgdir();
		memset(dest_tab, 0, sizeof(PAGE_SIZE));
	}
	return dest_tab;
}

//read pgdir, pgtab and map frame on given address
//pgdir is physical address
static void map_by_pgdir_addr(void* pg_addr, void* pgdir, void* dest_addr)
{
	if((uint32_t)dest_addr%PAGE_SIZE || (uint32_t)pg_addr%PAGE_SIZE){
		LOG_ERR("address not page aligned\n");
		panic();
	}
	uint32_t pg_addr_num = (uint32_t)pg_addr;

	uint32_t pd = pg_addr_num >> 22;
	uint32_t pt = (pg_addr_num >> 12) & 0x03FF;

	map_frame(A2F(pgdir), dest_addr);
	uint32_t tab_addr = ((struct page_dir_ent*)dest_addr)[pd].addr;

	map_frame(tab_addr, dest_addr);
	uint32_t frame_addr = ((struct page_tab_ent*)dest_addr)[pt].addr;

	map_frame(frame_addr, dest_addr);
}

//frame is 20bit frame position returned from alloc_frame()
//addr need to me page aligned
void map_frame(uint32_t frame, void* addr)
{
	uint32_t addr_num = (uint32_t)addr;
	if(addr_num % PAGE_SIZE){
		LOG_ERR("address not page aligned %d\n", addr_num);
		panic();
	}

	uint32_t pd = addr_num >> 22;
	uint32_t pt = (addr_num >> 12) & 0x03FF;

	struct page_tab_ent* page_tab = get_kern_page_tab(pd);
	page_tab[pt].addr = frame;
	page_tab[pt].P = 1;
	page_tab[pt].R = 1;
	set_kpgdir();
}

static void create_kern_heap_tab()
{
	for(int i=0; i<KERN_HEAP_PAGES; i++){
		uint32_t frame_addr = alloc_frame();
		if(!frame_addr){
			LOG_ERR("Can't alloc heap");
			panic();
		}
		map_frame(frame_addr, (void*)(KERN_HEAP_ADDR + PAGE_SIZE*i));
	}
}

static struct page_dir_ent* res_page;
#define KERN_POS (KERN_ADDR >> 22)
void setup_vm()
{
	//TODO HACK - hardcoded address, fix it
	kern_pgdir = (struct page_dir_ent*)0xc0116000;
	create_kern_heap_tab();
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

void init_start();
void init_end();
#define TEMP_POS ((void*)0xFF000000)
/*
 * Copy kernel space to user process,
 * setup one tab of pages starting at 0x0;
 * TODO - 0x0 page should generate SEGINT
 */
void* setup_user()
{
	uint32_t user_pgdir = alloc_frame();
	map_frame(user_pgdir, TEMP_POS);
	memcpy(TEMP_POS, kern_pgdir, PAGE_SIZE);
	uint32_t user_pgtab = alloc_frame();
	((struct page_dir_ent*)TEMP_POS)[0].addr = user_pgtab;
	((struct page_dir_ent*)TEMP_POS)[0].P = 1;
	((struct page_dir_ent*)TEMP_POS)[0].R = 1;
	((struct page_dir_ent*)TEMP_POS)[0].U = 1;

	map_frame(user_pgtab, TEMP_POS);
	uint32_t user_frame = alloc_frame();
	((struct page_tab_ent*)TEMP_POS)[0].addr = user_frame;
	((struct page_tab_ent*)TEMP_POS)[0].P = 1;
	((struct page_tab_ent*)TEMP_POS)[0].R = 1;
	((struct page_tab_ent*)TEMP_POS)[0].U = 1;

	map_frame(user_frame, TEMP_POS);
	memcpy(TEMP_POS + 0x10, (void*)init_start, init_end-init_start);
	return F2A(user_pgdir);
}


void set_kpgdir()
{
	set_cr3(V2P(kern_pgdir));
}

void set_upgdir(void* pgdir)
{
	set_cr3(pgdir);
}

//pg contains kernel space
//TODO this function works only on 1 page
void copy_from_user(void* to, void* from, unsigned len)
{
	void* pg = currproc->pgdir;
	uint32_t from_num = (uint32_t)from;
	map_by_pgdir_addr((void*)(from_num & ~(PAGE_SIZE-1)), pg, (TEMP_POS));
	
	memcpy(to, (TEMP_POS+(from_num%4096)), len);
}

void copy_to_user(void* to, void* from, unsigned len)
{
	void* pg = currproc->pgdir;
	set_cr3(V2P(pg));

	memcpy(to, from, len);

	set_cr3(V2P(kern_pgdir));
}
