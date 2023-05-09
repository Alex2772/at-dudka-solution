/* USER CODE BEGIN Header */

/*
 * Copyright (c) Alex2772, https://github.com/alex2772
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin GPIO_PIN_13
#define LED_GPIO_Port GPIOC
#define VCOIL_Pin GPIO_PIN_3
#define VCOIL_GPIO_Port GPIOA
#define VBAT_Pin GPIO_PIN_4
#define VBAT_GPIO_Port GPIOA
#define VSHUNT_Pin GPIO_PIN_5
#define VSHUNT_GPIO_Port GPIOA
#define CHARGING_Pin GPIO_PIN_1
#define CHARGING_GPIO_Port GPIOB
#define JOY_UP_Pin GPIO_PIN_12
#define JOY_UP_GPIO_Port GPIOB
#define JOY_UP_EXTI_IRQn EXTI15_10_IRQn
#define JOY_RIGHT_Pin GPIO_PIN_13
#define JOY_RIGHT_GPIO_Port GPIOB
#define JOY_RIGHT_EXTI_IRQn EXTI15_10_IRQn
#define JOY_DOWN_Pin GPIO_PIN_14
#define JOY_DOWN_GPIO_Port GPIOB
#define JOY_DOWN_EXTI_IRQn EXTI15_10_IRQn
#define JOY_OK_Pin GPIO_PIN_15
#define JOY_OK_GPIO_Port GPIOB
#define JOY_OK_EXTI_IRQn EXTI15_10_IRQn
#define JOY_LEFT_Pin GPIO_PIN_8
#define JOY_LEFT_GPIO_Port GPIOA
#define JOY_LEFT_EXTI_IRQn EXTI9_5_IRQn
#define MOSFET_FIRE_Pin GPIO_PIN_3
#define MOSFET_FIRE_GPIO_Port GPIOB
#define FIRE_RING_Pin GPIO_PIN_4
#define FIRE_RING_GPIO_Port GPIOB
#define MOSFET_FIRE_LEGACY_Pin GPIO_PIN_5
#define MOSFET_FIRE_LEGACY_GPIO_Port GPIOB
#define EN_5V_Pin GPIO_PIN_8
#define EN_5V_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
