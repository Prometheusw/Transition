/********************************Copyright (c)**********************************\
**                   (c) Copyright 2019, Main, China
**                           All Rights Reserved
**                           By(达实久信医疗科技有限公司)
**----------------------------------文件信息------------------------------------
** 文件名称: myconfig.h
** 创建人员: 王凯
** 创建日期: 2019-06-21
** 文档描述: 所有关于任务的配置，结构体的定义，所有在其他文件的全局变量都在这里进行外部变量声明
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
/*定义任务堆栈外部变量 防止出现重复定义 新加时要在mycofig.c中添加*/
extern OS_STK START_TASK_STK[START_STK_SIZE];


/*任务函数声明*/
void START_Task(void *pdata);
void RUN_Task(void *pdata);
void CAN2_Task(void *pdata);
void CAN2_Ack_Task(void *pdata);
void CAN2_Act_Task(void *pdata);
void LED_Task(void *pdata);
void STEPMOTOR_Task(void *pdata);
void TransLocation_TASK(void *pdata);
void MissionAnalyse_Task(void *pdata);//转轨器机械任务分析函数
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
/*配置函数声明*/
void HardwareInit(void);
void ParameterInit(void);

void EventCreate(void);
void TaskCreate(void);
void Index_Judegment(u8 canChan,u32 id,u8 index,CAN_DATA_FRAME * tempNode,CAN_DATA_FRAME *frameBuf);
void Master_Index_judegment(u8 canChan,CAN_DATA_FRAME * tempNode,CAN_DATA_FRAME *frameBuf);
void StateQuery_Index_judegment(CAN_DATA_FRAME * tempNode);
void Broadcast_Judegment(u8 canChan,CAN_DATA_FRAME * tempNode);
void ModBus_TrackCount_UpData(void);


