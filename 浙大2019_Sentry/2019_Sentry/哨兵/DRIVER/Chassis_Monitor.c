#include "Chassis_Monitor.h"

extern FRAME_RATE	FrameRate;
extern Chassis_info Chassis;
u8 OutlineNumber = 0;
void Chassis_Monitor(void *Parameters)
{
	uint16_t SysErrorStatus;
	for(;;)
	{
			FrameRate.GIMBALBOARD_FrameCounter = GIMBALBOARD_FrameCounter;
			GIMBALBOARD_FrameCounter 		= 0;//100
			FrameRate.F_ChassisFrameRate 	= Chassis.F.Esc.FrameCounter;
			Chassis.F.Esc.FrameCounter 		= 0;//203
			FrameRate.B_ChassisFrameRate 	= Chassis.B.Esc.FrameCounter;
			Chassis.B.Esc.FrameCounter 		= 0;//203
			FrameRate.JudgeFrameRate		= JudgeFrameCount;
			JudgeFrameCount					= 0;
			
			//≤√≈–œµÕ≥
		
		OutlineNumber = 0;
			if( FrameRate.GIMBALBOARD_FrameCounter < 60)   	{SysErrorStatus |= 0x0080;OutlineNumber++;}    //GIMBALBOARD  0000 1000 0000
        else                      SysErrorStatus &= 0xFF7F;
			if( FrameRate.F_ChassisFrameRate < 60)   		{SysErrorStatus |= 0x0100;OutlineNumber++;}    //F_Chassis  	0001 0000 0000
        else                      SysErrorStatus &= 0xFEFF;
			if( FrameRate.B_ChassisFrameRate < 60)   		{SysErrorStatus |= 0x0200;OutlineNumber++;}    //B_Chassis  	0010 0000 0000
        else                      SysErrorStatus &= 0xFDFF;
		
		
		Beep(OutlineNumber);
		
		vTaskDelay(200);
	}
}
