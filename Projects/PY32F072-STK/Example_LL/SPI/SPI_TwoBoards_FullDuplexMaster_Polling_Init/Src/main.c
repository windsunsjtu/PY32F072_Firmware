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
/* 发送缓冲区定义 */
uint8_t aTxBuffer[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
uint8_t ubNbDataToTransmit = sizeof(aTxBuffer);
uint8_t ubTransmitIndex = 0;

/* 接收缓冲区定义 */
uint8_t aRxBuffer[sizeof(aTxBuffer)] = {0};
uint8_t ubNbDataToReceive = sizeof(aTxBuffer);
uint8_t ubReceiveIndex = 0;

/* Private user code ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void APP_SystemClockConfig(void);
static void APP_ConfigSPI(void);
static void APP_SpiMasterTransmitReceive(void);
static void APP_WaitAndCheckEndOfTransfer(void);
static uint8_t APP_Buffercmp8(uint8_t* pBuffer1, uint8_t* pBuffer2, uint8_t BufferLength);
static void APP_LedBlinking(void);

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

 /* 初始化SPI */
  APP_ConfigSPI();

  /*等待按键按下*/
  while(BSP_PB_GetState(BUTTON_KEY) == 1);
  
  /* 使能 SPI1 */
  LL_SPI_Enable(SPI1);
  
  /* 主机收发数据 */
  APP_SpiMasterTransmitReceive();
  
  /* 等待传输结束并检查接收到的数据 */
  APP_WaitAndCheckEndOfTransfer();

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
  * @brief  SPI1配置函数
  * @param  无
  * @retval 无
  */
static void APP_ConfigSPI(void)
{
  LL_SPI_InitTypeDef SPI_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* 使能时钟 */
  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SPI1);

  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
  /**SPI1 引脚配置
  PB3   ------> SPI1_SCK
  PB4   ------> SPI1_MISO
  PB5   ------> SPI1_MOSI
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_3;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_4;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_5;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* SPI1 中断配置 */
  NVIC_SetPriority(SPI1_IRQn, 1);
  NVIC_EnableIRQ(SPI1_IRQn);

  /* SPI1 参数配置*/
  SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
  SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_HIGH;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
  SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV128;
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 0x0U;
  LL_SPI_Init(SPI1, &SPI_InitStruct);
}

/**
  * @brief  SPI1数据收发函数
  * @param  无
  * @retval 无
  */
static void APP_SpiMasterTransmitReceive(void)
{
  uint32_t             txallowed = 1U;
  
  /* 轮询数据收发 */
  while((ubReceiveIndex < ubNbDataToReceive) || (ubTransmitIndex < ubNbDataToTransmit))
  {
    /* 等待发送(发送条件：发送数据寄存器空，数据未发送完成，接收完成) */
    if ((LL_SPI_IsActiveFlag_TXE(SPI1) == 1) && (ubTransmitIndex < ubNbDataToTransmit) && txallowed == 1U)
    {
      LL_SPI_TransmitData8(SPI1, aTxBuffer[ubTransmitIndex++]);
      txallowed = 0U;
    }
    
    /* 等待接收(接收条件：接收数据寄存器满，数据未接收完成) */
    if((LL_SPI_IsActiveFlag_RXNE(SPI1) == 1) && (ubReceiveIndex < ubNbDataToReceive))
    {
      aRxBuffer[ubReceiveIndex++] = LL_SPI_ReceiveData8(SPI1);
      txallowed = 1U;
    }
  }
}

/**
  * @brief  SPI1等待传输完成，并校验数据
  * @param  无
  * @retval 无
  */
static void APP_WaitAndCheckEndOfTransfer(void)
{
  /* 1 - 等待传输结束 */
  while (ubTransmitIndex != ubNbDataToTransmit)
  {
  }
  /* 关 TXE 中断 */
  LL_SPI_DisableIT_TXE(SPI1);

  /* 2 - 等待接收结束 */
  while (ubNbDataToReceive > ubReceiveIndex)
  {
  }
  /* 关 RXNE 中断 */
  LL_SPI_DisableIT_RXNE(SPI1);

  /* 3 - 比较发送数据和接收数据 */
  if(APP_Buffercmp8((uint8_t*)aTxBuffer, (uint8_t*)aRxBuffer, ubNbDataToTransmit))
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
    LL_mDelay(200);
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
