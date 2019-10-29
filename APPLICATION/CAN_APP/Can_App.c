/********************************Copyright (c)**********************************\
**                   (c) Copyright 2019, Main, China
**                           All Rights Reserved
**                           By(��ʵ����ҽ�ƿƼ����޹�˾)
**----------------------------------�ļ���Ϣ------------------------------------
** �ļ�����: Can_App.c
** ������Ա: ����
** ��������: 2019-08-01
** �ĵ�����:
******************************************************************************/
#include "myconfig.h"
/****************************************************************
����  wangkai�����շ��䴦�����񣿣���
��ڲ�������
����ֵ����;
*****************************************************************/
void CAN1_Task(void *pdata)
{
    u8 err = 0;
    pdata = pdata;
    while(1)
    {
        OSSemPend(can1RecvSem,0,&err);//wangkai���ж��в���
        //Led_Blink_Mode(LED_NUM_1, &LED_MODE_3);
        CAN_DataDeal(CAN1_CHANNEL,g_canDataFrame[CAN1_CHANNEL]);
    }
}

/****************************************************************
���ܣ�   wangkai�����ܷ��䴦�����񣿣�����
��ڲ�������
����ֵ����;
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
���ܣ�CAN1����֡��������
��ڲ�������
����ֵ����;
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
���ܣ�CAN2����֡��������
��ڲ�������
����ֵ����;
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
���ܣ�CAN1Ӧ��֡��������
��ڲ�������
����ֵ����;
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
���ܣ�CAN2Ӧ��֡
��ڲ�������
����ֵ����;
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
���ܣ�CAN1��������֡����
��ڲ�����void *pdata�û�����ָ��
����ֵ����
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
            CAN1_Send_Frame(g_can1SendFrame->nextMsg);  //����֡��Ϣ
            /*�˴���ȴ�һ����Ӧ�ź������û�У��Ͳ���ɾ���ڴ�wangkai:�ȴ�ʱ��ĳ���һֱ�ȴ�*/

            //�ͷ��ڴ�
            deleteNode = g_can1SendFrame->nextMsg;
            g_can1SendFrame->nextMsg = deleteNode->nextMsg;
            OS_ENTER_CRITICAL();
            myfree(SRAMIN,deleteNode);
            OS_EXIT_CRITICAL();
        }
    }
}
/****************************************************************
���ܣ�CAN2��������֡����
��ڲ�����void *pdata�û�����ָ��
����ֵ����
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
            CAN2_Send_Frame(g_can2SendFrame->nextMsg);  //����֡��Ϣ
            /*�˴���ȴ�һ����Ӧ�ź������û�У��Ͳ���ɾ���ڴ�wangkai:�ȴ�ʱ��ĳ���һֱ�ȴ�*/
            deleteNode = g_can2SendFrame->nextMsg;
            g_can2SendFrame->nextMsg = deleteNode->nextMsg;
            OS_ENTER_CRITICAL();
            myfree(SRAMIN,deleteNode);	//�ͷ��ڴ�
            OS_EXIT_CRITICAL();
        }
    }
}
/*******************************End of File************************************/






