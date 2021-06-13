#include "Cloud_Control.h"


extern Cloud_info			Yaw,Pitch;
extern PID_Type				Pitch_In_PID,Pitch_Out_PID,Yaw_In_PID,Yaw_Out_PID;
extern PID_Type				Pitch_Heat_In_PID,Pitch_Heat_Out_PID,Yaw_Heat_In_PID,Yaw_Heat_Out_PID;
extern imagepro_info 		imageprocess;
extern HeatSubschema_Enum	GimbalMotionMode;
extern float  speed_Euler_Pitch,speed_Euler_Roll,speed_Euler_Yaw;
extern float  speed_Euler_Yaw_last,speed_Euler_Pitch_last,speed_Euler_Roll_last;

/*******************************  Param_PID  **********************************/
void Cloud_PID_Param(void)
{
	Pitch_Out_PID.P = 110.0f;
	Pitch_Out_PID.I = 0.0f;
	
	Pitch_In_PID.P = -3.0f;
	Pitch_In_PID.D = 0.5f;
	Pitch_In_PID.feed_param = 0.0;
	
	Pitch_Heat_Out_PID.P = 0.0f;
	Pitch_Heat_Out_PID.I = 0.0f;
	
	Pitch_Heat_In_PID.P = 0.0f;
	Pitch_Heat_In_PID.I = 0.0f;
	Pitch_Heat_In_PID.D = 0.0f;
	Pitch_Heat_In_PID.feed_param = 0.0;
	
	Yaw_Out_PID.P = -115.0f;
	Yaw_Out_PID.D = 0.0f;
	
	Yaw_In_PID.P = -1.5f;
	Yaw_In_PID.D = 0.0f;
	Yaw_In_PID.feed_param = 0.0f;
	
	Yaw_Heat_Out_PID.P = 0.0f;
	Yaw_Heat_Out_PID.D = 0.0f;
	
	Yaw_Heat_In_PID.P = 0.0f;
	Yaw_Heat_In_PID.D = 0.0f;
	Yaw_Heat_In_PID.feed_param = 0.0f;
	
	Pitch.Give = Pitch.Imu_Euler;//陀螺仪上电160,枪口向下上电
	Pitch.Give_Max = 170;//5150 
	Pitch.Give_Min = 161;//5750
	
	Yaw.Give = Yaw.Imu_Euler;
	
}


uint8_t JumpToRevengeFlag = 1,JumpToAutoFlag  = 1;
void Angle_Handle(void)
{
//		static TickType_t Angle_Ticks = 0;
    static int8_t revenge_dir = 1;
//	  Angle_Ticks = xTaskGetTickCount();
    switch(GimbalMotionMode)
    {

    case Arm_Revenge_Mode:
    {
        if(JumpToRevengeFlag == 1)
        {
            Yaw_Out_PID.P = 3.0f;
            Yaw_Out_PID.D = 0.0f;
            //内环
            Yaw_In_PID.P  = 2.0f;
            Yaw_In_PID.D  = 0.6f;									//PID参数初始化
            /*******************************    **********************************/
//            if(judge_rece_mesg.blood_changed_data.armor_type == 1)
//                Yaw.Give = 11700;
//            else
//                Yaw.Give = 3508;
            /*******************************    **********************************/
        }
        JumpToRevengeFlag = 0;
        JumpToAutoFlag = 1;
        Cloud_Angle_Set(Yaw.Give,0.0f,Pitch.rotate_coe,0.42f);

//        if(judge_rece_mesg.blood_changed_data.armor_type == 1) //0号装甲
//        {
//            if(revenge_dir == 1)
//            {
//                if(Yaw.Esc.Revenge_Angle <= 11700 - 2000)
//                {
//                    Yaw.Give += 7;
//                    revenge_dir = 0;
//                }
//                else
//                {
//                    Yaw.Give -= 7;
//                }
//            }
//            else
//            {
//                if(Yaw.Esc.Revenge_Angle >= 11700 + 2000)
//                {
//                    Yaw.Give -= 7;
//                    revenge_dir = 1;
//                }
//                else
//                {
//                    Yaw.Give += 7;
//                }
//            }
//        }
//        else																									//1号装甲
//        {
//            if(revenge_dir == 1)
//            {
//                if(Yaw.Esc.Revenge_Angle <= 3508 - 2000)
//                {
//                    Yaw.Give += 7;
//                    revenge_dir = 0;
//                }
//                else
//                {
//                    Yaw.Give -= 7;
//                }
//            }
//            else
//            {
//                if(Yaw.Esc.Revenge_Angle >= 3508 + 2000)
//                {
//                    Yaw.Give -= 7;
//                    revenge_dir = 1;
//                }
//                else
//                {
//                    Yaw.Give += 7;
//                }
//            }
//        }
        break;
    }
    case Auto_Normal_Mode:
    {
        if(JumpToAutoFlag == 1)
        {
            Yaw.Give = Yaw.Imu_Euler;
            Yaw_Out_PID.P = 175.0f;
            Yaw_Out_PID.D = 0.0f;
            //内环
            Yaw_In_PID.P  = 0.5f;
            Yaw_In_PID.D  = 0.1f;								//PID参数初始化
        }
        JumpToRevengeFlag = 1;
        JumpToAutoFlag = 0;
        Cloud_Angle_Set(Yaw.rotate_coe,0.090f,Pitch.rotate_coe,0.80f);
        break;
    }
    }
}

