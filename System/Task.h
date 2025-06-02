#ifndef _TASK_H
#define _TASK_H

typedef void (*task_func)(void* param);

void init_task_module(void);

void create_task(task_func func, void* param, char* stack, uint32_t stack_sz);

void start_task_module(void);
int is_start_shedule(void);

uint32_t get_cur_sp(int task_idx);
void set_task_sp(int task_idx, uint32_t sp);

int next_task(void);

#endif
