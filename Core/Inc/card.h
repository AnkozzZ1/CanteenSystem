#ifndef __CARD_H
#define __CARD_H

#include <stdint.h>

// 卡片结构体
typedef struct {
    uint8_t uid[5];      // 卡片UID
    uint32_t balance;    // 余额（单位：分）
    uint32_t last_time;  // 最后使用时间
} CardInfo;

// 函数声明
void CARD_Init(void);
uint8_t CARD_FindInDB(uint8_t *uid);
void CARD_AddToDB(uint8_t *uid, uint32_t init_balance);
void CARD_Recharge(uint8_t *uid, uint32_t amount);
void CARD_Consume(uint8_t *uid, uint32_t amount);
uint32_t CARD_GetBalance(uint8_t *uid);
void CARD_DisplayInfo(uint8_t *uid, uint8_t mode);

// 模拟数据库大小
#define MAX_CARDS 10

#endif