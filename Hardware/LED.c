#include "stm32f10x.h"

void LED_Init(void)
{
	// enable clock of GPIOA
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	// config mode of GPIOA Pin0
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; // 通用推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; // 输出模式，最大速度50MHz
	
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_SET);
}

void Control_LED(uint8_t on)
{
	if (on)
	{
		GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_RESET);
	}
	else
	{
		GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_SET);
	}
}

void Turn_LED()
{
	if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_0) == 1) // output high
	{
		GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_RESET); // output low
	}
	else
	{
		GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_SET);
	}
}
