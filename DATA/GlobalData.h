/********************************Copyright (c)**********************************\
**                   (c) Copyright 2019, Main, China
**                           All Rights Reserved
**                           By(达实久信医疗科技有限公司)
**----------------------------------文件信息------------------------------------
** 文件名称: GlobalData.h
** 创建人员: 王凯
** 创建日期: 2019-09-03
** 文档描述: */
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "includes.h"
#include "Structure.h"
#include "Macro.h"
#include "Can_Dri.h"
extern u32 sn0;//ARM Only One ID
extern u8 ThisTransitionNumber;//本设备设备号
extern u8 Burnin;
extern void * TransLocationQeutb[10];//定义轨道定位消息队列
extern u16 MAX_Count;
extern u16 MIN_Count;
extern volatile u8 Now_direction;//当前方向
extern T_Trans_Statistics TransStatistics;//转轨器统计结构
extern u8 EquipmentType;//设备类型号码；
extern CAN_DATA_FRAME *RTRbuf;
extern u8 alreadlydowntrack;

extern u8 LastFramFlag;
extern u8 ThresholdValue;//定位任务执行的次数阈值，当一定次数没有找零，阈值到达极限，强制校零

extern INT16U TrackCount[MAX_MovingTrack][MAX_FixedTrack];//转轨器配置结构
extern INT16U TempTrackCount[MAX_MovingTrack][MAX_FixedTrack];
extern INT8U W25QXXSendBuff[130];//W25QXX的发送缓存
extern INT8U W25QXXReceiveBuff[130];//接收缓存
extern uint64_t printfcount;//打印流程计数，便于查看

extern volatile INT8U   FindZeroGlob;
extern T_CAN_Communicat_Mode CANCommunicatMode;
extern u8 alreadlyuptrack;
extern OS_EVENT * TransLocationQeue;//轨道定位信号邮箱
extern OS_EVENT * CarAlreadyPassSem;//小车已经通过信号量
extern OS_EVENT * CarAlreadyUpMbox;//小车已经上轨信号量
extern OS_EVENT * CarAlreadyDownSem;//小车已经下轨道信号量
extern OS_EVENT * TransFindzeroSem;//轨道找零
extern OS_EVENT * AgreeChangeModeSem;//域同意改变模式
extern OS_EVENT * ApplyChangeModeBox;//申请模式变更信号邮箱

/*外部变量 信号量信号邮箱在myconfig.c中定义*/
extern OS_EVENT * can1Mbox;
extern OS_EVENT * ModBusFlagSem;

extern T_Trans_Status TransStatus;
extern T_Trans_Status DefoultTransStatus;
extern T_Control_Message  ControlMessage;//定义控制器的信息

extern u16 BeepChirptimes;//蜂鸣器鸣叫次数
extern u8  BeepAlwaysON;//蜂鸣器一直鸣叫开关


extern u8 TranferSpeed;
extern u8 EnableStopButton;
extern u8 FindZeroCtrl;
extern u8 BeepOFFORON;
extern u8 IsMotorRun;
/*******************************End of Head************************************/