void Cloud_Angle_Set(int16_t Yaw_Angle,float Y_Sensity, int16_t Pitch_Angle,float P_Sensity)
{
    static int8_t Pitch_Dir = 0; //Pitch 扫描方向
    Yaw.Give += (Yaw_Angle * Y_Sensity);

    if(Pitch_Dir == 1)
        Pitch.Give += Pitch_Angle *  P_Sensity;
    else
        Pitch.Give -= Pitch_Angle *  P_Sensity;
    Pitch.Give = Pitch.Give > Pitch.Give_Max ? Pitch.Give_Max :Pitch.Give; //Pitch输入限幅
    Pitch.Give = Pitch.Give < Pitch.Give_Min ? Pitch.Give_Min :Pitch.Give;
    if(Pitch.Esc.Machine_Angle >= Pitch.Give_Max - 200)
        Pitch_Dir = 0;
    if(Pitch.Esc.Machine_Angle <= Pitch.Give_Min + 90)
        Pitch_Dir = 1;
}

void GimbalHeat_AngleSet(int16_t Yaw_Angle,float Yaw_Offset,int16_t Pitch_Angle,float Pitch_Offset)
{
    //坐标转换
    //图像坐标系坐标
//			imageprocess.xc = (Yaw_Angle   - imageprocess.u0) * imageprocess.dx * imageprocess.zc/imageprocess.f;
//			imageprocess.yc = (Pitch_Angle - imageprocess.v0) * imageprocess.dy * imageprocess.zc/imageprocess.f;

    //kalman fliter

    Yaw.Give = Yaw_Angle + Yaw_Offset;
    Pitch.Give = Pitch_Angle + Pitch_Offset;

    Yaw.Give = Yaw.Give > 854 ? 854 :Yaw.Give; //Yaw输入限幅
    Yaw.Give = Yaw.Give < 0 ? 0 :Yaw.Give;
    Pitch.Give = Pitch.Give > 682 ? 682 :Pitch.Give; //Pitch输入限幅
    Pitch.Give = Pitch.Give < 0 ? 0 :Pitch.Give;
}

int16_t Cloud_Control_Pitch(float current_position_206,float target_position_206)
{
    portTickType CurrentTick = xTaskGetTickCount();
    static float target_position_206_last = 0.0f;
    /*********************  ↓  外环     ↓  **********************/
    Pitch_Out_PID.LastError = Pitch_Out_PID.CurrentError;
    Pitch_Out_PID.CurrentError =  current_position_206 - target_position_206;
    if(abs_f(Pitch_Out_PID.CurrentError) < 600)
        Pitch_Out_PID.Integral += Pitch_Out_PID.CurrentError;
    else
        Pitch_Out_PID.Integral = 0;
    Pitch_Out_PID.Integral = Pitch_Out_PID.Integral > 3000 ? 3000 : Pitch_Out_PID.Integral;
    Pitch_Out_PID.Integral = Pitch_Out_PID.Integral < -2000 ? -2000 : Pitch_Out_PID.Integral;
    Pitch_Out_PID.PIDOut = Pitch_Out_PID.CurrentError * Pitch_Out_PID.P + Pitch_Out_PID.Integral * Pitch_Out_PID.I;
    /*********************  ↑  外环     ↑  **********************/
    /*********************  ↓  内环     ↓  **********************/
    Pitch_In_PID.LastError = Pitch_In_PID.CurrentError;
    Pitch_In_PID.CurrentError =   speed_Euler_Pitch - Pitch_Out_PID.PIDOut;
    Pitch_In_PID.PIDOut = Pitch_In_PID.CurrentError * Pitch_In_PID.P
                          + ( speed_Euler_Pitch_last - speed_Euler_Pitch) *  Pitch_In_PID.D /(CurrentTick - Pitch_In_PID.LastTick)  //都需要注意方向 ，此处未实践！
                          +(target_position_206-target_position_206_last)*Pitch_In_PID.feed_param;     //前馈系数变量   变量符号方向  微分与时间的关系 参照电科
    /*********************  ↑ 内环      ↑  **********************/
    target_position_206_last = target_position_206;
    speed_Euler_Pitch_last = speed_Euler_Pitch;                  //不清楚此处逻辑是否错误，以及是否应该将其放于陀螺仪读数处
    /*********************  ↓ 限幅      ↓  **********************/
    Pitch_In_PID.PIDOut = Pitch_In_PID.PIDOut > CloudOutMax ? CloudOutMax : Pitch_In_PID.PIDOut;
    Pitch_In_PID.PIDOut = Pitch_In_PID.PIDOut < -CloudOutMax ? -CloudOutMax : Pitch_In_PID.PIDOut;
    //车辆差异导致的输出符号改变暂时删除
    Pitch_In_PID.LastTick = CurrentTick;

    return Pitch_In_PID.PIDOut;
}

