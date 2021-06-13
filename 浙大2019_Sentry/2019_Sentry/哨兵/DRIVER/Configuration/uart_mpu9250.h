#ifndef __UART_MPU9250_H__
#define __UART_MPU9250_H__
#include "stm32f4xx.h"
#include "stdlib.h"
#include "stdint.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_dma.h"
#include "uart_MiniPC.h"
#include "string.h"
extern int MPU9250FrameCounter;
extern int Pitch_Speed[30];
void USART_mpu9250_InitConfig(void);
void USART1_IRQHandler(void);
#endif
