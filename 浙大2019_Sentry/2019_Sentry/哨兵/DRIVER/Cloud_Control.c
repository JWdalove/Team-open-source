#include "Cloud_Control.h"
#include "kalman.h"
#include "delay.h"
extern Sentry_info			SentryInfo;
extern Cloud_info			Yaw,Pitch;
extern Chassis_info  		Chassis;
extern imagepro_info 		MiniPC;
extern PID_Type				Poked_InPID,Poked_OutPID;
extern PID_Type				Pitch_In_PID,Pitch_Out_PID,Yaw_In_PID,Yaw_Out_PID;
extern PID_Type				Pitch_Heat_In_PID,Pitch_Heat_Out_PID,Yaw_Heat_In_PID,Yaw_Heat_Out_PID;
extern HeatSubschema_Enum	GimbalMotionMode;

extern float  speed_Euler_Pitch,speed_Euler_Yaw,Euler_Yaw;
extern float  speed_Euler_Yaw_last,speed_Euler_Pitch_last;
int Heat_CloudOutMax=3000;						      //云台输出限幅
u8 Offset_flag_f=0 , Offset_flag_b=0;				//正负补偿标志位
float Yaw_Offset = 0.0f , Chassis_Offset = 0.0f;		//云台补偿
float m=0.0f , n=0.0f;
float Actual_Distance=0.0f;
float PC_Distance=0.0f;
float Pitch_Offset=0.0f;

/***************************************
  * @brief  :参数初始化

****************************************/
void Cloud_PID_Param(void)
{
	MiniPC.Correction_Yaw = -0.01f;
	Yaw.Plus = 0.0f;
	Pitch.Plus = 0.0f;
	Yaw.rotate_coe = 2.0f;
	Pitch.rotate_coe = 3.0f;
	
	Yaw_In_PID.P = 3.0f;
	Yaw_In_PID.D = 0.0f;
	Yaw_In_PID.feed_param = 0.0f;
	Yaw_Out_PID.P = 100.0f;
	Yaw_Out_PID.I = 0.0f;
	Yaw_Out_PID.D = 0.0f;
	
	Yaw_Heat_In_PID.P = 2.0f;//2.5//2.0
	Yaw_Heat_In_PID.D = 0.0f;
	Yaw_Heat_In_PID.feed_param = 0.0f;
	Yaw_Heat_Out_PID.P = 80.0f;//160.0//80.0
	Yaw_Heat_Out_PID.I = 0.0f;
	Yaw_Heat_Out_PID.D = 0.0f;
	
	Pitch_In_PID.P = -5.0f;
	Pitch_In_PID.D = 0.0f;
	Pitch_In_PID.feed_param = 0.0;
	Pitch_Out_PID.P = -40.0f;
	Pitch_Out_PID.I = -0.3f;
	Pitch_Out_PID.D = 0.0f;
	
	Pitch_Heat_In_PID.P = -6.5f;//-4.5//-6.5
	Pitch_Heat_In_PID.I = 0.0f;
	Pitch_Heat_In_PID.D = 0.0f;
	Pitch_Heat_In_PID.feed_param = 0.0;
	Pitch_Heat_Out_PID.P = -15.0f;//-50.0//-15.0
	Pitch_Heat_Out_PID.I = -0.4f;//-0.4 //-0.4
	
	Poked_InPID.P  = 3.0f;
	Poked_OutPID.P = 1.5f;
	Poked_OutPID.D = 15.0f;
	
	Pitch.Give = Pitch.Imu_Euler;
	Yaw.Give = Yaw.Imu_Euler;
}



// ********** ↓ 云台角度控制 ↓ **********//
void Cloud_Angle_Set(int16_t Yaw_Angle,float Y_Sensity, int16_t Pitch_Angle,float P_Sensity)
{
	Yaw_Offset  = 0.0f;       //补偿清零
  Yaw.Give   += (Yaw_Angle * Y_Sensity);
	Pitch.Give -= (Pitch_Angle * P_Sensity)*360.0f/8191.0f;
}
// ********** ↑ 云台角度控制 ↑ **********//



