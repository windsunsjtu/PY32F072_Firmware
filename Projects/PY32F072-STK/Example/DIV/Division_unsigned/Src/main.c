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
DIV_CalculatedTypeDef Calculatervalue;
DIV_HandleTypeDef Divhandle;
/* Private user code ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/


/**
  * @brief  应用程序入口函数.
  * @retval int
  */
int main(void)
{
  /* 初始化所有外设，Flash接口，SysTick */
  HAL_Init();
  
  /* 初始化调试串口(printf使用) */
  BSP_USART_Config();
  
  /* 使能DIV */
  __HAL_RCC_DIV_CLK_ENABLE();
  
  Divhandle.Instance        = DIV;                /* 除法器基地址        */
  Calculatervalue.Sign      = DIV_MODE_UNSIGNED;  /* 无符号除法          */
  Calculatervalue.Dividend  = 0x7250A3FB;         /* 被除数1,917,887,483 */
  Calculatervalue.Divisor   = 0x257D;             /* 除数9,597           */

  /* 启动除法器 */
  HAL_DIV_Calculate(&Divhandle, &Calculatervalue);
  
  /* 打印除法结果 */
  printf("Quotient:0x%x\r\nRemainder:0x%x\r\n",Calculatervalue.Quotient,Calculatervalue.Remainder);
  
  /* 无限循环 */
  while (1)
  {
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
