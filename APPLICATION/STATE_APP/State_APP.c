/********************************Copyright (c)**********************************\
**                   (c) Copyright 2019, Main, China
**                           All Rights Reserved
**                           By(达实久信医疗科技有限公司)
**----------------------------------文件信息------------------------------------
** 文件名称: State_APP.c
** 创建人员: 王凯
** 创建日期: 2019-09-23
** 文档描述: 状态改变上报线程（监控线程）
*/
#include "myconfig.h"
/*填充两个状态对比组*/
void FILL_STATE(u8 * point)
{
    *point=TransStatus.DeviceMode;//工作模式
    *(point+1)=TransStatus.WarningCode;//警告代码
    *(point+2)=TransStatus.ErrorCode;//错误代码
    *(point+3)=(u8)TransStatus.DockedNumber;//对准的轨道号
    *(point+4)=TransStatus.TrackUse.TrackStatus;//设备状态
}
/*状态上传任务，当检测到前一个状态和后一个状态不一样的时候，向域上传状态*/
void STATE_Task(void *pdata)
{
    OS_CPU_SR cpu_sr;
    u8 BackUP_STATE[5];//备份状态
    u8 Now_STATE[5];//当前状态
    CAN_DATA_FRAME *NotTrueNode;
    memset(BackUP_STATE,0,sizeof(BackUP_STATE));//将备份数组清空，以便第一次上电立刻向域发送一个帧
    //FILL_STATE(BackUP_STATE);//备份状态入数组
    pdata=pdata;
    while(1)
    {
        FILL_STATE(Now_STATE);//当前状态入数组
        if(memcmp(BackUP_STATE,Now_STATE,5)==0)//说明两个状态数组相等，相等不用上传
        {
            OSTimeDlyHMSM(0,0,0,300);
        }
        else//不相等的时候，说明状态发生了改变
        {
            memcpy((void *)BackUP_STATE,(void *)Now_STATE,sizeof(Now_STATE));//先把当前状态给备份状态
            OS_ENTER_CRITICAL();
            NotTrueNode = (CAN_DATA_FRAME *)mymalloc(SRAMIN,sizeof(CAN_DATA_FRAME));//申请内存
            OS_EXIT_CRITICAL();
            if(NotTrueNode!=NULL)
            {
                NotTrueNode->id.canId=0x00000201|(ThisTransitionNumber<<16);//这个can的帧填充本机设备号码
                SendTransStatus(NotTrueNode);//发送状态
            }
            OS_ENTER_CRITICAL();
            myfree(SRAMIN,NotTrueNode);
            OS_EXIT_CRITICAL();
        }
    }
}
/**模式改变线程，当检测到模式变更需求的时候，申请改变**/
void ModeChange_Task(void *pdata)
{
    OS_CPU_SR cpu_sr;
    u32 * ModeAdd1;
    u8 osser;
    pdata=pdata;
    while(1)
    {
        ModeAdd1=(u32 *)OSMboxPend(ApplyChangeModeBox,0,&osser);//等待模式变更信号邮箱
        Mode_Change(*ModeAdd1-1);//模式变更
        OS_ENTER_CRITICAL();
        myfree(SRAMIN,ModeAdd1);
        OS_EXIT_CRITICAL();
    }

}

/*******************************End of Head************************************/

