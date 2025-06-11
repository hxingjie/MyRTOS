#include "stm32f10x.h"
#include "Delay.h"

void Key_Init(void)
{
	// 开关K1由PB14控制
	GPIO_InitTypeDef GPIO_InitStruct;
	
	// enable clock of GPIOC
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	// config mode of GPIOC Pin13
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14;
	
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	//GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
}

uint8_t Key_GetInput(void)
{
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0) // pressed
	{
		Delay_ms(20); // 按下时的消抖
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0);
		Delay_ms(20); // 松开时的消抖
		return 1; // pressed
	}
	return 0; // not pressed
}
