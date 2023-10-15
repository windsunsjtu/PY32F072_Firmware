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
LPTIM_HandleTypeDef       LPTIMConf = {0};
EXTI_HandleTypeDef        ExtiHandle;
/* Private user code ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void APP_RCCOscConfig(void);
/**
  * @brief   应用程序入口函数
  * @retval  int
  */
int main(void)
{
	EXTI_ConfigTypeDef        ExtiCfg;
	
  /* 外设、systick初始化 */
  HAL_Init();
  
  /* 时钟设置 */
  APP_RCCOscConfig();
  
  /* 初始化LED */
  BSP_LED_Init(LED_GREEN);
  
  /* 初始化按键 */
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_GPIO);

  /* LPTIM初始化 */
  LPTIMConf.Instance = LPTIM1;                        /* LPTIM1 */
  LPTIMConf.Init.Prescaler = LPTIM_PRESCALER_DIV128;  /* 128分频 */
  LPTIMConf.Init.UpdateMode = LPTIM_UPDATE_IMMEDIATE; /* 立即更新模式 */
  /*初始化LPTIM*/
  if (HAL_LPTIM_Init(&LPTIMConf) != HAL_OK)
  {
    APP_ErrorHandler();
  }
	
	/* 配置EXTI中LPTIM Line为事件唤醒模式 */
	ExtiCfg.Line = EXTI_LINE_29;
	ExtiCfg.Mode = EXTI_MODE_EVENT;
  HAL_EXTI_SetConfigLine(&ExtiHandle, &ExtiCfg);
	  
  /* 关闭Systick中断 */
  HAL_SuspendTick();

  /* 点亮LED*/
  BSP_LED_On(LED_GREEN);
  
  /* 等待按键按下 */
  while (BSP_PB_GetState(BUTTON_USER) != 0)
  {
  }

  /* 关闭LED */
  BSP_LED_Off(LED_GREEN);
  
	/* 配置LPTIM连续模式运行，并使能中断 */
	HAL_LPTIM_SetContinue_Start_IT(&LPTIMConf, 51);

  while (1)
  {
    /* 进入STOP模式，使用中断唤醒 */
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFE);
		/* 翻转LED灯 */
    BSP_LED_Toggle(LED_GREEN);
  }
}

/**
  * @brief   时钟配置函数
  * @param   无
  * @retval  无
  */
static void APP_RCCOscConfig(void)
{
  RCC_OscInitTypeDef OSCINIT;
  RCC_PeriphCLKInitTypeDef LPTIM_RCC;

  /* LSI时钟配置 */
  OSCINIT.OscillatorType = RCC_OSCILLATORTYPE_LSI;  /* 选择配置LSI */
  OSCINIT.LSIState = RCC_LSI_ON;                    /* LSI开启 */
  /* 时钟初始化 */
  if (HAL_RCC_OscConfig(&OSCINIT) != HAL_OK)
  {
    APP_ErrorHandler();
  }
  
  /* LPTIM时钟配置 */
  LPTIM_RCC.PeriphClockSelection = RCC_PERIPHCLK_LPTIM;     /* 选择配置外设时钟：LPTIM */
  LPTIM_RCC.LptimClockSelection = RCC_LPTIMCLKSOURCE_LSI;   /* 选择LPTIM时钟源：LSI */
  /* 外设时钟初始化 */
  if (HAL_RCCEx_PeriphCLKConfig(&LPTIM_RCC) != HAL_OK)
  {
    APP_ErrorHandler();
  }
  
  /*使能LPTIM时钟*/
  __HAL_RCC_LPTIM_CLK_ENABLE();
}

/**
  * @brief   错误执行函数
  * @param   无
  * @retval  无
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
