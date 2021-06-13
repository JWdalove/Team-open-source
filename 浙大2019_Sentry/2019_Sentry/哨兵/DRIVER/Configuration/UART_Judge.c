
#include "uart_judge.h"

extern Judge_rece	Judge_DATA;
/* dma double buffer 双缓冲*/
uint8_t judge_dma_rxbuff[2][UART_RX_DMA_SIZE];

/*
  @UART4_TX PC10
  @UART4_RX PC11
*/
extern  int8_t  Rx_data1;
//uint8_t JudgeDataBuffer[150];
void UART4_Judge_InitConfig(void)
{
    GPIO_InitTypeDef  gpio;
    USART_InitTypeDef uart4;
    NVIC_InitTypeDef  nvic;
    DMA_InitTypeDef     DMA_InitStructure;
    //时钟初始化
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
//    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

    GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_UART4);
    GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_UART4);
    //GPIO初始化
    gpio.GPIO_Pin = GPIO_Pin_10;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_Speed = GPIO_Speed_100MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC,&gpio);

    gpio.GPIO_Pin = GPIO_Pin_11;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_OType = GPIO_OType_OD;
    gpio.GPIO_Speed = GPIO_Speed_100MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOC,&gpio);

    //UARST初始化
    uart4.USART_BaudRate = 115200;
    uart4.USART_WordLength = USART_WordLength_8b;
    uart4.USART_StopBits = USART_StopBits_1;
    uart4.USART_Parity = USART_Parity_No;
    uart4.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
    uart4.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(UART4,&uart4);

    USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);//使能串口空闲中断	IDLE
    USART_Cmd(UART4,ENABLE);

    USART_DMACmd(UART4, USART_DMAReq_Rx, ENABLE);
    USART_DMACmd(UART4, USART_DMAReq_Tx, ENABLE);
    //DMA  RX
    DMA_InitStructure.DMA_Channel           =   DMA_Channel_4;
    DMA_InitStructure.DMA_PeripheralBaseAddr=   (uint32_t)(&UART4->DR);
    DMA_InitStructure.DMA_Memory0BaseAddr   =   (uint32_t)judge_dma_rxbuff[0];
    DMA_InitStructure.DMA_DIR               =   DMA_DIR_PeripheralToMemory; //RX
    DMA_InitStructure.DMA_BufferSize        =   150; //1024
    DMA_InitStructure.DMA_PeripheralInc     =   DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc         =   DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_MemoryDataSize    =   DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_PeripheralDataSize=   DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_Mode              =   DMA_Mode_Circular;   //double buffer
    DMA_InitStructure.DMA_Priority          =   DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode          =   DMA_FIFOMode_Disable; //
    DMA_InitStructure.DMA_FIFOThreshold     =   DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst       =   DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst   =   DMA_PeripheralBurst_Single;
    /*DMA double buffer mode*///双缓冲区模式  2018/2/24
    DMA_DoubleBufferModeConfig(DMA1_Stream2,(uint32_t)judge_dma_rxbuff[0], DMA_Memory_0);	//DMA_Memory_0首先被传输
    DMA_DoubleBufferModeCmd(DMA1_Stream2, ENABLE);
    DMA_Init(DMA1_Stream2, &DMA_InitStructure);
    DMA_Cmd(DMA1_Stream2, ENABLE);
    //IRQ
    nvic.NVIC_IRQChannel = UART4_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 10;
    nvic.NVIC_IRQChannelSubPriority = 0;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);

    //开启DMA满中断 2018/2/24
    nvic.NVIC_IRQChannel = DMA1_Stream2_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 11;
    nvic.NVIC_IRQChannelSubPriority = 0;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
    /*DMA 中断开*/
    DMA_ITConfig(DMA1_Stream2,DMA_IT_TC,ENABLE);	
}


