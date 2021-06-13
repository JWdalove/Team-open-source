#ifndef __UART_DBUS_H__
#define __UART_DBUS_H__
#include "stdint.h"
#include "FreeRTOS.h"
#include "struct.h"
#include "stm32f4xx.h"
#include "constant.h"
extern uint8_t DBUSFrameCounter;
void UART5_DBUS_InitConfig(void);
void UART5_IRQHandler(void);

#endif
