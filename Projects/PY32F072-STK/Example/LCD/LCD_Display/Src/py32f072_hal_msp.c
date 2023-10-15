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
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
}

/**
  * @brief 初始化LCD的MSP
  */
void HAL_LCD_MspInit(LCD_HandleTypeDef *hlcd)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* 开LSI */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI; /* 选择振荡器LSI */
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;                   /* 开LSI */
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;             /* 不变更PLL配置 */
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    APP_ErrorHandler();
  }
  
  /* 选择LCD的时钟为LSI */
  HAL_RCCEx_EnableLSCO(RCC_LSCOSOURCE_LSI);
  
  /* 开LCD时钟 */
  __HAL_RCC_LCD_CLK_ENABLE();
  
  /* 开LCD引脚时钟 */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  
  /* 配置LCD引脚 */
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStructure.Pull = GPIO_NOPULL;

  GPIO_InitStructure.Pin = GPIO_PIN_9;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);/* COM0 */
  
  GPIO_InitStructure.Pin = GPIO_PIN_10;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);/* COM1 */
  
  GPIO_InitStructure.Pin = GPIO_PIN_11;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);/* COM2 */
  
  GPIO_InitStructure.Pin = GPIO_PIN_12;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);/* COM3 */
  
  GPIO_InitStructure.Pin = GPIO_PIN_8;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);/* SEG0 */
  
  GPIO_InitStructure.Pin = GPIO_PIN_9;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);/* SEG1 */
  
  GPIO_InitStructure.Pin = GPIO_PIN_8;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);/* SEG2 */
  
  GPIO_InitStructure.Pin = GPIO_PIN_7;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);/* SEG3 */
  
  GPIO_InitStructure.Pin = GPIO_PIN_6;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);/* SEG4 */
  
  GPIO_InitStructure.Pin = GPIO_PIN_15;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);/* SEG5 */
  
  GPIO_InitStructure.Pin = GPIO_PIN_14;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);/* SEG6 */
  
  GPIO_InitStructure.Pin = GPIO_PIN_13;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);/* SEG7 */  
}
/************************ (C) COPYRIGHT Puya *****END OF FILE******************/