int16_t Cloud_Control_Yaw(float current_position_205,float target_position_205)
{
    static float target_position_205_last = 0.0f;

    /*********************  ↓  外环     ↓  **********************/
    Yaw_Out_PID.LastError = Yaw_Out_PID.CurrentError;
    Yaw_Out_PID.CurrentError =  current_position_205 - target_position_205;
    Yaw_Out_PID.PIDOut = Yaw_Out_PID.CurrentError * Yaw_Out_PID.P + (Yaw_Out_PID.CurrentError-Yaw_Out_PID.LastError)*Yaw_Out_PID.D;
    /*********************  ↑  外环     ↑  **********************/
    /*********************  ↓  内环     ↓  **********************/
    Yaw_In_PID.LastError = Yaw_In_PID.CurrentError;
    Yaw_In_PID.CurrentError =speed_Euler_Yaw - Yaw_Out_PID.PIDOut ;
    Yaw_In_PID.PIDOut = Yaw_In_PID.CurrentError * Yaw_In_PID.P
                        + ( speed_Euler_Yaw_last - speed_Euler_Yaw) *  Yaw_In_PID.D  //都需要注意方向 ，此处未实践！
                        +(target_position_205-target_position_205_last)*Yaw_In_PID.feed_param;     //前馈系数变量   变量符号方向  微分与时间的关系 参照电科
    /*********************  ↑ 内环      ↑  **********************/
    target_position_205_last = target_position_205;
    speed_Euler_Yaw_last = speed_Euler_Yaw;                  //不清楚此处逻辑是否错误，以及是否应该将其放于陀螺仪读数处
    /*********************  ↓ 限幅      ↓  **********************/
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

    return -Yaw_In_PID.PIDOut;
}

int16_t Pitch_Heat_Control(float current_position_206,float target_position_206)
{
    portTickType CurrentTick = xTaskGetTickCount();
//		static float target_position_206_last = 0.0f;
    /*********************  ↓  外环     ↓  **********************/
    Pitch_Heat_Out_PID.LastError = Pitch_Heat_Out_PID.CurrentError;
    Pitch_Heat_Out_PID.CurrentError =  current_position_206 - target_position_206;
    if(abs_f(Pitch_Heat_Out_PID.CurrentError) < 100)
        Pitch_Heat_Out_PID.Integral += Pitch_Heat_Out_PID.CurrentError;
    else
        Pitch_Heat_Out_PID.Integral = 0;
    Pitch_Heat_Out_PID.Integral = Pitch_Heat_Out_PID.Integral > 2000 ? 2000:Pitch_Heat_Out_PID.Integral ;
    Pitch_Heat_Out_PID.Integral = Pitch_Heat_Out_PID.Integral < -2000 ? -2000:Pitch_Heat_Out_PID.Integral ;
    Pitch_Heat_Out_PID.PIDOut = Pitch_Heat_Out_PID.CurrentError * Pitch_Heat_Out_PID.P + Pitch_Heat_Out_PID.Integral * Pitch_Heat_Out_PID.I;
    /*********************  ↑  外环     ↑  **********************/
    /*********************  ↓  内环     ↓  **********************/
    Pitch_Heat_In_PID.LastError = Pitch_Heat_In_PID.CurrentError;
    Pitch_Heat_In_PID.CurrentError = speed_Euler_Pitch - Pitch_Heat_Out_PID.PIDOut;
    //积分分离
    if(abs_f(Pitch_Heat_In_PID.CurrentError) < 400)
        Pitch_Heat_In_PID.Integral += Pitch_Heat_In_PID.CurrentError;
    else
        Pitch_Heat_In_PID.Integral = 0;
    Pitch_Heat_In_PID.Integral = Pitch_Heat_In_PID.Integral > 1500 ? 1500:Pitch_Heat_In_PID.Integral ;
    Pitch_Heat_In_PID.Integral = Pitch_Heat_In_PID.Integral < -1500 ? -1500:Pitch_Heat_In_PID.Integral;
    Pitch_Heat_In_PID.PIDOut = Pitch_Heat_In_PID.CurrentError * Pitch_Heat_In_PID.P \
                               + Pitch_Heat_In_PID.Integral * Pitch_Heat_In_PID.I \
                               + (speed_Euler_Pitch_last - speed_Euler_Pitch) *  Pitch_Heat_In_PID.D /(CurrentTick - Pitch_Heat_In_PID.LastTick);  //都需要注意方向 ，此处未实践！
//							+ (target_position_206-target_position_206_last)*Pitch_Heat_In_PID.feed_param;     //前馈系数变量   变量符号方向  微分与时间的关系 参照电科
    /*********************  ↑ 内环      ↑  **********************/
//		target_position_206_last = target_position_206;
    speed_Euler_Pitch_last = speed_Euler_Pitch;                  //不清楚此处逻辑是否错误，以及是否应该将其放于陀螺仪读数处
    /*********************  ↓ 限幅      ↓  **********************/
    Pitch_Heat_In_PID.PIDOut = Pitch_Heat_In_PID.PIDOut > CloudOutMax ? CloudOutMax : Pitch_Heat_In_PID.PIDOut;
    Pitch_Heat_In_PID.PIDOut = Pitch_Heat_In_PID.PIDOut < -CloudOutMax ? -CloudOutMax : Pitch_Heat_In_PID.PIDOut;
    //车辆差异导致的输出符号改变暂时删除
    Pitch_Heat_In_PID.LastTick = CurrentTick;

    return Pitch_Heat_In_PID.PIDOut;
}

