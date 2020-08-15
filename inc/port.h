#ifndef _PORT_H_
#define _PORT_H_

#include "kerndefs.h"

static inline uint8_t inb(uint16_t port)
{
	uint8_t res = 0;
	asm volatile ( "inb %0, %1"
			: "=r"(res)
			: "Nd"(port));

	return res;
}

static inline void outb(uint8_t value, uint16_t port)
{
	asm volatile ( "outb %1, %0"
			: 
			: "a"(value), "Nd"(port));
}

#endif
