/********************************Copyright (c)**********************************\
**                   (c) Copyright 2019, Main, China
**                           All Rights Reserved
**                           By(��ʵ����ҽ�ƿƼ����޹�˾)
**----------------------------------�ļ���Ϣ------------------------------------
** �ļ�����: Timer_Dri.c
** ������Ա: ����
** ��������: 2019-07-17
** �ĵ�����:
******************************************************************************/
#include "myconfig.h"
T_HeartBeat HeartBeat= {0,0}; //������־
/*******************************************************************************
** ��������: TIM7_Int_Init
** ��������: ��ʱ��7��������֡�����500ms����һ���жϡ�
** ����˵��: arr: �Զ���װ��ֵ
**			 psc: Ԥ��Ƶϵ��
** ����˵��: None
** ������Ա: ����
** ��������: 2019-07-17
********************************************************************************/
void TIM7_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);  ///ʹ��TIM7ʱ��
    TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
    TIM_TimeBaseInitStructure.TIM_Period=arr;   //�Զ���װ��ֵ
    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM7,&TIM_TimeBaseInitStructure);
    TIM_ClearITPendingBit(TIM7,TIM_IT_Update);  //����жϱ�־λ��ֹ�����ͽ����ж�
    TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE); //����ʱ��7�����ж�/*kkw*/
    TIM_Cmd(TIM7,ENABLE); //ʹ�ܶ�ʱ��7
    NVIC_InitStructure.NVIC_IRQChannel=TIM7_IRQn; //��ʱ��7�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=14; //��ռ���ȼ�14
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0; //�����ȼ�1
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}



/*******************************************************************************
** ��������: TIM7_IRQHandler
** ��������: ��ʱ��7ʱ���жϺ���������֡ʹ�ã��Ա���++��
** ����˵��: None
** ����˵��: None
** ������Ա: ����
** ��������: 2019-07-17
********************************************************************************/
void TIM7_IRQHandler(void)
{
    OSIntEnter();
    if(TIM_GetITStatus(TIM7,TIM_IT_Update)==SET) //����ж�
    {
        if(CANCommunicatMode.CanMode.CanModebit.HeartBeatSendbit==T_ON)
        {
            HeartBeat.BeatTime++;
        }
        else
        {
            HeartBeat.BeatTime=0;
        }
    }
    TIM_ClearITPendingBit(TIM7,TIM_IT_Update);  //����жϱ�־λ
    OSIntExit();

}
/*******************************************************************************
** ��������: Timer2_Init
** ��������: Modbus�ֽڼ����ʱ  1us'  1.5�ַ����
** ����˵��: arr: 1000-1
**			 psc: 84-1
** ����˵��: None
** ������Ա: ����
** ��������: 2019-07-31
********************************************************************************/
void Timer2_Init(u16 arr,u16 psc)    //100us����1�θ����¼�
{
    TIM_TimeBaseInitTypeDef timer;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
    TIM_DeInit(TIM2);
    timer.TIM_Period=arr;//   99  1000-1
    timer.TIM_Prescaler=psc;// 84M/72=1MHZ-->1us   84-1
    timer.TIM_ClockDivision=TIM_CKD_DIV1;
    timer.TIM_CounterMode=TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2,&timer);
    TIM_Cmd(TIM2,ENABLE);
    TIM_ITConfig(TIM2, TIM_IT_Update,ENABLE);
    NVIC_InitTypeDef  isr;
    isr.NVIC_IRQChannel=TIM2_IRQn;
    isr.NVIC_IRQChannelCmd=ENABLE;
    isr.NVIC_IRQChannelPreemptionPriority=3;
    isr.NVIC_IRQChannelSubPriority=0;
    NVIC_Init(&isr);   //
}

/*******************************************************************************
** ��������: TIM2_IRQHandler
** ��������: �жϺ�����ModBus��ʱ,1.5�ַ����
** ����˵��: : [����/��]
** ����˵��: None
** ������Ա: ����
** ��������: 2019-07-31
********************************************************************************/
void TIM2_IRQHandler()//��ʱ��2���жϷ����Ӻ���  1msһ���ж�
{
    u8 st;
    OSIntEnter();
    st= TIM_GetFlagStatus(TIM2, TIM_FLAG_Update);
    if(st==SET)
    {
        TIM_ClearFlag(TIM2, TIM_FLAG_Update);
        if(modbus.timrun!=0)
        {
            modbus.timout++;
            if(modbus.timout>=4)  //���ʱ��ﵽ��ʱ��
            {
                modbus.timrun=0;//�رն�ʱ��--ֹͣ��ʱ
                modbus.reflag=1;  //�յ�һ֡����
                OSSemPost(ModBusFlagSem);
            }
        }
    }
    OSIntExit();
}
/*******************************End of File************************************/


