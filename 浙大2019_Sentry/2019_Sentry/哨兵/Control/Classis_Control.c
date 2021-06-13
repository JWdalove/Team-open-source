#include "Classis_Control.h"

float kp,kd;
extern PID_Type		MotorPID;
extern Chassis_info Chassis;
extern Sentry_info	SentryInfo;
smooth_t		lf_smooth = SMOOTH_GEN_DAFAULT;
smooth_t		rf_smooth = SMOOTH_GEN_DAFAULT;


ramp_t tr_ramp = RAMP_GEN_DAFAULT;
void Chassis_SpeedSet(float YSpeed)
{
    //存储上一次速度
    Chassis.LastV = Chassis.TargetV;
    //加入过渡速度缓冲
    YSpeed = YSpeed > MAXMOTORSPEED ? MAXMOTORSPEED : YSpeed;
    YSpeed = YSpeed < -MAXMOTORSPEED ? -MAXMOTORSPEED : YSpeed;


    Chassis.TargetV = YSpeed;
    if(abs_f(Chassis.TargetV - Chassis.LastV) > 1000)
        Chassis.TargetV = (1.0f-0.02f)*Chassis.LastV + 0.02f * Chassis.TargetV; // 一阶滞后
    else
        Chassis.TargetV = YSpeed;
    //+ (Chassis.TargetV - Chassis.LastV) * ramp_calc(&tr_ramp);
}

void Chassis_MoveCalculate(void)
{
    if(SentryInfo.Position == Behind_Curve)
    {
        Chassis.F.TargetSpeed = -Chassis.TargetV * 1.15f;
        Chassis.B.TargetSpeed = Chassis.TargetV * 0.85f;
    }
    else if(SentryInfo.Position == Front_Curve)
    {
        Chassis.F.TargetSpeed = -Chassis.TargetV * 0.85f;
        Chassis.B.TargetSpeed = Chassis.TargetV * 1.15f;
    }
    else
    {
        Chassis.F.TargetSpeed = -Chassis.TargetV;
        Chassis.B.TargetSpeed = Chassis.TargetV;
    }
}

void Chassis_Position(void)
{
    //获取哨兵位移
    SentryInfo.mil = EncodeGetMileage(IRQ_Count) * S + SentryInfo.compensation;

    SentryInfo.infrared_last = SentryInfo.infrared;
    SentryInfo.microswitch1_last = SentryInfo.microswitch1;
    SentryInfo.microswitch2_last = SentryInfo.microswitch2;
    SentryInfo.microswitch3_last = SentryInfo.microswitch3;

    SentryInfo.infrared = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_9);
    SentryInfo.microswitch1 = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8);
    SentryInfo.microswitch2 = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_9);
    SentryInfo.microswitch3 = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5);

    /*********************  ↓  哨兵定位   ↓  **********************/

    /*********************  ↑  哨兵定位   ↑  **********************/
    /*********************  ↓  红外校正   ↓  **********************/

    /*********************  ↑  红外校正   ↑  **********************/
}

void Chassis_Contral(void)
{
    Chassis.F.Out = (Chassis.F.TargetSpeed -  Chassis.F.Esc.Angle_Speed)*2.0f;
    Chassis.F.Out =  Chassis.F.Out >  13000 ?  13000 : Chassis.F.Out;   //限幅
    Chassis.F.Out =  Chassis.F.Out < -13000 ? -13000 : Chassis.F.Out;

    Chassis.B.Out = (Chassis.B.TargetSpeed -  Chassis.B.Esc.Angle_Speed)*2.0f;
    Chassis.B.Out =  Chassis.B.Out >  13000 ?  13000 : Chassis.B.Out;   //限幅
    Chassis.B.Out =  Chassis.B.Out < -13000 ? -13000 : Chassis.B.Out;
}


