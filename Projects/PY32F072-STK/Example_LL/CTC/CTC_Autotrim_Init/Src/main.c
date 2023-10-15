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
/* Private user code ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void APP_SystemClockConfig(void);
static void APP_MCOConfig(void);
static void APP_CTCConfig(void);

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
  
  /* 配置PA08为MCO复用功能 */
  APP_MCOConfig();
  
  /* 配置CTC */
  APP_CTCConfig();

  while (1)
  {
    if (LL_CTC_IsActiveFlag_CKOK(CTC) == 1)
    {
      /* 校准成功，led灯处于打开状态 */
      BSP_LED_On(LED_GREEN);
    }
    else
    {
      /* 校准未成功，ed灯处于关闭状态 */
      BSP_LED_Off(LED_GREEN);
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
  /* 使能HSE */
  LL_RCC_HSE_SetFreqRegion(LL_RCC_HSE_16_32MHz);
  LL_RCC_HSE_Enable();
  while(LL_RCC_HSE_IsReady() != 1)
  {
  }
  
  /* 使能HSI(16MHz) */
  LL_RCC_HSI_SetCalibFreq(LL_RCC_HSICALIBRATION_16MHz);
  LL_RCC_HSI_Enable();
  while(LL_RCC_HSI_IsReady() != 1)
  {
  }
  
  /* 使能LSE */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  LL_PWR_EnableBkUpAccess();
  while(LL_PWR_IsEnabledBkUpAccess() == 0)
  {
  }
  LL_RCC_LSE_SetDriveCapability(LL_RCC_LSEDRIVE_HIGH);
  LL_RCC_LSE_Enable();
  while(LL_RCC_LSE_IsReady() != 1)
  {
  }
  LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_PWR);
  
  /* PLL对HSI(16MHz)3倍频 */
  LL_RCC_PLL_Disable();
  while(LL_RCC_PLL_IsReady() != 0)
  {
  }
  LL_RCC_PLL_SetMainSource(LL_RCC_PLLSOURCE_HSI);
  LL_RCC_PLL_SetMulFactor(LL_RCC_PLLMUL_3);
  LL_RCC_PLL_Enable();
  while(LL_RCC_PLL_IsReady() != 1)
  {
  }  

  /* 设置 AHB 分频*/
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);

  /* 配置HSISYS作为系统时钟源 */
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSE);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSE)
  {
  }

  /* 设置 APB1 分频*/
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_Init1msTick(24000000);

  /* 更新系统时钟全局变量SystemCoreClock(也可以通过调用SystemCoreClockUpdate函数更新) */
  LL_SetSystemCoreClock(24000000);
}

/**
  * @brief  配置PA08为MCO复用功能
  * @param  无
  * @retval 无
  */
static void APP_MCOConfig(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct;
  
  /* GPIOA时钟使能 */
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
  
  /* 配置PA08为MCO复用功能 */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_8;                  /* 选择8号引脚 */
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;        /* 配置为复用模式 */
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH; /* 输出速度选择 */
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL; /* 输出模式选择 */
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;               /* 无上下拉 */
  GPIO_InitStruct.Alternate = LL_GPIO_AF0_MCO;          /* 选择复用为AF0功能 */
  LL_GPIO_Init(GPIOA,&GPIO_InitStruct);
  
  /* MCO输出时钟及分频初始化 */
  LL_RCC_ConfigMCO(LL_RCC_MCO1SOURCE_PLLCLK, LL_RCC_MCO1_DIV_1);
}

/**
  * @brief  配置CTC
  * @param  无
  * @retval 无
  */
static void APP_CTCConfig(void)
{
  LL_CTC_InitTypeDef CTC_InitStruct;
  
  /* 使能CTC时钟 */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_CTC);
  
  /* CTC初始化 */
  CTC_InitStruct.AutoTrim       = LL_CTC_AUTO_TRIM_ENABLE;          /* 使能自动校准 */
  CTC_InitStruct.LimitValue     = 1;                                /* 时钟校准时基限值 */
  CTC_InitStruct.ReloadValue    = 1464;                             /* 计数器重载值 */
  CTC_InitStruct.RefCLKSource   = LL_CTC_REF_CLOCK_SOURCE_LSE;      /* 参考时钟源LSE */
  CTC_InitStruct.RefCLKDivider  = LL_CTC_REF_CLOCK_DIV1;            /* 参考时钟分频1分频 */
  CTC_InitStruct.RefCLKPolarity = LL_CTC_REF_CLOCK_POLARITY_RISING; /* 参考时钟上升沿有效 */ 
  LL_CTC_Init(CTC, &CTC_InitStruct);
  
  /* 使能CTC计数器 */
  LL_CTC_EnableCount(CTC);
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
