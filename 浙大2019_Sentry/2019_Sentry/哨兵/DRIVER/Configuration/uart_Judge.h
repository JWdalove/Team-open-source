#ifndef __UART4_H__
#define __UART4_H__

#include <stm32f4xx.h>
#include <stdio.h>
#include "judgement_info.h"
#include "cmsis_os.h"
#include "comm_task.h"
#include "task.h"
#include "Start_task.h"
#include "protocol.h"

//#define UART_RX_DMA_SIZE 150
#define JudgeTxBufferLength 3  //裁判系统发送数据缓存
#define JudgeFrameLength_1      44
#define JudgeFrameLength_2      11
#define JudgeFrameLength_3      24

#define UART_RX_DMA_SIZE       1024
/* dma double buffer 双缓冲*/
extern uint8_t judge_dma_rxbuff[2][UART_RX_DMA_SIZE];
extern uint8_t pc_dma_rxbuff[2][UART_RX_DMA_SIZE];

//格式转换联合体
typedef union
{
    uint8_t U[4];
    float F;
    int I;
} FormatTrans;

typedef __packed struct
{
    uint8_t SOF;          //数据帧起始字节 0xA5
    uint16_t DataLength;  //帧内Date长度
    uint8_t Seq;          //包序号
    uint8_t CRC8;         //帧头CRC校验
} tFrameHeader;         // 报文帧头
typedef enum
{
    GameInfo = 0x0001,     //比赛进程信息 0x0001  50Hz
    RealBloodChangedData,  //实时血量变化数据 0x0002
    RealShootData,         //实时射击数据 0x0003
    SelfDefinedData =0x301, //学生上传自定义数据  200Hz
    Wrong = 0x1301 //错误码
} tCmdID;                // 命令码ID

typedef __packed struct
{
    uint8_t flag;
    float x;				//单位为m，小数点后两位为有效值
    float y;
    float z;
    float compass; //UWB角度信息
} tLocData; //LOC状态

typedef __packed struct
{
    uint32_t remainTime;      //比赛剩余时间（从倒计时三分钟开始计算，单位 s）
    uint16_t remainLifeValue; //机器人剩余血量
    float realChassisOutV;    //实时底盘输出电压（单位 V）
    float realChassisOutA;    //实时底盘输出电流（单位 A）
    tLocData locData;         //LOC 状态，见 tLocData 结构体定义
    float remainPower;        //剩余能量（满值 60J）
    float heatlimit;          //枪口热量限制
} tGameInfo;                //比赛进程信息


typedef __packed struct
{
		uint16_t data_cmd_id;  //1. 交互数据接收信息
		uint16_t send_ID;
		uint16_t receiver_ID;	
    uint8_t data1;           //1.客户端自定义数据： cmd_id:0x0301。内容 ID:0xD180。 发送频率：上限 10Hz
    uint8_t data2;
    uint8_t data3;
} tSelfDefineInfo;


typedef __packed struct
{
    tFrameHeader    FrameHeader;   //帧头
    tCmdID          CmdID;         // 命令码ID
    __packed union
    {
        tGameInfo       GameInfo;    //进程信息 (0x0001)
//        tRealBloodChangedData   RealBloodChangedData; //血量变化信息(0x0002)
//        tRealShootData   RealShootData; //受打击信息 (0x0003)
        tSelfDefineInfo SelfDefineInfo; //学生自定义数目 (0x0005)
    } Data;                           // 帧内数据
    uint16_t        CRC16;            //帧尾CRC
} tFrame;                           // 报文 格式

void UART4_Judge_InitConfig(void);
void Judge_data_pass(void);
void Judge_data_send(void);
void Send_FrameData(tCmdID cmdid, uint8_t * pchMessage,uint8_t dwLength);

extern u8 JudgeFrameCount;
#endif
