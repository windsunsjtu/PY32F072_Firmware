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
#define ADC_CALIBRATION_TIMEOUT_MS        2
#define VDDA_APPLI                        ((uint32_t)3300)
#define USE_TIMEOUT                       0

/* Private variables ---------------------------------------------------------*/
__IO uint32_t wait_loop_index = 0;
__IO uint32_t ADCxConvertedDatas[4];
__IO uint16_t uhADCxConvertedData_Voltage_mVolt = 0;

/* Private user code ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void APP_SystemClockConfig(void);
static void APP_AdcDmaConfig(void);
static void APP_AdcConfig(void);
static void APP_AdcCalibrate(void);

/**
  * @brief  应用程序入口函数.
  * @param  无
  * @retval int
  */
int main(void)
{
  /* 使能SYSCFG和PWR时钟 */
  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  /* 配置系统时钟 */
  APP_SystemClockConfig();
  
  BSP_USART_Config();
  
  /* DMA配置 */
  APP_AdcDmaConfig();
  
  /* ADC配置 */
  APP_AdcConfig();
  
  /* ADC校准 */
  APP_AdcCalibrate();
  
  LL_mDelay(1);
  
  /* 使能ADC */
  LL_ADC_Enable(ADC1);

  LL_mDelay(1);
  while (1)
  {
    LL_ADC_REG_StartConversionSWStart(ADC1);
    
    LL_mDelay(1000);
  }
}

/**
  * @brief  DMA传输完成回调函数
  * @param  无
  * @retval 无
  */
void APP_TransferCompleteCallback(void)
{
  /*将ADC的采样值转换为电压值*/
  uhADCxConvertedData_Voltage_mVolt = __LL_ADC_CALC_DATA_TO_VOLTAGE(VDDA_APPLI, ADCxConvertedDatas[0], LL_ADC_RESOLUTION_12B);
  printf("%s%s%d mV\r\n","Channel4","Voltage:",uhADCxConvertedData_Voltage_mVolt);
  
  /*将ADC的采样值转换为电压值*/
  uhADCxConvertedData_Voltage_mVolt = __LL_ADC_CALC_DATA_TO_VOLTAGE(VDDA_APPLI, ADCxConvertedDatas[1], LL_ADC_RESOLUTION_12B);
  printf("%s%s%d mV\r\n","Channel5","Voltage:",uhADCxConvertedData_Voltage_mVolt);
  
  /*将ADC的采样值转换为电压值*/
  uhADCxConvertedData_Voltage_mVolt = __LL_ADC_CALC_DATA_TO_VOLTAGE(VDDA_APPLI, ADCxConvertedDatas[2], LL_ADC_RESOLUTION_12B);
  printf("%s%s%d mV\r\n","Channel6","Voltage:",uhADCxConvertedData_Voltage_mVolt);
  
  /*将ADC的采样值转换为电压值*/
  uhADCxConvertedData_Voltage_mVolt = __LL_ADC_CALC_DATA_TO_VOLTAGE(VDDA_APPLI, ADCxConvertedDatas[3], LL_ADC_RESOLUTION_12B);
  printf("%s%s%d mV\r\n","Channel7","Voltage:",uhADCxConvertedData_Voltage_mVolt);
}

/**
  * @brief  DMA配置函数.
  * @param  无
  * @retval 无
  */
void APP_AdcDmaConfig()
{
  /* 使能DMA1 时钟 */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

  /* 使能SYSCFG 时钟 */
  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);
  
  /* DMA1 Channel1 映射为ADC1 */
  LL_SYSCFG_SetDMARemap(DMA1,LL_DMA_CHANNEL_1,LL_SYSCFG_DMA_MAP_ADC1);
 
  /* 配置DMA传输方向为外设到存储器 */
  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_1, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

  /* 配置DMA优先级为高 */
  LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PRIORITY_HIGH);

  /* 配置DMA循环模式 */
  LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MODE_CIRCULAR);

  /* 配置DMA外设地址不变模式 */
  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PERIPH_NOINCREMENT);

  /* 配置DMA存储地址递增模式 */
  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MEMORY_INCREMENT);

  /* 配置DMA外设传输方式为字 */
  LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PDATAALIGN_WORD);

  /* 配置DMA存储器传输方式为字 */
  LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MDATAALIGN_WORD);

  /* 配置DMA传输长度为4 */
  LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, 4);

  /* 配置DMA外设和存储器的地址 */
  LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_1, (uint32_t)&ADC1->DR, (uint32_t)&ADCxConvertedDatas, LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_1));

  /* 使能DMA传输完成中断 */
  LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_1);
  
  /* 使能DMA1 Channel1 */
  LL_DMA_EnableChannel(DMA1,LL_DMA_CHANNEL_1);

  NVIC_SetPriority(DMA1_Channel1_IRQn, 0);
  NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

