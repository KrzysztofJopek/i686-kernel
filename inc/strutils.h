#ifndef _STRUTILS_H_
#define _STRUTILS_H_
#include <stdint.h>
#include <stddef.h>

uint32_t strlen(const uint8_t* str);
void strcpy(uint8_t* to, const uint8_t* from);
void strncpy(uint8_t* to, const uint8_t* from, uint32_t size);
void memcpy(void* dst, void* src, size_t size);

#endif

