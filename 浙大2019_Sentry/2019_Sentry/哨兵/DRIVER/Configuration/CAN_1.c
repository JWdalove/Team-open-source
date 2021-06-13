#include "CAN_1.h"

CanRxMsg rx_message1;

int CHASSISBOARD_FrameCounter = 0;
int Edge_Flag=0;

u8 CAN_TX[8];

u8 CAN_RX_BOARD[8];
u8 CAN_RX_Yaw[8];

extern Sentry_info 		SentryInfo;
extern Cloud_info	    Yaw,Pitch;
extern Chassis_info  	Chassis;
extern Poked_info		Poked;
extern Judge_rece		Judge_DATA;
extern Communicate_info	Comm;		//暂时放在这,state_update

// ********** ↓ CAN1初始化配置 ↓ **********//
void CAN_First_InitConfig(void)
{
    CAN_InitTypeDef        can;
    CAN_FilterInitTypeDef  can_filter;
    GPIO_InitTypeDef       gpio;
    NVIC_InitTypeDef       nvic;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_CAN1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_CAN1);

    gpio.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Init(GPIOB, &gpio);

    nvic.NVIC_IRQChannel = CAN1_RX0_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 2;
    nvic.NVIC_IRQChannelSubPriority = 1;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);

    nvic.NVIC_IRQChannel = CAN1_TX_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 1;
    nvic.NVIC_IRQChannelSubPriority = 1;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);

    CAN_DeInit(CAN1);
    CAN_StructInit(&can);

    can.CAN_TTCM = DISABLE;
    can.CAN_ABOM = DISABLE;
    can.CAN_AWUM = DISABLE;
    can.CAN_NART = DISABLE;
    can.CAN_RFLM = DISABLE;
    can.CAN_TXFP = ENABLE;
    can.CAN_Mode = CAN_Mode_Normal;
    can.CAN_SJW  = CAN_SJW_1tq;
    can.CAN_BS1 = CAN_BS1_9tq;
    can.CAN_BS2 = CAN_BS2_4tq;
    can.CAN_Prescaler = 3;   //CAN BaudRate 42/(1+9+4)/3=1Mbps
    CAN_Init(CAN1, &can);

    can_filter.CAN_FilterNumber=0;
    can_filter.CAN_FilterMode=CAN_FilterMode_IdMask;
    can_filter.CAN_FilterScale=CAN_FilterScale_32bit;
    can_filter.CAN_FilterIdHigh=0x0000;
    can_filter.CAN_FilterIdLow=0x0000;
    can_filter.CAN_FilterMaskIdHigh=0x0000;
    can_filter.CAN_FilterMaskIdLow=0x0000;
    can_filter.CAN_FilterFIFOAssignment=0;
    can_filter.CAN_FilterActivation=ENABLE;
    CAN_FilterInit(&can_filter);

    CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);
    CAN_ITConfig(CAN1,CAN_IT_TME,ENABLE);
}
// ********** ↑ CAN1初始化配置 ↑ **********//

// ********** ↓ CAN1云台运动驱动 ↓ **********//
void CloudMotorDriveCan1(int16_t current_205,int16_t current_206,int16_t current_207,int16_t current_208)
{
    static CanSend_Type   SendData;
	
    SendData.CANx = 1;
    SendData.TxMessage.StdId = 0x1FF;
    SendData.TxMessage.IDE = CAN_Id_Standard;
    SendData.TxMessage.RTR = CAN_RTR_Data;
    SendData.TxMessage.DLC = 0x08;

    SendData.TxMessage.Data[0] = (unsigned char)(current_205 >> 8);      //Yaw轴
    SendData.TxMessage.Data[1] = (unsigned char)current_205;
    SendData.TxMessage.Data[2] = (unsigned char)(current_206 >> 8);  
    SendData.TxMessage.Data[3] = (unsigned char)current_206;
    SendData.TxMessage.Data[4] = (unsigned char)(current_207 >> 8);
    SendData.TxMessage.Data[5] = (unsigned char)current_207;
    SendData.TxMessage.Data[6] = (unsigned char)(current_208 >> 8);
    SendData.TxMessage.Data[7] = (unsigned char)current_208;
	
    xQueueSend(Queue_CANSend, &SendData, 10);
}
// ********** ↑ CAN1云台运动驱动 ↑ **********//

