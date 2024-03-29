/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define green_led_Pin GPIO_PIN_13
#define green_led_GPIO_Port GPIOC
#define EN_2_A_Pin GPIO_PIN_0
#define EN_2_A_GPIO_Port GPIOA
#define EN_2_B_Pin GPIO_PIN_1
#define EN_2_B_GPIO_Port GPIOA
#define EN_2_BTN_Pin GPIO_PIN_2
#define EN_2_BTN_GPIO_Port GPIOA
#define LED_RI_Pin GPIO_PIN_3
#define LED_RI_GPIO_Port GPIOA
#define LED_LE_Pin GPIO_PIN_4
#define LED_LE_GPIO_Port GPIOA
#define EN_3_BTN_Pin GPIO_PIN_5
#define EN_3_BTN_GPIO_Port GPIOA
#define EN_3_A_Pin GPIO_PIN_6
#define EN_3_A_GPIO_Port GPIOA
#define EN_3_B_Pin GPIO_PIN_7
#define EN_3_B_GPIO_Port GPIOA
#define BTN_A_Pin GPIO_PIN_0
#define BTN_A_GPIO_Port GPIOB
#define BTN_B_Pin GPIO_PIN_1
#define BTN_B_GPIO_Port GPIOB
#define BTN_C_Pin GPIO_PIN_2
#define BTN_C_GPIO_Port GPIOB
#define BTN_D_Pin GPIO_PIN_10
#define BTN_D_GPIO_Port GPIOB
#define LED_DN_Pin GPIO_PIN_11
#define LED_DN_GPIO_Port GPIOB
#define BTN_UP_Pin GPIO_PIN_12
#define BTN_UP_GPIO_Port GPIOB
#define BTN_DN_Pin GPIO_PIN_13
#define BTN_DN_GPIO_Port GPIOB
#define BTN_LE_Pin GPIO_PIN_14
#define BTN_LE_GPIO_Port GPIOB
#define BTN_RI_Pin GPIO_PIN_15
#define BTN_RI_GPIO_Port GPIOB
#define EN_1_A_Pin GPIO_PIN_8
#define EN_1_A_GPIO_Port GPIOA
#define EN_1_B_Pin GPIO_PIN_9
#define EN_1_B_GPIO_Port GPIOA
#define EN_1_BTN_Pin GPIO_PIN_10
#define EN_1_BTN_GPIO_Port GPIOA
#define LED_C_Pin GPIO_PIN_15
#define LED_C_GPIO_Port GPIOA
#define LED_B_Pin GPIO_PIN_3
#define LED_B_GPIO_Port GPIOB
#define LED_A_Pin GPIO_PIN_4
#define LED_A_GPIO_Port GPIOB
#define LED_UP_Pin GPIO_PIN_5
#define LED_UP_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
