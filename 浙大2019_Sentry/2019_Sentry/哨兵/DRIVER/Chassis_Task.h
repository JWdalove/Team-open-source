#ifndef __CHASSIS_TASK_H__
#define __CHASSIS_TASK_H__
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "struct.h"
#include "FreeRTOS.h"
#include "ramp.h"
#include "Status_Update.h"
#include "CAN_1.h"
#include "CAN_2.h"
#include "Poked_Control.h"
#include "Chassis_Control.h"
#include "task.h"
#include "uart_judge.h"
//#include "bluetooth.h"
void Chassis_Task(void *Parameters);
void Chassis_RC(void);
void Chassis_Patrolling(void);
extern int ChassisCounter;

#endif