/**
  * @brief  ADC配置函数.
  * @param  无
  * @retval 无
  */
void APP_AdcConfig()
{
  /* 使能GPIOA */
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
  
  /* 配置PA4为模拟输出 */
  LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_4, LL_GPIO_MODE_ANALOG);
  
  /* 配置PA5为模拟输出 */
  LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_5, LL_GPIO_MODE_ANALOG);
  
  /* 配置PA6为模拟输出 */
  LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_6, LL_GPIO_MODE_ANALOG);
  
  /* 配置PA7为模拟输出 */
  LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_7, LL_GPIO_MODE_ANALOG);
  
  /* 使能ADC时钟 */
  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_ADC1);
  
  /* 设置ADC时钟为PCLK四分频 */
  LL_RCC_SetADCClockSource(LL_RCC_ADC_CLKSOURCE_PCLK_DIV4);
  
  /* 设置内部模式为NONE */
  LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_PATH_INTERNAL_NONE);

  /* 设置分辨率为12位 */
  LL_ADC_SetResolution(ADC1, LL_ADC_RESOLUTION_12B);
    
  /* 设置数据右对齐 */
  LL_ADC_SetDataAlignment(ADC1, LL_ADC_DATA_ALIGN_RIGHT);

  /* 使能扫描模式 */
  LL_ADC_SetSequencersScanMode(ADC1, LL_ADC_SEQ_SCAN_ENABLE);  
 
  /* 设置常规组软件触发  */
  LL_ADC_REG_SetTriggerSource(ADC1, LL_ADC_REG_TRIG_SOFTWARE);
    
  /* 设置常规组单次转换模式 */
  LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_SINGLE);
    
  /* 使能常规组DMA模式 */
  LL_ADC_REG_SetDMATransfer(ADC1, LL_ADC_REG_DMA_TRANSFER_UNLIMITED);
    
  /* 设置常规组序列长度位4 */
  LL_ADC_REG_SetSequencerLength(ADC1, LL_ADC_REG_SEQ_SCAN_ENABLE_4RANKS);
    
  /* 关闭常规组间断模式 */
  LL_ADC_REG_SetSequencerDiscont(ADC1, LL_ADC_REG_SEQ_DISCONT_DISABLE);

  /* 设置常规组RANK1通道4 */
  LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_4);
  
  /* 设置常规组RANK2通道5 */
  LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_2, LL_ADC_CHANNEL_5);

  /* 设置常规组RANK3通道6 */
  LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_3, LL_ADC_CHANNEL_6);
  
  /* 设置常规组RANK4通道7 */
  LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_4, LL_ADC_CHANNEL_7);

  /* 设置通道4采样时间为41.5Cycles */
  LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_4, LL_ADC_SAMPLINGTIME_41CYCLES_5);   
  
  /* 设置通道5采样时间为41.5Cycles */
  LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_5, LL_ADC_SAMPLINGTIME_41CYCLES_5);  
  
  /* 设置通道6采样时间为41.5Cycles */
  LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_6, LL_ADC_SAMPLINGTIME_41CYCLES_5);  
  
  /* 设置通道7采样时间为41.5Cycles */
  LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_7, LL_ADC_SAMPLINGTIME_41CYCLES_5);   
}

/**
  * @brief  ADC校准函数.
  * @param  无
  * @retval 无
  */
static void APP_AdcCalibrate()
{
  #if (USE_TIMEOUT == 1)
  uint32_t Timeout = 0; /* Variable used for timeout management */
  #endif /* USE_TIMEOUT */
  
  if (LL_ADC_IsEnabled(ADC1) == 0)
  { 
    /* 使能ADC校准 */
    LL_ADC_StartCalibration(ADC1);
    
    /* Poll for ADC effectively calibrated */
    #if (USE_TIMEOUT == 1)
    Timeout = ADC_CALIBRATION_TIMEOUT_MS;
    #endif /* USE_TIMEOUT */
    
    while (LL_ADC_IsCalibrationOnGoing(ADC1) != 0)
    {
    #if (USE_TIMEOUT == 1)
      /* Check Systick counter flag to decrement the time-out value */
      if (LL_SYSTICK_IsActiveCounterFlag())
      {
        if(Timeout-- == 0)
        {
          APP_ErrorHandler();
        }
      }
    #endif /* USE_TIMEOUT */
    }   
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
