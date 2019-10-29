/****************************************************************
***@file CAN.h
***@brief CANģ����ؽӿں�������������ļ�
***@author  Ϳ˧
***@version v1.0
***@date    2015.08.17

***<b>�޸���ʷ��\n</b>
***- 1. 2015.08.17 Ϳ˧ �����ļ�

***<b>Copyright ��c�� 2015�� ����ҽ�ƿƼ��ɷ����޹�˾</b>
*****************************************************************/
#include "includes.h"
#ifndef __CAN_H
#define __CAN_H	 



//CAN1����RX0�ж�ʹ��
//#define CAN1_RX0_INT_ENABLE	0x01		//0,��ʹ��;1,ʹ��.	
//#define CAN1_RX1_INT_ENABLE	0x02		//0,��ʹ��;1,ʹ��.
//#define CAN2_RX0_INT_ENABLE	0x04		//0,��ʹ��;1,ʹ��.	
//#define CAN2_RX1_INT_ENABLE	0x08		//0,��ʹ��;1,ʹ��.

//CAN1����RX0�ж�ʹ��
//#define CAN1_RX0_INT_ENABLE	1	//0,��ʹ��;1,ʹ��.
//#define CAN2_RX0_INT_ENABLE	1		//0,��ʹ��;1,ʹ��.
#define CAN1_RX0_INT_ENABLE	0x01		//0,��ʹ��;1,ʹ��.	
#define CAN1_RX1_INT_ENABLE	0x02		//0,��ʹ��;1,ʹ��.
#define CAN2_RX0_INT_ENABLE	0x04		//0,��ʹ��;1,ʹ��.	
#define CAN2_RX1_INT_ENABLE	0x08		//0,��ʹ��;1,ʹ��.
#define CAN_FILTER_EN   1       //0����ʹ�ܣ�1��ʹ��
#define CAN1_USE_GPIOA_EN 1
#define CAN1_USE_GPIOB_EN 0
#define CAN1_USE_GPIOD_EN 0
#define CAN2_USE_GPIOB5_6_EN 1
#define CAN2_USE_GPIOB12_13_EN 0
#define CAN1_RX0_MASK_BIT 0x01
#define true 1
#define false 0

//CAN֡
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


#define CAN_BOFF_CHECK 0x04   //���߹رձ�־
#define CAN_EPVF_CHECK 0x02   //���󱻶���־
#define CAN_EWGF_CHECK 0x01   //���󾯸��־



//CANͨ�Ŵ�������
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
//CANͨѶ��������궨��
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
#define MAX_ACK_TIMEOUT       500 //�ظ���ʱ�ȴ�ʱ��
#define MAX_RETRY_TIMES       3   //���ʱ�ط�������Ϊ3

//����CAN1��CAN2����ͨ������
#define CAN1_CHANNEL_NUM 20
#define CAN2_CHANNEL_NUM 4

typedef uint8_t bool;

/*�����ʳ�ʼ�����ݽṹ*/
typedef struct _CAN_BPR_INIT
{
	float bps;
	u8 tsjw;
	u8 tbs1;
	u8 tbs2;
	u16 brp;
}CAN_BPR_INIT;
/*֡���ͽṹ*/
typedef enum _CAN_FRAME_TYPE
{
	FRAME_REPLY       =0,
	FRAME_ACTIVE      =1 
}CAN_FRAME_TYPE;
/*CAN���ͽ���ṹ*/
typedef enum _CAN_SEND_RESULT
{
	SEND_IDLE,        
	SEND_BUSY,
	SEND_BUS_CLOSED,
	SEND_FAILURE,    
	SEND_SUCCESS     
}CAN_SEND_RESULT;

/*CAN�����������ݽṹ*/
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

