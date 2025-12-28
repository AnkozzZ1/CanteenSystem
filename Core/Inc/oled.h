#ifndef __OLED_H
#define __OLED_H

#include "stm32f1xx_hal.h"

// OLED参数
#define OLED_ADDRESS    0x78  // OLED I2C地址
#define OLED_WIDTH      128
#define OLED_HEIGHT     64

// 函数声明
void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowString(uint8_t x, uint8_t y, char *str);
void OLED_ShowNumber(uint8_t x, uint8_t y, uint32_t num, uint8_t len);
void OLED_Refresh(void);

#endif