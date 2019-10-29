/********************************Copyright (c)**********************************\
**                   (c) Copyright 2019, Main, China
**                           All Rights Reserved
**                           By(��ʵ����ҽ�ƿƼ����޹�˾)
**----------------------------------�ļ���Ϣ------------------------------------
** �ļ�����: myconfig.h
** ������Ա: ����
** ��������: 2019-06-21
** �ĵ�����: ���й�����������ã��ṹ��Ķ��壬�����������ļ���ȫ�ֱ���������������ⲿ��������
*******************************End of Head************************************/
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "Led_Dri.h"
#include "Beep_Dri.h"
#include "includes.h"
#include "Can_Dri.h"
#include "StepMotor_Dir.h"
#include "malloc.h"
#include "W25QXX_Dir.h"
#include "TrackMission_Dri.h"
#include "TransitionFunction.h"
#include "Uart_Drv.h"
#include "Timer_Dri.h"
#include "crc.h"
#include "ModBus.h"
#include "GlobalData.h"
//#include "Macro.h"
//#include "Structure.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/*���������ջ�ⲿ���� ��ֹ�����ظ����� �¼�ʱҪ��mycofig.c�����*/
extern OS_STK START_TASK_STK[START_STK_SIZE];


/*����������*/
void START_Task(void *pdata);
void RUN_Task(void *pdata);
void CAN2_Task(void *pdata);
void CAN2_Ack_Task(void *pdata);
void CAN2_Act_Task(void *pdata);
void LED_Task(void *pdata);
void STEPMOTOR_Task(void *pdata);
void TransLocation_TASK(void *pdata);
void MissionAnalyse_Task(void *pdata);//ת������е�����������
void CarApplyPass_Task(void *pdata);
void CAN2_Send_Task(void *pdata);
void CarApplyPass_Task(void *pdata);
void CarApplyChange_Task(void *pdata);
void TransFindZero_Task(void *pdata);
void TranPerMission_Task(void *pdata);
void HEARBEAT_TASK(void *pdata);
void MODBUS_Task(void *pdata);
void STATE_Task(void *pdata);
void ModeChange_Task(void *pdata);

/********************************************************************************************/
/*���ú�������*/
void HardwareInit(void);
void ParameterInit(void);

void EventCreate(void);
void TaskCreate(void);
void Index_Judegment(u8 canChan,u32 id,u8 index,CAN_DATA_FRAME * tempNode,CAN_DATA_FRAME *frameBuf);
void Master_Index_judegment(u8 canChan,CAN_DATA_FRAME * tempNode,CAN_DATA_FRAME *frameBuf);
void StateQuery_Index_judegment(CAN_DATA_FRAME * tempNode);
void Broadcast_Judegment(u8 canChan,CAN_DATA_FRAME * tempNode);
void ModBus_TrackCount_UpData(void);


