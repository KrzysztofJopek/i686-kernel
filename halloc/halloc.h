#pragma once
#include "kerndefs.h"

void* halloc(size_t size);
void* hrealloc(void* src, size_t size);
void hfree(void* dest);
