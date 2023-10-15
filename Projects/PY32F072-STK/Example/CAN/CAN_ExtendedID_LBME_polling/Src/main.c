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
CAN_HandleTypeDef CanHandle;
CAN_FilterTypeDef CanFilter;

CAN_TxHeaderTypeDef CanTxHeader;
uint8_t TxData[] = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8};

CAN_RxHeaderTypeDef CanRxHeader;
uint8_t RxData[8] = {0};

uint8_t DLCtoBytes[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};

/* Private user code ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void APP_SystemClockConfig(void);

/**
  * @brief  应用程序入口函数.
  * @retval int
  */
int main(void)
{
  uint32_t i;
  
  /* 初始化所有外设，Flash接口，SysTick */
  HAL_Init();
  
  /* 系统时钟配置 */
  APP_SystemClockConfig(); 
  
  /* 初始化调试串口(printf使用) */
  BSP_USART_Config();
  
  /* 初始化CAN */
  CanHandle.Instance                      = CAN1;
  CanHandle.Init.FrameFormat              = CAN_FRAME_CLASSIC;
  CanHandle.Init.Mode                     = CAN_MODE_LOOPBACK_EXT_ACK; /* 外部循环模式，使能自应答，需将CAN的收发引脚短接 */
  CanHandle.Init.Prescaler                = 6U; 
  CanHandle.Init.NominalSyncJumpWidth     = 4U;/* 250KHz */
  CanHandle.Init.NominalTimeSeg1          = 12U;
  CanHandle.Init.NominalTimeSeg2          = 4U;
  if (HAL_CAN_Init(&CanHandle) != HAL_OK)
  {
    APP_ErrorHandler();
  }
  
  /* 配置CAN筛选器 */
  CanFilter.IdType         = CAN_EXTENDED_ID;
  CanFilter.FilterChannel  = CAN_FILTER_CHANNEL_0;
  CanFilter.Rank           = CAN_FILTER_RANK_CHANNEL_NUMBER;
  CanFilter.FilterID       = 0x12345678; /* 与发送消息ID一致 */
  CanFilter.FilterFormat   = 0xFFFFFFFF;
  CanFilter.MaskID         = 0x0; /* 接收时仅ID的29位作比较 */
  CanFilter.MaskFormat     = 0xFFFFFFFF;
  if (HAL_CAN_ConfigFilter(&CanHandle, &CanFilter) != HAL_OK)
  {
    APP_ErrorHandler();
  }
  
  /* 启动CAN */
  if (HAL_CAN_Start(&CanHandle) != HAL_OK)
  {
    APP_ErrorHandler();
  }
  
  /* 添加消息到PTB */
  CanTxHeader.Identifier   = 0x12345678; 
  CanTxHeader.IdType       = CAN_EXTENDED_ID;
  CanTxHeader.TxFrameType  = CAN_DATA_FRAME;
  CanTxHeader.FrameFormat  = CAN_FRAME_CLASSIC;
  CanTxHeader.Handle       = 0x0;
  CanTxHeader.DataLength   = CAN_DLC_BYTES_8;
  if (HAL_CAN_AddMessageToTxFifo(&CanHandle, &CanTxHeader, TxData, CAN_TX_FIFO_PTB) != HAL_OK)
  {
    APP_ErrorHandler();
  }
  
  /* 使能PTB发送 */
  if (HAL_CAN_ActivateTxRequest(&CanHandle, CAN_TXFIFO_PTB_SEND) != HAL_OK)
  {
    APP_ErrorHandler();
  }
  
  /* 等待接收完成 */
  while (__HAL_CAN_GET_RX_FIFO_FILL_LEVEL(&CanHandle) == CAN_RX_FIFO_EMPTY)
  {
  }
  
  /* 读接收到的数据 */
  if (HAL_CAN_GetRxMessage(&CanHandle, &CanRxHeader, RxData) != HAL_OK)
  {
    APP_ErrorHandler();
  }
  
  /* 打印接收到的数据 */
  printf("decimal data length: %d\r\n", DLCtoBytes[CanRxHeader.DataLength]);
  printf("hexadecimal data: \r\n");
  for (i = 0; i < DLCtoBytes[CanRxHeader.DataLength]; i++)
  {
    printf("%x ", RxData[i]);
  }
  
  /* 无限循环 */
  while (1)
  {
    
  }
}

/**
  * @brief  CAN接收完成回调函数
  * @param  hcan：CANFD句柄
  * @retval 无
  */
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef *hcan)
{
  uint32_t i;
  
  /* 读取接收到的消息 */
  HAL_CAN_GetRxMessage(hcan, &CanRxHeader, RxData);
  
  /* 打印接收到的数据 */
  printf("decimal data length: %d\r\n", DLCtoBytes[CanRxHeader.DataLength]);
  printf("hexadecimal data: \r\n");
  for (i = 0; i < DLCtoBytes[CanRxHeader.DataLength]; i++)
  {
    printf("%x ", RxData[i]);
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
