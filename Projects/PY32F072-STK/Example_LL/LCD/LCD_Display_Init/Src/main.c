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
LL_LCD_InitTypeDef LCD_InitStruct;
LL_LCD_SegComTypeDef LcdSegCom;

/* Private user code ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void APP_SystemClockConfig(void);
static void APP_ConfigLcdClock(void);
static void APP_ConfigLcdPin(void);

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
  
  /* 配置LCD时钟 */
  APP_ConfigLcdClock();
  
  /* 配置LCD引脚 */
  APP_ConfigLcdPin();
  
  /* LCD初始化 */
  LCD_InitStruct.Contrast = LL_LCD_CONTRASTLEVEL_0;
  LCD_InitStruct.BiasSrc = LL_LCD_BIAS_SRC_IN_RES_HIGH_POWER;
  LCD_InitStruct.Duty = LL_LCD_DUTY_1_4;
  LCD_InitStruct.Bias = LL_LCD_BIAS_1_3;
  LCD_InitStruct.ScanFre = LL_LCD_SCAN_FRE_128HZ;
  LCD_InitStruct.Mode = LL_LCD_MODE_0;
  LL_LCD_Init(LCD, &LCD_InitStruct);
  
  /* 设置SEG COM输出使能 */
  LcdSegCom.Seg0_31 = 0xffffff00;                            /* 配置LCD_POEN0寄存器 使能SEG0~SEG7 */
  LcdSegCom.Seg32_39_Com0_7_t.Seg32_39_Com0_7 = 0xffffffff;  /* 初始化LCD_POEN1寄存器 全部关闭输出端口 */
  LcdSegCom.Seg32_39_Com0_7_t.SegComBit.Com0_3 = 0;          /* 使能COM0~COM3 */
  LcdSegCom.Seg32_39_Com0_7_t.SegComBit.Mux = 0;             /* 内部电阻此位需要配置为0 */
  LL_LCD_SetSegCom(LCD, &LcdSegCom);
  
  /* 液晶全显,显示“88:88”字样 */
  LL_LCD_Write(LCD, LL_LCD_RAM_REGISTER0, 0x0f0f0f0f);
  LL_LCD_Write(LCD, LL_LCD_RAM_REGISTER1, 0x0f0f0f0f);

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
  * @brief  配置LCD时钟
  * @param  无
  * @retval 无
  */
static void APP_ConfigLcdClock(void)
{
  /* 使能LSI */
  LL_RCC_LSI_Enable();
  while(LL_RCC_LSI_IsReady() != 1)
  {
  }
  
  /* 选择LCD的时钟源为LSI */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  LL_PWR_EnableBkUpAccess();
  LL_RCC_LSCO_SetSource(LL_RCC_LSCO_CLKSOURCE_LSI);
  LL_RCC_LSCO_Enable();
  LL_PWR_DisableBkUpAccess();
  LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_PWR);
  
  /* 开LCD时钟 */
  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_LCD);
}

/**
  * @brief  配置LCD引脚
  * @param  无
  * @retval 无
  */
static void APP_ConfigLcdPin(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct;
  
  /* 开LCD引脚时钟 */
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);
  
  /* 配置LCD引脚 */
  GPIO_InitStruct.Mode       = LL_GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull       = LL_GPIO_PULL_NO;

  GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);/* COM0 */
  
  GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);/* COM1 */
  
  GPIO_InitStruct.Pin = LL_GPIO_PIN_11;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);/* COM2 */
  
  GPIO_InitStruct.Pin = LL_GPIO_PIN_12;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);/* COM3 */
  
  GPIO_InitStruct.Pin = LL_GPIO_PIN_8;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);/* SEG0 */
  
  GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);/* SEG1 */
  
  GPIO_InitStruct.Pin = LL_GPIO_PIN_8;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);/* SEG2 */
  
  GPIO_InitStruct.Pin = LL_GPIO_PIN_7;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);/* SEG3 */
  
  GPIO_InitStruct.Pin = LL_GPIO_PIN_6;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);/* SEG4 */
  
  GPIO_InitStruct.Pin = LL_GPIO_PIN_15;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);/* SEG5 */
  
  GPIO_InitStruct.Pin = LL_GPIO_PIN_14;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);/* SEG6 */
  
  GPIO_InitStruct.Pin = LL_GPIO_PIN_13;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);/* SEG7 */
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