/*CAN�����������ݽṹ*/
typedef struct _CAN_DATA_FRAME
{
	u8 dataLen;
	u8 RTR;//Զ��֡��־
	u8 IDE;//��Ϣ���ͱ�־
	union 
	{
		u32 canId;
		struct
		{
			
			u32 Subindex:8,
			MainIndex:8,
			deviceid:8,
			ReserveBit:1,/*����λ*/
			  RetryBit:1,/*�ط���־λ*/
			 	ackBit:1,/*Ӧ��֡��־λ*/
			  SubframeBit:1,/*��֡��־λ*/
			  MasterslaveBit:1;/*������־*/
		}MasteridBit;
					
		struct
		{
			u32 index:8,/*������ ���8λ*/
				sendDeviceId:8,	/*wk�������豸��ַ*/		
			  recvDeviceId:8,/*wk�������豸��ַ���λ*/
			  ReserveBit:1,/*����λ*/
			  RetryBit:1,/*�ط���־λ*/
			 	ackBit:1,/*Ӧ��֡��־λ*/
			  SubframeBit:1,/*��֡��־λ*/
			  MasterslaveBit:1;/*������־*/
		}idBit;
	}id;
	
	CAN_MSG canMsg;
	struct _CAN_DATA_FRAME *nextMsg;
}CAN_DATA_FRAME;

/*����ı�־λ��־��0Ϊ���ӣ�1Ϊ����*/
enum _MasterSlave{
     MasterSlave=0,
	   ManyMaster,
};




/*�豸����֡�ؼ��ֶ����ݽṹ*/
typedef struct _ReplyFrame									//3�ֽ�
{
	uint8_t   deviceId;    //�豸Id
	uint8_t		mainIndex;   //������
	uint8_t		subIndex;    //������
}ReplyFrame;

void CAN_Hardware_Init(u8 brp);
u8 CAN_Mode_Init(CAN_TypeDef *canChan,CAN_InitTypeDef *CAN_InitStructure,CAN_FilterInitTypeDef *CAN_FilterInitStructure,u8 rxIntType);
void CAN_Software_Init(void);
void CAN_HardwareInit(u8 canChan,float bps,u8 mode,u8 rxIntType);
void Load_CAN_Threshold(u8 err_limit);
u8 CAN1_Mode_Init(u8 tsjw,u8 tbs1,u8 tbs2,u16 brp,u8 mode);//CAN��ʼ��
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
void CANSEND_Init(void);	//�ⲿ�жϳ�ʼ��		 					    

extern CAN_SEND_FRAME *g_can1SendFrame;  //CAN1������������
extern CAN_DATA_FRAME *g_canDataFrame[MAX_CAN_CHANNEL];
extern OS_EVENT *can1RecvSem;	 //CAN1�ź���ָ��
extern OS_EVENT *can2RecvSem;	 //CAN1�ź���ָ��
extern OS_EVENT *can1ActSem;	 //CAN1�ź���ָ��
extern OS_EVENT *can2ActSem;	 //CAN1�ź���ָ��
extern OS_EVENT *can1AckSem;	 //CAN1�ź���ָ��
extern OS_EVENT *can2AckSem;	 //CAN1�ź���ָ��
extern OS_EVENT *can1CtlResSem; 

extern OS_EVENT *Can2Find0AckSem;//����ɹ�������ͽ���ź�����Ȼ����ظ��յ����ź���

extern OS_EVENT *can1CtlAckSem;	 //CAN1����Ӧ�����ź���ָ��
extern OS_EVENT *can2CtlAckSem;	 //CAN2����Ӧ�����ź���ָ��
extern OS_EVENT *can1InfoAckSem;  //CAN1��ϢӦ���ź���
extern OS_EVENT *can2InfoAckSem;  //CAN2��ϢӦ���ź���

extern OS_EVENT *can1SendSem;
extern OS_EVENT *can2SendSem;

extern OS_EVENT *StepRunMbox;

/************************************��Ϣ����******************************************/
extern OS_EVENT *can1Mbox;
extern OS_EVENT *can2Mbox;
extern OS_EVENT *canCtlResMbox;   //CANָ�ӽ������



extern CAN_DATA_FRAME *g_canDataFrame[MAX_CAN_CHANNEL];
extern CAN_DATA_FRAME *g_canDataAckFrame[MAX_CAN_CHANNEL];  //Ӧ������
extern CAN_DATA_FRAME *g_canDataActFrame[MAX_CAN_CHANNEL];  //��������

extern CAN_SEND_FRAME *g_can1SendFrame;  //CAN1��������֡����
extern CAN_SEND_FRAME *g_can2SendFrame;  //CAN2��������֡����
extern ReplyFrame can1CtlAckFrame,can1CtlResFrame,can1InfoAckFrame,can2InfoAckFrame;


//extern OS_STK CAN1_SEND_TASK_STK[CAN1_SEND_STK_SIZE];
//extern OS_STK CAN2_SEND_TASK_STK[CAN2_SEND_STK_SIZE];
//������  


#endif
