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
  * @brief 初始化I2C的MSP
  */
void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_SYSCFG_CLK_ENABLE();                              /* SYSCFG时钟使能 */
  __HAL_RCC_DMA_CLK_ENABLE();                                 /* DMA时钟使能 */
  __HAL_RCC_I2C1_CLK_ENABLE();                                /* I2C1时钟使能 */
  __HAL_RCC_GPIOB_CLK_ENABLE();                               /* GPIOB时钟使能 */

  /**I2C引脚配置
  PB6  -------> I2C1_SCL
  PB7  -------> I2C1_SDA
  */
  GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;                     /* 开漏 */
  GPIO_InitStruct.Pull = GPIO_PULLUP;                         /* 上拉 */
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF1_I2C1;                  /* 复用为I2C */
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);                     /* GPIO初始化 */
  /*复位I2C*/
  __HAL_RCC_I2C1_FORCE_RESET();
  __HAL_RCC_I2C1_RELEASE_RESET();
}

/************************ (C) COPYRIGHT Puya *****END OF FILE******************/
