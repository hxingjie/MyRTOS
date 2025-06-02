#include "stm32f10x.h"

void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	// enable clock of GPIOC
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	// config mode of GPIOC Pin13
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; // 通用推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; // 输出模式，最大速度50MHz
	
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
}

void Control_LED(uint8_t on)
{
	if (on)
	{
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
	}
	else
	{
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
	}
}

void Turn_LED()
{
	if (GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13) == 1) // output high
	{
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET); // output low
	}
	else
	{
		GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
	}
}
