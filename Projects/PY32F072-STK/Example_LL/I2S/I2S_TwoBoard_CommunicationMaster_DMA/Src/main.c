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
LL_I2S_InitTypeDef I2S_InitStruct = {0};

uint16_t      TxBuff[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
uint16_t      *pTxBuffPtr;
__IO uint16_t TxXferCount;

uint16_t RxBuff[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint16_t      *pRxBuffPtr;
__IO uint16_t RxXferCount;

__IO ITStatus I2sReady = RESET;

/* Private user code ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define DATA_LENGTH       16

/* Private function prototypes -----------------------------------------------*/
static void APP_SystemClockConfig(void);
static void APP_ConfigI2SPin(void);
static void APP_ConfigDma(void);
static void APP_I2STransmit_DMA(uint16_t *pData, uint16_t Size);
static void APP_I2SReceive_DMA(uint16_t *pData, uint16_t Size);
static void APP_CheckEndOfTransfer(void);
static uint8_t APP_Buffercmp8(uint8_t* pBuffer1, uint8_t* pBuffer2, uint8_t BufferLength);
static void APP_LedBlinking(void);

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

  /* 配置系统时钟 */
  APP_SystemClockConfig();

  /* 初始化LED */
  BSP_LED_Init(LED_GREEN);

  /* 初始化按键BUTTON */
  BSP_PB_Init(BUTTON_KEY,BUTTON_MODE_GPIO);

  /* 等待按键按下 */
  while(BSP_PB_GetState(BUTTON_KEY) == 1)
  {
  }
  
  /* 配置I2S2引脚 */
  APP_ConfigI2SPin();
  
  /* 配置I2S2的DMA */
  APP_ConfigDma();
  
  /* 使能I2S2时钟 */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);

  /* 初始化I2S2(主发) */
  I2S_InitStruct.Mode              = LL_I2S_MODE_MASTER_TX; /* 主发 */
  I2S_InitStruct.Standard          = LL_I2S_STANDARD_PHILIPS;
  I2S_InitStruct.DataFormat        = LL_I2S_DATAFORMAT_16B;
  I2S_InitStruct.MCLKOutput        = LL_I2S_MCLK_OUTPUT_DISABLE;
  I2S_InitStruct.AudioFreq         = LL_I2S_AUDIOFREQ_8K;
  I2S_InitStruct.ClockPolarity     = LL_I2S_POLARITY_HIGH;
  LL_I2S_Init(SPI2, &I2S_InitStruct);

  /* 主发数据 */
  APP_I2STransmit_DMA((uint16_t *)TxBuff, DATA_LENGTH);
  /* 等待发送完成 */
  while (I2sReady != SET)
  {
  }
  I2sReady = RESET;

  /* 等待从机准备好数据发送 */
  LL_mDelay(1);
  
  /* 初始化I2S2(主收) */
  LL_I2S_Disable(SPI2); /* 不使能，以完成重新初始化 */
  I2S_InitStruct.Mode              = LL_I2S_MODE_MASTER_RX; /* 主收 */
  I2S_InitStruct.Standard          = LL_I2S_STANDARD_PHILIPS;
  I2S_InitStruct.DataFormat        = LL_I2S_DATAFORMAT_16B;
  I2S_InitStruct.MCLKOutput        = LL_I2S_MCLK_OUTPUT_DISABLE;
  I2S_InitStruct.AudioFreq         = LL_I2S_AUDIOFREQ_8K;
  I2S_InitStruct.ClockPolarity     = LL_I2S_POLARITY_HIGH;
  LL_I2S_Init(SPI2, &I2S_InitStruct);
  
  /* 主收数据 */
  APP_I2SReceive_DMA((uint16_t *)RxBuff, DATA_LENGTH);
  /* 等待接收完成 */
  while (I2sReady != SET)
  {
  }
  I2sReady = RESET;

  /* 检查接收到的数据 */
  APP_CheckEndOfTransfer();

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
  * @brief  配置I2S2引脚
  * @param  无
  * @retval 无
  */
static void APP_ConfigI2SPin(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);

  /**I2S2 引脚配置
  PB12------> I2S2_WS
  PB13------> I2S2_CK
  PB15------> I2S2_SD
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_12 | LL_GPIO_PIN_13 | LL_GPIO_PIN_15;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/**
  * @brief  DMA配置函数
  * @param  无
  * @retval 无
  */
static void APP_ConfigDma(void)
{
  /*使能DMA时钟、syscfg的时钟*/
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);

  
  /*利用DMA通道LL_DMA_CHANNEL_1进行发送*/
  LL_DMA_ConfigTransfer(DMA1, LL_DMA_CHANNEL_1, LL_DMA_DIRECTION_MEMORY_TO_PERIPH | \
                      LL_DMA_MODE_NORMAL                   | \
                      LL_DMA_PERIPH_NOINCREMENT  | \
                      LL_DMA_MEMORY_INCREMENT  | \
                      LL_DMA_PDATAALIGN_HALFWORD | \
                      LL_DMA_MDATAALIGN_HALFWORD | \
                      LL_DMA_PRIORITY_VERYHIGH);
  
 
  /*利用DMA通道LL_DMA_CHANNEL_2进行接收*/
  LL_DMA_ConfigTransfer(DMA1, LL_DMA_CHANNEL_2, LL_DMA_DIRECTION_PERIPH_TO_MEMORY | \
                      LL_DMA_MODE_NORMAL                   | \
                      LL_DMA_PERIPH_NOINCREMENT  | \
                      LL_DMA_MEMORY_INCREMENT  | \
                      LL_DMA_PDATAALIGN_HALFWORD | \
                      LL_DMA_MDATAALIGN_HALFWORD | \
                      LL_DMA_PRIORITY_LOW);

  /* 配置DMA请求映像*/
  LL_SYSCFG_SetDMARemap(DMA1, LL_DMA_CHANNEL_1, LL_SYSCFG_DMA_MAP_SPI2_WR); /* I2S_SD:TX, DMA1_CH1 */
  LL_SYSCFG_SetDMARemap(DMA1, LL_DMA_CHANNEL_2, LL_SYSCFG_DMA_MAP_SPI2_RD); /* I2S_SD:RX, DMA1_CH2 */
  
  /*设置中断优先级*/
  NVIC_SetPriority(DMA1_Channel1_IRQn, 1);
  /*使能中断*/
  NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  
  /*设置中断优先级*/
  NVIC_SetPriority(DMA1_Channel2_3_IRQn, 1);
  /*使能中断*/
  NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
}

