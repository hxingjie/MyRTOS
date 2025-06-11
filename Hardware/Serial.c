#include "stm32f10x.h"                  // Device header
#include <stdio.h>

void Serial_Init(void)
{
	// 配置 USART1的输出引脚，配置USART1
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	// Config GPIOA_Pin_9
	GPIO_InitTypeDef gpio_struct;
	gpio_struct.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
	gpio_struct.GPIO_Pin = GPIO_Pin_9;
	gpio_struct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio_struct);
	
	// Config USART
	USART_InitTypeDef usart_struct;
	USART_StructInit(&usart_struct);
	usart_struct.USART_BaudRate = 9600;
	usart_struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 不需要流控
	usart_struct.USART_Mode = USART_Mode_Tx;
	usart_struct.USART_Parity = USART_Parity_No;
	usart_struct.USART_StopBits = USART_StopBits_1;
	usart_struct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &usart_struct);
	
	USART_Cmd(USART1, ENABLE);
}

void Serial_SendByte(uint8_t byte)
{
	USART_SendData(USART1, byte);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

// 重定向 fputc
int fputc(int ch, FILE* f)
{
	Serial_SendByte((uint8_t)ch);
	return ch;
}
