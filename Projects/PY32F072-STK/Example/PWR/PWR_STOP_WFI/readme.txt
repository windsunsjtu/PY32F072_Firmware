================================================================================
                                样例使用说明
================================================================================
功能描述：
此样例演示了stop模式下，通过GPIO中断唤醒功能。

================================================================================
测试环境：
测试用板：PY32F072_STK
MDK版本： 5.28
IAR版本： 9.20

================================================================================
使用步骤：
1. 编译下载程序到MCU，并运行；
2. LED灯亮，按下按键，LED灯灭，串口打印"STOP MODE!",进入stop模式；
3. 在PA06上产生一个下降沿，程序退出stop模式，并且串口打印"WAKEUP OK!"；
4. LED以200ms的间隔进行翻转。

================================================================================
注意事项：
MCU与串口调试助手连线:
RX ---> PA02
TX ---> PA03
波特率：115200bps

1，演示此样例功能时需要断开swd连接线并重新上电，因为默认情况下，仿真器会把
DBGMCU_CR.DBG_STOP置位。
2，注意关闭systick中断，防止该中断唤醒MCU。
================================================================================