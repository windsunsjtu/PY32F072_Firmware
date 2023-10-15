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
RTC_HandleTypeDef RTCinit;
RTC_TimeTypeDef RTCtime;
uint8_t gsecond, gminute, ghour;

/* Private user code ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void APP_SystemClockConfig(void);
static void APP_RtcInit(void);
static void APP_RtcSetAlarm_IT(uint8_t Sec, uint8_t Min, uint8_t Hour);

/**
  * @brief  应用程序入口函数.
  * @retval int
  */
int main(void)
{
  /* 初始化systick */
  HAL_Init();
  
  /* 时钟使能LSE */
  APP_SystemClockConfig();
  
  /* 初始化LED */
  BSP_LED_Init(LED_GREEN);

  /* 初始化按键BUTTON */
  BSP_PB_Init(BUTTON_KEY,BUTTON_MODE_GPIO);

  /* 初始化调试串口(printf使用) */
  BSP_USART_Config();
  
  /* RTC初始化 */
  APP_RtcInit();
  
  /* LED开启 */
  BSP_LED_On(LED_GREEN);
  
  /*等待按键按下*/
  while (BSP_PB_GetState(BUTTON_USER) != 0)
  {
  }
  
   /* 关闭LED */
  BSP_LED_Off(LED_GREEN);
  
  /* 暂停systick中断 */
  HAL_SuspendTick();
  
  while (1)
  {
    /* 等待同步完成 */
    HAL_RTC_WaitForSynchro(&RTCinit);
    
    /* 获取RTC当前时间,格式为BIN */
    HAL_RTC_GetTime(&RTCinit, &RTCtime, RTC_FORMAT_BIN);
    
    /* 设置RTC闹钟中断 */
    ghour = RTCtime.Hours;
    gminute = RTCtime.Minutes;
    gsecond = RTCtime.Seconds;
    APP_RtcSetAlarm_IT(gsecond, gminute, ghour);
    
    /* 进入STOP模式 */
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
  }
}

/**
  * @brief  RTC初始化函数，RTC设置为22年1月1日星期六，0:0:0
  * @param  无
  * @retval 无
  */
static void APP_RtcInit(void)
{
  RTC_TimeTypeDef Timeinit;
  
  /* RTC初始化 */
  RTCinit.Instance = RTC;                               /* 选择RTC */
  RTCinit.Init.AsynchPrediv = RTC_AUTO_1_SECOND;        /* RTC的1S时基自动计算 */
  /*2022-1-1-00:00:00*/
  RTCinit.DateToUpdate.Year = 22;                       /* 22年 */
  RTCinit.DateToUpdate.Month = RTC_MONTH_JANUARY;       /* 1月 */
  RTCinit.DateToUpdate.Date = 1;                        /* 1日 */
  RTCinit.DateToUpdate.WeekDay = RTC_WEEKDAY_SATURDAY;  /* 星期六 */
  Timeinit.Hours = 0x00;                                /* 0时 */
  Timeinit.Minutes = 0x00;                              /* 0分 */
  Timeinit.Seconds = 0x00;                              /* 0秒 */
  
  /* RTC反初始化 */
  HAL_RTC_DeInit(&RTCinit);
  
  /* RTC初始化 */
  HAL_RTC_Init(&RTCinit);
  
  /* 设置RTC当前时间，格式为BIN */
  HAL_RTC_SetTime(&RTCinit, &Timeinit, RTC_FORMAT_BIN);
}

/**
  * @brief  设置RTC闹钟中断
  * @param  Sec：秒；
  * @param  Min：分；
  * @param  Hour：时；
  * @retval 无
  */
static void APP_RtcSetAlarm_IT(uint8_t Sec, uint8_t Min, uint8_t Hour)
{
  RTC_AlarmTypeDef Alarminit;
  /*00:00:5*/
  RTCinit.Instance = RTC;
  Alarminit.AlarmTime.Hours = Hour;                           /* 时 */
  Alarminit.AlarmTime.Minutes = Min;                          /* 分 */
  Alarminit.AlarmTime.Seconds = Sec + 1;                      /* 秒 */
  HAL_RTC_SetAlarm_IT(&RTCinit, &Alarminit, RTC_FORMAT_BIN);
}

/**
  * @brief  RTC中断执行函数，退出低功耗时，LED灯亮
  * @param  hrtc：RTC句柄
  * @retval 无
  */
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
  BSP_LED_Toggle(LED_GREEN);
  printf("%02d:%02d:%02d\r\n", ghour, gminute, gsecond);
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
