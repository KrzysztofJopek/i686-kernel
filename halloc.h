#pragma once
#include <stddef.h>

void* halloc(size_t size);
void* hrealloc(void* src, size_t size);
void hfree(void* dest);
