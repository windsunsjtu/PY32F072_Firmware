================================================================================
                                样例使用说明
================================================================================
功能描述：
此样例演示了在TIM1输入捕获功能，PA8或PA9或PA10输入时钟信号，TIM1捕获成功后，
会进入捕获中断，每进一次中断，翻转一次LED

================================================================================
测试环境：
测试用板：PY32F072_STK
MDK版本： 5.28
IAR版本： 9.20

================================================================================
使用步骤：
1. 下载并运行程序
2. PA8和PA10引脚不输入时钟信号的情况下，PA9引脚输入时钟信号，LED会翻转
3. PA8和PA9引脚不输入时钟信号的情况下，PA10引脚输入时钟信号，LED会翻转
4. PA9和PA10引脚不输入时钟信号的情况下，PA8引脚输入时钟信号，LED会翻转

================================================================================
注意事项：

================================================================================