/***************************************
  * @brief  :自瞄预测补偿
  * @param  :Offset_flag_f,Offset_flag_b正负补偿标志;	MiniPC.Angle_error[]据目标的角度差值数组
				     Yaw_Offset补偿	MiniPC.EnemyError前5(根据情况更改)次敌人角度与现在敌人角度的差值
  * @details:
****************************************/
// ********** ↓ 云台打击模式角度控制 ↓ **********//
void GimbalHeat_AngleSet(float Yaw_Kalman,float Pitch_Kalman,float Yaw_Speed_Kalman)
{

}

// ********** ↑ 云台打击模式角度控制 ↑ **********//



// ********** ↓ 云台Pitch轴控制 ↓ **********//
int16_t Cloud_Control_Pitch(float current_position_206,float target_position_206)
{
    portTickType CurrentTick = xTaskGetTickCount();
    static float target_position_206_last = 0.0f;
	
    // ********** ↓ Pitch轴外环PID ↓ **********//
    Pitch_Out_PID.LastError = Pitch_Out_PID.CurrentError;
    Pitch_Out_PID.CurrentError =  -current_position_206 + target_position_206;
    if(abs_f(Pitch_Out_PID.CurrentError) < 600)
        Pitch_Out_PID.Integral += Pitch_Out_PID.CurrentError;
    else
        Pitch_Out_PID.Integral = 0;
    Pitch_Out_PID.Integral = Pitch_Out_PID.Integral > 2000 ? 2000 : Pitch_Out_PID.Integral;
    Pitch_Out_PID.Integral = Pitch_Out_PID.Integral < -2000 ? -2000 : Pitch_Out_PID.Integral;
    Pitch_Out_PID.PIDOut = Pitch_Out_PID.CurrentError * Pitch_Out_PID.P
						 + Pitch_Out_PID.Integral * Pitch_Out_PID.I
						 +(Pitch_Out_PID.CurrentError-Pitch_Out_PID.LastError)*Pitch_Out_PID.D;
    // ********** ↑ Pitch轴外环PID ↑ **********//
		
    // ********** ↓ Pitch轴内环PID ↓ **********//
    Pitch_In_PID.LastError = Pitch_In_PID.CurrentError;
    Pitch_In_PID.CurrentError = -speed_Euler_Pitch + Pitch_Out_PID.PIDOut;
    Pitch_In_PID.PIDOut = Pitch_In_PID.CurrentError * Pitch_In_PID.P
                          + ( speed_Euler_Pitch_last - speed_Euler_Pitch) *  Pitch_In_PID.D /(CurrentTick - Pitch_In_PID.LastTick)      //都需要注意方向 ，此处未实践！
                          +(target_position_206-target_position_206_last)*Pitch_In_PID.feed_param;      //前馈系数变量   变量符号方向  微分与时间的关系 参照电科
    Pitch_In_PID.PIDOut = Pitch_In_PID.PIDOut > 14000 ? 14000 : Pitch_In_PID.PIDOut;
    Pitch_In_PID.PIDOut = Pitch_In_PID.PIDOut < -14000 ? -14000 : Pitch_In_PID.PIDOut;
		// ********** ↑ Pitch轴内环PID ↑ **********//
		
    target_position_206_last = target_position_206;
    speed_Euler_Pitch_last = speed_Euler_Pitch;                  //不清楚此处逻辑是否错误，以及是否应该将其放于陀螺仪读数处
    //车辆差异导致的输出符号改变暂时删除
    Pitch_In_PID.LastTick = CurrentTick;

    return Pitch_In_PID.PIDOut;
}
// ********** ↑ 云台Pitch轴控制 ↑ **********//



