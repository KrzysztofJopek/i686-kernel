#include "proc.h"
#include "halloc.h"
#include "strutils.h"
#include "log.h"
#include <stdint.h>

struct context{
	uint32_t edi;
	uint32_t esi;
	uint32_t ebx;
	uint32_t ebp;
	uint32_t eip;
};

struct proc{
	uint32_t size;
	struct context* ctx;
	uint8_t start;
};
#define MAX_PROC 10
static struct proc procs[MAX_PROC];
static struct proc sched_proc;
static struct proc* currproc;

void swtch(struct context** old, struct context* new);
void swtch2(struct context* old, struct context* new);
void setup_ctx(struct context* ctx);

extern uint32_t stack_top;
extern uint32_t stack_bottom;
void print_a();
void print_b();
static void setup_2_procs_test()
{
	uint32_t stack_size = stack_top - stack_bottom;
	procs[0].ctx = halloc(stack_size);
	procs[1].ctx = halloc(stack_size);
	if(!procs[0].ctx || !procs[1].ctx){
		LOG("ERROR, cant allocate procs context");
	}
	procs[0].ctx = procs[0].ctx + stack_size;
	procs[1].ctx = procs[1].ctx + stack_size;
	memcpy(procs[0].ctx, (void*)stack_top, stack_size);
	memcpy(procs[1].ctx, (void*)stack_top, stack_size);
	setup_ctx(procs[0].ctx);
	setup_ctx(procs[1].ctx);
	procs[0].ctx->eip = (uint32_t)print_a;
	procs[1].ctx->eip = (uint32_t)print_b;
	procs[0].start = 1;
	procs[1].start = 1;
}

void scheduler()
{
	//init scheduler
	sched_proc.ctx = halloc(sizeof(struct context));
	if(!sched_proc.ctx){
		LOG("ERROR, cant allocate sched context");
	}
	setup_2_procs_test();

	for(;;){
		for(int p = 0; p<MAX_PROC; p++){
			if(procs[p].start == 0){
				continue;
			}
			currproc = procs+p;
			swtch(&sched_proc.ctx, procs[p].ctx);
		}
	};
}


void sched()
{
	swtch(&currproc->ctx, sched_proc.ctx);
}
