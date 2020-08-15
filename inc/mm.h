#ifndef _MM_H_
#define _MM_H_
#include "multiboot.h"
#include "kerndefs.h"

void setup_mem(multiboot_info_t* m_info);
uint32_t alloc_frame();
void free_frame(uint32_t p);

#endif
