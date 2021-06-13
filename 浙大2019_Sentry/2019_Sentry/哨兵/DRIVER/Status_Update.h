#ifndef __STATUS_UPDATE_H__
#define __STATUS_UPDATE_H__
#include "FreeRTOS.h"
#include "struct.h"
#include "constant.h"
#include "uart_MiniPC.h"
void Control_Status_Init(void);
void Control_Status_Update(portTickType Tick);

#endif
