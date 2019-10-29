/****************************************************************
***@file CAN.h
***@brief CAN模块相关接口函数与变量声明文件
***@author  涂帅
***@version v1.0
***@date    2015.08.17

***<b>修改历史：\n</b>
***- 1. 2015.08.17 涂帅 创建文件

***<b>Copyright （c） 2015， 久信医疗科技股份有限公司</b>
*****************************************************************/
#include "includes.h"
#ifndef __CAN_H
#define __CAN_H	 



//CAN1接收RX0中断使能
//#define CAN1_RX0_INT_ENABLE	0x01		//0,不使能;1,使能.	
//#define CAN1_RX1_INT_ENABLE	0x02		//0,不使能;1,使能.
//#define CAN2_RX0_INT_ENABLE	0x04		//0,不使能;1,使能.	
//#define CAN2_RX1_INT_ENABLE	0x08		//0,不使能;1,使能.

//CAN1接收RX0中断使能
//#define CAN1_RX0_INT_ENABLE	1	//0,不使能;1,使能.
//#define CAN2_RX0_INT_ENABLE	1		//0,不使能;1,使能.
#define CAN1_RX0_INT_ENABLE	0x01		//0,不使能;1,使能.	
#define CAN1_RX1_INT_ENABLE	0x02		//0,不使能;1,使能.
#define CAN2_RX0_INT_ENABLE	0x04		//0,不使能;1,使能.	
#define CAN2_RX1_INT_ENABLE	0x08		//0,不使能;1,使能.
#define CAN_FILTER_EN   1       //0：不使能，1：使能
#define CAN1_USE_GPIOA_EN 1
#define CAN1_USE_GPIOB_EN 0
#define CAN1_USE_GPIOD_EN 0
#define CAN2_USE_GPIOB5_6_EN 1
#define CAN2_USE_GPIOB12_13_EN 0
#define CAN1_RX0_MASK_BIT 0x01
#define true 1
#define false 0

//CAN帧
#define CAN_HEARTBEAT_RETRY_TIME    50  //50*2ms=100ms;
#define CAN_ACK_RETRY_TIME          100
#define CAN_INFO_RETRY_TIME         200
#define CAN_ACT_RETRY_TIME          500
#define CAN2_INFO_RETRY_TIME         500

#define CAN1_FIFO0 0
#define CAN1_FIFO1 1
#define CAN2_FIFO0 0
#define CAN2_FIFO1 1

#define CAN_BPS_1000K                 1000
#define CAN_BPS_800K                  800
#define CAN_BPS_500K                  500
#define CAN_BPS_250K                  250
#define CAN_BPS_125K                  125
#define CAN_BPS_833K				          83.3
#define CAN_BPS_50K                   50
#define CAN_BPS_333K                  33.3
#define CAN_BPS_30K                   30
#define CAN_BPS_25K                   25
#define CAN_BPS_20K                   20
#define CAN_BPS_10K                   10
#define CAN_BPS_5K                    5
#define CAN_BPS_34K                   34

#define CAN1_CHANNEL    0
#define CAN2_CHANNEL    1
#define MAX_CAN_CHANNEL 2


#define FMR_INIT              0x01
#define MAX_BPS_SIZE					14
#define USE_CAN1_EN		  1
#define USE_CAN2_EN		  1


#define CAN_BOFF_CHECK 0x04   //总线关闭标志
#define CAN_EPVF_CHECK 0x02   //错误被动标志
#define CAN_EWGF_CHECK 0x01   //错误警告标志



//CAN通信错误类型
#define ERR_FILL       1
#define ERR_FORMAT     2
#define ERR_ACK        3
#define ERR_INVISIBLE  4
#define ERR_VISIBLE    5
#define ERR_CRC        6


#define CAN1_BUF_SIZE  256
#define CAN2_BUF_SIZE  256


#define INT_MAX    65535


#define USE_CAN1_FIFO0_EN 1
#define USE_CAN1_FIFO1_EN 0
#define USE_CAN2_FIFO0_EN 1
#define USE_CAN2_FIFO1_EN 0

#define CAN2_USE_GPIOB5_6_EN   1
#define CAN2_USE_GPIOB12_13_EN 0

#define ID_LIST_MAXSIZE         30
#define MAX_CAN_DATA_SIZE 8
#define MAX_FIF0_SIZE 2
//CAN通讯错误次数宏定义
#define CAN_ESR_MIN   0
#define CAN_ESR_MID   40
#define CAN_ESR_MAX   95


