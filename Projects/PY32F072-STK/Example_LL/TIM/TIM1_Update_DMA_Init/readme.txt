================================================================================
                                样例使用说明
================================================================================
功能描述：
此样例演示了在TIM1中使用DMA传输数据的功能,通过DMA从SRAM中搬运数据到ARR寄存器实现
TIM1更新周期变化,TIM1第一次溢出后LED会翻转,此次翻转时间间隔为1000ms,DMA将数据搬
运到TIM1_ARR,第二次LED翻转间隔为900ms,以此类推,最后LED翻转间隔为100msDMA搬运结束,
LED保持100ms的翻转间隔闪烁。

================================================================================
测试环境：
测试用板：PY32F072_STK
MDK版本： 5.28
IAR版本： 9.20

================================================================================
使用步骤：
1. 编译下载程序到MCU，并运行
2. 使用示波器捕捉PB2波形
3. 电平保持时间为：1000,900,800,700,600,500,400,300,200,100,100...
4. LED闪烁逐渐加快，最后保持5Hz频率闪烁

================================================================================
注意事项：

================================================================================