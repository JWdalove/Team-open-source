#ifndef __CAN_SECOND_H__
#define __CAN_SECOND_H__

#include "FreeRTOS.h"
#include "queue.h"
#include "stdint.h"
#include "stm32f4xx.h"
#include "stm32f4xx_can.h"
#include "Task_CANSend.h"
#include "struct.h"

void CAN_Second_InitConfig(void);
void CloudMotorDriveCan2(int16_t current_205,int16_t current_206,int16_t current_207,int16_t current_208);
void ChassisMotorDriveCan2(int16_t current_201,int16_t current_202,int16_t current_203,int16_t current_204);
void Chassis_Board_Communicate(int16_t data1,int16_t data2,int16_t data3,int16_t data4);
void CAN2_RX0_IRQHandler(void);

extern int GIMBALBOARD_FrameCounter;

#define  GIMBAL_MAX_LIMIT  16383

#define  CHASSISF    	0x201
#define  CHASSISB	  	0x202

#define	 PITCHMOTOR		0x206
#define	 POKEDMOTOR  	0x207

#define  GIMBALBOARD	0x188


#endif
