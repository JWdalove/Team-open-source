#ifndef __CLASSIS_CONTROL__
#define __CLASSIS_CONTROL__

#include "struct.h"
#include "constant.h"
#include "ramp.h"
#include "stdint.h"
//#include "Encoder.h"
#include "task.h"
#include "FreeRTOS.h"
#include "Gimbal_Monitor.h"
#include "Chassis_Task.h"
void Chassis_Parm(void);
void Power_Calculate(void);
void ShootPower_Calculate(void);
void Chassis_SpeedSet(float YSpeed);
void Chassis_MoveCalculate(void);
void Chassis_Position(void);
void Chassis_Contral(void);
void Chassis_Heat_Mode_Update(void);
#endif
