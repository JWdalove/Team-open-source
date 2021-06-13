#include "Status_Update.h"

extern GimbalControl_Mode_Enum Gimbal_Control;
extern MotionMode_Enum		MontionMode;
extern FricStatus_Enum		FricStatus;
extern DBUSDecoding_Type	DBUS_ReceiveData,LASTDBUS_ReceiveData;
extern Communicate_info		Comm;
extern imagepro_info 		MiniPC;
extern Judge_rece			Judge_DATA;
extern Cloud_info			Yaw,Pitch;
extern Sentry_info 			SentryInfo;
extern FRAME_RATE			FrameRate;
extern HeatSubschema_Enum   GimbalMotionMode;
extern Chassis_info	   Chassis;

extern int Time2Counter; 

// ********** ↓ 控制状态更新 ↓ **********//
void Control_Status_Update(portTickType Tick)
{
	static u8 Move_Flag=0;
	if(DBUS_ReceiveData.ch3==-660)Move_Flag=0;
	else if(DBUS_ReceiveData.ch3==660)Move_Flag=1;
	
#if Sentry == 0  //行走部分
    /*
    receive data :	5 MONTION_INIT     //复位
					          2 MONTION_RELAX    //安全
					          3 MONTION_RC       //遥控器
					         1/4 MONTION_PATROL  //巡逻       
    */
	
	// ********** ↓ 底盘接收云台发送的模式 ↓ **********//	
	if(Comm.CRecriveMotionMode == 5)
	{
		    MontionMode = MONTION_INIT;		    //复位
	}
    else if(Comm.CRecriveMotionMode == 1 || Comm.CRecriveMotionMode == 4)
    {
        MontionMode = MONTION_PATROL;	    //巡逻
    }
    else if(Comm.CRecriveMotionMode == 3)
    {
        MontionMode = MONTION_RC; 		    //遥控器
			  Chassis.initial_position=0;
    }
    else if(Comm.CRecriveMotionMode == 2)
    {
		    MontionMode = MONTION_RELAX;	    //安全
    }
    else
    {
		    MontionMode = MONTION_RELAX;      //安全
    }
  // ********** ↑ 底盘接收云台发送的模式 ↑ **********//
		
#elif Sentry == 1   //云台部分

  // ********** ↓ 云台发送给底盘的模式 ↓ **********//	 
	if( DBUS_ReceiveData.ch0==-660&&
		  DBUS_ReceiveData.ch1==-660&&
		  DBUS_ReceiveData.ch2== 660&&
		  DBUS_ReceiveData.ch3==-660  )
	    Comm.GSendMotionMode=5;                 //复位
	else if(DBUS_ReceiveData.switch_left == 1)  //自动
	{
		if(DBUS_ReceiveData.ch0== -660 && DBUS_ReceiveData.ch1== -660 )      //巡逻
			Comm.GSendMotionMode = 1;
		if(DBUS_ReceiveData.ch0==  660 && DBUS_ReceiveData.ch1== -660 )      //遥控器
			Comm.GSendMotionMode = 3;
	}
	else if((DBUS_ReceiveData.switch_left == 3) || (DBUS_ReceiveData.switch_left == 1 && Move_Flag==0))
	{
		Comm.GSendMotionMode = 3;      //遥控器
	}
	else if(DBUS_ReceiveData.switch_left == 2)
	{
		Comm.GSendMotionMode = 2;      //安全
	}
  // ********** ↑ 云台发送给底盘的模式 ↑ **********//
	
	
	
  // ********** ↓ 云台模式 ↓ **********//	 	
	if( DBUS_ReceiveData.ch0==-660&&
		  DBUS_ReceiveData.ch1==-660&&
		  DBUS_ReceiveData.ch2== 660&&
		  DBUS_ReceiveData.ch3==-660  )
	    Gimbal_Control = GIMBAL_INIT;           //复位
    else if(DBUS_ReceiveData.switch_left == 3)
    {
        Gimbal_Control = GIMABL_RC;		        //遥控器
    }
	  else if(DBUS_ReceiveData.switch_left == 1)
	  {
		  if(DBUS_ReceiveData.switch_right == 2 || MiniPC.Enemy_Count>=3)
		  {
			  Gimbal_Control = GIMBAL_HEAT;         //打击
		  }
		else if(DBUS_ReceiveData.switch_right == 1)
			  Gimbal_Control = GIMBAL_FLOOW;	      //巡逻
		else
			  Gimbal_Control = GIMBAL_FLOOW;        //巡逻
		  FricStatus = FricStatus_Working;        //摩擦轮工作
	  }
    else
    {
        Gimbal_Control = GIMBAL_RELAX;        //安全
			  FricStatus = FricStatus_Stop;
    }
  // ********** ↑ 云台模式 ↑ **********//

	// ********** ↓ 云台遥控器模式下摩擦轮控制 ↓ **********//	
	if(Gimbal_Control == GIMABL_RC)
    {
        if( DBUS_ReceiveData.switch_right ==2 )
				{
            FricStatus = FricStatus_Stop;
        }
        else if( DBUS_ReceiveData.switch_right !=2 )
        {
            FricStatus = FricStatus_Working;
        }

    }
	// ********** ↑ 云台遥控器模式下摩擦轮控制 ↑ **********//
	
#endif
	DBUS_ReceiveData.switch_left_last = DBUS_ReceiveData.switch_left;
  DBUS_ReceiveData.switch_right_last = DBUS_ReceiveData.switch_right;
}
// ********** ↑ 控制状态更新 ↑ **********//
