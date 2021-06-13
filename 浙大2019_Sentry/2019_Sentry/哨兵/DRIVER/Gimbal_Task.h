#ifndef __GIMBAL_TASK_H__
#define __GIMBAL_TASK_H__
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "ramp.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Poked_Control.h"
#include "Cloud_Control.h"
#include "uart_DBUS.h"
#include "Kalman_Filter.h"
#include "Status_Update.h"
#include "CAN_1.h"
void Gimbal_Task(void *Parameters);
void GIMBAL__RELAX(void);
void GIMABL__RC(void);
void GIMBAL__FLOOW(void);
void GIMBAL__HEAT(void);

#endif
