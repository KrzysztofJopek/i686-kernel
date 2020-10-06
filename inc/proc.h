#ifndef _PROC_H_
#define _PROC_H_
#include "kerndefs.h"
void scheduler();
void sched();

struct trapframe{
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t oesp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;

	uint16_t gs;
	uint16_t padding1;
	uint16_t fs;
	uint16_t padding2;
	uint16_t es;
	uint16_t padding3;
	uint16_t ds;
	uint16_t padding4;
	uint32_t trapno;

	uint32_t err;
	uint32_t eip;
	uint16_t cs;
	uint16_t padding5;
	uint32_t eflags;

	uint32_t esp;
	uint16_t ss;
	uint16_t padding6;
};

struct proc{
	uint32_t size;
	void* pgdir;
	struct context* ctx;
	struct trapframe* tf;
	void* kstack;
	uint8_t start;
	uint8_t used;
	uint32_t pid;
};

#endif
