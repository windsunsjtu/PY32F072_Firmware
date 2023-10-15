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
#define I2C_ADDRESS        0xA0     /* 本机\从机地址 */
#define I2C_SPEEDCLOCK     100000   /* 通讯速度100K */
#define I2C_STATE_READY    0        /* 就绪状态 */
#define I2C_STATE_BUSY_TX  1        /* 发送状态 */
#define I2C_STATE_BUSY_RX  2        /* 接收状态 */

/* Private variables ---------------------------------------------------------*/
uint8_t aTxBuffer[15] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
uint8_t aRxBuffer[15] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

uint8_t         *pBuffPtr   = NULL;
__IO uint16_t   XferCount   = 0;
__IO uint32_t   Devaddress  = 0;
__IO uint32_t   State       = I2C_STATE_READY;
__IO uint32_t   EventCount  = 0;

/* Private user code ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void APP_SystemClockConfig(void);
static void APP_ConfigI2cMaster(void);
static void APP_MasterTransmit_IT(uint16_t DevAddress, uint8_t *pData, uint16_t Size);
static void APP_MasterReceive_IT(uint16_t DevAddress, uint8_t *pData, uint16_t Size);
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
  /* 配置系统时钟 */
  APP_SystemClockConfig();
  
  /* 初始化LED */
  BSP_LED_Init(LED_GREEN);
  
  /* 初始化按键BUTTON */
  BSP_PB_Init(BUTTON_KEY,BUTTON_MODE_GPIO);
  
  /* 配置I2C */
  APP_ConfigI2cMaster();

  /*等待按键按下*/
  while(BSP_PB_GetState(BUTTON_KEY) == 1)
  {
  }
  
  /* 主机发送数据 */
  APP_MasterTransmit_IT(I2C_ADDRESS, (uint8_t *)aTxBuffer, sizeof(aTxBuffer));
  
  /* 等待主机发送数据完成 */
  while (State != I2C_STATE_READY);
  
  /* 主机接收数据 */
  APP_MasterReceive_IT(I2C_ADDRESS, (uint8_t *)aRxBuffer, sizeof(aRxBuffer));
  
  /* 等待主机接收数据完成 */
  while (State != I2C_STATE_READY);
  
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
  * @brief  I2C配置函数
  * @param  无
  * @retval 无
  */
static void APP_ConfigI2cMaster(void)
{
  /* 使能 GPIOA 的外设时钟 */
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);

  /* 启用 I2C1 的外设时钟 */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

  /* 将 SCL 引脚配置为：可选功能、高速、开漏、上拉 */
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_6;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* 将 SDA 引脚配置为：可选功能、高速、开漏、上拉 */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_6;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  /* 复位I2C */
  LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_I2C1);
  LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_I2C1);
  
  /* 使能NVIC中断 */
  NVIC_SetPriority(I2C1_IRQn, 0);
  NVIC_EnableIRQ(I2C1_IRQn);
  
  /* I2C初始化 */
  LL_I2C_InitTypeDef I2C_InitStruct;
  I2C_InitStruct.PeripheralMode  = LL_I2C_MODE_I2C;
  I2C_InitStruct.ClockSpeed      = I2C_SPEEDCLOCK;
  I2C_InitStruct.DutyCycle       = LL_I2C_DUTYCYCLE_16_9;
  I2C_InitStruct.OwnAddress1     = I2C_ADDRESS;
  I2C_InitStruct.TypeAcknowledge = LL_I2C_NACK;
  I2C_InitStruct.OwnAddrSize     = LL_I2C_OWNADDRESS1_10BIT;
  LL_I2C_Init(I2C1, &I2C_InitStruct);
  
  /* 启用时钟拉伸 */
  /* 复位值是启用时钟延长 */
  /* LL_I2C_EnableClockStretching(I2C1); */
  
  /* 启用广播呼叫 */
  /* 复位值为禁用广播呼叫 */
  /* LL_I2C_EnableGeneralCall(I2C1); */
}

/**
  * @brief  I2C发送函数
  * @param  DevAddress：从机地址
  * @param  pData：要发送数据指针
  * @param  Size：要发送数据大小
  * @retval 无
  */