// ********** ↓ 云台Yaw轴控制 ↓ **********//
int16_t Cloud_Control_Yaw(float current_position_205,float target_position_205)
{
    static float target_position_205_last = 0.0f;

    // ********** ↓ Yaw轴外环PID ↓ **********//
    Yaw_Out_PID.LastError = Yaw_Out_PID.CurrentError;
    Yaw_Out_PID.CurrentError = -target_position_205 + current_position_205;
	if(abs_f(Yaw_Out_PID.CurrentError) < 10)
        Yaw_Out_PID.Integral += Yaw_Out_PID.CurrentError;
    else
        Yaw_Out_PID.Integral = 0;
    Yaw_Out_PID.PIDOut = Yaw_Out_PID.CurrentError * Yaw_Out_PID.P
						 + Yaw_Out_PID.Integral * Yaw_Out_PID.I
						 + (Yaw_Out_PID.CurrentError-Yaw_Out_PID.LastError)*Yaw_Out_PID.D;
    // ********** ↑ Yaw轴外环PID ↑ **********//
		
    // ********** ↓ Yaw轴内环PID ↓ **********//
    Yaw_In_PID.LastError = Yaw_In_PID.CurrentError;
    Yaw_In_PID.CurrentError = -Yaw_Out_PID.PIDOut + speed_Euler_Yaw ;
    Yaw_In_PID.PIDOut = Yaw_In_PID.CurrentError * Yaw_In_PID.P
                        + ( speed_Euler_Yaw_last - speed_Euler_Yaw) *  Yaw_In_PID.D  //都需要注意方向 ，此处未实践！
                        +(target_position_205-target_position_205_last)*Yaw_In_PID.feed_param;     //前馈系数变量   变量符号方向  微分与时间的关系 参照电科
    if(GimbalMotionMode == Arm_Revenge_Mode)
    {
        Yaw_In_PID.PIDOut = Yaw_In_PID.PIDOut > CloudOutMax*0.4 ? CloudOutMax*0.4 : Yaw_In_PID.PIDOut;
        Yaw_In_PID.PIDOut = Yaw_In_PID.PIDOut < -CloudOutMax*0.4 ? -CloudOutMax*0.4 : Yaw_In_PID.PIDOut;
    }
    else
    {
        Yaw_In_PID.PIDOut = Yaw_In_PID.PIDOut > CloudOutMax ? CloudOutMax : Yaw_In_PID.PIDOut;
        Yaw_In_PID.PIDOut = Yaw_In_PID.PIDOut < -CloudOutMax ? -CloudOutMax : Yaw_In_PID.PIDOut;
    }
		// ********** ↑ Pitch轴内环PID ↑ **********//
		
    target_position_205_last = target_position_205;
    speed_Euler_Yaw_last = speed_Euler_Yaw;                  //不清楚此处逻辑是否错误，以及是否应该将其放于陀螺仪读数处
    
		return Yaw_In_PID.PIDOut;
}
// ********** ↑ 云台Yaw轴控制 ↑ **********//



// ********** ↓ 云台打击模式Pitch轴PID控制 ↓ **********//
int16_t Pitch_Heat_Control(float current_position_206,float target_position_206)
{
    portTickType CurrentTick = xTaskGetTickCount();
    
	  // ********** ↓ Pitch轴外环PID ↓ **********//
    Pitch_Heat_Out_PID.LastError = Pitch_Heat_Out_PID.CurrentError;
    Pitch_Heat_Out_PID.CurrentError =  -current_position_206 + target_position_206;
    if(abs_f(Pitch_Heat_Out_PID.CurrentError) < 100)
        Pitch_Heat_Out_PID.Integral += Pitch_Heat_Out_PID.CurrentError;
    else
        Pitch_Heat_Out_PID.Integral = 0;
    Pitch_Heat_Out_PID.Integral = Pitch_Heat_Out_PID.Integral > 3000 ? 3000:Pitch_Heat_Out_PID.Integral ;
    Pitch_Heat_Out_PID.Integral = Pitch_Heat_Out_PID.Integral < -3000 ? -3000:Pitch_Heat_Out_PID.Integral ;
    Pitch_Heat_Out_PID.PIDOut = Pitch_Heat_Out_PID.CurrentError * Pitch_Heat_Out_PID.P + Pitch_Heat_Out_PID.Integral * Pitch_Heat_Out_PID.I;
    // ********** ↑ Pitch轴外环PID ↑ **********//
		
    // ********** ↓ Pitch轴内环PID ↓ **********//
    Pitch_Heat_In_PID.LastError = Pitch_Heat_In_PID.CurrentError;
    Pitch_Heat_In_PID.CurrentError = -speed_Euler_Pitch + Pitch_Heat_Out_PID.PIDOut;
    //积分分离
    if(abs_f(Pitch_Heat_In_PID.CurrentError) < 400)
        Pitch_Heat_In_PID.Integral += Pitch_Heat_In_PID.CurrentError;
    else
        Pitch_Heat_In_PID.Integral = 0;
    Pitch_Heat_In_PID.Integral = Pitch_Heat_In_PID.Integral > 1500 ? 1500:Pitch_Heat_In_PID.Integral ;
    Pitch_Heat_In_PID.Integral = Pitch_Heat_In_PID.Integral < -1500 ? -1500:Pitch_Heat_In_PID.Integral;
    Pitch_Heat_In_PID.PIDOut = Pitch_Heat_In_PID.CurrentError * Pitch_Heat_In_PID.P \
                               + Pitch_Heat_In_PID.Integral * Pitch_Heat_In_PID.I \
                               + (speed_Euler_Pitch_last - speed_Euler_Pitch) *  Pitch_Heat_In_PID.D /(CurrentTick - Pitch_Heat_In_PID.LastTick);
    Pitch_Heat_In_PID.PIDOut = Pitch_Heat_In_PID.PIDOut > CloudOutMax ? CloudOutMax : Pitch_Heat_In_PID.PIDOut;
    Pitch_Heat_In_PID.PIDOut = Pitch_Heat_In_PID.PIDOut < -CloudOutMax ? -CloudOutMax : Pitch_Heat_In_PID.PIDOut;
		// ********** ↑ Pitch轴内环PID ↑ **********//
    
		speed_Euler_Pitch_last = speed_Euler_Pitch;
    Pitch_Heat_In_PID.LastTick = CurrentTick;
		
    return Pitch_Heat_In_PID.PIDOut;
}
// ********** ↑ 云台打击模式Pitch轴PID控制 ↑ **********//



