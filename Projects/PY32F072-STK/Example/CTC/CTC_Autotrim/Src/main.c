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
CTC_HandleTypeDef CTChadle;
/* Private user code ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void APP_SystemClockConfig(void);

/**
  * @brief   应用程序入口函数
  * @retval  int
  */
int main(void)
{
  /* 初始化所有外设，Flash接口，SysTick */
  HAL_Init();
  
  /* 配置PA08引脚为MCO功能，输出PLL */
  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_PLLCLK, RCC_MCODIV_1);
  
  /* 系统时钟配置 */
  APP_SystemClockConfig(); 
  
  /* 开启CTC时钟 */
  __HAL_RCC_CTC_CLK_ENABLE();

  /* 初始化CTC配置 */
  CTChadle.Instance             = CTC;                          /* 外设基地址 */
  CTChadle.Init.AutoTrim        = CTC_AUTO_TRIM_ENABLE;         /* 使能自动校准 */
  CTChadle.Init.RefCLKSource    = CTC_REF_CLOCK_SOURCE_LSE;     /* 参考时钟源LSE */
  CTChadle.Init.RefCLKDivider   = CTC_REF_CLOCK_DIV1;           /* 参考时钟分频1分频 */
  CTChadle.Init.RefCLKPolarity  = CTC_REF_CLOCK_POLARITY_RISING;/* 参考时钟上升沿有效 */  
  CTChadle.Init.ReloadValue     = 1465-1;                       /* 计数器重载值 */
  CTChadle.Init.LimitValue      = 1;                            /* 时钟校准时基限值 */
  HAL_CTC_Init(&CTChadle);
  
  /* 开始校准 */
  HAL_CTC_Start_IT(&CTChadle);

  /* 无限循环 */
  while (1)
  {
  }
}

/**
  * @brief   系统时钟配置函数
  * @param   无
  * @retval  无
  */
static void APP_SystemClockConfig(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE;  /* 选择RCC振荡器HSE,HSI,LSE */
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;                                                     /* 开启HSI */
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;                                                     /* 4分频 */
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_16MHz;                            /* 配置HSI输出时钟为16MHz */
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;                                                     /* 开启HSE */
  RCC_OscInitStruct.HSEFreq = RCC_HSE_16_32MHz;                                                /* HSE晶振工作频率16M~32M */
  RCC_OscInitStruct.LSIState = RCC_LSI_OFF;                                                    /* 关闭LSI */
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;                                                     /* 开启LSE */
  RCC_OscInitStruct.LSEDriver = RCC_LSEDRIVE_HIGH;                                             /* LSE默认驱动能力 */
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;                                                 /* 开启PLL */
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;                                         /* 选择PLL源为HSI */
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL3;                                                 /* PLL三倍频 */
  /* 初始化RCC振荡器 */
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    APP_ErrorHandler();
  }

  /* 初始化CPU,AHB,APB总线时钟 */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;/* RCC系统时钟类型 */
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;                                        /* SYSCLK的源选择为HSE */
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;                                            /* APH时钟不分频 */
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;                                             /* APB时钟2分频 */
  /* 初始化RCC系统时钟(FLASH_LATENCY_0=24M以下;FLASH_LATENCY_1=48M) */
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    APP_ErrorHandler();
  }
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
