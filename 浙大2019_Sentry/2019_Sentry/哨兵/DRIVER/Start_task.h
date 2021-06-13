#ifndef __START_TASK_H__
#define __START_TASK_H__
#include "FreeRTOS.h"
#include "task.h"
#include "Task_CANSend.h"
#include "Chassis_Monitor.h"
#include "Chassis_Task.h"
#include "Chassis_Control.h"
#include "Gimbal_Task.h"
#include "Gimbal_Monitor.h"
#include "Task_Communication.h"
#include "CAN_1.h"
#include "CAN_2.h"
#include "PWM.h"
#include "uart_MiniPC.h"
#include "uart_mpu9250.h"
#include "uart_DBUS.h"
#include "uart_judge.h"
//#include "bluetooth.h"
#include "Kalman_Filter.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
//#include "Encoder.h"
//#include "Ultrasonic.h"
//#include "Ultrasonic_2.h"
#include "GPIO.h"
#include "Status_Update.h"
#include "struct.h"
#include "comm_task.h"
extern TaskHandle_t judge_unpack_task_t;
void Task_SysInitConfig(void *Parameters);

#endif