// ********** ↓ 云台打击模式Yaw轴PID控制 ↓ **********//
int16_t Yaw_Heat_Control(float current_position_205,float target_position_205)
{
    static float target_position_205_last = 0.0f;
	  portTickType CurrentTick = xTaskGetTickCount();
	
    // ********** ↓ Yaw轴外环PID ↓ **********//
    Yaw_Heat_Out_PID.LastError = Yaw_Heat_Out_PID.CurrentError;
    Yaw_Heat_Out_PID.CurrentError =  -target_position_205 + current_position_205;
	if(abs_f(Yaw_Heat_Out_PID.CurrentError) < 3)
        Yaw_Heat_Out_PID.Integral += Yaw_Heat_Out_PID.CurrentError;
    else
        Yaw_Heat_Out_PID.Integral = 0;
    Yaw_Heat_Out_PID.PIDOut = Yaw_Heat_Out_PID.CurrentError * Yaw_Heat_Out_PID.P
							+ Yaw_Heat_Out_PID.Integral * Yaw_Heat_Out_PID.I
							+(Yaw_Heat_Out_PID.CurrentError -  Yaw_Heat_Out_PID.LastError) * Yaw_Heat_Out_PID.D;
    // ********** ↑ Yaw轴外环PID ↑ **********//
		
    // ********** ↓ Yaw轴内环PID ↓ **********//
    Yaw_Heat_In_PID.LastError = Yaw_Heat_In_PID.CurrentError;
    Yaw_Heat_In_PID.CurrentError =  -speed_Euler_Yaw + Yaw_Heat_Out_PID.PIDOut;
    Yaw_Heat_In_PID.PIDOut = Yaw_Heat_In_PID.CurrentError * Yaw_Heat_In_PID.P
                             + ( speed_Euler_Yaw_last - speed_Euler_Yaw) *  Yaw_Heat_In_PID.D
                             + (target_position_205-target_position_205_last)*Yaw_Heat_In_PID.feed_param;
		Yaw_Heat_In_PID.PIDOut = Yaw_Heat_In_PID.PIDOut > Heat_CloudOutMax ? Heat_CloudOutMax : Yaw_Heat_In_PID.PIDOut;
    Yaw_Heat_In_PID.PIDOut = Yaw_Heat_In_PID.PIDOut < -Heat_CloudOutMax ? -Heat_CloudOutMax : Yaw_Heat_In_PID.PIDOut;
    // ********** ↑ Yaw轴内环PID ↑ **********//
		
    target_position_205_last = target_position_205;
    speed_Euler_Yaw_last = speed_Euler_Yaw;
	  Yaw_Heat_In_PID.LastTick = CurrentTick;

    return -Yaw_Heat_In_PID.PIDOut;
}
// ********** ↑ 云台打击模式Yaw轴PID控制 ↑ **********//
