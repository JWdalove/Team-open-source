#include "uart_DBUS.h"

uint8_t DBUSFrameCounter;	//DBUS帧计数
u8   DBUS_rx_buffer[25];    //遥控器原始数据
int  UARTtemp5;
extern DBUSDecoding_Type  DBUS_ReceiveData;

void UART5_DBUS_InitConfig(void)
{
    USART_InitTypeDef uart5;
    GPIO_InitTypeDef  gpio;
    NVIC_InitTypeDef  nvic;
    DMA_InitTypeDef   dma;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_DMA1,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);

    //UART5(DBUS)
    gpio.GPIO_Mode   =   GPIO_Mode_AF;
    gpio.GPIO_OType  =   GPIO_OType_PP;
    gpio.GPIO_Pin    =   GPIO_Pin_2;
    gpio.GPIO_PuPd   =   GPIO_PuPd_UP;
    gpio.GPIO_Speed  =   GPIO_Speed_100MHz;
    GPIO_Init(GPIOD, &gpio);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5);

    //UART5(DBUS)
    uart5.USART_BaudRate              =   100000;
    uart5.USART_HardwareFlowControl   =   USART_HardwareFlowControl_None;
    uart5.USART_Mode                  =   USART_Mode_Rx;
    uart5.USART_Parity                =   USART_Parity_No;
    uart5.USART_StopBits              =   USART_StopBits_1;
    uart5.USART_WordLength            =   USART_WordLength_8b;
    USART_Init(UART5, &uart5);

    USART_DMACmd(UART5, USART_DMAReq_Rx, ENABLE);

    USART_Cmd(UART5, ENABLE);

    //UART5_RX
    dma.DMA_Channel           =   DMA_Channel_4;
    dma.DMA_PeripheralBaseAddr=   (uint32_t)(&UART5->DR);
    dma.DMA_Memory0BaseAddr   =   (uint32_t)(DBUS_rx_buffer);
    dma.DMA_DIR               =   DMA_DIR_PeripheralToMemory;
    dma.DMA_BufferSize        =   25;
    dma.DMA_PeripheralInc     =   DMA_PeripheralInc_Disable;
    dma.DMA_MemoryInc         =   DMA_MemoryInc_Enable;
    dma.DMA_MemoryDataSize    =   DMA_MemoryDataSize_Byte;
    dma.DMA_PeripheralDataSize=   DMA_PeripheralDataSize_Byte;
    dma.DMA_Mode              =   DMA_Mode_Normal;
    dma.DMA_Priority          =   DMA_Priority_Medium;
    dma.DMA_FIFOMode          =   DMA_FIFOMode_Disable;
    dma.DMA_FIFOThreshold     =   DMA_FIFOThreshold_Full;
    dma.DMA_MemoryBurst       =   DMA_MemoryBurst_Single;
    dma.DMA_PeripheralBurst   =   DMA_PeripheralBurst_Single;
    DMA_Init(DMA1_Stream0, &dma);
    DMA_Cmd(DMA1_Stream0, ENABLE);

    //UART5
    nvic.NVIC_IRQChannel						=	UART5_IRQn;
    nvic.NVIC_IRQChannelCmd					=	ENABLE;
    nvic.NVIC_IRQChannelPreemptionPriority	=	11;
    nvic.NVIC_IRQChannelSubPriority			=	0;
    NVIC_Init(&nvic);
    USART_ITConfig(UART5, USART_IT_IDLE, ENABLE); //使能串口空闲中断
}
void UART5_IRQHandler(void)
{
    UARTtemp5 = UART5->DR;   //  清除标志位
    UARTtemp5 = UART5->SR;

    DMA_Cmd(DMA1_Stream0, DISABLE);
    //数据量正确
    if(DMA1_Stream0->NDTR == (25-18))
    {    }
    DBUSFrameCounter++;         //帧数增加


    //遥控器控制输数据
    DBUS_ReceiveData.ch0 = (DBUS_rx_buffer[0]|(DBUS_rx_buffer[1] << 8)) & 0x07ff;
    if(DBUS_ReceiveData.ch0==0) DBUS_ReceiveData.ch0=1024;
    DBUS_ReceiveData.ch0-=1024;

    DBUS_ReceiveData.ch1 = ((DBUS_rx_buffer[1] >> 3) | (DBUS_rx_buffer[2] << 5)) & 0x07ff;
    if(DBUS_ReceiveData.ch1==0) DBUS_ReceiveData.ch1=1024;
    DBUS_ReceiveData.ch1-=1024;

    DBUS_ReceiveData.ch2 = ((DBUS_rx_buffer[2] >> 6) | (DBUS_rx_buffer[3] << 2) | (DBUS_rx_buffer[4] << 10)) & 0x07ff;
    if(DBUS_ReceiveData.ch2==0) DBUS_ReceiveData.ch2=1024;
    DBUS_ReceiveData.ch2-=1024;

    DBUS_ReceiveData.ch3 = ((DBUS_rx_buffer[4] >> 1) | (DBUS_rx_buffer[5] << 7)) & 0x07ff;
    if(DBUS_ReceiveData.ch3==0) DBUS_ReceiveData.ch3=1024;
    DBUS_ReceiveData.ch3-=1024;


    DBUS_ReceiveData.switch_left = ((DBUS_rx_buffer[5] >> 4)& 0x000C) >> 2;     //1,3,2

    DBUS_ReceiveData.switch_right = ((DBUS_rx_buffer[5] >> 4)& 0x0003);

    //电脑控制输出数据
    DBUS_ReceiveData.mouse.x = DBUS_rx_buffer[6] | (DBUS_rx_buffer[7] << 8);
    DBUS_ReceiveData.mouse.y = DBUS_rx_buffer[8] | (DBUS_rx_buffer[9] << 8);
    DBUS_ReceiveData.mouse.z = DBUS_rx_buffer[10] | (DBUS_rx_buffer[11] << 8);
    DBUS_ReceiveData.mouse.press_left = DBUS_rx_buffer[12];
    DBUS_ReceiveData.mouse.press_right = DBUS_rx_buffer[13];



    DBUS_ReceiveData.keyBoard.key = DBUS_rx_buffer[14] | (DBUS_rx_buffer[15] << 8);

    //重启DMA
    DMA_ClearFlag(DMA1_Stream0, DMA_FLAG_TCIF0 | DMA_FLAG_HTIF0);
    while(DMA_GetCmdStatus(DMA1_Stream0) != DISABLE);
    DMA_SetCurrDataCounter(DMA1_Stream0, 25);
    DMA_Cmd(DMA1_Stream0, ENABLE);
}