/**
  * @brief  I2S2发送函数
  * @param  无
  * @retval 无
  */
static void APP_I2STransmit_DMA(uint16_t *pData, uint16_t Size)
{
  /* 记录发送数据、发送次数 */
  pTxBuffPtr = pData;
  TxXferCount = Size;
  
  /*配置DMA通道1*/
  LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1);
  LL_DMA_ClearFlag_GI1(DMA1);
  LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_1, (uint32_t)pTxBuffPtr);
  LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_1, LL_SPI_DMA_GetRegAddr(SPI2));
  LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, TxXferCount);
  LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_1);
  LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
  
  /* 使能I2S2 */
  LL_I2S_Enable(SPI2);
  
  /* 使能I2S2的DMA发送请求 */
  LL_I2S_EnableDMAReq_TX(SPI2);
}

/**
  * @brief  I2S2接收函数
  * @param  无
  * @retval 无
  */
static void APP_I2SReceive_DMA(uint16_t *pData, uint16_t Size)
{
  /* 记录接收数据、接收次数 */
  pRxBuffPtr = pData;
  RxXferCount = Size;
  
  /* 主收处理 */
  if (LL_I2S_GetTransferMode(SPI2) == LL_I2S_MODE_MASTER_RX)
  {
    LL_I2S_ClearFlag_OVR(SPI2);
  }
  
  /*配置DMA通道2*/
  LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_2);
  LL_DMA_ClearFlag_GI2(DMA1);
  LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_2, (uint32_t)pRxBuffPtr);
  LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_2, LL_SPI_DMA_GetRegAddr(SPI2));
  LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_2, RxXferCount);
  LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_2);
  LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_2);
  
  /* 使能I2S2 */
  LL_I2S_Enable(SPI2);
  
  /* 使能I2S2的DMA接收请求 */
  LL_I2S_EnableDMAReq_RX(SPI2);
}

/**
  * @brief  等待传输完成，并校验数据
  * @param  无
  * @retval 无
  */
static void APP_CheckEndOfTransfer(void)
{
  /* 比较发送数据和接收数据 */
  if((APP_Buffercmp8((uint8_t*)TxBuff, (uint8_t*)RxBuff, DATA_LENGTH)))
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
  * @brief  利用DMA进行发送的中断处理函数
  * @param  无
  * @retval 无
  */
void APP_DmaChannel1IRQCallback(void)
{
  if((LL_DMA_IsActiveFlag_TC1(DMA1) == 1) && (LL_DMA_IsEnabledIT_TC(DMA1, LL_DMA_CHANNEL_1) == 1))
  {
    LL_DMA_DisableIT_TC(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_ClearFlag_GI1(DMA1);
    
    LL_I2S_DisableDMAReq_TX(SPI2);
    TxXferCount = 0U;
    I2sReady = SET;
  }
}

/**
  * @brief  利用DMA进行接收的中断处理函数
  * @param  无
  * @retval 无
  */
void APP_DmaChannel2_3_IRQCallback(void)
{
  if((LL_DMA_IsActiveFlag_TC2(DMA1) == 1) && (LL_DMA_IsEnabledIT_TC(DMA1, LL_DMA_CHANNEL_2) == 1))
  {
    LL_DMA_DisableIT_TC(DMA1, LL_DMA_CHANNEL_2);
    LL_DMA_ClearFlag_GI2(DMA1);
    
    LL_I2S_DisableDMAReq_RX(SPI2);
    RxXferCount = 0U;
    I2sReady = SET;
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
