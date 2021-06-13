#include "struct.h"

//云台PID参数
PID_Type 	Pitch_In_PID , Pitch_Out_PID , Pitch_Heat_In_PID , Pitch_Heat_Out_PID ,
Yaw_In_PID , Yaw_Out_PID , Yaw_Heat_In_PID , Yaw_Heat_Out_PID,Chassis_Follow_PID;

//拨弹PID参数
PID_Type	                   Poked_InPID , Poked_OutPID;

//底盘信息
Chassis_info	               Chassis;

//云台信息
Cloud_info		               Yaw,Pitch;

//拨弹信息
Poked_info		               Poked;

//视觉识别像素信息,待改
imagepro_info 			         MiniPC;

//哨兵信息
Sentry_info 			           SentryInfo;

//通讯信息
Communicate_info 		         Comm;
//位置
POSITION				             Position;

//帧率监测
FRAME_RATE				           FrameRate;

//控制模式
ControlMode_Enum		         Control_Mode;

//摩擦轮状态
FricStatus_Enum			         FricStatus;

//运动模式
MotionMode_Enum			         MontionMode;

//底盘打击模式
ChassisMode_Enum   		       Chassis_Heat_Mode;

//自动运动模式
HeatSubschema_Enum           GimbalMotionMode;

//自动云台模式
GimbalControl_Mode_Enum      Gimbal_Control;

//裁判系统信息
Judge_rece				           Judge_DATA;

//遥控器接收信息
DBUSDecoding_Type            DBUS_ReceiveData,LASTDBUS_ReceiveData;

//最小二乘法参数
Least_Square_INFO		         Least_Square_Info;