static void APP_MasterTransmit_IT(uint16_t DevAddress, uint8_t *pData, uint16_t Size)
{
  /* 清pos */
  LL_I2C_DisableBitPOS(I2C1);
  
  /* 将从机地址、要发送数据、数据大小、状态赋给全局变量 */
  pBuffPtr    = pData;
  XferCount   = Size;
  Devaddress  = DevAddress;
  State       = I2C_STATE_BUSY_TX;
  
  /* 产生起始位 */
  LL_I2C_GenerateStartCondition(I2C1);
  
  /* 使能中断 */
  LL_I2C_EnableIT_EVT(I2C1);
  LL_I2C_EnableIT_BUF(I2C1);
  LL_I2C_EnableIT_ERR(I2C1);
}

/**
  * @brief  I2C接收函数
  * @param  DevAddress：从机地址
  * @param  pData：要接收数据指针
  * @param  Size：要接收数据大小
  * @retval 无
  */
static void APP_MasterReceive_IT(uint16_t DevAddress, uint8_t *pData, uint16_t Size)
{
  /* 清pos */
  LL_I2C_DisableBitPOS(I2C1);
  
  /* 将从机地址、要发送数据、数据大小赋给全局变量 */
  pBuffPtr    = pData;
  XferCount   = Size;
  Devaddress  = DevAddress;
  State       = I2C_STATE_BUSY_RX;
  
  /* 使能应答 */
  LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK);
  
  /* 产生起始位 */
  LL_I2C_GenerateStartCondition(I2C1);
  
  /* 使能中断 */
  LL_I2C_EnableIT_EVT(I2C1);
  LL_I2C_EnableIT_BUF(I2C1);
  LL_I2C_EnableIT_ERR(I2C1);
}

/**
  * @brief  I2C中断回调函数
  * @param  无
  * @retval 无
  */
