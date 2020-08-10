#ifndef _MM_H_
#define _MM_H_
#include "multiboot.h"
#include <stdint.h>

void setup_mem(multiboot_info_t* m_info);
uint8_t get_ram_region(void* ptr);

#endif
