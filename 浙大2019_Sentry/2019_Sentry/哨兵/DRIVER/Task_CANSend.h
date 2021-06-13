#ifndef __TASK_CANSEND_H__
#define __TASK_CANSEND_H__

#include "FreeRTOS.h"
#include "queue.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "struct.h"
void Task_CANSend(void *Parameters);


/************************   队列句柄    ************************/
extern   xQueueHandle    Queue_CANSend;                 //CAN发送队列句柄

#endif

