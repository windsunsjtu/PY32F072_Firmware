/**
  ******************************************************************************
  * @file    main.c
  * @author  MCU Application Team
  * @brief   Main program body
  * @date
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
#define DARA_LENGTH       15

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef Spi1Handle;
uint8_t TxBuff[15] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
uint8_t RxBuff[15] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/* Private user code ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void APP_SystemClockConfig(void);
static void APP_WaitAndCheckEndOfTransfer(void);
static uint8_t APP_Buffercmp8(uint8_t* pBuffer1, uint8_t* pBuffer2, uint8_t BufferLength);
static void APP_LedBlinking(void);

/**
  * @brief  应用程序入口函数.
  * @retval int
  */
int main(void)
{
  /* 复位所有外设，初始化flash接口和systick */
  HAL_Init();
  
  /* 系统时钟配置 */
  APP_SystemClockConfig();
  
  /* 初始化LED */
  BSP_LED_Init(LED_GREEN);

  /* 初始化按键BUTTON */
  BSP_PB_Init(BUTTON_KEY,BUTTON_MODE_GPIO);
  
  /*反初始化SPI配置*/
  Spi1Handle.Instance               = SPI1;                       /* SPI1 */
  Spi1Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;  /* 256分频 */
  Spi1Handle.Init.Direction         = SPI_DIRECTION_2LINES;       /* 全双工 */
  Spi1Handle.Init.CLKPolarity       = SPI_POLARITY_LOW;           /* 时钟极性低 */
  Spi1Handle.Init.CLKPhase          = SPI_PHASE_1EDGE ;           /* 数据采样从第一个时钟边沿开始 */
  Spi1Handle.Init.DataSize          = SPI_DATASIZE_8BIT;          /* SPI数据长度为8bit */
  Spi1Handle.Init.FirstBit          = SPI_FIRSTBIT_MSB;           /* 先发送MSB */
  Spi1Handle.Init.NSS               = SPI_NSS_HARD_INPUT;         /* NSS软件模式 */
  Spi1Handle.Init.Mode = SPI_MODE_SLAVE;                          /* 配置为从机 */
  Spi1Handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;    /* 不使能CRC校验 */
  /* Spi1Handle.Init.CRCPolynomial = 1; */                              /* CRC多项式值 */
  if (HAL_SPI_DeInit(&Spi1Handle) != HAL_OK)                      /* SPI反初始化 */
  {

  }
  
  /*SPI初始化*/
  if (HAL_SPI_Init(&Spi1Handle) != HAL_OK)
  {
    APP_ErrorHandler();
  }
  
  /*SPI Polling方式传输*/
  if (HAL_SPI_TransmitReceive(&Spi1Handle, (uint8_t *)TxBuff, (uint8_t *)RxBuff, DARA_LENGTH, 5000) != HAL_OK)
  {
    APP_ErrorHandler();
  }
  
  /* 等待传输结束并检查接收到的数据 */
  APP_WaitAndCheckEndOfTransfer();
  
  while (1)
  {
  }
}

/**
  * @brief  SPI1等待传输完成，并校验数据
  * @param  无
  * @retval 无
  */
static void APP_WaitAndCheckEndOfTransfer(void)
{
  /* 等待传输结束 */
  while (Spi1Handle.State != HAL_SPI_STATE_READY)
  {}

  /* 比较发送数据和接收数据 */
  if(APP_Buffercmp8((uint8_t*)TxBuff, (uint8_t*)RxBuff, DARA_LENGTH))
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
    BSP_LED_Toggle(LED_GREEN);; 
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

/************************ (C) COPYRIGHT Puya *****END OF FILE****/
