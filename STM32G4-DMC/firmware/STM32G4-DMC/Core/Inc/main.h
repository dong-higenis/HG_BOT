/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32g4xx_hal.h"

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
#define MT_FL_IN1_Pin GPIO_PIN_0
#define MT_FL_IN1_GPIO_Port GPIOA
#define MT_FL_IN2_Pin GPIO_PIN_1
#define MT_FL_IN2_GPIO_Port GPIOA
#define MT_L_IN1_Pin GPIO_PIN_2
#define MT_L_IN1_GPIO_Port GPIOA
#define MT_L_IN2_Pin GPIO_PIN_3
#define MT_L_IN2_GPIO_Port GPIOA
#define L_ENC1_Pin GPIO_PIN_4
#define L_ENC1_GPIO_Port GPIOA
#define L_ENC1_EXTI_IRQn EXTI4_IRQn
#define L_ENC2_Pin GPIO_PIN_5
#define L_ENC2_GPIO_Port GPIOA
#define L_ENC2_EXTI_IRQn EXTI9_5_IRQn
#define MT_FR_IN1_Pin GPIO_PIN_6
#define MT_FR_IN1_GPIO_Port GPIOA
#define SW_Pin GPIO_PIN_7
#define SW_GPIO_Port GPIOA
#define MT_FR_IN2_Pin GPIO_PIN_0
#define MT_FR_IN2_GPIO_Port GPIOB
#define R_ENC1_Pin GPIO_PIN_8
#define R_ENC1_GPIO_Port GPIOA
#define R_ENC1_EXTI_IRQn EXTI9_5_IRQn
#define R_ENC2_Pin GPIO_PIN_15
#define R_ENC2_GPIO_Port GPIOA
#define R_ENC2_EXTI_IRQn EXTI15_10_IRQn
#define MT_R_IN1_Pin GPIO_PIN_5
#define MT_R_IN1_GPIO_Port GPIOB
#define LED_Pin GPIO_PIN_6
#define LED_GPIO_Port GPIOB
#define MT_R_IN2_Pin GPIO_PIN_7
#define MT_R_IN2_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
