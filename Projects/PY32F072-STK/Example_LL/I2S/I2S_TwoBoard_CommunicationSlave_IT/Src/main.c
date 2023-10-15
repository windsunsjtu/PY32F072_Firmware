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
static void APP_I2STransmit_IT(uint16_t *pData, uint16_t Size);
static void APP_I2SReceive_IT(uint16_t *pData, uint16_t Size);
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
  
  /* 使能NVIC中断 */
  NVIC_SetPriority(SPI2_IRQn,0);
  NVIC_EnableIRQ(SPI2_IRQn);
  
  /* 使能I2S2时钟 */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);

  /* 初始化I2S2(从收) */
  I2S_InitStruct.Mode              = LL_I2S_MODE_SLAVE_RX; /* 从收 */
  I2S_InitStruct.Standard          = LL_I2S_STANDARD_PHILIPS;
  I2S_InitStruct.DataFormat        = LL_I2S_DATAFORMAT_16B;
  I2S_InitStruct.MCLKOutput        = LL_I2S_MCLK_OUTPUT_DISABLE;
  I2S_InitStruct.AudioFreq         = LL_I2S_AUDIOFREQ_8K;
  I2S_InitStruct.ClockPolarity     = LL_I2S_POLARITY_HIGH;
  LL_I2S_Init(SPI2, &I2S_InitStruct);

  /* 从收数据 */
  APP_I2SReceive_IT((uint16_t *)RxBuff, DATA_LENGTH);

  /* 等待接收完成 */
  while (I2sReady != SET)
  {
  }
  I2sReady = RESET;
  
  /* 初始化I2S2(从发) */
  LL_I2S_Disable(SPI2); /* 不使能，以完成重新初始化 */
  I2S_InitStruct.Mode              = LL_I2S_MODE_SLAVE_TX; /* 从发 */
  I2S_InitStruct.Standard          = LL_I2S_STANDARD_PHILIPS;
  I2S_InitStruct.DataFormat        = LL_I2S_DATAFORMAT_16B;
  I2S_InitStruct.MCLKOutput        = LL_I2S_MCLK_OUTPUT_DISABLE;
  I2S_InitStruct.AudioFreq         = LL_I2S_AUDIOFREQ_8K;
  I2S_InitStruct.ClockPolarity     = LL_I2S_POLARITY_HIGH;
  LL_I2S_Init(SPI2, &I2S_InitStruct);
  
  /* 从发数据 */
  APP_I2STransmit_IT((uint16_t *)TxBuff, DATA_LENGTH);
  /* 等待发送完成 */
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
  * @brief  I2S2发送函数
  * @param  无
  * @retval 无
  */
static void APP_I2STransmit_IT(uint16_t *pData, uint16_t Size)
{
  /* 记录发送数据、发送次数 */
  pTxBuffPtr = pData;
  TxXferCount = Size;
  
  /* 使能发送数据寄存器空中断、使能错误中断 */
  LL_I2S_EnableIT_TXE(SPI2);
  LL_I2S_EnableIT_ERR(SPI2);
  
  /* 使能I2S2 */
  LL_I2S_Enable(SPI2);
}

/**
  * @brief  I2S2接收函数
  * @param  无
  * @retval 无
  */
static void APP_I2SReceive_IT(uint16_t *pData, uint16_t Size)
{
  /* 记录接收数据、接收次数 */
  pRxBuffPtr = pData;
  RxXferCount = Size;
  
  /* 使能接收数据寄存器满中断、使能错误中断 */
  LL_I2S_EnableIT_RXNE(SPI2);
  LL_I2S_EnableIT_ERR(SPI2);
  
  /* 使能I2S2 */
  LL_I2S_Enable(SPI2);
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
  * @brief  I2S2中断处理函数
  * @param  无
  * @retval 无
  */
void APP_I2sIRQCallback(void)
{
  /* 接收数据 */
  if ((LL_I2S_IsActiveFlag_OVR(SPI2) == 0) &&
      (LL_I2S_IsActiveFlag_RXNE(SPI2) == 1) && (LL_I2S_IsEnabledIT_RXNE(SPI2) == 1))
  {
    (*pRxBuffPtr) = LL_I2S_ReceiveData16(SPI2);
    pRxBuffPtr++;
    RxXferCount--;
    
    if (RxXferCount == 0U)
    {
      LL_I2S_DisableIT_RXNE(SPI2);
      LL_I2S_DisableIT_ERR(SPI2);

      I2sReady = SET;
    }
    
    return;
  }
  
  /* 发送数据 */
  if ((LL_I2S_IsActiveFlag_TXE(SPI2) == 1) && (LL_I2S_IsEnabledIT_TXE(SPI2) == 1))
  {
    LL_I2S_TransmitData16(SPI2, (*pTxBuffPtr));
    pTxBuffPtr++;
    TxXferCount--;
    
    if (TxXferCount == 0U)
    {
      LL_I2S_DisableIT_TXE(SPI2);
      LL_I2S_DisableIT_ERR(SPI2);

      I2sReady = SET;
    }
    
    return;
  }
  
  /* 错误处理 */
  if (LL_I2S_IsEnabledIT_ERR(SPI2) == 1)
  {
    if((LL_I2S_IsActiveFlag_OVR(SPI2) == 1) || (LL_I2S_IsActiveFlag_UDR(SPI2) == 1))
    {
      APP_ErrorHandler();
    }
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
