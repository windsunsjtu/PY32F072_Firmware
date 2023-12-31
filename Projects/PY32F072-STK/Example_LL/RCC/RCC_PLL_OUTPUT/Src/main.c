/**
  ******************************************************************************
  * @file    main.c
  * @author  MCU Application Team
  * @brief   Main program body
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

/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private user code ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void APP_SystemClockConfig(void);
static void APP_GPIOConfig(void);

/**
  * @brief  应用程序入口函数.
  * @retval int
  */
int main(void)
{
  /* 时钟初始化,配置系统时钟为HSI */
  APP_SystemClockConfig();
 
  /* 引脚输出初始化 */
  APP_GPIOConfig();
  
  /* MCO输出时钟及分频初始化 */
  LL_RCC_ConfigMCO(LL_RCC_MCO1SOURCE_SYSCLK,LL_RCC_MCO1_DIV_1);
  
  while (1)
  {
  }
}

/**
  * @brief  系统时钟配置函数
  * @param  无
  * @retval 无
  */
static void APP_SystemClockConfig(void)
{
  /* HSI使能及初始化 */
  LL_RCC_HSI_Enable();
  LL_RCC_HSI_SetCalibFreq(LL_RCC_HSICALIBRATION_16MHz);
  while(LL_RCC_HSI_IsReady() != 1)
  {
  }
  
  /* PLL对HSI(16MHz)2倍频 */
  LL_RCC_PLL_Disable();
  while(LL_RCC_PLL_IsReady() != 0)
  {
  }
  LL_RCC_PLL_SetMainSource(LL_RCC_PLLSOURCE_HSI);
  LL_RCC_PLL_SetMulFactor(LL_RCC_PLLMUL_2);
  LL_RCC_PLL_Enable();
  while(LL_RCC_PLL_IsReady() != 1)
  {
  }
  
  /* 设置flash延迟 */
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);
  while(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_1)
  {
  }
  
  /* 设置AHB分频 */
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);

  /* 配置HSISYS为系统时钟及初始化 */
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  }

  /* 设置APB1分频及初始化 */
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  
  /* 更新系统时钟全局变量SystemCoreClock(也可以通过调用SystemCoreClockUpdate函数更新) */
  LL_SetSystemCoreClock(32000000);
}

/**
  * @brief  配置PA08为MCO复用功能
  * @param  无
  * @retval 无
  */
static void APP_GPIOConfig(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct;
  
  /* GPIOA时钟使能 */
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
  
  /* 配置PA08为MCO复用功能 */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_8;                  /* 选择8号引脚 */
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;        /* 配置为复用模式 */
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH; /* 输出速度选择 */
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL; /* 输出模式选择 */
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;               /* 无上下拉 */
  GPIO_InitStruct.Alternate = LL_GPIO_AF0_MCO;          /* 选择复用为AF0功能 */
  LL_GPIO_Init(GPIOA,&GPIO_InitStruct);
}

/**
  * @brief  错误执行函数
  * @param  无
  * @retval 无
  */
void APP_ErrorHandler(void)
{
  /* 无限循环 */
  while(1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  输出产生断言错误的源文件名及行号
  * @param  file：源文件名指针
  * @param  line：发生断言错误的行号
  * @retval 无
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* 用户可以根据需要添加自己的打印信息,
     例如: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* 无限循环 */
  while (1)
  {
  }
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT Puya *****END OF FILE******************/
