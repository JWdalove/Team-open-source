#include "Chassis_Task.h"
ramp_t   fb_ramp = RAMP_GEN_DAFAULT;
uint8_t  JumpToPatrolFlag;

extern GimbalControl_Mode_Enum 	Gimbal_Control;
extern MotionMode_Enum	MontionMode;
extern Sentry_info 		SentryInfo;
extern Chassis_info  	Chassis;
extern Poked_info		Poked;
extern POSITION			Position;
extern Judge_rece		Judge_DATA;
extern Communicate_info Comm;
extern ChassisMode_Enum   Chassis_Heat_Mode;
extern u8 front_hurt,edge_hurt;
int Chassiscount=1,ChassisCounter=1;				//计数
int hurt_Count=0;
int Edge_position=0;
u8 JumpToNormal = 1,								//状态跳转标志位 开始置一为了让其进入任一种模式
   JumpToOHeat = 1,
   JumpToEscape = 1,
   JumpToAmazing = 1;	
u8 turn_one = 1,
   turn_two = 1,
   turn_three = 1,
   mil_flag = 1;
char string;

// ********** ↓ 底盘任务 ↓ **********//
void Chassis_Task(void *Parameters)
{
    portTickType CurrentControlTick = 0;
    for(;;)
    {
	  CAN2->MSR=0;
      CurrentControlTick = xTaskGetTickCount();   //读取自开机以来当前系统时间
			
	// ********** ↓ 裁判系统更新 ↓ **********//
		Chassiscount++;
    // ********** ↑ 裁判系统更新 ↑ **********//
			
		Judge_data_pass();
		
	// ********** ↓ 控制状态更新 ↓ **********//
        Control_Status_Update(CurrentControlTick);
	// ********** ↑ 控制状态更新 ↑ **********//
			
    // ********** ↓ 剩余热量计算 ↓ **********//
		Power_Calculate();
		ShootPower_Calculate();
    // ********** ↑ 剩余热量计算 ↑ **********//
			
	// ********** ↓ 底盘控制模式 ↓ **********//	
		if(Chassis.F.Esc.Zero_Flag<=-75 && Chassis.B.Esc.Zero_Flag>= 75)Edge_position= 1;
		else if(Chassis.F.Esc.Zero_Flag>= 95 && Chassis.B.Esc.Zero_Flag<=-95)Edge_position=-1;
        else Edge_position=0;		
		
        switch(MontionMode)
        {
			//底盘复位
		    case MONTION_INIT:{
			       __set_FAULTMASK(1);
			       NVIC_SystemReset();}
			//底盘释放
			case MONTION_RELAX:{
                   Chassis.F.Out = 0;
                   Chassis.B.Out = 0;
            break;}
			//RC遥控
            case MONTION_RC:{
                   Chassis_RC();												    	//遥控器模式下底盘速度设定
                   Chassis_MoveCalculate();										//哨兵速度解算
                   Chassis_Contral();												  //哨兵速度闭环控制
            break;}
			//底盘巡逻
            case MONTION_PATROL:{
			       Chassis_Heat_Mode_Update();
                   Chassis_Patrolling();   										//自动巡逻模式下底盘速度设定
                   Chassis_MoveCalculate();										//哨兵速度解算
		           Chassis_Contral();										  		//哨兵速度闭环控制
            break;}
            default:
                 MontionMode = MONTION_RELAX;                   					//无信号  认为 底盘释放//
            break;
        }
	// ********** ↑ 底盘控制模式 ↑ **********//
				
	// ********** ↓ 输出通信 ↓ **********//
		ChassisMotorDriveCan2(Chassis.F.Out,Chassis.B.Out,0,0);
		Chassis_Board_Communicate(Judge_DATA.Shooterheat,Edge_position,(int)(Chassis.F.Esc.Angle_Speed/100-Chassis.B.Esc.Angle_Speed/100),Judge_DATA.Shoot_Speed);
	// ********** ↑ 输出通信 ↑ **********//    
				
		vTaskDelayUntil(&CurrentControlTick, 2/ portTICK_RATE_MS);  		//2ms周期  绝对延时  释放CPU占用权
    }
}
// ********** ↑ 底盘任务 ↑ **********//

/***************************************
  * @brief  :控制模式
  * @param  :RC,Patrolling
****************************************/
void Chassis_RC(void)
{
	JumpToNormal = 1;
	JumpToOHeat = 1;
	JumpToEscape = 1;
	Chassis_SpeedSet(15.0f*Comm.CReceiveSpeedData);
}

void Chassis_Patrolling(void)
{
	//6500刚好不消耗能量
    /*********************  ↓  哨兵运动方向设置 ↓ ***********************/
	switch(Chassis_Heat_Mode)
	{
		case Normal_Mode:                //正常巡逻
		{
			if(Chassis.initial_position==0)
			{
				Chassis.F.Esc.Zero_Flag=0;
				Chassis.B.Esc.Zero_Flag=0;
				Chassis.initial_position++;
			}
			if(Chassis.initial_position==1)
			{
			    SentryInfo.Chassis_Speed = 6000.0f;
			    SentryInfo.dir=1;
			    ramp_init(&fb_ramp,400);
			}
			if(Chassis.F.Esc.Zero_Flag<=-80 && Chassis.B.Esc.Zero_Flag>= 80)
		    {
				ramp_init(&fb_ramp,400);
				Chassis.initial_position++;
                SentryInfo.dir = -1;
		    }
		    if(Chassis.F.Esc.Zero_Flag>= 100 && Chassis.B.Esc.Zero_Flag<=-100)
		    {
				ramp_init(&fb_ramp,400);
				Chassis.initial_position++;
				SentryInfo.dir =  1;   
		    }	
			break;
		}
		
		case Escape_Mode:                //躲避碉堡
		{

			break;
		}
		
		case Ordinary_Heat_Mode:         //躲避无人机or正常打
		{

			break;
		}		
		
		case Amazing_Mode:
		{

			break;
		}
		
	}
    /*********************  ↑  哨兵运动方向设置  ↑  ***********************/
	
    /*********************  ↓  哨兵速度设置 ↓ ***********************/
	Chassis_SpeedSet(SentryInfo.dir * SentryInfo.Chassis_Speed);                //哨兵向左方走 dir=-1 哨兵向右方走  dir=1
    /*********************  ↑  哨兵速度设置  ↑  ***********************/
}
