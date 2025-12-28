#include "card.h"
#include "oled.h"
#include "stm32f1xx_hal.h" 
#include "main.h"
#include <string.h>
#include <stdio.h>

// 模拟数据库
static CardInfo card_database[MAX_CARDS] = {
    {{0x12, 0x34, 0x56, 0x78, 0x90}, 5000, 0},  // 50.00元
    {{0xAB, 0xCD, 0xEF, 0x12, 0x34}, 3000, 0},  // 30.00元
    {{0x11, 0x22, 0x33, 0x44, 0x55}, 10000, 0}  // 100.00元
};

static uint8_t card_count = 3;

// 卡片管理初始化
void CARD_Init(void) {
    // 可以在这里初始化EEPROM或Flash
}

// 查找卡片
uint8_t CARD_FindInDB(uint8_t *uid) {
    for (uint8_t i = 0; i < card_count; i++) {
        if (memcmp(card_database[i].uid, uid, 5) == 0) {
            return i;
        }
    }
    return 0xFF;
}

// 添加新卡片
void CARD_AddToDB(uint8_t *uid, uint32_t init_balance) {
    if (card_count < MAX_CARDS) {
        memcpy(card_database[card_count].uid, uid, 5);
        card_database[card_count].balance = init_balance;
        card_database[card_count].last_time = HAL_GetTick();
        card_count++;
        
        OLED_Clear();
        OLED_ShowString(0, 0, "New Card Added");
        OLED_Refresh();
        HAL_Delay(1000);
    }
}

// 充值
void CARD_Recharge(uint8_t *uid, uint32_t amount) {
    uint8_t index = CARD_FindInDB(uid);
    
    if (index != 0xFF) {
        uint32_t old_balance = card_database[index].balance;
        card_database[index].balance += amount;
        card_database[index].last_time = HAL_GetTick();
        
        // 显示充值成功
        OLED_Clear();
        OLED_ShowString(0, 0, "Recharge Success");
        
        char msg[32];
        sprintf(msg, "Add: %d.%02d", amount / 100, amount % 100);
        OLED_ShowString(0, 2, msg);
        
        sprintf(msg, "Total: %d.%02d", 
                card_database[index].balance / 100,
                card_database[index].balance % 100);
        OLED_ShowString(0, 4, msg);
        OLED_Refresh();
        
       // LED亮起指示充值成功（共阳极：低电平亮，高电平灭）
		LED_ON();    // 可能实际上是熄灭
HAL_Delay(1000);
LED_OFF();   // 可能实际上是亮起
    } else {
        // 卡片不存在，询问是否添加
        OLED_Clear();
        OLED_ShowString(0, 0, "Card Not Found");
        OLED_ShowString(0, 2, "Add as new?");
        OLED_Refresh();
        HAL_Delay(2000);
        
        CARD_AddToDB(uid, amount);  // 添加新卡片并充值
    }
}

// 消费
void CARD_Consume(uint8_t *uid, uint32_t amount) {
    uint8_t index = CARD_FindInDB(uid);
    
    if (index != 0xFF) {
        if (card_database[index].balance >= amount) {
            uint32_t old_balance = card_database[index].balance;
            card_database[index].balance -= amount;
            card_database[index].last_time = HAL_GetTick();
            
            // 显示消费成功
            OLED_Clear();
            OLED_ShowString(0, 0, "Consume Success");
            
            char msg[32];
            sprintf(msg, "Pay: %d.%02d", amount / 100, amount % 100);
            OLED_ShowString(0, 2, msg);
            
            sprintf(msg, "Remain: %d.%02d", 
                    card_database[index].balance / 100,
                    card_database[index].balance % 100);
            OLED_ShowString(0, 4, msg);
            OLED_Refresh();
        } else {
            // 余额不足
            OLED_Clear();
            OLED_ShowString(0, 0, "Insufficient");
            OLED_ShowString(0, 2, "Balance!");
            OLED_ShowString(0, 4, "Please recharge");
            OLED_Refresh();
        }
    } else {
        OLED_Clear();
        OLED_ShowString(0, 0, "Card Not Found");
        OLED_Refresh();
    }
    
    HAL_Delay(1000);
}

// 查询余额
uint32_t CARD_GetBalance(uint8_t *uid) {
    uint8_t index = CARD_FindInDB(uid);
    
    if (index != 0xFF) {
        // 显示余额
        OLED_Clear();
        OLED_ShowString(0, 0, "Card Balance");
        
        char msg[32];
        sprintf(msg, "Amount: %d.%02d", 
                card_database[index].balance / 100,
                card_database[index].balance % 100);
        OLED_ShowString(0, 2, msg);
        OLED_Refresh();
        
        return card_database[index].balance;
    }
    
    OLED_Clear();
    OLED_ShowString(0, 0, "Card Not Found");
    OLED_Refresh();
    HAL_Delay(1000);
    return 0;
}