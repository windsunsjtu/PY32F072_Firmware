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
#include "py32f072xx_ll_Start_Kit.h"

/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private user code ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void APP_SystemClockConfig(void);
static void APP_ExtiConfig(void);
static void APP_PvdConfig(void);

/**
  * @brief  应用程序入口函数.
  * @param  无
  * @retval int
  */
int main(void)
{
  /* 开SYSCFG和PWR时钟 */
  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  
  /* 时钟初始化,配置系统时钟为HSI */
  APP_SystemClockConfig();

  /* 初始化LED */
  BSP_LED_Init(LED_GREEN);

  /* 配置EXTI */
  APP_ExtiConfig();
  
  /* 配置PVD */
  APP_PvdConfig();
  
  /* 使能PVD */
  LL_PWR_EnablePVD();
  
  while (1)
  {
  }
}

/**
  * @brief  配置EXTI
  * @param  无
  * @retval 无
  */
static void APP_ExtiConfig(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct={0};
  
  LL_EXTI_InitTypeDef EXTI_InitStruct={0};
  
  /* GPIOB时钟使能 */
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);

  /* 选择PB07引脚 */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_7;
  
  /* 选择模拟模式 */
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
  
  /* 选择上拉 */
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  
  /* GPIOB初始化 */
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  /* 选择EXTI16 */
  EXTI_InitStruct.Line = LL_EXTI_LINE_16;
  
  /* 使能 */
  EXTI_InitStruct.LineCommand = ENABLE;
  
  /* 选择中断模式 */
  EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
  
  /* 选择上升/下降沿触发 */
  EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_RISING_FALLING;
  
  /* 外部中断初始化 */
  LL_EXTI_Init(&EXTI_InitStruct);
}

/**
  * @brief  配置PVD
  * @param  无
  * @retval 无
  */
static void APP_PvdConfig(void)
{
  /* 使能PWR时钟 */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  
  /* PB07作为检测源，此参数设置无效 */
  LL_PWR_SetPVDLevel(LL_PWR_PVDLEVEL_0);
  
  /* 滤波功能禁止 */
  LL_PWR_DisablePVDFilter();
  LL_PWR_SetPVDFilter(LL_PWR_PVD_FILTER_1CLOCK);
  
  /* PVD检测为PB07 */
  LL_PWR_SetPVDSource(LL_PWR_PVD_SOURCE_PB7);

  /* 设置中断优先级 */
  NVIC_SetPriority(PVD_IRQn, 1);
  
  /* 使能中断 */
  NVIC_EnableIRQ(PVD_IRQn);
}

/**
  * @brief  PVD回调函数
  * @param  无
  * @retval 无
  */
void PvdCallback()
{
  if (LL_PWR_IsActiveFlag_PVDO())
  {
    BSP_LED_On(LED_GREEN);
  }
  else
  {
    BSP_LED_Off(LED_GREEN);
  }
}

/**
  * @brief  系统时钟配置函数
  * @param  无
  * @retval 无
  */
static void APP_SystemClockConfig(void)
{
  /* 使能HSI */
  LL_RCC_HSI_Enable();
  while(LL_RCC_HSI_IsReady() != 1)
  {
  }

  /* 设置 AHB 分频*/
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);

  /* 配置HSISYS作为系统时钟源 */
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSISYS);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSISYS)
  {
  }

  /* 设置 APB1 分频*/
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_Init1msTick(8000000);

  /* 更新系统时钟全局变量SystemCoreClock(也可以通过调用SystemCoreClockUpdate函数更新) */
  LL_SetSystemCoreClock(8000000);
}

/**
  * @brief  错误执行函数
  * @param  无
  * @retval 无
  */
void APP_ErrorHandler(void)
{
  /* 无限循环 */
  while (1)
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
