/**
  ******************************************************************************
  * @file    py32f4xx_hal_msp.c
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
  * @brief 初始CAN的MSP
  */
void HAL_CAN_MspInit(CAN_HandleTypeDef *hcan)
{
  RCC_OscInitTypeDef  OscInitstruct = {0};
  GPIO_InitTypeDef  GPIO_InitStruct;

  /* 开启HSE（24MHz） */
  OscInitstruct.OscillatorType  = RCC_OSCILLATORTYPE_HSE;
  OscInitstruct.HSEState        = RCC_HSE_ON;                           /* 开启HSE */
  OscInitstruct.HSEFreq         = RCC_HSE_16_32MHz;                     /* 选择HSE频率16~32M */
  
  OscInitstruct.PLL.PLLState    = RCC_PLL_NONE;                         /* PLL不做处理 */
  if(HAL_RCC_OscConfig(&OscInitstruct) != HAL_OK)
  {
    APP_ErrorHandler();
  }

  /* CAN的时钟源选择为HSE（24MHz） */
  __HAL_RCC_CAN_CONFIG(RCC_CANCLKSOURCE_HSE); /* CAN的时钟源频率为HSE（24MHz） */
  
  /* 使能时钟 */
  __HAL_RCC_CAN1_CLK_ENABLE();  /* 使能CAN时钟 */
  __HAL_RCC_GPIOB_CLK_ENABLE(); /* 使能GPIOB的时钟 */
  
  /* 配置CAN引脚 */
  GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9; /* CAN_RX:PB8,CAN_TX:PB9 */
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF4_CAN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  /* 使能CANFD的NVIC中断 */
  HAL_NVIC_SetPriority(CAN_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(CAN_IRQn);
}

/************************ (C) COPYRIGHT Puya *****END OF FILE******************/
