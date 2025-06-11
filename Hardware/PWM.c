// PWM.c

#include "stm32f10x.h"

void PWM_Init(void)
{
    // 开启GPIOA的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	// 配置GPIOA Pin0
    GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; // 输出模式，最大速度50MHz
	
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	// 开启TIM3的时钟，TIM3是APB1的外设
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	// 选择内部时钟为TIM3的时钟输入
	TIM_InternalClockConfig(TIM3);
	
	// 配置TIM3的时基单元
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInit_Struct;
	TIM_TimeBaseInit_Struct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit_Struct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit_Struct.TIM_Period = 100 - 1; // ARR
	TIM_TimeBaseInit_Struct.TIM_Prescaler = 720 - 1; //PSC
	TIM_TimeBaseInit_Struct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInit_Struct);
	
	// 初始化输入比较单元
	TIM_OCInitTypeDef TIM_OCInit_Struct;
	TIM_OCStructInit(&TIM_OCInit_Struct);
	
	TIM_OCInit_Struct.TIM_OCMode = TIM_OCMode_PWM1; // 输出比较模式
	TIM_OCInit_Struct.TIM_OCPolarity = TIM_OCPolarity_High; // 不反转电平
	TIM_OCInit_Struct.TIM_OutputState = TIM_OutputState_Enable; // 使能
	TIM_OCInit_Struct.TIM_Pulse = 0; // 0x0000-0xffff, CCR
	
	TIM_OC1Init(TIM3, &TIM_OCInit_Struct);
	
	// 启动TIM2
	TIM_Cmd(TIM3, ENABLE);
}

void PWM_TIM3_SetCompare(uint16_t compare)
{
	TIM_SetCompare1(TIM3, compare);
}
