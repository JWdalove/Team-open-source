#ifndef _Task_Communication_h__
#define _Task_Communication_h__
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "ramp.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Poked_Control.h"
#include "Cloud_Control.h"
#include "uart_DBUS.h"
#include "Status_Update.h"
#include "uart_judge.h"
#include "judgement_info.h"

void Task_Communication(void *pvParameters);

#endif

