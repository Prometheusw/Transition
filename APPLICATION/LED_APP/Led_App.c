/********************************Copyright (c)**********************************\
**                   (c) Copyright 2019, Main, China
**                           All Rights Reserved
**                           By(达实久信医疗科技有限公司)
**----------------------------------文件信息------------------------------------
** 文件名称: Led_App.c
** 创建人员: 王凯
** 创建日期: 2019-06-21
** 文档描述: LED应用层任务，指示正常运行和工作状态
*******************************************************************************/
#include "myconfig.h"
/*******************************************************************************
** 函数名称: RUN_Task
** 功能描述: 指示系统运行任务，Led_Blink_Mode函数中有OStimedelay，进行任务调度
** 参数说明: pdata: [输入/出] LED_NUM_0：灯号码编号；LED_MODE_3：闪烁模式
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-06-13
********************************************************************************/
void RUN_Task(void *pdata)
{
  	pdata=pdata;
	  u8 useage;
	  u8 count=0;
    while(1)
    {
        GPIO_ResetBits(GPIOF,GPIO_Pin_9);
        OSTimeDlyHMSM(0,0,0,500);
//       UpdataNowLoction();
        GPIO_SetBits(GPIOF,GPIO_Pin_9);
        OSTimeDlyHMSM(0,0,0,800);
			  useage=my_mem_perused(SRAMIN);
			  count++;
			if(count==9)
					{
					count=0;
          printf("LastFramFlag%d;TransStatus.TrackUse.TrackStatus&0x3c=%d\r\n",LastFramFlag,TransStatus.TrackUse.TrackStatus&0x3c);
					LastFramFlag=0;
			  if(TransStatus.DeviceMode!=1||TransStatus.ErrorCode!=0||useage>1)
					/*如果设备工作模式不为1或者错误代码不为无错误或者内存利用率不为1，打印*/
				  {
					
					  printf("TransStatus.DeviceMode=%d;TransStatus.ErrorCode!=%d;内存使用=%d;\r\n",
					  TransStatus.DeviceMode,TransStatus.ErrorCode,useage);
						
				  }
				}
//       UpdataNowLoction();
        Self_Function();//自运行
    }
}

/*******************************************************************************
** 函数名称: LED_Task
** 功能描述: LED_TIME结构体中的数据发生改变，这个任务就会执行其中调用的函数，让不同的灯闪一下
** 参数说明: pdata: [输入/出]
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-06-20
********************************************************************************/
void LED_Task(void *pdata)
{
    pdata=pdata;
    while(1)
    {
        Led_Blink_Times();
        Beep_Chirp();
        OSTimeDlyHMSM(0,0,0,10);
    }
}

/*******************************End of File************************************/

