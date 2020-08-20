#ifndef _SYSCALL_H_
#define _SYSCALL_H_
#include "kerndefs.h"
#include "proc.h"

int32_t add_syscall(uint32_t pos, void* syscall, uint8_t args);
int32_t remove_syscall(uint32_t pos);
uint32_t call_syscall(struct trapframe* tf);
#endif
