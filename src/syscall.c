#include "syscall.h"
#include "log.h"

#define MAX_SYSCALL 20
#define MAX_ARGS 3
struct syscall_entry{
	void* syscall;
	uint8_t args;
};

struct syscall_entry syscall_tab[MAX_SYSCALL];
int32_t add_syscall(uint32_t pos, void* syscall, uint8_t args)
{
	if(pos >= MAX_SYSCALL){
		LOG_ERR("Trying to add syscall %d, MAX_SYSCALL:%d", pos, MAX_SYSCALL);
		return -1;
	}
	if(syscall_tab[pos].syscall != NULL){
		LOG_ERR("Syscal %d already defined", pos);
		return -2;
	}
	if(args > MAX_ARGS){
		LOG_ERR("Can't define syscall with %d args, MAX_ARGS:%d", pos, MAX_ARGS);
		return -3;
	}
	if(syscall == NULL){
		LOG_ERR("Can't add NULL syscall");
		return -4;
	}
	syscall_tab[pos].syscall = syscall;
	syscall_tab[pos].args = args;
	return 1;
}
int32_t remove_syscall(uint32_t pos)
{
	if(pos >= MAX_SYSCALL){
		LOG_ERR("Trying to remove syscall %d, MAX_SYSCALL:%d", pos, MAX_SYSCALL);
		return -1;
	}
	if(syscall_tab[pos].syscall == NULL){
		LOG_ERR("Syscal %d not defined", pos);
		return -2;
	}
	syscall_tab[pos].syscall = NULL;
}

uint32_t call_syscall(struct trapframe* tf)
{
	uint32_t pos = tf->eax;
	if(pos >= MAX_SYSCALL){
		LOG_ERR("Trying to call syscall %d, MAX_SYSCALL:%d", pos, MAX_SYSCALL);
		return -1;
	}
	if(syscall_tab[pos].syscall == NULL){
		LOG_ERR("Syscal %d not defined", pos);
		return -2;
	}
	switch(syscall_tab[pos].args){
		case 0:
			return ((int32_t(*)())syscall_tab[pos].syscall)();
		case 1:
			return ((int32_t(*)(int32_t))syscall_tab[pos].syscall)(tf->ebx);
		case 2:
			return ((int32_t(*)(int32_t, int32_t))syscall_tab[pos].syscall)(tf->ebx, tf->ecx);
		case 3:
			return ((int32_t(*)(int32_t, int32_t, int32_t))syscall_tab[pos].syscall)(tf->ebx, tf->ecx, tf->edx);
		default:
			LOG_ERR("Trying to call syscall for args count:%d",syscall_tab[pos].args);
			panic();
	}
}
