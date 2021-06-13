#ifndef __CLASSIS_CONTROL__
#define __CLASSIS_CONTROL__

#include "struct.h"
#include "constant.h"
#include "ramp.h"
#include "stdint.h"
#include "Encoder.h"
void Chassis_SpeedSet(float YSpeed);
void Chassis_MoveCalculate(void);
void Chassis_Position(void);
void Chassis_Contral(void);
#endif
