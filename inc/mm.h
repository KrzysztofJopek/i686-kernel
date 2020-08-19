#ifndef _MM_H_
#define _MM_H_
#include "multiboot.h"
#include "kerndefs.h"

#define F2A(x) (void*)(x << 12)
#define A2F(x) ((uint32_t)x >> 12)

void setup_mem(multiboot_info_t* m_info);
uint32_t alloc_frame();
void free_frame(uint32_t p);

#endif
