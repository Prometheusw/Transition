/********************************Copyright (c)**********************************\
**                   (c) Copyright 2019, Main, China
**                           All Rights Reserved
**                           By(达实久信医疗科技有限公司)
**----------------------------------文件信息------------------------------------
** 文件名称: beep_Dri.c
** 创建人员: 王凯
** 创建日期: 2019-06-29
** 文档描述:
******************************************************************************/
#include "myconfig.h"
u16 BeepChirptimes=0;//蜂鸣器鸣叫次数
u8  BeepAlwaysON=0;//蜂鸣器一直鸣叫开关
u8  BeepOFFORON=1;//默认是开
void Beep_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//GPIOF8
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOF, &GPIO_InitStructure);
    BeepON;
}

void Beep_Chirp(void)
{
	if(BeepOFFORON==0)//如果软件设置关闭蜂鸣器
	{
		BeepChirptimes=0;
		BeepAlwaysON=0;
	}
	if(BeepChirptimes!=0)//蜂鸣次数
	{
		BeepON;
		OSTimeDlyHMSM(0,0,0,200);
		BeepOFF;
	  OSTimeDlyHMSM(0,0,0,50);
		BeepChirptimes--;
	}
	if(BeepAlwaysON==1)//永久蜂鸣
	{
		BeepON;
		OSTimeDlyHMSM(0,0,0,200);
		BeepOFF;
	  OSTimeDlyHMSM(0,0,0,100);
	}
}
/*******************************End of File************************************/

