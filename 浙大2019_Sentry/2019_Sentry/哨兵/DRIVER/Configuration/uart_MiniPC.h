#ifndef __UART_MINIPC_H__
#define __UART_MINIPC_H__
#include "stdint.h"
#include "stm32f4xx.h"
#include "stdlib.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "FreeRTOS.h"
#include "struct.h"
#include "math.h"
#include "constant.h"
#include "uart_mpu9250.h"

void USART_MiniPC_InitConfig(void);
void DMA1_Stream5_IRQHandler(void);
void USART2_Senddata(char D);
void PC_Communicate(float RealData,u8 Time);

extern int 	 MiniPCFrameCounter;
extern unsigned char  USART_RX_TEST[20];
extern char  USART2_TX_LEN;
extern char  USART2_RX[6];
extern char  USART2_TX[6];


#endif
