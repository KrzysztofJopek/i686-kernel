#include "log.h"
#include "proc.h"

void yield()
{
	asm("int 0x80");
}

void print_a()
{
	for(;;){
		LOG("a");
		yield();
	}
}

void print_b()
{
	for(;;){
		LOG("b");
		yield();
	}
}
