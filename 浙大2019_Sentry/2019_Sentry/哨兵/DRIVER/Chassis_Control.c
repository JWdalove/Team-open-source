#include "Chassis_Control.h"
extern Judge_rece	Judge_DATA;
extern Chassis_info Chassis;
extern Sentry_info	SentryInfo;
extern Poked_info	Poked;
extern POSITION		Position;
extern imagepro_info MiniPC;
extern Communicate_info 		Comm;
extern ChassisMode_Enum   		Chassis_Heat_Mode;
extern int hurt_Count;
extern DBUSDecoding_Type  DBUS_ReceiveData,LASTDBUS_ReceiveData;

u8 front_hurt=0,edge_hurt=0;

/***************************************
  * @brief  :底盘参数初始化
  * @param  :
****************************************/
void Chassis_Parm()
{
	SentryInfo.dir = -1;
	SentryInfo.Chassis_Speed=5000.0f;
}

/***************************************
  * @brief  :底盘功率计算
  * @param  :Judge_DATA.Power,Judge_DATA.Power_Buffer,Power_MAX,Power_Buffer_MAX
****************************************/
void Power_Calculate()
{
	static u8 flag=0;
	
	if(Judge_DATA.Power_Buffer < 30)flag = 1;
	if(flag==1 && Judge_DATA.Power_Buffer > 80)flag=0;
	
	if(flag==1)Chassis.PowerLimit = 0.6f;
	else Chassis.PowerLimit = 1.0f;
	
}
/***************************************
  * @brief  :枪口热量计算
  * @param  :Judge_DATA.Shoot_Speed,Judge_DATA.Shooterheat,Shooterheat_MAX
****************************************/
void ShootPower_Calculate()
{
	Poked.Preview_n = (480.0f - Judge_DATA.Shooterheat) / Judge_DATA.Shoot_Speed;	//剩余可发射弹丸数量
}

/***************************************
  * @brief  :底盘运动模式判断
  * @param  :
****************************************/
void Chassis_Heat_Mode_Update()
{
	Chassis_Heat_Mode = Normal_Mode;
}

/***************************************
  * @brief  :底盘运动速度设定
  * @param  :
****************************************/
void Chassis_SpeedSet(float YSpeed)
{
    Chassis.LastV = Chassis.TargetV;
    YSpeed = YSpeed > MAXMOTORSPEED ? MAXMOTORSPEED : YSpeed;
    YSpeed = YSpeed < -MAXMOTORSPEED ? -MAXMOTORSPEED : YSpeed;
	
    Chassis.TargetV = YSpeed;
    if(abs_f(Chassis.TargetV - Chassis.LastV) > 1000)
        Chassis.TargetV = (1.0f-0.02f)*Chassis.LastV + 0.02f * Chassis.TargetV; // 一阶滞后
    else
        Chassis.TargetV = YSpeed;
}

/***************************************
  * @brief  :底盘差速计算
  * @param  :
****************************************/
void Chassis_MoveCalculate(void)
{
    if(Position == Front_Curve)
    {
        Chassis.F.TargetSpeed = -Chassis.TargetV * Chassis.PowerLimit * 1.1f;//1.1
        Chassis.B.TargetSpeed = Chassis.TargetV * Chassis.PowerLimit  * 0.97f;//0.97
    }
    else if(Position == Behind_Curve)
    {
        Chassis.F.TargetSpeed = -Chassis.TargetV * Chassis.PowerLimit * 0.97f;//0.97
        Chassis.B.TargetSpeed = Chassis.TargetV * Chassis.PowerLimit  * 1.1f;//1.1
    }
    else
    {
        Chassis.F.TargetSpeed = -Chassis.TargetV * Chassis.PowerLimit;
        Chassis.B.TargetSpeed = Chassis.TargetV * Chassis.PowerLimit;
    }
}

/***************************************
  * @brief  :底盘控制
  * @param  :
****************************************/
void Chassis_Contral(void)
{
	
    Chassis.F.Out = (Chassis.F.TargetSpeed -  Chassis.F.Esc.Angle_Speed)*(5.0f);
    Chassis.F.Out =  Chassis.F.Out >  3000 ?  3000 : Chassis.F.Out;
    Chassis.F.Out =  Chassis.F.Out < -3000 ? -3000 : Chassis.F.Out;

    Chassis.B.Out = (Chassis.B.TargetSpeed -  Chassis.B.Esc.Angle_Speed)*(5.0f);
    Chassis.B.Out =  Chassis.B.Out >  3000 ?  3000 : Chassis.B.Out;
    Chassis.B.Out =  Chassis.B.Out < -3000 ? -3000 : Chassis.B.Out;
}
