#include "stm32f10x.h"
#include "Timer.h"
#include "Task.h"

#include "OLED.h"
#include "PWM.h"
#include "LED.h"
#include "game.h"
#include "MPU6050.h"
#include "Serial.h"

#include "Delay.h"

static char stack_breathe_led_task[1024] __attribute__((aligned(4)));
static char stack_platform_task[1024] __attribute__((aligned(4)));
static char stack_game_task[2048] __attribute__((aligned(4)));
static char stack_led_task[1024] __attribute__((aligned(4)));

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

void my_breathe_led_task(void* param)
{
	PWM_Init();
	
	uint8_t cnt = 0;
	
	while (1)
	{
		for (uint16_t i = 0; i <= 100; i++)
		{
			PWM_TIM3_SetCompare(i); // 0-100
			Delay_ms(10);
		}
		
		for (uint16_t i = 0; i <= 100; i++)
		{
			PWM_TIM3_SetCompare(100 - i); // 0-100
			Delay_ms(10);
		}
		
		cnt += 1;
		if (cnt == 6)
		{
			Disable_Task((struct TCB*)0);
		}
	}
}

void my_led_task(void* param)
{
	LED_Init();
	
	while (1)
	{
		Turn_LED();
		Delay_ms(500);
		Turn_LED();
		Delay_ms(500);
	}
}

int main(void)
{
	Serial_Init();
	printf("hello, myrtos\r\n");
	
	OLED_Init();
	MPU6050_Init();
	
	init_task_module();
	
	create_task(my_breathe_led_task, 0, stack_breathe_led_task, 1024, 1, "breathe");
	struct TCB* p_platform_task = create_task(my_platform_task, 0, stack_platform_task, 1024, 1, "platform");
	create_task(my_game_task, (void*)p_platform_task, stack_game_task, 2048, 1, "game");
	
	create_task(my_led_task, 0, stack_led_task, 1024, 0, "led");
	
	Timer_Init();
	start_task_module();
	
	uint32_t num = 0;
	
	while (1)
	{
		num += 1;
		OLED_ShowNum(3, 1, num, 6);
	}
}
