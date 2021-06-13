#include "Poked_Control.h"

extern PID_Type   Poked_InPID,Poked_OutPID;
extern Poked_info Poked;
/***************************************
  * @brief 拨弹电机运算
  * @param  
  * @retval void
****************************************/
void Poke_Calculation(portTickType System_Tick)
{
	
}
/***************************************
  * @brief 拨弹电机控制
  * @param  current_Poked_angle ： 当前拨弹电机机械角度
			target_Poked_angle  ： 目标角度
			Poked_deviation:偏差
  * @retval void
****************************************/
int16_t  Poked_Control( int current_Poked_angle , int target_Poked_angle)
{
    Poked_OutPID.LastError    = Poked_OutPID.CurrentError;
    Poked_OutPID.CurrentError = (target_Poked_angle - current_Poked_angle);
    Poked_OutPID.PIDOut       = Poked_OutPID.CurrentError * Poked_OutPID.P \
                                + (Poked_OutPID.CurrentError - Poked_OutPID.LastError)*Poked_OutPID.D;
    Poked_InPID.LastError     = Poked_InPID.CurrentError;
    Poked_InPID.CurrentError  = 	Poked_OutPID.PIDOut - Poked.Esc.Angle_Speed;
    Poked_InPID.PIDOut        =   Poked_InPID.CurrentError * Poked_InPID.P \
                                  + (Poked_InPID.CurrentError - Poked_InPID.LastError)	* Poked_InPID.D;

    Poked_InPID.PIDOut = Poked_InPID.PIDOut >  Poked_out_Max ?   Poked_out_Max : Poked_InPID.PIDOut;
    Poked_InPID.PIDOut = Poked_InPID.PIDOut < -Poked_out_Max ?  -Poked_out_Max : Poked_InPID.PIDOut;



    return Poked_InPID.PIDOut;
}

