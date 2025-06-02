#include "stm32f10x.h"

#define SCB_BASE_ADDR  0xE000ED00
#define TASK_MAX_COUNT 16
typedef void (*task_func)(void* param);

void Restore_REG_ASM(uint32_t, uint32_t);

static uint32_t task_sp[TASK_MAX_COUNT];
static uint32_t task_count;

static uint8_t start_shedule;
int8_t cur_task; // global var

void init_task_module(void)
{
    task_count = 0;
	// C/C++中选择c99标准
    for (int i = 0; i < TASK_MAX_COUNT; i++)
    {
        task_sp[i] = 0;
    }
    start_shedule = 0;
    cur_task = -1;
}

void create_task(task_func func, void* param, char* stack, uint32_t stack_sz)
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

   task_sp[task_count] = (uint32_t)sp;
   task_count += 1;
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

uint32_t get_cur_sp(int task_idx)
{
    return task_sp[task_idx];
}

void set_task_sp(int task_idx, uint32_t sp)
{
    task_sp[task_idx] = sp;
}

// for switch
// 只是获取下一个任务，并不改变 cur_task 这一全局变量
int next_task(void)
{
    return (cur_task+1) % task_count;
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

	// start first task or switch task
	if (cur_task == -1)
	{
		// start first task
		cur_task = 0; // set cur_task

		// restore regs
		uint32_t sp = get_cur_sp(0);
		Restore_REG_ASM(sp, lr);
	}
	else
	{
		// switch task

		// choose next task
		int nxt_task = next_task();

		if (cur_task != nxt_task) {
			// save cur_task
			set_task_sp(cur_task, old_sp);

			// restore nxt_task
			cur_task = nxt_task; // update cur_task
			u32 sp = get_cur_sp(cur_task);
			Restore_REG_ASM(sp, lr);
		}
	}
	return lr;
}
