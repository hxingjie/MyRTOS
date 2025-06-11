#ifndef __OLED_H
#define __OLED_H

void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);

void OLED_ShowCube(uint8_t row, uint8_t col);
void OLED_ShowBrick(uint8_t row, uint8_t col);

void OLED_HideCube(uint8_t row, uint8_t col);
void OLED_HideBrick(uint8_t row, uint8_t col);

void OLED_ShowPlatform(uint8_t col);
void OLED_HidePlatform(uint8_t col);

#endif
