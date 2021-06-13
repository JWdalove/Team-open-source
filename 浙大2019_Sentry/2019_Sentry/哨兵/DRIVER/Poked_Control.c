#include "Poked_Control.h"

extern GimbalControl_Mode_Enum  Gimbal_Control;
extern DBUSDecoding_Type  		DBUS_ReceiveData;
extern FricStatus_Enum       	FricStatus;
extern PID_Type   Poked_InPID,Poked_OutPID;
extern Judge_rece	Judge_DATA;
extern Sentry_info 	SentryInfo;
extern Poked_info Poked;
extern Cloud_info Yaw,Pitch;
extern imagepro_info MiniPC;
extern int Edge_position;

u16 Poked_Count = 1;		      //拨弹计数
/***************************************
  * @brief  :摩擦轮控制
  * @param  :2450/25
****************************************/
 
void FricStatus_Control()
{
	if(FricStatus == FricStatus_Working)
        {
            TIM2->CCR1=2450;  //2450
            TIM2->CCR2=2450;  //2450
        }
        else
        {
            TIM2->CCR1=2000;
            TIM2->CCR2=2000;
        }
}

/***************************************
  * @brief 拨弹电机运算
  * @param  
  * @retval void
****************************************/
void Poke_Calculation(portTickType System_Tick)
{
	if(DBUS_ReceiveData.switch_right == 2 && (Judge_DATA.Shooterheat < 400) && DBUS_ReceiveData.switch_left == 1
  	 && MiniPC.Enemy_Count>=95)                                                                                        //打击
	{
		if(Poked_Count % 100 == 0 && Poked.Turn_Angle-Poked.Esc.USE_Machine<120000){Poked.Turn_Angle += 21060;}
		Poked_Count++;
	}
  else if(DBUS_ReceiveData.switch_right == 1 && (Judge_DATA.Shooterheat < 400) && DBUS_ReceiveData.switch_left == 3 )  //遥控
	{
		if(Poked_Count % 100== 0 && Poked.Turn_Angle-Poked.Esc.USE_Machine<120000){Poked.Turn_Angle += 21060;}
		Poked_Count++;
	}
	else if(DBUS_ReceiveData.switch_right == 1 && (Judge_DATA.Shooterheat < 400) && DBUS_ReceiveData.switch_left == 1 
		      && MiniPC.Enemy_Count>=95)                                                                                   //浮动
	{
		if(Pitch.Give<=2250 && Pitch.Give>=1700)
		{
			if((fabs(MiniPC.MiniPC_Msg[1])<4.0f ) && Poked_Count % 50 == 0 && Poked.Turn_Angle-Poked.Esc.USE_Machine<120000)
      {
			  Poked.Turn_Angle += 42120;
			}
		}
		else if(Pitch.Give<1700 && Pitch.Give>=1350)
		{
			if((fabs(MiniPC.MiniPC_Msg[1])<2.0f )&& Poked_Count % 200 == 0 && Poked.Turn_Angle-Poked.Esc.USE_Machine<120000)
      {
			  Poked.Turn_Angle += 42120*2;
			}
		}	
		Poked_Count++;
	}
	else{Poked_Count = 0;Poked.Turn_Angle = Poked.Esc.USE_Machine;}
	
	// ********** ↓ 卡弹反转 ↓ **********//
	if(Poked.Turn_Angle-Poked.Esc.USE_Machine>200000)
	{
		Poked.Turn_Angle=Poked.Esc.USE_Machine-63180;
		Poked_Count=1;
	}
	// ********** ↑ 卡弹反转 ↑ **********//
	
}
/***************************************
  * @brief 拨弹电机控制
  * @param  current_Poked_angle ： 当前拨弹电机机械角度
			      target_Poked_angle  ： 目标角度
			      Poked_deviation     :偏差
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
