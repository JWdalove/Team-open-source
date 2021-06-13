# STM32例程

​    本例程提供了C 语言例程代码

​	测试环境：Windows10_x64 操作系统

​	编译器：keil_V5.28

​	开发板：正点原子-战舰V3 STM32F103ZET6

​	测试设备：HI226/HI229(同样可适用于其他型号模块)

### 硬件连接

1. 将Hi226/Hi229 正确插入到模块评估板上。

| 超核调试板 | 正点原子开发板 |
| ---------- | -------------- |
| RXD        | PA2(TXD)       |
| TXD        | PA3(RXD)       |
| 3.3V       | 3V3            |
| GND        | GND            |

2. 用杜邦线将上表相对应的引脚连接起来。*
3. 用USB线插到开发板的 __USB_232__ 插口上，另一端插到电脑上。

### 观察输出

​	打开串口调试助手，打开开发板对应的串口号，观察数据输出

```
    Device ID:  0       
   Frame Rate:  100Hz
       Acc(G):	   0.064    0.258    0.985
   gyr(deg/s):	   -0.76     0.20     0.04
      mag(uT):	  -55.97   -48.05   -57.17
   eul(R P Y):     14.61    -3.45     8.88
quat(W X Y Z):     0.988    0.129   -0.020    0.081
```

### 代码结构

解码程序分为两部分: 

* packet.c/h : 用户接收一帧数据，并做CRC校验
* imu_data_decode.c/h:    用于将一帧中的PAYLOAD部分翻译为加速度，加速度，欧拉角等数据

使用说明:

1. 先确硬件连接正确，并且有基本的MCU串口操作知识，使用过串口中断接收

2. 初始化解码函数

   ```
       imu_data_decode_init();
   ```

3. 在对应的串口中断中调用packet_decode

    ```
    /* serial usart2 interrupt functional */
    void USART2_IRQHandler(void)                	            
    {
        uint8_t ch;
        if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)   
            ch = USART_ReceiveData(USART2);	                    

        packet_decode(ch);                                      
    } 
    ```

4. 在主线程中(while(1)) 中调用打印函数，打印接收到的数据

   ```
               if(id0x62.tag != KItemGWSOL)
   			{
   				/* printf imu data packet */
   				dump_data_packet(&id0x91);
   				putchar(10);
   			}
   			else
   			{
   				/* printf gw data packet */
   				printf("        GW ID:  %-8d\n", id0x62.gw_id);
   				for(i = 0; i < id0x62.n; i++)
   				{ 
   					dump_data_packet(&id0x62.id0x91[i]);
   					puts("");
   				}
   			}
   ```

   

