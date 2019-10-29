/********************************Copyright (c)**********************************\
**                   (c) Copyright 2019, Main, China
**                           All Rights Reserved
**                           By(达实久信医疗科技有限公司)
**----------------------------------文件信息------------------------------------
** 文件名称: Timer_APP.c
** 创建人员: 王凯
** 创建日期: 2019-07-17
** 文档描述: 心跳帧线程
******************************************************************************/
#include "myconfig.h"
/*******************************************************************************
** 函数名称: HEARBEAT_TASK
** 功能描述: 心跳帧处理线程，当没有发送给域控制器的帧5秒之后，就给域控制器发送一个心跳帧
** 参数说明: pdata: [输入/出]
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-07-17
********************************************************************************/
void HEARBEAT_TASK(void *pdata)
{
    CAN_DATA_FRAME *TempNode;
	OS_CPU_SR cpu_sr;
    pdata=pdata;


    while(1)
    {
        if(HeartBeat.BeatTime==10)
        {
					  OS_ENTER_CRITICAL();
            TempNode = (CAN_DATA_FRAME *)mymalloc(SRAMIN,sizeof(CAN_DATA_FRAME));//申请内存
					  OS_EXIT_CRITICAL();
//            memset(TempNode->canMsg.dataBuf,0,sizeof(CAN_MSG));
					  memset(TempNode,0,sizeof(CAN_DATA_FRAME));
            TempNode->dataLen=0;
            TempNode->id.MasteridBit.Subindex=0x00;//主索引
            TempNode->id.MasteridBit.MainIndex=0x00;//自索引
            TempNode->id.MasteridBit.deviceid=0x00;//设备号//域的地址
            TempNode->id.MasteridBit.ReserveBit=0;//保留位
            TempNode->id.MasteridBit.RetryBit=0;//重发标志
            TempNode->id.MasteridBit.ackBit=0;//应答帧标志
            TempNode->id.MasteridBit.SubframeBit=0;//分帧标志
            TempNode->id.MasteridBit.MasterslaveBit=0;//多主标志
             CAN2_Single_Send(TempNode);//单帧发送函数
            
            HeartBeat.BeatTime=0;//计数清零
					OS_ENTER_CRITICAL();
            myfree(SRAMIN,TempNode);
					OS_EXIT_CRITICAL();
        }
        else OSTimeDlyHMSM(0,0,0,20);
    }

}
/*******************************End of File************************************/

