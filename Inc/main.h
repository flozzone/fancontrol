/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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
#define BUTTON_BACK_Pin GPIO_PIN_14
#define BUTTON_BACK_GPIO_Port GPIOC
#define BUTTON_OK_Pin GPIO_PIN_15
#define BUTTON_OK_GPIO_Port GPIOC
#define PWM_OUT_Pin GPIO_PIN_0
#define PWM_OUT_GPIO_Port GPIOA
#define DS18B20_Pin GPIO_PIN_1
#define DS18B20_GPIO_Port GPIOA
#define DC_EN_Pin GPIO_PIN_4
#define DC_EN_GPIO_Port GPIOA
#define OLED_SCK_Pin GPIO_PIN_5
#define OLED_SCK_GPIO_Port GPIOA
#define OLED_MOSI_Pin GPIO_PIN_7
#define OLED_MOSI_GPIO_Port GPIOA
#define BUTTON_UP_Pin GPIO_PIN_0
#define BUTTON_UP_GPIO_Port GPIOB
#define DHT22_Pin GPIO_PIN_11
#define DHT22_GPIO_Port GPIOB
#define BUTTON_LEFT_Pin GPIO_PIN_13
#define BUTTON_LEFT_GPIO_Port GPIOB
#define BUTTON_RIGHT_Pin GPIO_PIN_14
#define BUTTON_RIGHT_GPIO_Port GPIOB
#define BUTTON_DOWN_Pin GPIO_PIN_15
#define BUTTON_DOWN_GPIO_Port GPIOB
#define OLED_DC_Pin GPIO_PIN_8
#define OLED_DC_GPIO_Port GPIOA
#define OLED_RST_Pin GPIO_PIN_15
#define OLED_RST_GPIO_Port GPIOA
#define OLED_CS_Pin GPIO_PIN_6
#define OLED_CS_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
