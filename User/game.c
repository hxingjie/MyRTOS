#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "MPU6050.h"
#include "Task.h"
#include "Serial.h"

static int16_t g_mpu6050_acc_y = 0;
static int8_t g_platform_col = 50; // [0, 112]

int8_t g_platform_speed = 8;

int16_t cube_row = 7;
int16_t cube_col = 0;
int8_t cube_row_speed = -1;
int8_t cube_col_speed = 4;

const uint8_t brick_map[26][2] = {
	0,0,  0,10, 0,20,  0,30,  0,40,  0,50, 0,60, 0,70,
	0,80, 0,90, 0,100, 0,110, 0,120,
	
	1,0,  1,10, 1,20,  1,30,  1,40,  1,50, 1,60, 1,70,
	1,80, 1,90, 1,100, 1,110, 1,120,
};

uint8_t brick_exist[26] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};


uint8_t Is_Collision(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t y_sz)
{
	return x0 == x1
		&& y0 >= y1 && y0 < y1 + y_sz;
}

void Check_Collision(uint8_t row_cube, uint8_t col_cube, int8_t* p_cube_row_speed)
{
	uint8_t i;
	for (i = 0; i < 26; i++)
	{
		if (brick_exist[i] == 1 
			&& Is_Collision(row_cube, col_cube, brick_map[i][0], brick_map[i][1], 8))
		{
			brick_exist[i] = 0;
			OLED_HideBrick(brick_map[i][0], brick_map[i][1]);
			*p_cube_row_speed = -(*p_cube_row_speed);
		}
	}
}

void Draw_Map()
{	
	uint8_t i;
	for (i = 0; i < 26; i++)
	{
		if (brick_exist[i] == 1)
		{
			OLED_ShowBrick(brick_map[i][0], brick_map[i][1]);
		}
	}
}


uint8_t Move_Cube()
{
	uint8_t cnt = 0; // 撞到地面的次数
	
	cube_row += cube_row_speed;
	cube_col += cube_col_speed;
	if (cube_row_speed < 0 && cube_row < 0)
	{
		cube_row = 0;
		cube_row_speed = -cube_row_speed;
	}
	if (cube_row_speed > 0 && cube_row > 7)
	{
		cube_row = 7;
		cube_row_speed = -cube_row_speed;
		
		cnt += 1;
	}
	if (cube_col_speed < 0 && cube_col < 0)
	{
		cube_col = 0;
		cube_col_speed = -cube_col_speed;
	}
	if (cube_col_speed > 0 && cube_col > 124)
	{
		cube_col = 124;
		cube_col_speed = -cube_col_speed;
	}
	
	return cnt;
}


void Move_Platform()
{
	if (g_mpu6050_acc_y > 400) {
		g_platform_col -= g_platform_speed;
	}
	if (g_mpu6050_acc_y < -400)
	{
		g_platform_col += g_platform_speed;
	}
	if (g_platform_col < 0)
	{
		g_platform_col = 0;
	}
	if (g_platform_col > 112)
	{
		g_platform_col = 112;
	}
}

void my_platform_task(void* param)
{
	
	while (1)
	{
		TIM_Cmd(TIM2, DISABLE);
		MPU6050_Get_ACC_Y(&g_mpu6050_acc_y);
		TIM_Cmd(TIM2, ENABLE);
		
		Delay_ms(100);
	}
}

void my_game_task(void* param)
{
	uint8_t cnt = 0;
	struct TCB* p_platform_task = (struct TCB*)param;
	
	while (1)
	{
		Check_Collision(cube_row, cube_col, &cube_row_speed);
		
		TIM_Cmd(TIM2, DISABLE);
		if (Is_Collision(cube_row, cube_col, 6, g_platform_col, 16))
		{
			cube_row_speed = -cube_row_speed;
		}
		TIM_Cmd(TIM2, ENABLE);
		
		Draw_Map();
		
		OLED_HideCube(cube_row, cube_col);
		uint8_t tmp = Move_Cube();
		if (tmp > 0)
		{
			cnt += tmp;
			printf("cnt: [%d]\r\n", cnt);
		}
		OLED_ShowCube(cube_row, cube_col);
		
		OLED_ShowSignedNum(2, 1, g_mpu6050_acc_y%1000, 3);
		
		TIM_Cmd(TIM2, DISABLE);
		OLED_HidePlatform((uint8_t)g_platform_col);
		Move_Platform();
		OLED_ShowPlatform((uint8_t)g_platform_col);
		TIM_Cmd(TIM2, ENABLE);
		
		if (cnt == 8)
		{
			Disable_Task(p_platform_task);
			Disable_Task((struct TCB*)0);
		}
		
		Delay_ms(30);
	}
}

