#ifndef _KERNDEFS_H_
#define _KERNDEFS_H_
#include <stdint.h>

#define NULL ((void*)0)
typedef uint32_t size_t;

void panic() __attribute__ ((noreturn));

#endif
