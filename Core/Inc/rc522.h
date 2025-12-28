#ifndef __RC522_H
#define __RC522_H

#include "stm32f1xx_hal.h"

// RC522寄存器定义
#define PCD_IDLE              0x00
#define PCD_AUTHENT           0x0E
#define PCD_RECEIVE           0x08
#define PCD_TRANSMIT          0x04
#define PCD_TRANSCEIVE        0x0C
#define PCD_RESETPHASE        0x0F
#define PCD_CALCCRC           0x03

// RC522命令
#define PICC_REQIDL           0x26
#define PICC_REQALL           0x52
#define PICC_ANTICOLL         0x93
#define PICC_SELECTTAG        0x93
#define PICC_AUTHENT1A        0x60
#define PICC_AUTHENT1B        0x61
#define PICC_READ             0x30
#define PICC_WRITE            0xA0
#define PICC_DECREMENT        0xC0
#define PICC_INCREMENT        0xC1
#define PICC_RESTORE          0xC2
#define PICC_TRANSFER         0xB0
#define PICC_HALT             0x50

// RC522错误代码
#define MI_OK                 0
#define MI_NOTAGERR           1
#define MI_ERR                2

// 函数声明
void RC522_Init(void);
uint8_t RC522_CheckCard(uint8_t *uid);
uint8_t RC522_Request(uint8_t req_mode, uint8_t *tag_type);
uint8_t RC522_Anticoll(uint8_t *ser_num);
void RC522_SelectTag(uint8_t *ser_num);
void RC522_Halt(void);
void RC522_Diagnose(void);
uint8_t RC522_SimpleCheck(void);
// SPI读写函数
void RC522_WriteReg(uint8_t addr, uint8_t val);
uint8_t RC522_ReadReg(uint8_t addr);
void RC522_SetBitMask(uint8_t reg, uint8_t mask);
void RC522_ClearBitMask(uint8_t reg, uint8_t mask);

// 添加这个函数声明 - 最重要的一行！
uint8_t RC522_ToCard(uint8_t command, uint8_t *send_data, uint8_t send_len, 
                     uint8_t *back_data, uint16_t *back_len);

#endif