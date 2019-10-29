/********************************Copyright (c)**********************************\
**                   (c) Copyright 2019, Main, China
**                           All Rights Reserved
**                           By(��ʵ����ҽ�ƿƼ����޹�˾)
**----------------------------------�ļ���Ϣ------------------------------------
** �ļ�����: Application.c
** ������Ա: ����
** ��������: 2019-08-09
** �ĵ�����:
*******************************End of Head************************************/
#include "myconfig.h"
int main(void)
{

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//�жϷ�������
    ParameterInit();//��������
    HardwareInit();//Ӳ����ʼ��
    //MOTOR_DIR_CTRL=1;
    OSInit();
    OSTaskCreate(START_Task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//������ʼ����
    OSStart();
}
/*******************************************************************************
** ��������: start_task
** ��������: ��ʼ���� ������������
** ����˵��: pdata: [����/��]
** ����˵��: None
** ������Ա: ����
** ��������: 2019-04-29
********************************************************************************/
void START_Task(void *pdata)
{
    OS_CPU_SR cpu_sr=0;
    pdata = pdata;
    EventCreate();//�����ź���
    OS_ENTER_CRITICAL();//�����ٽ���(�޷����жϴ��)
    TaskCreate();
    OSTaskSuspend(START_TASK_PRIO);	//������ʼ����.�ó�cpu
    OS_EXIT_CRITICAL();	//�˳��ٽ���(���Ա��жϴ��)
}