#define CAN_IS_MULTIFRAME_TYPE 0x80
#define MIN_CAN_DEVICE_ADDR 0
#define MAX_CAN_DEVICE_ADDR 255
#define MIN_CAR_ADDR 0
#define MAX_CAR_ADDR 1023
#define MIN_CAN_BPS_INDEX 1
#define MAX_CAN_BPS_INDEX 8



#define CAN1_BUF_SIZE  				256
#define CAN2_BUF_SIZE  				256
#define MAX_ACK_TIMEOUT       500 //回复超时等待时间
#define MAX_RETRY_TIMES       3   //最大超时重发次数定为3

//定义CAN1和CAN2两个通道数量
#define CAN1_CHANNEL_NUM 20
#define CAN2_CHANNEL_NUM 4

typedef uint8_t bool;

/*波特率初始化数据结构*/
typedef struct _CAN_BPR_INIT
{
	float bps;
	u8 tsjw;
	u8 tbs1;
	u8 tbs2;
	u16 brp;
}CAN_BPR_INIT;
/*帧类型结构*/
typedef enum _CAN_FRAME_TYPE
{
	FRAME_REPLY       =0,
	FRAME_ACTIVE      =1 
}CAN_FRAME_TYPE;
/*CAN发送结果结构*/
typedef enum _CAN_SEND_RESULT
{
	SEND_IDLE,        
	SEND_BUSY,
	SEND_BUS_CLOSED,
	SEND_FAILURE,    
	SEND_SUCCESS     
}CAN_SEND_RESULT;

/*CAN设置命令数据结构*/
typedef struct _CAN_MSG
{
	u8 dataBuf[8];
}CAN_MSG;

typedef struct _CAN_SEND_FRAME
{
	u8  len;
	u32 id;
	CAN_MSG canMsg;
	struct _CAN_SEND_FRAME *nextMsg;
}CAN_SEND_FRAME;

/*CAN数据链表数据结构*/
typedef struct _CAN_DATA_FRAME
{
	u8 dataLen;
	u8 RTR;//远程帧标志
	u8 IDE;//消息类型标志
	union 
	{
		u32 canId;
		struct
		{
			
			u32 Subindex:8,
			MainIndex:8,
			deviceid:8,
			ReserveBit:1,/*保留位*/
			  RetryBit:1,/*重发标志位*/
			 	ackBit:1,/*应答帧标志位*/
			  SubframeBit:1,/*分帧标志位*/
			  MasterslaveBit:1;/*多主标志*/
		}MasteridBit;
					
		struct
		{
			u32 index:8,/*索引号 最低8位*/
				sendDeviceId:8,	/*wk：发送设备地址*/		
			  recvDeviceId:8,/*wk：接受设备地址最低位*/
			  ReserveBit:1,/*保留位*/
			  RetryBit:1,/*重发标志位*/
			 	ackBit:1,/*应答帧标志位*/
			  SubframeBit:1,/*分帧标志位*/
			  MasterslaveBit:1;/*多主标志*/
		}idBit;
	}id;
	
	CAN_MSG canMsg;
	struct _CAN_DATA_FRAME *nextMsg;
}CAN_DATA_FRAME;

/*定义的标志位标志，0为主从，1为多主*/
enum _MasterSlave{
     MasterSlave=0,
	   ManyMaster,
};




/*设备返回帧关键字段数据结构*/
typedef struct _ReplyFrame									//3字节
{
	uint8_t   deviceId;    //设备Id
	uint8_t		mainIndex;   //主索引
	uint8_t		subIndex;    //子索引
}ReplyFrame;

void CAN_Hardware_Init(u8 brp);
u8 CAN_Mode_Init(CAN_TypeDef *canChan,CAN_InitTypeDef *CAN_InitStructure,CAN_FilterInitTypeDef *CAN_FilterInitStructure,u8 rxIntType);
void CAN_Software_Init(void);
void CAN_HardwareInit(u8 canChan,float bps,u8 mode,u8 rxIntType);
void Load_CAN_Threshold(u8 err_limit);
u8 CAN1_Mode_Init(u8 tsjw,u8 tbs1,u8 tbs2,u16 brp,u8 mode);//CAN初始化
u8 CAN2_Mode_Init(u8 tsjw,u8 tbs1,u8 tbs2,u16 brp,u8 mode);
u8 CAN_Send_Msg(u8 channel,u32 id,u8 frameType,CAN_MSG *msg,u8 len);
u8  CAN_Frame_Send(u8 canChan,CAN_DATA_FRAME *frame);
u8 CAN_RTRsend_Msg(u8 channel,u32 id,u8 frameType);
u8  CAN_RTRframe_Send(u8 canChan,u32 id,u8 IDE);

