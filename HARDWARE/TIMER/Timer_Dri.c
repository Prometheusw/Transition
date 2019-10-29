/********************************Copyright (c)**********************************\
**                   (c) Copyright 2019, Main, China
**                           All Rights Reserved
**                           By(达实久信医疗科技有限公司)
**----------------------------------文件信息------------------------------------
** 文件名称: Timer_Dri.c
** 创建人员: 王凯
** 创建日期: 2019-07-17
** 文档描述:
******************************************************************************/
#include "myconfig.h"
T_HeartBeat HeartBeat= {0,0}; //心跳标志
/*******************************************************************************
** 函数名称: TIM7_Int_Init
** 功能描述: 定时器7用作心跳帧间隔，500ms发声一次中断。
** 参数说明: arr: 自动重装载值
**			 psc: 预分频系数
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-07-17
********************************************************************************/
void TIM7_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);  ///使能TIM7时钟
    TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分频
    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
    TIM_TimeBaseInitStructure.TIM_Period=arr;   //自动重装载值
    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM7,&TIM_TimeBaseInitStructure);
    TIM_ClearITPendingBit(TIM7,TIM_IT_Update);  //清除中断标志位防止启动就进入中断
    TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE); //允许定时器7更新中断/*kkw*/
    TIM_Cmd(TIM7,ENABLE); //使能定时器7
    NVIC_InitStructure.NVIC_IRQChannel=TIM7_IRQn; //定时器7中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=14; //抢占优先级14
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0; //子优先级1
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}



/*******************************************************************************
** 函数名称: TIM7_IRQHandler
** 功能描述: 定时器7时钟中断函数，心跳帧使用，对变量++，
** 参数说明: None
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-07-17
********************************************************************************/
void TIM7_IRQHandler(void)
{
    OSIntEnter();
    if(TIM_GetITStatus(TIM7,TIM_IT_Update)==SET) //溢出中断
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
    TIM_ClearITPendingBit(TIM7,TIM_IT_Update);  //清除中断标志位
    OSIntExit();

}
/*******************************************************************************
** 函数名称: Timer2_Init
** 功能描述: Modbus字节间隔定时  1us'  1.5字符间隔
** 参数说明: arr: 1000-1
**			 psc: 84-1
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-07-31
********************************************************************************/
void Timer2_Init(u16 arr,u16 psc)    //100us产生1次更新事件
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
** 函数名称: TIM2_IRQHandler
** 功能描述: 中断函数，ModBus定时,1.5字符间隔
** 参数说明: : [输入/出]
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-07-31
********************************************************************************/
void TIM2_IRQHandler()//定时器2的中断服务子函数  1ms一次中断
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
            if(modbus.timout>=4)  //间隔时间达到了时间
            {
                modbus.timrun=0;//关闭定时器--停止定时
                modbus.reflag=1;  //收到一帧数据
                OSSemPost(ModBusFlagSem);
            }
        }
    }
    OSIntExit();
}
/*******************************End of File************************************/


