================================================================================
                                样例使用说明
================================================================================
功能描述：
此样例演示了TIM1的外部时钟模式1功能，选择ETR(PA12)引脚作为外部时钟输入源，并使能
更新中断，在中断中翻转LED灯

================================================================================
测试环境：
测试用板：PY32F072_STK
MDK版本： 5.28
IAR版本： 9.20

================================================================================
使用步骤：
1. 编译下载程序到MCU，并运行；
2. PA12引脚，输入外部时钟；
3. 可观察到MCU从PA12引脚上每检测到800个脉冲，会让LED灯翻转一次；

================================================================================
注意事项：

================================================================================