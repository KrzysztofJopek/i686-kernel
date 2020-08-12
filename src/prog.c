#include "log.h"
#include "proc.h"

void yield()
{
	sched();
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