int16_t Yaw_Heat_Control(float current_position_205,float target_position_205)
{
    static float target_position_205_last = 0.0f;

    /*********************  ↓  外环     ↓  **********************/
    Yaw_Heat_Out_PID.LastError = Yaw_Heat_Out_PID.CurrentError;
    Yaw_Heat_Out_PID.CurrentError =  target_position_205 - current_position_205;
    if(abs_f(Yaw_Heat_Out_PID.CurrentError) < 100)
        Yaw_Heat_Out_PID.Integral += Yaw_Heat_Out_PID.CurrentError;
    else
        Yaw_Heat_Out_PID.Integral = 0;
    Yaw_Heat_Out_PID.Integral = Yaw_Heat_Out_PID.Integral > 1000 ? 1000 : Yaw_Heat_Out_PID.Integral ;
    Yaw_Heat_Out_PID.Integral = Yaw_Heat_Out_PID.Integral < -1000 ? -1000 : Yaw_Heat_Out_PID.Integral ;
    Yaw_Heat_Out_PID.PIDOut = Yaw_Heat_Out_PID.CurrentError * Yaw_Heat_Out_PID.P
                              + Yaw_Heat_Out_PID.Integral * Yaw_Heat_Out_PID.I;
    /*********************  ↑  外环     ↑  **********************/
    /*********************  ↓  内环     ↓  **********************/
    Yaw_Heat_Out_PID.LastError = Yaw_Heat_In_PID.CurrentError;
    Yaw_Heat_In_PID.CurrentError =  speed_Euler_Yaw - Yaw_Heat_Out_PID.PIDOut;
    Yaw_Heat_In_PID.PIDOut = Yaw_Heat_In_PID.CurrentError * Yaw_Heat_In_PID.P
                             + ( speed_Euler_Yaw_last - speed_Euler_Yaw) *  Yaw_Heat_In_PID.D  //都需要注意方向 ，此处未实践！
                             +(target_position_205-target_position_205_last)*Yaw_Heat_In_PID.feed_param;     //前馈系数变量   变量符号方向  微分与时间的关系 参照电科
    /*********************  ↑ 内环      ↑  **********************/
    target_position_205_last = target_position_205;
    speed_Euler_Yaw_last = speed_Euler_Yaw;                  //不清楚此处逻辑是否错误，以及是否应该将其放于陀螺仪读数处
    /*********************  ↓ 限幅      ↓  **********************/
    Yaw_Heat_In_PID.PIDOut = Yaw_Heat_In_PID.PIDOut > CloudOutMax ? CloudOutMax : Yaw_Heat_In_PID.PIDOut;
    Yaw_Heat_In_PID.PIDOut = Yaw_Heat_In_PID.PIDOut < -CloudOutMax ? -CloudOutMax : Yaw_Heat_In_PID.PIDOut;


    return -Yaw_Heat_In_PID.PIDOut;
}
