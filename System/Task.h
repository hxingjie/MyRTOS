#ifndef _TASK_H
#define _TASK_H

#include "List.h"

struct TCB
{
	uint8_t valid;
	uint32_t sp;
	uint8_t priority;
	char name[20];
	struct List_Node node;
};

typedef void (*task_func)(void* param);

void init_task_module(void);

struct TCB* create_task(task_func func, void* param, char* stack, uint32_t stack_sz, uint8_t priority, char* name);

void start_task_module(void);
int is_start_shedule(void);

uint32_t get_cur_sp(int task_idx);
void set_task_sp(int task_idx, uint32_t sp);

int next_task(void);
void Disable_Task(struct TCB* tcb);

#endif
