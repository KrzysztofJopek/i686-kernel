#ifndef _VM_H_
#define _VM_H_

#include "kerndefs.h"

#define PAGE_SIZE 4096
#define PAGE_ENTRY (PAGE_SIZE/4)

#define KERN_ADDR 0xC0000000
#define KERN_HEAP_ADDR (KERN_ADDR + PAGE_ENTRY*PAGE_SIZE)
#define KERN_HEAP_PAGES PAGE_ENTRY
#define KERN_HEAP_SIZE (KERN_HEAP_PAGES * PAGE_SIZE)

#define V2P(x) ((void*)x - KERN_ADDR)
#define P2V(x) ((void*)(x + KERN_ADDR))

void setup_vm();
void setup_user(void* pgdir);
void setup_code(void* pgdir, void* code, size_t size);
void* alloc_page();
void free_page(void* addr);
void* vtop(void* vaddr);
void set_upgdir(void* pgdir);
void set_kpgdir();

#endif
