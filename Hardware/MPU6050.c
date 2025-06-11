#include "stm32f10x.h"                  // Device header
#include "MPU6050_Reg.h"

#define MPU6050_ADDR	0xD0

void MPU6050_WriteReg(uint8_t reg_addr, uint8_t data)
{
	I2C_GenerateSTART(I2C2, ENABLE);
	while(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);
	
	I2C_Send7bitAddress(I2C2, MPU6050_ADDR, I2C_Direction_Transmitter);
	while(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR);
	
	I2C_SendData(I2C2, reg_addr);
	while(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR);
	
	I2C_SendData(I2C2, data);
	while(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);

	I2C_GenerateSTOP(I2C2, ENABLE);
}

uint8_t MPU6050_ReadReg(uint8_t reg_addr)
{
	uint8_t byte = 0;
	
	I2C_GenerateSTART(I2C2, ENABLE);
	while(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);
	
	I2C_Send7bitAddress(I2C2, MPU6050_ADDR, I2C_Direction_Transmitter);
	while(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR);
	
	I2C_SendData(I2C2, reg_addr);
	while(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);
	
	// ---- ---- ---- ----
	
	I2C_GenerateSTART(I2C2, ENABLE);
	while(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);
	
	I2C_Send7bitAddress(I2C2, MPU6050_ADDR, I2C_Direction_Receiver);
	while(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == ERROR);
	
	I2C_AcknowledgeConfig(I2C2, DISABLE);
	I2C_GenerateSTOP(I2C2, ENABLE);
	
	while(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR);
	byte = I2C_ReceiveData(I2C2);
	
	I2C_AcknowledgeConfig(I2C2, ENABLE);
	
	return byte;
}

void MPU6050_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef gpio_struct;
	gpio_struct.GPIO_Mode = GPIO_Mode_AF_OD;
	gpio_struct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	gpio_struct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpio_struct);
	
	I2C_InitTypeDef i2c_struct;
	I2C_StructInit(&i2c_struct);
	i2c_struct.I2C_Mode = I2C_Mode_I2C;
	i2c_struct.I2C_Ack = I2C_Ack_Enable;
	i2c_struct.I2C_ClockSpeed = 50000;
	i2c_struct.I2C_DutyCycle = I2C_DutyCycle_2;
	i2c_struct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	i2c_struct.I2C_OwnAddress1 = 0x00;
	I2C_Init(I2C2, &i2c_struct);
	
	I2C_Cmd(I2C2, ENABLE);
	
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01); // 解除睡眠，选择时钟
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00); // 不待机
	
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09); // 采样分频
	MPU6050_WriteReg(MPU6050_CONFIG, 0x06); // 滤波参数
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18); // 陀螺仪量程
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18); // 加速度计量程
}

void MPU6050_GetData(int16_t* acc_x, int16_t* acc_y, int16_t* acc_z,
						int16_t* gyro_x, int16_t* gyro_y, int16_t* gyro_z)
{
	uint16_t data_H = 0;
	uint16_t data_L = 0;
	
	// acc
	data_H = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
	data_L = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);
	*acc_x = (data_H << 8) | data_L;
	
	data_H = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);
	data_L = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);
	*acc_y = (data_H << 8) | data_L;
	
	data_H = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);
	data_L = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);
	*acc_z = (data_H << 8) | data_L;
	
	// gyro
	data_H = MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);
	data_L = MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);
	*gyro_x = (data_H << 8) | data_L;
	
	data_H = MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);
	data_L = MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);
	*gyro_y = (data_H << 8) | data_L;
	
	data_H = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);
	data_L = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);
	*gyro_z = (data_H << 8) | data_L;	
}

void MPU6050_Get_ACC_Y(int16_t* acc_y)
{
	uint16_t data_H = 0;
	uint16_t data_L = 0;
	
	data_H = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);
	data_L = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);
	*acc_y = (data_H << 8) | data_L;
}

uint8_t MPU6050_GetID(void)
{
	uint8_t id = MPU6050_ReadReg(MPU6050_WHO_AM_I);
	return id;
}