/***************************************
  * @brief  :裁判系统数据更新
  * @param  :
****************************************/
void Judge_data_pass()
{
	Judge_DATA.Last_HP		=	Judge_DATA.Remain_HP;
	
	Judge_DATA.Remain_HP 	=	judge_rece_mesg.game_information.remain_HP;
    Judge_DATA.Shoot_Freq  	=	judge_rece_mesg.real_shoot_data.bullet_freq;
    Judge_DATA.Shoot_Speed 	=	judge_rece_mesg.real_shoot_data.bullet_speed;
    Judge_DATA.Volt        	=	judge_rece_mesg.power_heat.chassis_volt;
    Judge_DATA.Current     	=	judge_rece_mesg.power_heat.chassis_current;
    Judge_DATA.Power       	=	judge_rece_mesg.power_heat.chassis_power;
    Judge_DATA.Power_Buffer	=	judge_rece_mesg.power_heat.chassis_power_buffer;
    Judge_DATA.Shooterheat 	=	judge_rece_mesg.power_heat.shooter_heat0;
//	Judge_DATA.Process		=	judge_rece_mesg.game_state.game_progress;      //0-未开始比赛	1-准备阶段	2-自检阶段	3-5秒倒计时	4-对战中	5-比赛结束
	Judge_DATA.Armor_ID		=	judge_rece_mesg.blood_changed_data.armor_id;
}


void USART4_SendChar(unsigned char b)
{
    USART_SendData(UART4,b);
	while(USART_GetFlagStatus(UART4,USART_FLAG_TC)!=SET);
}
void Uart4_SendBuff ( uint8_t *buff, uint32_t len)
{
    while(len--)
    {
        USART4_SendChar(*buff);
        buff++;
    }
}
/***************************************
  * @brief  :交互数据发送
  * @param  :
****************************************/
void Send_FrameData(tCmdID cmdid, uint8_t * pchMessage,uint8_t dwLength)
{
    uint8_t i;
    uint8_t *addr;
    static tFrame Frame = {.FrameHeader.SOF = 0xA5};//SOF帧起始字节 0XA5
    if (dwLength != sizeof(tSelfDefineInfo))    //校验所发数据长度

        return ;

    addr = (uint8_t *)&Frame.Data.GameInfo; //首地址为Date中的第一个

    Frame.CmdID = cmdid; //命令码
    Frame.FrameHeader.DataLength = dwLength;//帧内date长度 12
    Frame.FrameHeader.Seq++; //包序号 即第几次发送的包
    for(i = 0; i < dwLength; i++)
    {
        *addr++ = *pchMessage++;//数据拷贝
    }
    append_crc8_check_sum((unsigned char *)&Frame.FrameHeader,sizeof(Frame.FrameHeader));//帧头CRC校验
    i = sizeof(Frame.FrameHeader) + sizeof(Frame.CmdID)  + sizeof(Frame.CRC16) + dwLength;//帧长度
    append_crc16_check_sum((unsigned char *)&Frame,i); //帧尾CRC校验
    Uart4_SendBuff((uint8_t *)&Frame,i);               //帧发送
}

/**
  * @brief  裁判系统空闲中断(UART4)
  * @param  void
  * @retval void
  * @note
在串口空闲中断时，读取 DMA 缓冲中的所有数据并储存，然后重启 DMA，让 DMA 重新开始接收数据；
在 DMA 缓冲满中断时，读取 DMA 缓冲中的所有数据并储存，然后让 DMA 重新开始接收数据。在这里可以使用 DMA 双缓冲，既提高效率，也不用使用程序来复位 DMA 接收缓冲；
使用空闲中断处理数据的时候需要注意，要记录此时 DMA 缓冲中的数据位数，然后作为缓冲的读写指针，下一次 DMA 满中断或者空闲中断时从这个位置开始读取。
  */

uint8_t UART4temp; //用于清空串口标志位的临时变量
void UART4_IRQHandler(void)
{

    UART4temp = UART4->DR;
    UART4temp = UART4->SR;
    osSignalSet(judge_unpack_task_t, JUDGE_UART_IDLE_SIGNAL);  //开启任务通知
}


//DMA满中断
void DMA1_Stream2_IRQHandler(void)
{

    if(DMA_GetITStatus(DMA1_Stream2,DMA_IT_TCIF2) != RESET) //中断判断与数据流有关
    {
        if(DMA_GetCurrentMemoryTarget(DMA1_Stream2) == 1)		//获取当前DMA的memory
        {
            DMA_MemoryTargetConfig(DMA1_Stream2,(uint32_t)judge_dma_rxbuff[0],DMA_Memory_0);
        }
        else
        {
            DMA_MemoryTargetConfig(DMA1_Stream2,(uint32_t)judge_dma_rxbuff[1],DMA_Memory_1);
        }
        osSignalSet(judge_unpack_task_t, JUDGE_DMA_FULL_SIGNAL);
        DMA_ClearITPendingBit(DMA1_Stream2,DMA_IT_TCIF2);
    }
    else if(DMA_GetITStatus(DMA1_Stream2,DMA_IT_DMEIF2) != RESET)
    {

    }
}

