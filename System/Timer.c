#include "stm32f10x.h"

void Timer_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	// TIM2是APB1的外设
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	// 选择内部时钟为TIM2的时钟输入
	TIM_InternalClockConfig(TIM2);
	
	// 配置时基单元
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数
	// CK_CNT_OV = CK_PSC / (PSC + 1) / (ARR + 1)
	// 1000ms / 1ms = 1000
	// 如果需要定时1s，即1hz，CK_CNT_OV = 1 = 72*10^6 / 7200 / 10
	TIM_TimeBaseInitStruct.TIM_Period = 10-1; // [0, 65535]
	TIM_TimeBaseInitStruct.TIM_Prescaler = 7200-1; // [0, 65535]
	// 10k频率下计数10000次
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0; // 高级定时器的参数
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
	
	// 中断设置
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	// NVIC设置
	NVIC_PriorityGroupConfig(2);
	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
	
	// 启动定时器2
	TIM_Cmd(TIM2, ENABLE);
}

// 定时器2中断函数
/*void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		timer2_cnt += 1;
	}
}*/