void CAN_DataDeal(u8 canChan,CAN_DATA_FRAME *frameBuf);
void CAN_Act_DataDeal(u8 canChan,CAN_DATA_FRAME *frameBuf);
void CAN_Ack_DataDeal(u8 canChan,CAN_DATA_FRAME *frameBuf);
void CAN1_Send_Frame(CAN_SEND_FRAME *frame);
void CAN2_Send_Frame(CAN_SEND_FRAME *frame);

u32 Get_CAN_ExId(bool ackBit,u8 sendAddr,u8 recvAddr);
void CAN_Fill_Data(u8 data0,u8 data1,u8 data2,u8 data3,u8 data4,u8 data5,u8 data6,u8 data7,CAN_MSG *msg);
bool CAN_Send_Frame_App(u8 canChan,u32 id,u8 frameType,CAN_MSG *canMsg,u8 len,CAN_DATA_FRAME *frameBuf,u16 sendRetryTime);
void CAN_Post_Queue(u8 canChan,CAN_SEND_FRAME *frame);
u32 CAN_FramID_Change(CAN_DATA_FRAME *frameBuf);
void CAN2_Single_Send(CAN_DATA_FRAME * tempNode);
void ACKSendFram(u8 canChan,CAN_DATA_FRAME *tempNode);

//void CAN_Fill_Mission(uint8_t mainIndex,Mission task,CAN_MSG *msg);
void CAN_FMR_Config(u8 canChan);
void CANSEND_Init(void);	//外部中断初始化		 					    

extern CAN_SEND_FRAME *g_can1SendFrame;  //CAN1发送主动链表
extern CAN_DATA_FRAME *g_canDataFrame[MAX_CAN_CHANNEL];
extern OS_EVENT *can1RecvSem;	 //CAN1信号量指针
extern OS_EVENT *can2RecvSem;	 //CAN1信号量指针
extern OS_EVENT *can1ActSem;	 //CAN1信号量指针
extern OS_EVENT *can2ActSem;	 //CAN1信号量指针
extern OS_EVENT *can1AckSem;	 //CAN1信号量指针
extern OS_EVENT *can2AckSem;	 //CAN1信号量指针
extern OS_EVENT *can1CtlResSem; 

extern OS_EVENT *Can2Find0AckSem;//找零成功后给域发送结果信号量，然后域回复收到的信号量

extern OS_EVENT *can1CtlAckSem;	 //CAN1控制应答处理信号量指针
extern OS_EVENT *can2CtlAckSem;	 //CAN2控制应答处理信号量指针
extern OS_EVENT *can1InfoAckSem;  //CAN1信息应答信号量
extern OS_EVENT *can2InfoAckSem;  //CAN2信息应答信号量

extern OS_EVENT *can1SendSem;
extern OS_EVENT *can2SendSem;

extern OS_EVENT *StepRunMbox;

/************************************消息邮箱******************************************/
extern OS_EVENT *can1Mbox;
extern OS_EVENT *can2Mbox;
extern OS_EVENT *canCtlResMbox;   //CAN指挥结果邮箱



extern CAN_DATA_FRAME *g_canDataFrame[MAX_CAN_CHANNEL];
extern CAN_DATA_FRAME *g_canDataAckFrame[MAX_CAN_CHANNEL];  //应答链表
extern CAN_DATA_FRAME *g_canDataActFrame[MAX_CAN_CHANNEL];  //主动链表

extern CAN_SEND_FRAME *g_can1SendFrame;  //CAN1发送主动帧链表
extern CAN_SEND_FRAME *g_can2SendFrame;  //CAN2发送主动帧链表
extern ReplyFrame can1CtlAckFrame,can1CtlResFrame,can1InfoAckFrame,can2InfoAckFrame;


//extern OS_STK CAN1_SEND_TASK_STK[CAN1_SEND_STK_SIZE];
//extern OS_STK CAN2_SEND_TASK_STK[CAN2_SEND_STK_SIZE];
//任务函数  


#endif
