#ifndef _PRINT_H_
#define _PRINT_H_

#define	COL_BLACK	0x0	
#define	COL_BLUE	0x1
#define	COL_GREEN	0x2
#define	COL_CYAN	0x3
#define	COL_RED		0x4
#define	COL_MAGENTA	0x5
#define	COL_BROWN	0x6
#define	COL_LIGHT_GRAY	0x7
#define	COL_DARK_GRAY	0x8
#define	COL_LIGHT_BLUE	0x9
#define	COL_LIGHT_GREEN	0xa
#define	COL_LIGHT_CYAN	0xb
#define	COL_LIGHT_RED	0xc
#define	COL_PINK	0xd
#define	COL_YELLOW	0xe
#define	COL_WHITE	0xf
#include "kerndefs.h"

void clear();
void print(const uint8_t* str);
void putc(const uint8_t c);
void log(char* fmt, ...);

#endif
