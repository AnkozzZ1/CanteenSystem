/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "rc522.h"
#include "oled.h"
#include "key.h"
#include "card.h"
#include <string.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
// 系统状态枚举
typedef enum {
    STATE_WAIT_CARD = 0,      // 等待卡片
    STATE_CARD_DETECTED,      // 卡片已检测
    STATE_PROCESSING          // 处理中
} SystemState;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
uint8_t current_card_uid[5] = {0};
uint8_t card_detected = 0;
SystemState system_state = STATE_WAIT_CARD;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  
  /* USER CODE BEGIN 2 */
  // 初始化外设
  OLED_Init();
  RC522_Init();
  KEY_Init();
  CARD_Init();
  
  // 设置初始状态
	LED_OFF();
  //HAL_GPIO_WritePin(LED_PIN_GPIO_Port, LED_PIN_Pin, GPIO_PIN_RESET);  // LED灭
  HAL_GPIO_WritePin(RC522_NSS_GPIO_Port, RC522_NSS_Pin, GPIO_PIN_SET);   // NSS高
  HAL_GPIO_WritePin(RC522_RST_GPIO_Port, RC522_RST_Pin, GPIO_PIN_SET);   // RST高
  
  // 显示欢迎界面
  OLED_Clear();
  OLED_ShowString(0, 0, "Canteen System");
  OLED_ShowString(0, 2, "Place Card");
  OLED_ShowString(0, 4, "Ready...");
  OLED_Refresh();
  
  // 测试LED闪烁一次
  HAL_GPIO_WritePin(LED_PIN_GPIO_Port, LED_PIN_Pin, GPIO_PIN_RESET);
  HAL_Delay(200);
  HAL_GPIO_WritePin(LED_PIN_GPIO_Port, LED_PIN_Pin, GPIO_PIN_SET);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
/* USER CODE BEGIN 3 */
/* USER CODE BEGIN 3 */
static uint8_t quick_action_mode = 0;
static uint32_t action_start_time = 0;

// 确保LED熄灭（除非正在充值）
HAL_GPIO_WritePin(LED_PIN_GPIO_Port, LED_PIN_Pin, GPIO_PIN_RESET);

if (quick_action_mode) {
    // 快速操作模式：放卡后立即按键
    if (HAL_GetTick() - action_start_time > 3000) {
        // 3秒超时
        quick_action_mode = 0;
        RC522_Halt();
        
        OLED_Clear();
        OLED_ShowString(0, 0, "Timeout");
        OLED_ShowString(0, 2, "Place Card Again");
        OLED_Refresh();
        HAL_Delay(1000);
        
        OLED_Clear();
        OLED_ShowString(0, 0, "Canteen System");
        OLED_ShowString(0, 2, "Place Card");
        OLED_ShowString(0, 4, "Ready...");
        OLED_Refresh();
    }
    
    // 处理按键
    Key_Type key = KEY_Scan();
    
    if (key != KEY_NONE) {
        switch (key) {
            case KEY_QUERY:
                CARD_GetBalance(current_card_uid);
                break;
            case KEY_RECHARGE:
                CARD_Recharge(current_card_uid, 1000);
                break;
            case KEY_CONSUME:
                CARD_Consume(current_card_uid, 500);
                break;
        }
        
        RC522_Halt();
        quick_action_mode = 0;
        
        HAL_Delay(2000);
        OLED_Clear();
        OLED_ShowString(0, 0, "Place Card");
        OLED_ShowString(0, 2, "Ready...");
        OLED_Refresh();
    }
} else {
    // 正常检测模式
    uint8_t tag_type[2];
    if (RC522_Request(PICC_REQIDL, tag_type) == MI_OK) {
        // 检测到卡片
        if (RC522_Anticoll(current_card_uid) == MI_OK) {
            RC522_SelectTag(current_card_uid);
            
            // 进入快速操作模式
            quick_action_mode = 1;
            action_start_time = HAL_GetTick();
            
            OLED_Clear();
            OLED_ShowString(0, 0, "Quick Action!");
            OLED_ShowString(0, 2, "Press Key NOW");
            OLED_ShowString(0, 4, "1 2 3");
            OLED_Refresh();
        }
    }
}

HAL_Delay(100);
/* USER CODE END 3 */
/* USER CODE END 3 */ // 主循环延时
    /* USER CODE END 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
// 可以在这里添加其他辅助函数
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  // LED快速闪烁表示错误
  __disable_irq();
  while (1) {
    HAL_GPIO_TogglePin(LED_PIN_GPIO_Port, LED_PIN_Pin);
    HAL_Delay(100);
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */