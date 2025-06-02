#include "stm32f10x.h"
#include "Timer.h"
#include "OLED.h"
#include "Task.h"

static char stack_a[1024] __attribute__((aligned(4)));
static char stack_b[1024] __attribute__((aligned(4)));

void my_task(void* param)
{
	uint32_t num = 0;
	while (1)
	{
		num += 1;
		OLED_ShowNum(1, 1, num, 6);
	}
}

void my_sum_task(void* param)
{
	uint32_t num = 100000;
	while (1)
	{
		num -= 1;
		OLED_ShowNum(2, 1, num, 6);
	}
}

int main(void)
{
	OLED_Init();
	OLED_Clear();
	
	init_task_module();
	create_task(my_task, 0, stack_a, 1024);
	create_task(my_sum_task, 0, stack_b, 1024);
	
	Timer_Init();
	
	start_task_module();
	
	uint32_t num = 0;
	while (1)
	{
		num += 1;
		OLED_ShowNum(3, 1, num, 6);
	}
}
