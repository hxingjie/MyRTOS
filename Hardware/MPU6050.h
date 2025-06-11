#ifndef __MPU6050_H
#define __MPU6050_H

void MPU6050_WriteReg(uint8_t reg_addr, uint8_t data);
uint8_t MPU6050_ReadReg(uint8_t reg_addr);
void MPU6050_Init(void);
void MPU6050_GetData(int16_t* acc_x, int16_t* acc_y, int16_t* acc_z,
						int16_t* gyro_x, int16_t* gyro_y, int16_t* gyro_z);
uint8_t MPU6050_GetID(void);

void MPU6050_Get_ACC_Y(int16_t* acc_y);

#endif
