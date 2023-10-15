/**
  ******************************************************************************
  * @file    py32f072_hal_msp.c
  * @author  MCU Application Team
  * @brief   This file provides code for the MSP Initialization
  *          and de-Initialization codes.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) Puya Semiconductor Co.
  * All rights reserved.</center></h2>
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* External functions --------------------------------------------------------*/

/**
  * @brief 初始化全局MSP
  */
void HAL_MspInit(void)
{
  /* 清除所有复位标志位 */
  __HAL_RCC_CLEAR_RESET_FLAGS();
}

/**
  * @brief 初始化WWDG MSP
  */
void HAL_WWDG_MspInit(WWDG_HandleTypeDef *hwwdg)
{
  /* WWDG 时钟使能 */
  __HAL_RCC_WWDG_CLK_ENABLE();
  NVIC_SetPriority(WWDG_IRQn, 0);
  NVIC_EnableIRQ(WWDG_IRQn);
}

/************************ (C) COPYRIGHT Puya *****END OF FILE******************/
