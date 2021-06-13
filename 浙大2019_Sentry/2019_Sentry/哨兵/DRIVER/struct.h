#ifndef __STRUCT_H__
#define __STRUCT_H__
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "arm_math.h"
#include "constant.h"

/*
*********************************************************************************************************
*                                           结构体变量声明
*********************************************************************************************************
*/

/*******************************  Task_CANSend  **********************************/
typedef struct
{
    uint8_t     CANx;            //CAN编号     1 CAN1      2 CAN2
    CanTxMsg    TxMessage;       //发送数据
} CanSend_Type;
/****************************************   ↓  电调反馈信息 ↓  ***********************************/
typedef struct
{
    u8  FrameCounter;			  //数据帧监控
    u16 Machine_Angle; 			//机械角度
    u16	Machine_Angle_Last;	//上一次机械角度
    int16_t Angle_Speed;		//转速 注意是无符号短整型！！！
    int USE_Machine;			  //使用的机械信息
    int Zero_Flag;				  //过零点标志位
    
} Esc820_info;              //底盘电调

typedef struct
{
    u8  FrameCounter;			  //数据帧监控
    u16 Machine_Angle; 			//机械角度
    u16	Machine_Angle_Last;	//上一次机械角度
    int16_t Revenge_Angle;
    int16_t Elec_Measure;		//转矩电流测量值
    int16_t Elec_Give;			//转矩电流给定值
	int16_t Rotate_Speed;
    int Relative_Angle;
    int USE_Machine;			  //使用的机械信息
    int Zero_Flag;				  //过零点标志位
    int Angle_Error;
    int16_t Init_Machine_Angle; //上电初始角度
    u8  Init;
    u16 Last_Angle;
} Esc6623_info; //云台电调
/***************************************   ↑  电调反馈信息 ↑   ***********************************/
/****************************************   ↓  PID参量 ↓  ***********************************/

typedef struct
{
    float P;
    float I;
    float D;
    float feed_param;     //前馈系数
    float CurrentError;
    float LastError;
    float PIDOut;
    float Integral;
    portTickType LastTick;
} PID_Type;

/***************************************    ↑  PID参量 ↑   ***********************************/
/****************************************   ↓  底盘   ↓  ***********************************/
//底盘单电机参数
typedef struct
{
    Esc820_info Esc;			      //底盘电调信息反馈
    int TargetSpeed;            //电机目标速度
    int Out;					          //底盘电机输出
    int Current_Error;          //限制电流
} OneChassisParam_info;

//底盘 chassis
typedef struct
{
    OneChassisParam_info F;					//前
    OneChassisParam_info B;					//后

	  int16_t Angle_Speed;
	  int   initial_position;
    float LastV;									  //上一次速度
    float TargetV;									//期望速度   前后
    float TargetVfliter;						//平滑后的滤波值
    float TargetOmega;							//期望旋转速度	  旋转
	  float PowerLimit;								//输出限制
} Chassis_info;



/***************************************    ↑  底盘   ↑   ***********************************/
/****************************************   ↓  云台   ↓  ***********************************/
typedef struct
{
    float Give; 				   	//给定值
    int   Out;					   	//输出值
	  float Plus;						  //弹道补偿
    float Imu_Euler;		   	//陀螺仪Euler
    float Imu_Euler_Fit;   	//校正Euler
    float rotate_coe; 	   	//旋转系数
    u8    init_complete;	 	//初始化完成标志位
    float 		Machine_Angle_Max;    		//机械角度限幅
    float 		Machine_Angle_Min; 	   		//机械角度限幅
	  uint16_t 	Give_Max;    	 	//给定值限幅
    uint16_t 	Give_Min; 	    //给定值限幅
    Esc6623_info Esc;
    float		EnemyDataBuffer[50];//存取过去100次数据

} Cloud_info;



/***************************************    ↑  云台   ↑   ***********************************/
/****************************************   ↓  拨弹   ↓  ***********************************/

//拨弹 poked
typedef struct
{
	int Turn_Angle;
  int Out;              //输出值
  int Num_Bullet;       //子弹数量
  int Num_Bullet_Last;
  Esc820_info Esc;
  int16_t Preview_n;

} Poked_info;
/***************************************    ↑  拨弹   ↑   ***********************************/
/****************************************   ↓  哨兵传感信息 ↓  ***********************************/
typedef struct
{
  int mil;						   //哨兵位置
  int last_mil;          //哨兵上次位置
  int dir;						   //哨兵运动方向
  int last_dir;				   //哨兵上一次运动方向 用于底盘速度的平滑过渡
  u8  infrared;				   //红外开关状态
	u8  infrared_last;
	float Yaw_Error;
	float Ultrasonic;		   //超声波1
	float Ultrasonic2;	   //超声波2
  int16_t Shoot_Freq;
  int16_t Shoot_Dis;
  float targetspeedvx;   //云台Yaw轴想要的速度  
  float targetspeedvy;   //云台Pitch轴想要的速度
  float targetspeedvx_last;
  float targetspeedvy_last;
  float motion_kalman[4];
  enum  Pos {Behind_Edge,Behind,Behind_Curve,Rail_Mid,Front_Curve,Front,Front_Edge,} Position,LastPosition;						//哨兵底盘定位
  enum  Origin_D {Rail_MidBehind,Rail_MidFront, }	Origin;																															//哨兵起始原点
  enum  Rail_Direction {B_TO_F,F_TO_B,} Rail_Dir;																																			//哨兵方向
	float Chassis_Speed;
} Sentry_info;

