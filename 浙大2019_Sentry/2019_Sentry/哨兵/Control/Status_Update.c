#include "Status_Update.h"

extern GimbalControl_Mode_Enum Gimbal_Control;
extern MotionMode_Enum		MontionMode;
extern FricStatus_Enum		FricStatus;
extern DBUSDecoding_Type	DBUS_ReceiveData,LASTDBUS_ReceiveData;
extern Communicate_info		Comm;
extern Sentry_info 			SentryInfo;
uint8_t DBUSFrameRate=0;//gimbal_monitor
void Control_Status_Init(void)
{
//	Control_Mode = Auto_Control_Mode;
}


void Control_Status_Update(portTickType Tick)
{
    static int Revenge_Tick = -12000;

#if Sentry == 0  //行走部分
    /*
    receive data :	1 chassis rc
					3 chassis relax
    				2 chsssis patrol
    */
    if(Comm.CRecriveMotionMode == 1)
    {
        MontionMode = MONTION_RC; 			//遥控器控制
    }
    else if(Comm.CRecriveMotionMode == 3)
    {
        MontionMode = MONTION_RELAX;	//安全模式
    }
//    else if(Comm.CRecriveMotionMode == 2 || Comm.CRecriveMotionMode == 4)
//    {
//        MontionMode = MONTION_PATROL;	//全自动
//    }
    else if(Comm.CRecriveMotionMode == 0)
    {
        MontionMode = MONTION_RELAX;
    }
    else                            //默认全自动
    {
		MontionMode = MONTION_RELAX;
        /*//默认全自动   此处需要修改   仅为调试用*/
    }
#elif Sentry == 1 //云台部分          
	 /*************行走部分控制信息传递**************/
	if(DBUS_ReceiveData.switch_left == 1)
    {
        Comm.GSendMotionMode = 1;			//行走部分遥控控制
    }
	else if(DBUS_ReceiveData.switch_left == 2)
    {
        Comm.GSendMotionMode = 2;			//行走部分转为巡逻模式
        if(SentryInfo.nobullet)				//???????????????????????????????????????????
            Comm.GSendMotionMode = 4;
    }
    //云台部分控制模式选择
    if(DBUS_ReceiveData.switch_left == 3 )
    {
        Gimbal_Control = GIMABL_RC;		//云台模式转为遥控控制
    }
    else
    {
        Gimbal_Control = GIMBAL_RELAX;	//follow
		if(USART2_MiniPC[5] == 1){
			Gimbal_Control = GIMBAL_HEAT;//转为too too too 模式
		}
    }


	if(Gimbal_Control == GIMABL_RC)
    {
        /*******************  ↓  摩擦轮控制方式  ↓  *******************/
        if(DBUS_ReceiveData.switch_right == 2 && DBUS_ReceiveData.switch_right_last != 2)//跳变到2
        {
            FricStatus = FricStatus_Stop;
        }
        else if(DBUS_ReceiveData.switch_right == 3 && DBUS_ReceiveData.switch_right_last != 3)//跳变到3
        {
            FricStatus = FricStatus_Working;
        }

    }
//    else if(Gimbal_Control == GIMBAL_FLOOW || Gimbal_Control == GIMBAL_HEAT)
//    {
//        FricStatus = FricStatus_Working; //摩擦轮在自动模式下默认工作
//    }
    /*******************  ↓  云台角度设定  ↓  *******************/
    // 装甲收到伤害
//    if(Gimbal_Control == GIMBAL_FLOOW)
//    {
//        if((((SentryInfo.remain_last_hp - judge_rece_mesg.game_information.remain_hp) >= 50)\
//                || ((xTaskGetTickCount() - Revenge_Tick) < 4500)) && xTaskGetTickCount() > 2000)
//        {
//            if((SentryInfo.remain_last_hp - judge_rece_mesg.game_information.remain_hp) >= 50)
//                Revenge_Tick = xTaskGetTickCount();

//            GimbalMotionMode = Arm_Revenge_Mode;   //装甲复仇 @ @

//            SentryInfo.remain_last_hp = judge_rece_mesg.game_information.remain_hp;
//        }
//        else
//        {
//            GimbalMotionMode = Auto_Normal_Mode;	//正常旋转
//        }

//    }
    /*******************  ↑  云台角度设定  ↑  *******************/
#endif
	DBUS_ReceiveData.switch_left_last = DBUS_ReceiveData.switch_left;
    DBUS_ReceiveData.switch_right_last = DBUS_ReceiveData.switch_right;
}
