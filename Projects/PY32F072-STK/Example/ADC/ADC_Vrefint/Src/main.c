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
ADC_HandleTypeDef        AdcHandle;
uint16_t                 aADCxConvertedData;
ADC_ChannelConfTypeDef   sConfig;
TIM_HandleTypeDef        TimHandle;
TIM_MasterConfigTypeDef  sMasterConfig;
uint16_t                 T_VCC;

/* Private user code ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void APP_SystemClockConfig(void);
static void APP_AdcConfig(void);
static void APP_TimConfig(void);

/**
  * @brief  应用程序入口函数.
  * @retval int
  */
int main(void)
{ 
  /* 初始化所有外设，Flash接口，SysTick */
  HAL_Init();
  
  /* 系统时钟配置 */
  APP_SystemClockConfig(); 

  /* 初始化LED */
  BSP_LED_Init(LED_GREEN);
   
  /* 初始化调试串口(printf使用) */
  BSP_USART_Config();

  /* ADC配置 */
  APP_AdcConfig();
  
  /* ADC开启转换 */
  HAL_ADC_Start(&AdcHandle);
  
  /* TIM配置 */
  APP_TimConfig();

  while(1)
  {
     /* 等待ADC转换完成 */
    if(HAL_ADC_PollForConversion(&AdcHandle, 2000) == HAL_OK)
    {
      /* 获取ADC值 */
      aADCxConvertedData = HAL_ADC_GetValue(&AdcHandle); 
       
      T_VCC = 4095*1200/aADCxConvertedData; 

      /* 打印VCC电压 */
      printf("VCC : %dmV",T_VCC);
      BSP_LED_Toggle(LED_GREEN);
    }
  }   
}

static void APP_AdcConfig()
{   
  ADC_ChannelConfTypeDef   sConfig;  
  RCC_PeriphCLKInitTypeDef RCC_PeriphCLKInit;
  
    /* 使能ADC时钟 */
  __HAL_RCC_ADC_CLK_ENABLE();
  
  RCC_PeriphCLKInit.PeriphClockSelection= RCC_PERIPHCLK_ADC;
  RCC_PeriphCLKInit.ADCClockSelection   = RCC_ADCCLKSOURCE_PCLK_DIV4;
  HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInit);
  
  AdcHandle.Instance = ADC1;
  
  AdcHandle.Init.Resolution            = ADC_RESOLUTION_12B;             /* 分辨率12位 */
  AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;            /* 对齐方式右对齐 */
  AdcHandle.Init.ScanConvMode          = ADC_SCAN_DISABLE;               /* 扫描方式关闭 */
  AdcHandle.Init.ContinuousConvMode    = DISABLE;                        /* 单次模式 */
  AdcHandle.Init.NbrOfConversion       = 1;                              /* 转换通道数1 */
  AdcHandle.Init.DiscontinuousConvMode = DISABLE;                        /* 间断模式不使能 */
  AdcHandle.Init.NbrOfDiscConversion   = 1;                              /* 间断模式短序列长度为1 */
  AdcHandle.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T15_TRGO;  /* TIM15 TRGO触发 */
  /* ADC初始化 */
  if (HAL_ADC_Init(&AdcHandle) != HAL_OK)
  {
    APP_ErrorHandler();
  }
 
  sConfig.Channel      = ADC_CHANNEL_VREFINT;
  sConfig.Rank         = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES_5;
  /* ADC通道配置 */
  if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
  {
    APP_ErrorHandler();
  }
  /* ADC校准 */
  if(HAL_ADCEx_Calibration_Start(&AdcHandle) != HAL_OK)
  {
    APP_ErrorHandler();
  }
}

/**
  * @brief  TIM配置函数
  * @param  无
  * @retval 无
  */
static void APP_TimConfig(void)
{
  __HAL_RCC_TIM15_CLK_ENABLE();                                       /* TIM15时钟使能 */
  TimHandle.Instance = TIM15;                                         /* TIM15 */
  TimHandle.Init.Period            = 8000 - 1;                        /* TIM15重装载值位8000-1 */
  TimHandle.Init.Prescaler         = 1000 - 1;                        /* 预分频为1000-1 */
  TimHandle.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;          /* 时钟不分频 */
  TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;              /* 向上计数 */
  TimHandle.Init.RepetitionCounter = 0;                               /* 不重复 */
  TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;  /* 自动重装载寄存器没有缓冲 */
  if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)                        /* 初始化TIM15 */
  {
    APP_ErrorHandler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;                /* 选择更新事件作为触发源 */
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;        /* 主/从模式无作用 */
  HAL_TIMEx_MasterConfigSynchronization(&TimHandle, &sMasterConfig);  /* 配置TIM15*/
  if (HAL_TIM_Base_Start(&TimHandle) != HAL_OK)                       /* TIM15启动 */
  {
    APP_ErrorHandler();
  }
}

/**
  * @brief  系统时钟配置函数
  * @param  无
  * @retval 无
  */
static void APP_SystemClockConfig(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /* 振荡器配置 */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE; /* 选择振荡器HSE,HSI,LSI,LSE */
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;                          /* 开启HSI */
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;                          /* HSI 1分频 */
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_8MHz;  /* 配置HSI时钟8MHz */
  RCC_OscInitStruct.HSEState = RCC_HSE_OFF;                         /* 关闭HSE */
  /*RCC_OscInitStruct.HSEFreq = RCC_HSE_16_32MHz;*/
  RCC_OscInitStruct.LSIState = RCC_LSI_OFF;                         /* 关闭LSI */
  RCC_OscInitStruct.LSEState = RCC_LSE_OFF;                         /* 关闭LSE */
  /*RCC_OscInitStruct.LSEDriver = RCC_LSEDRIVE_MEDIUM;*/
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_OFF;                     /* 关闭PLL */
  /*RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_NONE;*/
  /*RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;*/
  /* 配置振荡器 */
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    APP_ErrorHandler();
  }

  /* 时钟源配置 */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1; /* 选择配置时钟 HCLK,SYSCLK,PCLK1 */
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSISYS; /* 选择HSI作为系统时钟 */
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;     /* AHB时钟 1分频 */
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;      /* APB时钟 1分频 */
  /* 配置时钟源 */
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    APP_ErrorHandler();
  }
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
