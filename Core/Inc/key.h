#ifndef __KEY_H
#define __KEY_H

#include "stm32f1xx_hal.h"

// 按键定义
#define KEY_QUERY_PIN      GPIO_PIN_8
#define KEY_QUERY_PORT     GPIOA
#define KEY_RECHARGE_PIN   GPIO_PIN_9
#define KEY_RECHARGE_PORT  GPIOA
#define KEY_CONSUME_PIN    GPIO_PIN_10
#define KEY_CONSUME_PORT   GPIOA

// 按键状态
#define KEY_PRESSED        0
#define KEY_RELEASED       1

// 按键值
typedef enum {
    KEY_NONE = 0,
    KEY_QUERY,
    KEY_RECHARGE,
    KEY_CONSUME
} Key_Type;

// 函数声明
void KEY_Init(void);
Key_Type KEY_Scan(void);

#endif




