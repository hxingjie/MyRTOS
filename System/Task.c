#include "stm32f10x.h"
#include "Task.h"
#include "Serial.h"

#define TASK_MAX_PRIORITY 3
#define TASK_MAX_COUNT 8

void Restore_REG_ASM(uint32_t, uint32_t);

static struct TCB running_list[TASK_MAX_PRIORITY];
static struct TCB tasks[TASK_MAX_COUNT];
static struct TCB* current_running_task;

static uint32_t task_count;
static uint8_t start_shedule;

void init_task_module(void)
{
    task_count = 0;
	current_running_task = 0;
    start_shedule = 0;
	
	// init running_list
	for (uint8_t i = 0; i < TASK_MAX_PRIORITY; i++)
	{
		running_list[i].valid = 0;
		running_list[i].sp = 0;
		running_list[i].priority = 0;
		running_list[i].node.container = &running_list;
		running_list[i].node.nxt = 0;
	}
	
	for (uint8_t i = 0; i < TASK_MAX_COUNT; i++)
	{
		tasks[i].valid = 0;
		tasks[i].sp = 0;
		tasks[i].priority = 0;
		tasks[i].node.container = 0;
		tasks[i].node.nxt = 0;
	}
}

struct TCB* create_task(task_func func, void* param, char* stack, uint32_t stack_sz, uint8_t priority, char* name)
{
	uint32_t* sp = (uint32_t*)((uint32_t)stack + stack_sz);
	sp -= 16;

	// r4-r11, r0-r3, r12, lr, return_addr, xPSR
	for (int i = 0; i < 16; i++)
	{
		sp[i] = 0;
	}
	sp[8] = (u32)param; // r0
	sp[14] = (u32)func; // return_addr

	/*
	PSR
		  31 30 29 28 27 26:25 24 23:20 19:16 15:10 9 8 7 6 5 4:0
	APSR   N  Z  C  V  Q                 GE*
	IPSR                                             | exception_id 
	EPSR                 ICI/IT T             ICI/IT

	T: 0 表示使用 ARM 指令集（32位），1 表示使用 Thumb 指令集（16位）
	*/
	sp[15] = ((u32)1) << 24; // PSR
	
	tasks[task_count].valid = 1;
	tasks[task_count].sp = (uint32_t)sp;
	tasks[task_count].priority = priority;
	tasks[task_count].node.container = &tasks[task_count];
	tasks[task_count].node.nxt = 0;
	
	uint8_t ptr = 0;
	while (name[ptr] != '\0')
	{
		tasks[task_count].name[ptr] = name[ptr];
		ptr += 1;
	}
	
	// insert the running list corresponding to the priority
	Append_Node(&running_list[priority].node, &tasks[task_count].node);

	task_count += 1;
	
	return &tasks[task_count-1];
}

void start_task_module(void)
{
    start_shedule = 1;
    while (1)
    {

    }
}

int is_start_shedule(void)
{
    return start_shedule;
}

struct TCB* Get_NextTask(void)
{
	struct List_Node* nxt_task;
	// 遍历running list，按照优先级高到低的顺序
	for (uint8_t i = 0; i < TASK_MAX_PRIORITY; i++)
	{
		uint8_t priority = TASK_MAX_PRIORITY - i - 1;
		if (running_list[priority].node.nxt == 0)
		{
			continue; // 该优先级没有任务
		}
		nxt_task = running_list[priority].node.nxt;
		if (nxt_task != 0)
		{
			return (struct TCB*)nxt_task->container;
		}
	}
	return 0;
}

void Disable_Task(struct TCB* tcb)
{
	TIM_Cmd(TIM2, DISABLE);
	if (tcb == 0)
	{
		current_running_task->valid = 0;
		Delete_Node(&running_list[current_running_task->priority].node, &current_running_task->node);
		printf("---- TASK[%s] is disable ----\r\n", current_running_task->name);
	}
	else
	{
		tcb->valid = 0;
		Delete_Node(&running_list[current_running_task->priority].node, &tcb->node);
		printf("---- TASK[%s] is disable ----\r\n", tcb->name);
	}
	
	for (uint8_t i = 0; i < TASK_MAX_PRIORITY; i++)
	{
		uint8_t priority = TASK_MAX_PRIORITY - i - 1;
		printf("Priority: [%d]\r\n", priority);
		if (running_list[priority].node.nxt == 0)
		{
			continue; // 该优先级没有任务
		}
		struct List_Node* ptr = running_list[priority].node.nxt;
		while (ptr != 0)
		{	
			printf("TASK[%s] valid is [%d]  \r\n", 
				((struct TCB*)(ptr->container))->name, 
				((struct TCB*)(ptr->container))->valid);
			ptr = ptr->nxt;
		}
		printf("\r\n");
	}
	
	TIM_Cmd(TIM2, ENABLE);
}

uint32_t timer_handle(uint32_t lr, uint32_t old_sp)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != SET)
	{
		return lr;
	}
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

	if (is_start_shedule() == 0)
	{
		return lr;
	}

	// get task from running list
	if (current_running_task == 0)
	{
		struct TCB* nxt_running_task = Get_NextTask();
		current_running_task = nxt_running_task;
		
		Deque_Enque_Node(&running_list[nxt_running_task->priority].node, &nxt_running_task->node);
		Restore_REG_ASM(nxt_running_task->sp, lr);
	}
	else
	{
		// switch task

		// choose next task
		struct TCB* nxt_running_task = Get_NextTask();
		if (current_running_task != nxt_running_task) {
			// save cur_task
			current_running_task->sp = old_sp;

			// restore nxt_task
			current_running_task = nxt_running_task;
			
			Deque_Enque_Node(&running_list[nxt_running_task->priority].node, &nxt_running_task->node);
			Restore_REG_ASM(nxt_running_task->sp, lr);
		}
	}
	
	return lr;
}
