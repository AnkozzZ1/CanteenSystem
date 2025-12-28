#include "key.h"
#include "main.h"

// 按键初始化
void KEY_Init(void) {
    // CubeMX已初始化，这里不需要额外代码
}

// 按键扫描
Key_Type KEY_Scan(void) {
    static uint8_t key_up = 1;
    Key_Type key_value = KEY_NONE;
    
    if (key_up && 
        (HAL_GPIO_ReadPin(KEY_QUERY_PORT, KEY_QUERY_PIN) == GPIO_PIN_RESET ||
         HAL_GPIO_ReadPin(KEY_RECHARGE_PORT, KEY_RECHARGE_PIN) == GPIO_PIN_RESET ||
         HAL_GPIO_ReadPin(KEY_CONSUME_PORT, KEY_CONSUME_PIN) == GPIO_PIN_RESET)) {
        
        HAL_Delay(10);  // 消抖
        key_up = 0;
        
        if (HAL_GPIO_ReadPin(KEY_QUERY_PORT, KEY_QUERY_PIN) == GPIO_PIN_RESET) {
            key_value = KEY_QUERY;
        }
        else if (HAL_GPIO_ReadPin(KEY_RECHARGE_PORT, KEY_RECHARGE_PIN) == GPIO_PIN_RESET) {
            key_value = KEY_RECHARGE;
        }
        else if (HAL_GPIO_ReadPin(KEY_CONSUME_PORT, KEY_CONSUME_PIN) == GPIO_PIN_RESET) {
            key_value = KEY_CONSUME;
        }
    }
    else if (HAL_GPIO_ReadPin(KEY_QUERY_PORT, KEY_QUERY_PIN) == GPIO_PIN_SET &&
             HAL_GPIO_ReadPin(KEY_RECHARGE_PORT, KEY_RECHARGE_PIN) == GPIO_PIN_SET &&
             HAL_GPIO_ReadPin(KEY_CONSUME_PORT, KEY_CONSUME_PIN) == GPIO_PIN_SET) {
        key_up = 1;
    }
    
    return key_value;
}