/***************************************    ↑  哨兵传感信息 ↑   ***********************************/
/****************************************   ↓  裁判系统信息 ↓  ************************************/
typedef struct
{
	int Remain_HP;				  //剩余血量
	int Last_HP;				    //上次HP;
	int Shoot_Freq;				  //射频
	float Shoot_Speed;			//射速
	float Volt;
	float Current;
	float Power;        		//底盘实时功率
	float Power_Buffer;			//能量槽
	float Shooterheat;			//枪口热量
	u8    Process;				  //比赛进程
	u8	  Armor_ID;				  //装甲板
}Judge_rece;

/***************************************    ↑  裁判系统信息 ↑   ***********************************/
/****************************************   ↓  哨兵通信信息 ↓  ************************************/
typedef struct
{
    int16_t CReceiveSpeedData;				//底盘接收速度数据
    int16_t	CRecriveMotionMode;				//底盘接收移动模式
	  /*
    receive data :3  chassis rc        //遥控器
    						 1/4 chsssis patrol    //巡逻
                  2  chassis relax     //释放
	                5  chassis init      //初始化
    */
	  int16_t Pitchlmul;
    int16_t	GSendMotionMode;				  //云台传递底盘运动模式
	  /*
	  send data :1 
	             2 安全
	             3 遥控器
	             4 
	             5 复位
	  */
    int16_t GSendSpeedData; 				//云台发送底盘速度

} Communicate_info;
/***************************************    ↑  哨兵通信信息 ↑   ***********************************/
/****************************************   ↓  帧率监测信息 ↓  ************************************/
typedef struct
{
	u8  DBUSFrameRate;
	u8  PitchFrameRate;
	u8  YawFrameRate;
	u8  PokeFrameRate;
	u8  F_ChassisFrameRate;
	u8  B_ChassisFrameRate;
	u8  GIMBALBOARD_FrameCounter;	
	u8  CHASSISBOARD_FrameCounter;
	u8  JudgeFrameRate;
	int MiniPCFrameCounter;
	int MPU9250FrameRate;
}FRAME_RATE;

typedef struct
{
	u8 Enemy_Flag[100];
	u8 Enemy_Count;
	u8 Motion_Count;
	float MiniPC_Msg[6];
	float Angle_error[30];
	float Correction_Yaw;      //Yaw修正
	float Correction_Pitch;    //Pitch修正
	float Motion[100];
	float EnemyError;
} imagepro_info;
/****************************************   ↓  最小二乘法系数 ↓  ************************************/
typedef struct
{
	u8 A,B,C; 
} Least_Square_INFO;

/****************************************   ↑  最小二乘法系数 ↑   ***********************************/


/******************************DBUS相关结构体***************************************/
typedef struct
{
    int16_t ch0;
    int16_t ch1;
    int16_t ch2;
    int16_t ch3;
    uint8_t switch_left;            /*   1    */
    uint8_t switch_left_last;       /*   3    */
    uint8_t switch_right;		      	/*   2    */
    uint8_t switch_right_last;


    struct
    {
        int16_t x;
        int16_t y;
        int16_t z;
        uint8_t press_left;
        uint8_t press_right;

        uint8_t press_left_hop;//鼠标左键跳变
        uint8_t press_right_hop;//鼠标右键跳变
    } mouse;

    struct
    {
        uint16_t key;
        uint16_t key_hop; //跳变键值
    } keyBoard;
} DBUSDecoding_Type;

/*************************************************************************/






/*
*********************************************************************************************************
*                                            状态变换
*********************************************************************************************************
*/
//Friction wheel control mode.
typedef enum
{
    FricStatus_Stop = 0,
    FricStatus_Working = 1,
} FricStatus_Enum;

//the base control mode
typedef enum
{
    Security_Control_Mode,          //保护模式，全部停止
    Auto_Control_Mode,			      	//全自动模式
    RC_Contral_Mode,                //遥控器控制
} ControlMode_Enum;

//the Auto mode subschema
typedef enum
{
    Auto_Normal_Mode,								//正常模式
    Arm_Revenge_Mode,								//装甲

} HeatSubschema_Enum;

//the control mode of chaiss motor
typedef enum
{
	  MONTION_INIT,									//底盘复位
    MONTION_RELAX,							  //底盘释放
    MONTION_RC,									  //底盘遥控
    MONTION_PATROL,							  //fight模式处理
    Following_Mode,
} MotionMode_Enum;

typedef enum
{
	  Normal_Mode,						//溜达
	  Escape_Mode,						//躲避  
	  Ordinary_Heat_Mode,		  //正常打击
	  Amazing_Mode,						//跑！
} ChassisMode_Enum;

//the set mode of gimbal
//typedef enum
//{
//    Low_Circle_Mode,								//低速旋转模式
//    SemiCircle_Mode,								//半圆遍历
//    BaseDefend_Mode,								//回守基地
//    StaticShoot_Mode,								//静止射击
//} CloudMode_Enum;

//the control mode of gimbal
typedef enum
{
    GIMBAL_RELAX        = 0,				//云台控制释放
    GIMBAL_INIT         = 1,				//云台初始化
    GIMBAL_FLOOW        = 2,				//云台浮动
    GIMBAL_HEAT       	= 3,				//云台打击
    GIMABL_RC    		    = 4,				//遥控器控制
} GimbalControl_Mode_Enum;

//位置
typedef enum
{
	  Front_edge,
	  Front_road,
	  Front_curve,
	  Mid_road,
	  Behind_curve,
	  Behind_road,
	  Behind_edge,
} POSITION;

#endif
