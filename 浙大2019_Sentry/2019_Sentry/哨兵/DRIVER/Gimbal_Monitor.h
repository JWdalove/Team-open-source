#ifndef __GIMBAL_MONITOR_H__
#define __GIMBAL_MONITOR_H__
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "struct.h"
#include "constant.h"
#include "uart_DBUS.h"
#include "uart_mpu9250.h"
#include "CAN_1.h"
#include "CAN_2.h"
#include "task.h"

void Gimbal_Monitor(void *Parameters);
void Beep_Configuration(void);
void Beep(u8 type);

#endif
