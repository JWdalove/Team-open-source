#include "uart_mpu9250.h"
/***************************************
  * @brief  :USART3
  * @param  :
  * @retval :
****************************************/
 

int UARTtemp3;
int dma_cnt=0;
int Euler_Yaw_Flag=0,Euler_Pitch_Flag=0;
int MPU9250FrameCounter=0;
int Pitch_Speed[30];
u8 IMU_CRC[35] = {0};
u8 USART_RX_BUF3[52]= {0};

float  Euler_Pitch,Euler_Yaw;
float  Euler_Yaw_Last = 180.0,Euler_Pitch_Last =180.0;
float  speed_Euler_Pitch,speed_Euler_Yaw=0.0;
float  speed_Euler_Yaw_last,speed_Euler_Pitch_last = 0.0;
extern Cloud_info		Yaw,Pitch;

uint8_t data_length;
uint16_t CRCReceived = 0;            /* CRC value received from a frame */
uint16_t CRCCalculated = 0;          /* CRC value caluated from a frame */
uint8_t crc_header[4] = {0x5A, 0xA5, 0x00, 0x00};
static void crc16_update(uint16_t *currectCrc, const uint8_t *src, uint32_t lengthInBytes)
{
    uint32_t crc = *currectCrc;
    uint32_t j;
    for (j=0; j < lengthInBytes; ++j)
    {
        uint32_t i;
        uint32_t byte = src[j];
        crc ^= byte << 8;
        for (i = 0; i < 8; ++i)
        {
            uint32_t temp = crc << 1;
            if (crc & 0x8000)
            {
                temp ^= 0x1021;
            }
            crc = temp;
        }
    }
    *currectCrc = crc;
}
void USART_mpu9250_InitConfig(void)
{

    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef  GPIO_MPU9250;
    NVIC_InitTypeDef  NVIC_InitStructure;

    DMA_InitTypeDef     DMA_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);

    GPIO_MPU9250.GPIO_Mode  =   GPIO_Mode_AF;
    GPIO_MPU9250.GPIO_OType  =   GPIO_OType_PP;
    GPIO_MPU9250.GPIO_Pin   =   GPIO_Pin_8| GPIO_Pin_9;
    GPIO_MPU9250.GPIO_PuPd  =   GPIO_PuPd_UP;
    GPIO_MPU9250.GPIO_Speed  =   GPIO_Speed_100MHz;
    GPIO_Init(GPIOD, &GPIO_MPU9250);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);

    USART_InitStructure.USART_BaudRate              =   921600;// ;961200 1khz 115200 500hz
    USART_InitStructure.USART_HardwareFlowControl   =   USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                  =   USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_Parity                =   USART_Parity_No;
    USART_InitStructure.USART_StopBits              =   USART_StopBits_1;
    USART_InitStructure.USART_WordLength            =   USART_WordLength_8b;
    USART_Init(USART3, &USART_InitStructure);

    USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);
    USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);

    USART_Cmd(USART3, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel						=	USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd					=	ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	=	2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority			=	0;
    NVIC_Init(&NVIC_InitStructure);
    USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);

    DMA_InitStructure.DMA_Channel           =   DMA_Channel_4;
    DMA_InitStructure.DMA_PeripheralBaseAddr=   (uint32_t)(&USART3->DR);
    DMA_InitStructure.DMA_Memory0BaseAddr   =   (uint32_t)(USART_RX_BUF3);
    DMA_InitStructure.DMA_DIR               =   DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize        =   41;
    DMA_InitStructure.DMA_PeripheralInc     =   DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc         =   DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_MemoryDataSize    =   DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_PeripheralDataSize=   DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_Mode              =   DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority          =   DMA_Priority_Medium;
    DMA_InitStructure.DMA_FIFOMode          =   DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold     =   DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst       =   DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst   =   DMA_PeripheralBurst_Single;
    DMA_Init(DMA1_Stream1, &DMA_InitStructure);
    DMA_ITConfig(DMA1_Stream1,DMA_IT_TC, ENABLE);
    DMA_Cmd(DMA1_Stream1, ENABLE);

}

void USART3_IRQHandler(void)
{
    UARTtemp3 = USART3->DR;
    UARTtemp3 = USART3->SR;

    DMA_Cmd(DMA1_Stream1, DISABLE);
    MPU9250FrameCounter++;
    dma_cnt++;
    if(dma_cnt>18) dma_cnt=18;
    crc_header[2] = *(USART_RX_BUF3 + 2);
    crc_header[3] = *(USART_RX_BUF3 + 3);

    CRCReceived = (USART_RX_BUF3[4]|(USART_RX_BUF3[5]<<8));

    data_length = (USART_RX_BUF3[2]|(USART_RX_BUF3[3]<<8));
    memcpy(IMU_CRC, USART_RX_BUF3 + 6,data_length);
    /* calculate CRC */
    CRCCalculated = 0;
    crc16_update(&CRCCalculated, crc_header, 4);
    crc16_update(&CRCCalculated, IMU_CRC, data_length);
    /*   CRC match   */
    if(CRCCalculated == CRCReceived)
    {
        speed_Euler_Pitch	= 	((float)(int16_t)(USART_RX_BUF3[16] | (USART_RX_BUF3[17]<<8)));
        speed_Euler_Yaw		= 	((float)(int16_t)(USART_RX_BUF3[20] | (USART_RX_BUF3[21]<<8)));
        Euler_Pitch			  = 	((float)(int16_t)(USART_RX_BUF3[32] | (USART_RX_BUF3[33]<<8)))/100;     //俯仰角   -90 ~ 90
        Euler_Yaw			    = 	((float)(int16_t)(USART_RX_BUF3[34] | (USART_RX_BUF3[35]<<8)))/10;      //航向角    0~-360

        for(u8 k = 29; k > 0; k--)
        {
            Pitch_Speed[k] = Pitch_Speed[k-1];
        }
        Pitch_Speed[0] = speed_Euler_Pitch;

        Yaw.Imu_Euler 	= Euler_Yaw;
        Pitch.Imu_Euler = Euler_Pitch;
        if(Euler_Yaw-Euler_Yaw_Last>180) Euler_Yaw_Flag++;           //180使用裕度最大    0~360
        else if(Euler_Yaw-Euler_Yaw_Last<-180) Euler_Yaw_Flag--;
        if(Euler_Pitch-Euler_Pitch_Last>180)Euler_Pitch_Flag++;
        else if(Euler_Pitch-Euler_Pitch_Last<-180)Euler_Pitch_Flag--;
        if(dma_cnt<12) Euler_Yaw_Flag=0,Euler_Pitch_Flag=0;

        Yaw.Imu_Euler -= (360.0f*Euler_Yaw_Flag);
        Pitch.Imu_Euler -=(360.0f*Euler_Pitch_Flag);
        Euler_Yaw_Last=Euler_Yaw;
        Euler_Pitch_Last=Euler_Pitch;
    }
    //重启DMA
    DMA_ClearFlag(DMA1_Stream1, DMA_FLAG_TCIF1 | DMA_FLAG_HTIF1);
    while(DMA_GetCmdStatus(DMA1_Stream1) != DISABLE);
    DMA_SetCurrDataCounter(DMA1_Stream1, 41);
    DMA_Cmd(DMA1_Stream1, ENABLE);
}