// ********** ↓ CAN1底盘运动驱动 ↓ **********//
void ChassisMotorDriveCan1(int16_t current_201,int16_t current_202,int16_t current_203,int16_t current_204)
{
    static CanSend_Type   SendData;
	
    SendData.CANx = 1;
    SendData.TxMessage.StdId = 0x200;
    SendData.TxMessage.IDE = CAN_Id_Standard;
    SendData.TxMessage.RTR = CAN_RTR_Data;
    SendData.TxMessage.DLC = 0x08;

    SendData.TxMessage.Data[0] = (unsigned char)(current_201 >> 8);      
    SendData.TxMessage.Data[1] = (unsigned char)current_201;
    SendData.TxMessage.Data[2] = (unsigned char)(current_202 >> 8);      
    SendData.TxMessage.Data[3] = (unsigned char)current_202;
    SendData.TxMessage.Data[4] = (unsigned char)(current_203 >> 8);
    SendData.TxMessage.Data[5] = (unsigned char)current_203;
    SendData.TxMessage.Data[6] = (unsigned char)(current_204 >> 8);
    SendData.TxMessage.Data[7] = (unsigned char) current_204;

    xQueueSend(Queue_CANSend, &SendData, 10);
}
// ********** ↑ CAN1底盘运动驱动 ↑ **********//

// ********** ↓ 云台发送信息 ↓ **********//
void Gimbal_Board_Communicate(int16_t data1,int16_t data2,int16_t data3,int16_t data4)
{
    static CanSend_Type   SendData;
	
    SendData.CANx = 1;
    SendData.TxMessage.StdId = 0x188;
    SendData.TxMessage.IDE = CAN_Id_Standard;
    SendData.TxMessage.RTR = CAN_RTR_Data;
    SendData.TxMessage.DLC = 0x08;

    SendData.TxMessage.Data[0] = (unsigned char)(data1 >> 8);      //Comm.GSendMotionMode
    SendData.TxMessage.Data[1] = (unsigned char)data1;
    SendData.TxMessage.Data[2] = (unsigned char)(data2 >> 8);      //DBUS_ReceiveData.ch2
    SendData.TxMessage.Data[3] = (unsigned char)data2;
    SendData.TxMessage.Data[4] = (unsigned char)(data3 >> 8);      //Pitch.Imu_Euler
    SendData.TxMessage.Data[5] = (unsigned char)data3;
    SendData.TxMessage.Data[6] = (unsigned char)(data4 >> 8);
    SendData.TxMessage.Data[7] = (unsigned char)data4;
	
    xQueueSend(Queue_CANSend, &SendData, 10);
}
// ********** ↑ 云台发送信息 ↑ **********//

// ********** ↓ CAN1发送中断 ↓ **********//
void CAN1_TX_IRQHandler(void)
{
    if (CAN_GetITStatus(CAN1,CAN_IT_TME)!= RESET)
    {
        CAN_ClearITPendingBit(CAN1,CAN_IT_TME);
    }
}
// ********** ↑ CAN1发送中断 ↑ **********//

