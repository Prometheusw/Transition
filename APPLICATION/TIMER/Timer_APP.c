/********************************Copyright (c)**********************************\
**                   (c) Copyright 2019, Main, China
**                           All Rights Reserved
**                           By(��ʵ����ҽ�ƿƼ����޹�˾)
**----------------------------------�ļ���Ϣ------------------------------------
** �ļ�����: Timer_APP.c
** ������Ա: ����
** ��������: 2019-07-17
** �ĵ�����: ����֡�߳�
******************************************************************************/
#include "myconfig.h"
/*******************************************************************************
** ��������: HEARBEAT_TASK
** ��������: ����֡�����̣߳���û�з��͸����������֡5��֮�󣬾͸������������һ������֡
** ����˵��: pdata: [����/��]
** ����˵��: None
** ������Ա: ����
** ��������: 2019-07-17
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
            TempNode = (CAN_DATA_FRAME *)mymalloc(SRAMIN,sizeof(CAN_DATA_FRAME));//�����ڴ�
					  OS_EXIT_CRITICAL();
//            memset(TempNode->canMsg.dataBuf,0,sizeof(CAN_MSG));
					  memset(TempNode,0,sizeof(CAN_DATA_FRAME));
            TempNode->dataLen=0;
            TempNode->id.MasteridBit.Subindex=0x00;//������
            TempNode->id.MasteridBit.MainIndex=0x00;//������
            TempNode->id.MasteridBit.deviceid=0x00;//�豸��//��ĵ�ַ
            TempNode->id.MasteridBit.ReserveBit=0;//����λ
            TempNode->id.MasteridBit.RetryBit=0;//�ط���־
            TempNode->id.MasteridBit.ackBit=0;//Ӧ��֡��־
            TempNode->id.MasteridBit.SubframeBit=0;//��֡��־
            TempNode->id.MasteridBit.MasterslaveBit=0;//������־
             CAN2_Single_Send(TempNode);//��֡���ͺ���
            
            HeartBeat.BeatTime=0;//��������
					OS_ENTER_CRITICAL();
            myfree(SRAMIN,TempNode);
					OS_EXIT_CRITICAL();
        }
        else OSTimeDlyHMSM(0,0,0,20);
    }

}
/*******************************End of File************************************/
