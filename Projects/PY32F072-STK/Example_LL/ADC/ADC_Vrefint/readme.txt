================================================================================
                                样例使用说明
================================================================================
功能描述：
此样例演示了ADC模块的Vrefint采样功能，通过采样Vrefint的值，计算得出VCC的值，并
通过串口打印出来。

================================================================================
测试环境：
测试用板：PY32F072_STK
MDK版本： 5.28
IAR版本： 9.20

================================================================================
使用步骤：
1. 编译下载程序到MCU，并运行；
2. 串口每隔1s打印VCC的电压值。

================================================================================
注意事项：
通过USB转TTL模块连接PC与STK板,STK板与USB转TTL模块的连线方式如下；
STK板        USB转TTL模块
PA02(TX)  -->  RX
PA03(RX)  -->  TX
GND       -->  GND

================================================================================