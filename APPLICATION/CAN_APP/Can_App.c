/********************************Copyright (c)**********************************\
**                   (c) Copyright 2019, Main, China
**                           All Rights Reserved
**                           By(达实久信医疗科技有限公司)
**----------------------------------文件信息------------------------------------
** 文件名称: Can_App.c
** 创建人员: 王凯
** 创建日期: 2019-08-01
** 文档描述:
******************************************************************************/
#include "myconfig.h"
/****************************************************************
功能  wangkai：接收分配处理任务？？？
入口参数：无
返回值：无;
*****************************************************************/
void CAN1_Task(void *pdata)
{
    u8 err = 0;
    pdata = pdata;
    while(1)
    {
        OSSemPend(can1RecvSem,0,&err);//wangkai：中断中产生
        //Led_Blink_Mode(LED_NUM_1, &LED_MODE_3);
        CAN_DataDeal(CAN1_CHANNEL,g_canDataFrame[CAN1_CHANNEL]);
    }
}

/****************************************************************
功能：   wangkai：接受分配处理任务？？？？
入口参数：无
返回值：无;
*****************************************************************/
void CAN2_Task(void *pdata)
{
    u8 err = 0;
    pdata = pdata;
    while(1)
    {
        OSSemPend(can2RecvSem,0,&err);
        CAN_DataDeal(CAN2_CHANNEL,g_canDataFrame[CAN2_CHANNEL]);
    }
}
/****************************************************************
功能：CAN1主动帧处理任务
入口参数：无
返回值：无;
*****************************************************************/
void CAN1_Act_Task(void *pdata)
{
    u8 err = 0;
    pdata = pdata;
    while(1)
    {
        OSSemPend(can1ActSem,0,&err);
        CAN_Act_DataDeal(CAN1_CHANNEL,g_canDataActFrame[CAN1_CHANNEL]);
    }
}

/****************************************************************
功能：CAN2主动帧处理任务
入口参数：无
返回值：无;
*****************************************************************/
void CAN2_Act_Task(void *pdata)
{
    u8 err = 0;
    pdata = pdata;
    while(1)
    {
        OSSemPend(can2ActSem,0,&err);
        CAN_Act_DataDeal(CAN2_CHANNEL,g_canDataActFrame[CAN2_CHANNEL]);
    }
}
/****************************************************************
功能：CAN1应答帧处理任务
入口参数：无
返回值：无;
*****************************************************************/
void CAN1_Ack_Task(void *pdata)
{
    u8 err = 0;
    pdata = pdata;
    while(1)
    {
        OSSemPend(can1AckSem,0,&err);
        CAN_Ack_DataDeal(CAN1_CHANNEL,g_canDataAckFrame[CAN1_CHANNEL]);
    }
}
/****************************************************************
功能：CAN2应答帧
入口参数：无
返回值：无;
*****************************************************************/
void CAN2_Ack_Task(void *pdata)
{
    u8 err = 0;
    pdata = pdata;
    while(1)
    {
        OSSemPend(can2AckSem,0,&err);
        CAN_Ack_DataDeal(CAN2_CHANNEL,g_canDataAckFrame[CAN2_CHANNEL]);
    }
}
/****************************************************************
功能：CAN1发送主动帧任务
入口参数：void *pdata用户参数指针
返回值：无
*****************************************************************/
void CAN1_Send_Task(void *pdata)
{
    OS_CPU_SR cpu_sr;
    //CAN_SEND_FRAME *frame;
    CAN_SEND_FRAME *deleteNode = NULL;
    u8 err = 0;
    while(1)
    {
        //frame = (CAN_SEND_FRAME*)OSQPend(can1SendQueue,0,&err);
        OSSemPend(can1SendSem,0,&err);
        while(g_can1SendFrame->nextMsg!=NULL)
        {
            CAN1_Send_Frame(g_can1SendFrame->nextMsg);  //发送帧信息
            /*此处会等待一个回应信号量如果没有，就不会删除内存wangkai:等待时间改成了一直等待*/

            //释放内存
            deleteNode = g_can1SendFrame->nextMsg;
            g_can1SendFrame->nextMsg = deleteNode->nextMsg;
            OS_ENTER_CRITICAL();
            myfree(SRAMIN,deleteNode);
            OS_EXIT_CRITICAL();
        }
    }
}
/****************************************************************
功能：CAN2发送主动帧任务
入口参数：void *pdata用户参数指针
返回值：无
*****************************************************************/
void CAN2_Send_Task(void *pdata)
{
    OS_CPU_SR cpu_sr;
    CAN_SEND_FRAME *deleteNode = NULL;
    pdata=pdata;
    u8 err = 0;
    while(1)
    {
        OSSemPend(can2SendSem,0,&err);
        /**/

        while(g_can2SendFrame->nextMsg!=NULL)
        {
            CAN2_Send_Frame(g_can2SendFrame->nextMsg);  //发送帧信息
            /*此处会等待一个回应信号量如果没有，就不会删除内存wangkai:等待时间改成了一直等待*/
            deleteNode = g_can2SendFrame->nextMsg;
            g_can2SendFrame->nextMsg = deleteNode->nextMsg;
            OS_ENTER_CRITICAL();
            myfree(SRAMIN,deleteNode);	//释放内存
            OS_EXIT_CRITICAL();
        }
    }
}
/*******************************End of File************************************/






