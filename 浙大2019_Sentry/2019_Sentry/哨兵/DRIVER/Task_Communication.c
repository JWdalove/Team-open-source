#include "Task_Communication.h"

extern Judge_rece		Judge_DATA;
extern imagepro_info 	MiniPC;
extern u8 front_hurt,edge_hurt;
u8 id_conter=0x1;
tSelfDefineInfo	robot_Communicate;
/*
#define Hero_Red			0x01
#define Engineering_Red		0x02
#define Infantry_Red_1		0x03
#define Infantry_Red_2		0x04
#define Infantry_Red_3		0x05
#define Air_Red				0x06
#define Sentry_Red			0x07
#define Hero_Blue			0x11
#define Engineering_Blue	0x12
#define Infantry_Blue_1		0x13
#define Infantry_Blue_2		0x14
#define Infantry_Blue_3		0x15
#define Air_Blue			0x16
#define Sentry_Blue			0x17
*/
int hfg;
void Task_Communication(void *pvParameters)
{
    portTickType CurrentControlTick = 0;
    for(;;)
    {
        CurrentControlTick = xTaskGetTickCount();
		id_conter++;
		if(id_conter>5)id_conter=0x1;
    /*****************学生机器人间通信:无人机-哨兵*********************/
		robot_Communicate.data_cmd_id = 0x210;
		robot_Communicate.send_ID = 0x07;
		robot_Communicate.receiver_ID = id_conter;
		if(front_hurt==1 || edge_hurt==1 || MiniPC.MiniPC_Msg[4]==1)
		{
			robot_Communicate.data1 = 1;
		}
		else robot_Communicate.data1 = 0;
		robot_Communicate.data2 = 0;
		robot_Communicate.data3 = 0;

		Send_FrameData(SelfDefinedData,(uint8_t *)&robot_Communicate,sizeof(robot_Communicate));
        vTaskDelayUntil(&CurrentControlTick, 110/ portTICK_RATE_MS);
    }
}




