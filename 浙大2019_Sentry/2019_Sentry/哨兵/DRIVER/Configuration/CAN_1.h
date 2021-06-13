#ifndef __CAN_FIRST_H__
#define __CAN_FIRST_H__
#include "stdint.h"
#include "stm32f4xx.h"
#include "stm32f4xx_can.h"
#include "struct.h"
#include "CAN_2.h"
#include "Task_CANSend.h"

extern int CHASSISBOARD_FrameCounter;

void CAN_First_InitConfig(void);
void CloudMotorDriveCan1(int16_t current_205,int16_t current_206,int16_t current_207,int16_t current_208);
void ChassisMotorDriveCan1(int16_t current_201,int16_t current_202,int16_t current_203,int16_t current_204);
void Gimbal_Board_Communicate(int16_t data1,int16_t data2,int16_t data3,int16_t data4);
void CAN1_TX_IRQHandler(void);
void CAN1_RX0_IRQHandler(void);

#define YAWMOTOR		  0X205

#define CHASSISBOARD  0x166

#endif
