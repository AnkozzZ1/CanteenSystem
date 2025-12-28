#include "oled.h"
#include "font.h"
#include <string.h>
#include <stdio.h>  // 添加这个头文件

extern I2C_HandleTypeDef hi2c1;

// OLED命令/数据
#define OLED_CMD     0x00
#define OLED_DATA    0x40

// OLED命令定义
#define OLED_SETCONTRAST       0x81
#define OLED_DISPLAYALLON_RESUME 0xA4
#define OLED_DISPLAYALLON      0xA5
#define OLED_NORMALDISPLAY     0xA6
#define OLED_INVERTDISPLAY     0xA7
#define OLED_DISPLAYOFF        0xAE
#define OLED_DISPLAYON         0xAF
#define OLED_SETDISPLAYOFFSET  0xD3
#define OLED_SETCOMPINS        0xDA
#define OLED_SETVCOMDETECT     0xDB
#define OLED_SETDISPLAYCLOCKDIV 0xD5
#define OLED_SETPRECHARGE      0xD9
#define OLED_SETMULTIPLEX      0xA8
#define OLED_SETLOWCOLUMN      0x00
#define OLED_SETHIGHCOLUMN     0x10
#define OLED_SETSTARTLINE      0x40
#define OLED_MEMORYMODE        0x20
#define OLED_COLUMNADDR        0x21
#define OLED_PAGEADDR          0x22
#define OLED_COMSCANINC        0xC0
#define OLED_COMSCANDEC        0xC8
#define OLED_SEGREMAP          0xA0
#define OLED_CHARGEPUMP        0x8D

// 显示缓冲区
static uint8_t OLED_Buffer[8][128];

// 发送命令
static void OLED_WriteCmd(uint8_t cmd) {
    uint8_t buf[2] = {OLED_CMD, cmd};
    HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDRESS, buf, 2, 100);
}

// 发送数据
static void OLED_WriteData(uint8_t data) {
    uint8_t buf[2] = {OLED_DATA, data};
    HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDRESS, buf, 2, 100);
}

// 显示字符函数 - 先声明
static void OLED_ShowChar(uint8_t x, uint8_t y, char ch);

// 初始化OLED
void OLED_Init(void) {
    HAL_Delay(100);
    
    OLED_WriteCmd(OLED_DISPLAYOFF);
    OLED_WriteCmd(OLED_SETDISPLAYCLOCKDIV);
    OLED_WriteCmd(0x80);
    OLED_WriteCmd(OLED_SETMULTIPLEX);
    OLED_WriteCmd(0x3F);
    OLED_WriteCmd(OLED_SETDISPLAYOFFSET);
    OLED_WriteCmd(0x00);
    OLED_WriteCmd(OLED_SETSTARTLINE | 0x00);
    OLED_WriteCmd(OLED_CHARGEPUMP);
    OLED_WriteCmd(0x14);
    OLED_WriteCmd(OLED_MEMORYMODE);
    OLED_WriteCmd(0x00);
    OLED_WriteCmd(OLED_SEGREMAP | 0x01);
    OLED_WriteCmd(OLED_COMSCANDEC);
    OLED_WriteCmd(OLED_SETCOMPINS);
    OLED_WriteCmd(0x12);
    OLED_WriteCmd(OLED_SETCONTRAST);
    OLED_WriteCmd(0xCF);
    OLED_WriteCmd(OLED_SETPRECHARGE);
    OLED_WriteCmd(0xF1);
    OLED_WriteCmd(OLED_SETVCOMDETECT);
    OLED_WriteCmd(0x40);
    OLED_WriteCmd(OLED_DISPLAYALLON_RESUME);
    OLED_WriteCmd(OLED_NORMALDISPLAY);
    OLED_WriteCmd(OLED_DISPLAYON);
    
    OLED_Clear();
    OLED_Refresh();
}

// 清屏
void OLED_Clear(void) {
    memset(OLED_Buffer, 0, sizeof(OLED_Buffer));
}

// 显示字符串
void OLED_ShowString(uint8_t x, uint8_t y, char *str) {
    uint8_t i = 0;
    while (str[i] != '\0') {
        OLED_ShowChar(x + i * 8, y, str[i]);
        i++;
    }
}

// 显示数字 - 修正这里！
void OLED_ShowNumber(uint8_t x, uint8_t y, uint32_t num, uint8_t len) {
    char str[12];
    sprintf(str, "%lu", num);  // 修正：%lu 用于 uint32_t
    OLED_ShowString(x, y, str);
}

// 显示字符函数定义
static void OLED_ShowChar(uint8_t x, uint8_t y, char ch) {
    if (x > 127 || y > 7) return;
    
    // ASCII字符从32开始（空格）
    if (ch >= 32 && ch <= 126) {
        for (uint8_t i = 0; i < 8; i++) {
            OLED_Buffer[y][x + i] = Font8x8[ch - 32][i];
        }
    }
}

// 刷新显示
void OLED_Refresh(void) {
    for (uint8_t page = 0; page < 8; page++) {
        OLED_WriteCmd(OLED_PAGEADDR);
        OLED_WriteCmd(page);
        OLED_WriteCmd(page);
        
        OLED_WriteCmd(OLED_COLUMNADDR);
        OLED_WriteCmd(0);
        OLED_WriteCmd(127);
        
        for (uint8_t col = 0; col < 128; col++) {
            OLED_WriteData(OLED_Buffer[page][col]);
        }
    }
}