void APP_MasterIRQCallback(void)
{
  /* SB标志位置位 */
  if ((LL_I2C_IsActiveFlag_SB(I2C1) == 1) && (LL_I2C_IsEnabledIT_EVT(I2C1) == 1))
  {
    /* 发送头序列 */
    if (EventCount == 0U)
    {
      LL_I2C_TransmitData8(I2C1, (__LL_I2C_10BIT_HEADER_WRITE(Devaddress)));
    }
    else if (EventCount == 1U)
    {
      LL_I2C_TransmitData8(I2C1, (__LL_I2C_10BIT_HEADER_READ(Devaddress)));
    }
    else
    {
    }
  }
  /* ADD10标志位置位 */
  else if ((LL_I2C_IsActiveFlag_ADD10(I2C1) == 1) && (LL_I2C_IsEnabledIT_EVT(I2C1) == 1))
  {
    /* 发送10位地址的低8位 */
    LL_I2C_TransmitData8(I2C1, (__LL_I2C_10BIT_ADDRESS(Devaddress)));
  }
  /* ADDR标志位置位 */
  else if ((LL_I2C_IsActiveFlag_ADDR(I2C1) == 1) && (LL_I2C_IsEnabledIT_EVT(I2C1) == 1))
  {
    if (State == I2C_STATE_BUSY_RX)
    {
      if (EventCount == 0U)
      {
        LL_I2C_ClearFlag_ADDR(I2C1);
        
        /* 重新产生起始信号 */
        LL_I2C_GenerateStartCondition(I2C1);
        
        EventCount++;
      }
      else
      {
        if (XferCount == 0U)
        {
          LL_I2C_ClearFlag_ADDR(I2C1);
          LL_I2C_GenerateStopCondition(I2C1);
        }
        else if (XferCount == 1U)
        {
          LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_NACK);
          LL_I2C_ClearFlag_ADDR(I2C1);
          LL_I2C_GenerateStopCondition(I2C1);
        }
        else if (XferCount == 2U)
        {
          LL_I2C_EnableBitPOS(I2C1);
          LL_I2C_ClearFlag_ADDR(I2C1);
          LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_NACK);
        }
        else
        {
          LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK);
          LL_I2C_ClearFlag_ADDR(I2C1);
        }
        
        EventCount = 0U;
      }
    }
    else
    {
      LL_I2C_ClearFlag_ADDR(I2C1);
    }
  }
  /* 主机发送方向 */
  else if (LL_I2C_GetTransferDirection(I2C1) == LL_I2C_DIRECTION_WRITE)
  {
    /* TXE标志位置位，BTF标志位未置位 */
    if ((LL_I2C_IsActiveFlag_TXE(I2C1) == 1) && (LL_I2C_IsEnabledIT_BUF(I2C1) == 1) && (LL_I2C_IsActiveFlag_BTF(I2C1) == 0))
    {
      if (XferCount == 0U)
      {
        LL_I2C_DisableIT_BUF(I2C1);
      }
      else
      {
        LL_I2C_TransmitData8(I2C1, *pBuffPtr);
        pBuffPtr++;
        XferCount--;
      }
    }
    /* BTF标志位置位 */
    else if ((LL_I2C_IsActiveFlag_BTF(I2C1) == 1) && (LL_I2C_IsEnabledIT_EVT(I2C1) == 1))
    {
      if (XferCount != 0U)
      {
        LL_I2C_TransmitData8(I2C1, *pBuffPtr);
        pBuffPtr++;
        XferCount--;
      }
      else
      {
        LL_I2C_DisableIT_EVT(I2C1);
        LL_I2C_DisableIT_BUF(I2C1);
        LL_I2C_DisableIT_ERR(I2C1);
        
        LL_I2C_GenerateStopCondition(I2C1);
        State = I2C_STATE_READY;
        
      }
    }
  }
  /* 主机接收方向 */
  else
  {
    /* RXNE标志位置位，BTF标志位未置位 */
    if ((LL_I2C_IsActiveFlag_RXNE(I2C1) == 1) && (LL_I2C_IsEnabledIT_BUF(I2C1) == 1) && (LL_I2C_IsActiveFlag_BTF(I2C1) == 0))
    {
      if (XferCount > 3U)
      {
        *pBuffPtr = LL_I2C_ReceiveData8(I2C1);
        pBuffPtr++;
        XferCount--;
        
        if (XferCount == (uint16_t)3)
        {
          /* 禁用 BUF 中断，这有助于正确处理 BTF 子程序的最后 4 个字节 */
          LL_I2C_DisableIT_BUF(I2C1);
        }
      }
      else if ((XferCount == 1U) || (XferCount == 0U))
      {
        LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_NACK);
        
        LL_I2C_DisableIT_EVT(I2C1);
        LL_I2C_DisableIT_BUF(I2C1);
        LL_I2C_DisableIT_ERR(I2C1);
        
        *pBuffPtr = LL_I2C_ReceiveData8(I2C1);
        pBuffPtr++;
        XferCount--;
        
        State = I2C_STATE_READY;
      }
    }
    /* BTF标志位置位 */
    else if ((LL_I2C_IsActiveFlag_BTF(I2C1) == 1) && (LL_I2C_IsEnabledIT_EVT(I2C1) == 1))
    {
      if (XferCount == 4U)
      {
        /* 禁用 BUF 中断，如果在 N-1 和 N 字节之间存在接收延迟，这有助于正确处理 BTF 子程序的最后 2 个字节 */
        LL_I2C_DisableIT_BUF(I2C1);
        
        *pBuffPtr = LL_I2C_ReceiveData8(I2C1);
        pBuffPtr++;
        XferCount--;
      }
      else if (XferCount == 3U)
      {
        /* 禁用 BUF 中断，如果在 N-1 和 N 字节之间存在接收延迟，这有助于正确处理 BTF 子程序的最后 2 个字节 */
        LL_I2C_DisableIT_BUF(I2C1);
        
        LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_NACK);
        
        *pBuffPtr = LL_I2C_ReceiveData8(I2C1);
        pBuffPtr++;
        XferCount--;
      }
      else if (XferCount == 2U)
      {
        LL_I2C_GenerateStopCondition(I2C1);
        
        *pBuffPtr = LL_I2C_ReceiveData8(I2C1);
        pBuffPtr++;
        XferCount--;
        
        *pBuffPtr = LL_I2C_ReceiveData8(I2C1);
        pBuffPtr++;
        XferCount--;
        
        LL_I2C_DisableIT_EVT(I2C1);
        LL_I2C_DisableIT_ERR(I2C1);
        
        State = I2C_STATE_READY;
      }
      else
      {
        *pBuffPtr = LL_I2C_ReceiveData8(I2C1);
        pBuffPtr++;
        XferCount--;
      }
    }
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
  if(APP_Buffercmp8((uint8_t*)aTxBuffer, (uint8_t*)aRxBuffer, sizeof(aRxBuffer)))
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
    LL_mDelay(500);
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
