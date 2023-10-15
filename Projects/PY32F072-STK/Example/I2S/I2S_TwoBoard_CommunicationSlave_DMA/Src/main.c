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
I2S_HandleTypeDef I2sHandle;

DMA_HandleTypeDef HdmaCh1;
DMA_HandleTypeDef HdmaCh2;

uint16_t TxBuff[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
uint16_t RxBuff[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
#define DATA_LENGTH       16

/* Private user code ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void APP_SystemClockConfig(void);
static void APP_CheckEndOfTransfer(void);
static uint8_t APP_Buffercmp8(uint8_t* pBuffer1, uint8_t* pBuffer2, uint8_t BufferLength);
static void APP_LedBlinking(void);

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
  
  /* 初始化按键BUTTON */
  BSP_PB_Init(BUTTON_KEY,BUTTON_MODE_GPIO);

  /*等待按键按下*/
  while(BSP_PB_GetState(BUTTON_KEY) == 1)
  {
  }
  
  /* 初始化I2S(从收) */
  I2sHandle.Instance          = SPI2;
  I2sHandle.Init.Mode         = I2S_MODE_SLAVE_RX; /* 从收 */
  I2sHandle.Init.Standard     = I2S_STANDARD_PHILIPS;
  I2sHandle.Init.DataFormat   = I2S_DATAFORMAT_16B;
  I2sHandle.Init.MCLKOutput   = I2S_MCLKOUTPUT_DISABLE;
  I2sHandle.Init.AudioFreq    = I2S_AUDIOFREQ_8K;
  I2sHandle.Init.CPOL         = I2S_CPOL_HIGH;  
  if (HAL_I2S_Init(&I2sHandle) != HAL_OK)
  {
    APP_ErrorHandler();
  }
  
  /* 从收数据 */
  if (HAL_I2S_Receive_DMA(&I2sHandle, (uint16_t *)RxBuff, DATA_LENGTH) != HAL_OK)
  {
    APP_ErrorHandler();
  }
  /* 等待接收完成 */
  while (HAL_I2S_GetState(&I2sHandle) != HAL_I2S_STATE_READY)
  {
  }
  
  /* 初始化I2S(从发) */
  I2sHandle.Instance          = SPI2;
  I2sHandle.Init.Mode         = I2S_MODE_SLAVE_TX; /* 从发 */
  I2sHandle.Init.Standard     = I2S_STANDARD_PHILIPS;
  I2sHandle.Init.DataFormat   = I2S_DATAFORMAT_16B;
  I2sHandle.Init.MCLKOutput   = I2S_MCLKOUTPUT_DISABLE;
  I2sHandle.Init.AudioFreq    = I2S_AUDIOFREQ_8K;
  I2sHandle.Init.CPOL         = I2S_CPOL_HIGH;  
  if (HAL_I2S_Init(&I2sHandle) != HAL_OK)
  {
    APP_ErrorHandler();
  }
  
  /* 从发数据 */
  if (HAL_I2S_Transmit_DMA(&I2sHandle, (uint16_t *)TxBuff, DATA_LENGTH) != HAL_OK)
  {
    APP_ErrorHandler();
  }
  /* 等待发送完成 */
  while (HAL_I2S_GetState(&I2sHandle) != HAL_I2S_STATE_READY)
  {
  }
  
  /* 检查接收到的数据 */
  APP_CheckEndOfTransfer();
  
  /* 无限循环 */
  while (1)
  {
  }
}

/**
  * @brief  校验数据函数
  * @param  无
  * @retval 无
  */
static void APP_CheckEndOfTransfer(void)
{
  /* 比较发送数据和接收数据 */
  if(APP_Buffercmp8((uint8_t*)TxBuff, (uint8_t*)RxBuff, DATA_LENGTH))
  {
    /* 错误处理 */
    APP_LedBlinking();
  }
  else
  {
    /* 如果数据接收到，则打开 LED */
    BSP_LED_On(LED_GREEN);
  }
}

/**
  * @brief  字符比较函数
  * @param  pBuffer1：待比较缓冲区1
  * @param  pBuffer2：待比较缓冲区2
  * @param  BufferLength：待比较字符的个数
  * @retval 0：比较值相同；1：比较值不同
  */
static uint8_t APP_Buffercmp8(uint8_t* pBuffer1, uint8_t* pBuffer2, uint8_t BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return 1;
    }
    pBuffer1++;
    pBuffer2++;
  }

  return 0;
}

/**
  * @brief  LED灯闪烁
  * @param  无
  * @retval 无
  */
static void APP_LedBlinking(void)
{
  while (1)
  {
    BSP_LED_Toggle(LED_GREEN);
    HAL_Delay(500);
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