// ********** ↓ CAN1接收中断 ↓ **********//
void CAN1_RX0_IRQHandler(void)
{
    if (CAN_GetITStatus(CAN1,CAN_IT_FMP0)!= RESET)
    {
        CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
        CAN_Receive(CAN1, CAN_FIFO0, &rx_message1);
        switch(rx_message1.StdId)
        {
					
        case CHASSISBOARD:      //来底盘主控的信息(0x166)
        {
            CHASSISBOARD_FrameCounter++;      //监测系统
					
            CAN_RX_BOARD[0]=rx_message1.Data[0];
            CAN_RX_BOARD[1]=rx_message1.Data[1];
            CAN_RX_BOARD[2]=rx_message1.Data[2];
            CAN_RX_BOARD[3]=rx_message1.Data[3];
            CAN_RX_BOARD[4]=rx_message1.Data[4];
            CAN_RX_BOARD[5]=rx_message1.Data[5];
            CAN_RX_BOARD[6]=rx_message1.Data[6];
            CAN_RX_BOARD[7]=rx_message1.Data[7];

            Judge_DATA.Shooterheat 	=  CAN_RX_BOARD[0]<<8 | CAN_RX_BOARD[1];      //获得当前枪口热量
            Edge_Flag		            =  CAN_RX_BOARD[2]<<8 | CAN_RX_BOARD[3];      
            Chassis.Angle_Speed     =  CAN_RX_BOARD[4]<<8 | CAN_RX_BOARD[5];      //获得当前底盘速度
            Judge_DATA.Shoot_Speed	=  CAN_RX_BOARD[6]<<8 | CAN_RX_BOARD[7];      //获取当前剩余血量
            break;
        }
				
				case YAWMOTOR:      //Yaw轴电机(0x205)
        {
            Yaw.Esc.FrameCounter++;      //检测系统
					
            CAN_RX_Yaw[0]=rx_message1.Data[0];
            CAN_RX_Yaw[1]=rx_message1.Data[1];
            CAN_RX_Yaw[2]=rx_message1.Data[2];
            CAN_RX_Yaw[3]=rx_message1.Data[3];
            CAN_RX_Yaw[4]=rx_message1.Data[4];
            CAN_RX_Yaw[5]=rx_message1.Data[5];
            CAN_RX_Yaw[6]=rx_message1.Data[6];
            CAN_RX_Yaw[7]=rx_message1.Data[7];

            Yaw.Esc.Machine_Angle_Last = Yaw.Esc.Machine_Angle;          
					
            Yaw.Esc.Machine_Angle =  CAN_RX_Yaw[0]<<8 | CAN_RX_Yaw[1];      //获得Yaw轴机械角度
            Yaw.Esc.Elec_Measure  =  CAN_RX_Yaw[2]<<8 | CAN_RX_Yaw[3];      //获得Yaw轴实际转矩电流测量值
            Yaw.Esc.Elec_Give     =  CAN_RX_Yaw[4]<<8 | CAN_RX_Yaw[5];      //获得Yaw轴转矩电流给定值

            Yaw.Esc.USE_Machine = Yaw.Esc.Machine_Angle;	    //将当前值给USE
					
            if     (Yaw.Esc.Machine_Angle-Yaw.Esc.Machine_Angle_Last>4096)
                Yaw.Esc.Zero_Flag--;  //反方向
            else if(Yaw.Esc.Machine_Angle-Yaw.Esc.Machine_Angle_Last<-4096)
                Yaw.Esc.Zero_Flag++;  //正方向
            Yaw.Esc.USE_Machine += (8191*Yaw.Esc.Zero_Flag);	                 //在USE（未满一圈）+超过一圈

            /*同步带延拓 0- 16400*/
            if(!Yaw.Esc.Init && Yaw.Esc.Machine_Angle!= 0)
            {
                Yaw.Esc.Revenge_Angle = Yaw.Esc.Machine_Angle;
                Yaw.Esc.Last_Angle = Yaw.Esc.Machine_Angle;
                Yaw.Esc.Init = 1;
            }
            Yaw.Esc.Angle_Error = ((int16_t)Yaw.Esc.Machine_Angle - (int16_t)Yaw.Esc.Last_Angle);
            if(Yaw.Esc.Angle_Error == 0) //忽略数值跳变(不懂啥意思)
            {
                Yaw.Esc.Angle_Error = 0;
            }
            else if(Yaw.Esc.Angle_Error > 7000)
            {
                Yaw.Esc.Angle_Error = Yaw.Esc.Angle_Error - 8191;
            }
            else if(Yaw.Esc.Angle_Error < -7000)
            {
                Yaw.Esc.Angle_Error = -8191 - Yaw.Esc.Angle_Error;
            }
            Yaw.Esc.Last_Angle = Yaw.Esc.Machine_Angle;
            Yaw.Esc.Revenge_Angle += Yaw.Esc.Angle_Error;
            if(Yaw.Esc.Revenge_Angle > GIMBAL_MAX_LIMIT)
            {
                Yaw.Esc.Revenge_Angle = 0;
            }
            else if(Yaw.Esc.Revenge_Angle < 0)
            {
                Yaw.Esc.Revenge_Angle = GIMBAL_MAX_LIMIT;
            }
            Yaw.Esc.Angle_Error = 0;
            break;
        }
				
        }
    }
}
// ********** ↑ CAN1接收中断 ↑ **********//
