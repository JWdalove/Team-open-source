#include "Gimbal_Monitor.h"


extern FRAME_RATE	FrameRate;
extern Cloud_info	Yaw,Pitch;
extern Poked_info	Poked;
extern Chassis_info Chassis;
u8 OutLineNumber = 0;
u8 tone_sequence=0;
u8 tone[10][18]= {
					{0, 0, 0, 0, 0, 0, 0, 0, 0},  //无鸣叫任务
					{1, 0, 0, 0, 0, 0, 0, 0, 0},  //一声
					{1, 0, 1, 0, 0, 0, 0, 0, 0},  //两声
					{1, 0, 1, 0, 1, 0, 0, 0, 0},  //三声
					{1, 0, 1, 0, 1, 0, 1, 0, 0},  //四声
					{1, 1, 1, 1, 1, 1, 1, 1, 1}   //一直鸣叫
};


void Gimbal_Monitor(void *Parameters)
{
	uint16_t SysErrorStatus;
	for(;;)
	{
			FrameRate.DBUSFrameRate 		= DBUSFrameCounter;
			DBUSFrameCounter 				= 0;//30
			FrameRate.MPU9250FrameRate 		= MPU9250FrameCounter;
			MPU9250FrameCounter 			= 0;//80
			FrameRate.PitchFrameRate 		= Pitch.Esc.FrameCounter;
			Pitch.Esc.FrameCounter 			= 0;//203
			FrameRate.YawFrameRate 			= Yaw.Esc.FrameCounter;
			Yaw.Esc.FrameCounter 			= 0;//203
			FrameRate.PokeFrameRate 		= Poked.Esc.FrameCounter;
			Poked.Esc.FrameCounter 			= 0;//203
			FrameRate.CHASSISBOARD_FrameCounter = CHASSISBOARD_FrameCounter;
			CHASSISBOARD_FrameCounter 		= 0;//100
			FrameRate.MiniPCFrameCounter	= MiniPCFrameCounter;
			MiniPCFrameCounter				= 0;
		
		
		OutLineNumber=0;   //掉线数目清零   进行新一轮运算	
			if( FrameRate.DBUSFrameRate  < 10)   			{SysErrorStatus |= 0x0001;}   //DBUS       	0000 0000 0001
        else        		SysErrorStatus &= 0xFFFE;
			if( FrameRate.MPU9250FrameRate < 15)   			{SysErrorStatus |= 0x0002;OutLineNumber++;}   //MPU9250    	0000 0000 0010
        else        		SysErrorStatus &= 0xFFFD;
			if( FrameRate.PitchFrameRate < 60)   			{SysErrorStatus |= 0x0004;OutLineNumber++;}   //Pitch      	0000 0000 0100
        else        		SysErrorStatus &= 0xFFFB;
			if( FrameRate.YawFrameRate < 60)   				{SysErrorStatus |= 0x0008;OutLineNumber++;}   //Yaw        	0000 0000 1000
        else        		SysErrorStatus &= 0xFFF7;
			if( FrameRate.PokeFrameRate < 60)   			{SysErrorStatus |= 0x0010;OutLineNumber++;}   //Poke      	0000 0001 0000 
        else        		SysErrorStatus &= 0xFFEF;
			if( FrameRate.CHASSISBOARD_FrameCounter < 30)   {SysErrorStatus |= 0x0040;OutLineNumber++;}   //CHASSISBOARD  0000 0100 0000
        else        		SysErrorStatus &= 0xFFBF;
		
		
		
		//Beep(OutLineNumber);
		
		vTaskDelay(200);
	}
}
void Beep_Configuration(void)
{
	GPIO_InitTypeDef gpio;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	gpio.GPIO_Pin = GPIO_Pin_2 ;
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOE,&gpio);
	GPIO_ResetBits(GPIOE, GPIO_Pin_2);
}
void Beep(u8 type)
{
	tone_sequence++;
	if(tone_sequence>8)  tone_sequence=0;
	if(tone[type][tone_sequence]==0)     GPIO_ResetBits(GPIOE, GPIO_Pin_2);
	if(tone[type][tone_sequence]==1)     GPIO_SetBits(GPIOE, GPIO_Pin_2);
}
