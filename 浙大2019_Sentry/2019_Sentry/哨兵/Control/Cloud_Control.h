#ifndef __CLOUD_CONTROL__
#define __CLOUD_CONTROL__
#include "FreeRTOS.h"
#include "task.h"
#include "Kalman_Filter.h"
#include "struct.h"
#include "constant.h"
#include "uart_mpu9250.h"


void Cloud_PID_Param(void);
void Angle_Handle(void);
void Cloud_Angle_Set(int16_t Yaw_Angle,float Y_Sensity, int16_t Pitch_Angle,float P_Sensity);
void GimbalHeat_AngleSet(int16_t Yaw_Angle,float Yaw_Offset,int16_t Pitch_Angle,float Pitch_Offset);
int16_t Cloud_Control_Pitch(float current_position_206,float target_position_206);
int16_t Cloud_Control_Yaw(float current_position_205,float target_position_205);
int16_t Pitch_Heat_Control(float current_position_206,float target_position_206);
int16_t Yaw_Heat_Control(float current_position_205,float target_position_205);

#endif
