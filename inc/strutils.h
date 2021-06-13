#ifndef _STRUTILS_H_
#define _STRUTILS_H_
#include "kerndefs.h"

uint32_t strlen(const uint8_t* str);
void strcpy(uint8_t* to, const uint8_t* from);
void strncpy(uint8_t* to, const uint8_t* from, uint32_t size);
void memcpy(void* dst, void* src, size_t size);
uint32_t itostr(int64_t it, char* res, uint32_t base);
void memset(void* dst, uint8_t byte, size_t size);
uint32_t strcmp(char* str1, char* str2);
#endif

