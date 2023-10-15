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
LL_LPTIM_InitTypeDef LPTIM_InitStruct = {0};

/* Private user code ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void APP_SystemClockConfig(void);
static void APP_ConfigLptimClock(void);
static void APP_ConfigLptim(void);
static void APP_EnterStop(void);

/**
  * @brief  应用程序入口函数.
  * @param  无
  * @retval int
  */
int main(void)
{
  /* 配置系统时钟 */
  APP_SystemClockConfig();

  /* 初始化LED */
  BSP_LED_Init(LED_GREEN);

  /* 初始化按键BUTTON */
  BSP_PB_Init(BUTTON_KEY,BUTTON_MODE_GPIO);
  
  /* 配置LPTIM对应的EXTI Line29为事件唤醒模式 */
  LL_EXTI_DisableIT(LL_EXTI_LINE_29);   /* 屏蔽EXTI Line29中断唤醒 */
  LL_EXTI_EnableEvent(LL_EXTI_LINE_29); /* 使能EXTI Line29事件唤醒 */
  
  /* 配置LPTIM的时钟源为LSI */
  APP_ConfigLptimClock();
  
  /* 初始化LPTIM */
  LPTIM_InitStruct.Prescaler = LL_LPTIM_PRESCALER_DIV128;        /* 128分频 */
  LPTIM_InitStruct.UpdateMode = LL_LPTIM_UPDATE_MODE_IMMEDIATE;  /* 立即更新模式 */
  if (LL_LPTIM_Init(LPTIM, &LPTIM_InitStruct) != SUCCESS)
  {
    APP_ErrorHandler();
  }
  
  /* 点亮LED*/
  BSP_LED_On(LED_GREEN);
  
  /* 等待按键按下 */
  while (BSP_PB_GetState(BUTTON_USER) != 0)
  {
  }

  /* 关闭LED */
  BSP_LED_Off(LED_GREEN);
  
  /* 配置LPTIM连续模式运行，并使能中断 */
  APP_ConfigLptim();
  
  while (1)
  {
    /* 进入STOP模式 */
    APP_EnterStop();
    
    /* LED灯闪烁 */
    BSP_LED_Toggle(LED_GREEN);
  }
}

/**
  * @brief  配置LPTIM时钟
  * @param  无
  * @retval 无
  */
static void APP_ConfigLptimClock(void)
{
  /* 使能LSI */
  LL_RCC_LSI_Enable();
  while(LL_RCC_LSI_IsReady() != 1)
  {
  }
  
  /* 选择LTPIM的时钟源为LSI */
  LL_RCC_SetLPTIMClockSource(LL_RCC_LPTIM1_CLKSOURCE_LSI);
  
  /* 使能LPTIM */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_LPTIM1);
}

/**
  * @brief  配置LPTIM
  * @param  无
  * @retval 无
  */
static void APP_ConfigLptim(void)
{   
  /* 使能LPTIM匹配中断 */
  LL_LPTIM_EnableIT_ARRM(LPTIM);
  
  /* 使能LPTIM */
  LL_LPTIM_Enable(LPTIM);
  
  /* 设置重载值 */
  LL_LPTIM_SetAutoReload(LPTIM, 51);
  
  /* 启动循环模式 */
  LL_LPTIM_StartCounter(LPTIM, LL_LPTIM_OPERATING_MODE_CONTINUOUS);
}

/**
  * @brief  进入Stop模式
  * @param  无
  * @retval 无
  */
static void APP_EnterStop(void)
{
  /* 使能PWR时钟 */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  
  /* 低功耗运行模式 */
  LL_PWR_EnableLowPowerRunMode();

  /* 进入DeepSleep模式 */
  LL_LPM_EnableDeepSleep();
  
  /* 等待事件指令 */
  __SEV();
  __WFE();
  __WFE();

  LL_LPM_EnableSleep();
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
