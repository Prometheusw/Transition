/********************************Copyright (c)**********************************\
**                   (c) Copyright 2019, Main, China
**                           All Rights Reserved
**                           By(达实久信医疗科技有限公司)
**----------------------------------文件信息------------------------------------
** 文件名称: CarMission_Dri.c
** 创建人员: 王凯
** 创建日期: 2019-06-29
** 文档描述: 轨道任务所需要的底层函数
******************************************************************************/
#include "myconfig.h"
CAR_CHANGEMISSION_DATA *g_CarApplyChangedata;//小车申请转轨任务头结点

//OS_EVENT * CarApplyPassTransSem;
OS_EVENT * CarApplyPassTransMbox;//链表分析任务，为预动任务发送的信号邮箱，发送到申请通过转轨器任务
OS_EVENT * CarApplyChangeTranMbox;//链表分析任务，为变化轨道任务发送的信号邮箱，发送到申请转轨任务
OS_EVENT * TranPreMissionMbox;
/*******************************************************************************
** 函数名称: CarMission_Software_Init
** 功能描述: 小车通过转轨器任务链表头结点初始化
** 参数说明: None
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-06-29
********************************************************************************/
void CarMission_Software_Init(void)
{
    //初始化申请转轨任务头结点
    g_CarApplyChangedata = (CAR_CHANGEMISSION_DATA*)mymalloc(SRAMIN,sizeof(CAR_CHANGEMISSION_DATA));
    if(g_CarApplyChangedata!=NULL)
    {
        g_CarApplyChangedata->NextMission=NULL;
    }
}

/*******************************************************************************
** 函数名称: Choose_TransLocate_Change
** 功能描述: 带选中的活轨号和要到的定轨号的轨道转换函数
** 参数说明: MovingTrack: 选中的活轨
**			     FixedTrack: 要到的定轨号
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-08-14
** 补充说明：当返回值是0xffff时候，说明当前活轨到达不了目标定轨
********************************************************************************/
u16 Choose_TransLocate_Change(u8 MovingTrack,u8 FixedTrack)
{
    OS_CPU_SR cpu_sr;
    CAN_DATA_FRAME * tempNodel;
//    u8 count;//计数
    INT8U oserr;
    INT32S InsertingValue;
    OS_ENTER_CRITICAL();
    tempNodel = (CAN_DATA_FRAME *)mymalloc(SRAMIN,sizeof(CAN_DATA_FRAME));//统一使用申请内存
    if(tempNodel!=NULL)
    {
        OS_EXIT_CRITICAL();
        memset(tempNodel,0,sizeof(CAN_DATA_FRAME));//将申请的内存清零
    }

    if(TrackCount[0][FixedTrack]==0xffff)//如果是ffff的话 说明这个定轨到不了这个动轨
    {
        OS_ENTER_CRITICAL();
        myfree(SRAMIN,tempNodel);
        OS_EXIT_CRITICAL();
        return 0xffff;//返回错误信息
    }
    /**************************当所需要定位的轨道距离零点最近，校零****************************************/
		/*2019年10月21日17点01分修改：当轨道要到的位置是零点方向并且不是找零任务的时候，才进行逻辑判读*/
		/*逻辑判断：如果轨道是最近的轨道或者达到阈值，找零*/
    if(MovingTrack!=0xff&&FixedTrack<(u8)(TransStatus.DockedNumber))//当不是校零任务引起的轨道切换，才会执行下面的逻辑
    {
        if(FixedTrack==Find_NearestTrack()||ThresholdValue>=MAXThresholdValue)//如果目标轨道号是最近的轨道，，或者阈值到达上限，这个时候强制校零
        {
            FindZeroFuction(Needt_Back);//找零函数
            ThresholdValue=0;//阈值变成0
        }
				
    }
    else//如果等于0xff，说明是校零的任务，这时候将值编程0；
    {
        MovingTrack=0;
    }
    InsertingValue=(INT32S)(TrackCount[MovingTrack][FixedTrack]-TransStatus.EncoderCount);//目标轨道号和当前编码差值（有符号数）
    if(InsertingValue==(INT32S)0)//如果当前的编码数和需要移动的轨道号编码数相等（差值为0）
    {
        TransStatus.DockedNumber=MovingTrack<<8|FixedTrack;
        OS_ENTER_CRITICAL();
        myfree(SRAMIN,tempNodel);
        OS_EXIT_CRITICAL();
        return TransStatus.DockedNumber;//返回当前那一条活轨对准哪一条定轨
    }
    else if(InsertingValue!=(INT32S)0)//当前的轨道编码和需要移动的编码不相等
    {
        tempNodel->canMsg.dataBuf[3]=InsertingValue>>24;
        tempNodel->canMsg.dataBuf[2]=InsertingValue>>16;
        tempNodel->canMsg.dataBuf[1]=InsertingValue>>8;//差值的高8位给帧的数据部分第1个字节
        tempNodel->canMsg.dataBuf[0]=InsertingValue;//低8位给第0个字节
        OSMboxPost(StepRunMbox,tempNodel);//将tempNode的值发送到步进电机执行线程中
        TransStatus.TrackUse.Usebit.Monopolize=T_Yes;//独享位置1
        /*此处需要无限等待，否则会出现问题，什么时候电机执行完是未知的，无法判断*/
        OSSemPend(stepFreeSem,0,&oserr);//等待步进电机执行线程返回的信号量 电机释放
        /**如果在执行线程中使用了急停按键，这时候轨道的位置未知*/
        if(EnableStopButton==T_Yes)
        {
            TransStatus.DockedNumber=0xff;//改变正好对准的轨道号(急停情况下未知位置)
            EnableStopButton=T_No;
        }
        else
        {
            TransStatus.DockedNumber=MovingTrack<<8|FixedTrack;//改变正好对准的轨道号
        }
        ThresholdValue++;//阈值++，说明转轨器执行了一次轨道切换任务

        TransStatus.TrackUse.Usebit.Monopolize=T_No;//独享位清零
        //myfree(SRAMIN,tempNodel);//此处不可以释放释放结点，在电机执行线程中把这个地址释放
        return TransStatus.DockedNumber;//返回当前那一条活轨对准哪一条定轨
    }

    return TransStatus.DockedNumber;//返回当前那一条活轨对准哪一条定轨

}
/*******************************************************************************
** 函数名称: TransLocate_Change
** 功能描述: TransLocate(轨道号)转轨函数,在这个函数中将需要转到的轨道号编码数和当前
编码进行比较，然后临时使用一个CAN帧结点，将编码数的差值发送给电机转动线程，在电机转动线程中
执行要转动的步数。然后等待电机释放信号量，data[0]和打他[1]组成了一个十六位的有符号数，
这个16位的数就是需要在步进电机转动线程执行的步数
** 参数说明: TransNum: [输入/出]
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-06-28
** 改动：对不同活轨到目标轨道的编码进行比较，找到其中的最小值，将差值替换成32位有符号数
2019年8月13日08:58:17:将返回值改成16位数，前八位是动轨编号，后八位是当前对准的定轨编号
********************************************************************************/
u16 TransLocate_Change(u8 TransNum)
{
    CAN_DATA_FRAME * tempNodel;
    u8 count;//计数
    u8 ChooseTrack;//离目标轨道最近的活动轨道编号
    INT8U oserr;
    INT32S InsertingValue;//选中的动轨到目标轨道号的差值//有符号数  16位不够用的了，改32位
    INT16U InsertingValueGroup[MAX_MovingTrack];//当前轨道号和目标轨道号差值（有符号数）不同活动轨和定轨
    INT16U MIN_Group=0xff;//临时变量，用于找到数组中的最小数
    memset(InsertingValueGroup,0xffff,sizeof(InsertingValueGroup));//将临时差值数组全部填充为最大

    for(count=0; count<MAX_MovingTrack; count++)
    {
        /*2019年8月12日13:24:32求取每一根活动轨和当前轨道号的差值绝对值*/
        if(TrackCount[count][TransNum]!=0)//等于0说明count号活轨到不到TransNum号定轨
        {
            InsertingValueGroup[count]=abs(TrackCount[count][TransNum]-TransStatus.EncoderCount);
        }
    }


    for(count=0; count<MAX_MovingTrack; count++)
    {
        if(InsertingValueGroup[count]<MIN_Group)
        {
            MIN_Group=InsertingValueGroup[count];
            ChooseTrack=count;
        }
    }

    tempNodel = (CAN_DATA_FRAME *)mymalloc(SRAMIN,sizeof(CAN_DATA_FRAME));//统一使用申请内存
    memset(tempNodel,0,sizeof(CAN_DATA_FRAME));//将申请的内存清零

    InsertingValue=(INT32S)(TrackCount[ChooseTrack][TransNum]-TransStatus.EncoderCount);//目标轨道号和当前编码差值（有符号数）
    if(InsertingValue==(INT32S)0)//如果当前的编码数和需要移动的轨道号编码数相等（差值为0）
    {
        TransStatus.DockedNumber=TransNum;
        return TransStatus.DockedNumber|ChooseTrack<<8;//返回当前那一条活轨对准哪一条定轨
    }
    else if(InsertingValue!=(INT32S)0)//当前的轨道编码和需要移动的编码不相等
    {
        tempNodel->canMsg.dataBuf[3]=InsertingValue>>24;
        tempNodel->canMsg.dataBuf[2]=InsertingValue>>16;
        tempNodel->canMsg.dataBuf[1]=InsertingValue>>8;//差值的高8位给帧的数据部分第1个字节
        tempNodel->canMsg.dataBuf[0]=InsertingValue;//低8位给第0个字节
        OSMboxPost(StepRunMbox,tempNodel);//将tempNode的值发送到步进电机执行线程中
        TransStatus.TrackUse.Usebit.Monopolize=T_Yes;//独享位置1

        OSSemPend(stepFreeSem,0,&oserr);//等待步进电机执行线程返回的信号量 电机释放

        TransStatus.DockedNumber=TransNum;//改变正好对准的轨道号
        TransStatus.TrackUse.Usebit.Monopolize=T_No;//独享位清零
        //myfree(SRAMIN,tempNodel);//此处不可以释放释放结点，在电机执行线程中把这个地址释放
        return TransStatus.DockedNumber|ChooseTrack<<8;//返回当前那一条活轨对准哪一条定轨
    }

    return TransStatus.DockedNumber|ChooseTrack<<8;//返回当前那一条活轨对准哪一条定轨

}
/*******************************************************************************
** 函数名称: Apply_Pass_Mission
** 功能描述: 轨道需要处理的机械任务入链表
** 参数说明: tempNode: 数据结点指针
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-06-29
改动说明：2019-7-5王凯：将找零任务也接到转轨器任务链表中来，找零分为手动与自动模式
防止在自动模式下不必要的校零引起事故。方便统一管理，CAR_CHANGEMISSION_DATA中增加找零任务标记
********************************************************************************/
void Apply_Change_Mission(CAN_DATA_FRAME * tempNode)
{
    OS_CPU_SR cpu_sr;
    CAR_CHANGEMISSION_DATA *NewMissionNode;//新任务数据结点
    CAR_CHANGEMISSION_DATA *TempMissionNode;//临时结点，指向申请转轨任务头结点
    INT8U ParityJudge;//奇数偶数判断
    OS_ENTER_CRITICAL();
    NewMissionNode = (CAR_CHANGEMISSION_DATA*)mymalloc(SRAMIN,sizeof(CAR_CHANGEMISSION_DATA));//申请内存
    if(NewMissionNode!=NULL)//申请成功
    {
        OS_EXIT_CRITICAL();
        memset(NewMissionNode,0,sizeof(CAR_CHANGEMISSION_DATA));
        /**如果是预动的任务，预动任务只能是域发送到转轨器，如果这时候轨道是锁定的，直接返回给域正在执行命令的错误代码*/
        if(tempNode->id.MasteridBit.Subindex==TranPerMission)//如果是预动任务
        {
            NewMissionNode->NextMission=NULL;
            NewMissionNode->InitialPoint=tempNode->canMsg.dataBuf[0];//获得小车的起始位置
            NewMissionNode->PreMission=1;//预动标志置1
            NewMissionNode->MissionMark=0;//是预动任务
            NewMissionNode->TerminalPoint=tempNode->canMsg.dataBuf[0];//起始位和终点位置一样
            NewMissionNode->FarmID=tempNode->id.canId;
        }
        /*如果是直接通过的任务，删掉，这个已经不用了*/
        else if(tempNode->id.idBit.index==CarApplyPass)//如果任务是直接通过的任务
        {
            if((tempNode->canMsg.dataBuf[0])%2==0) ParityJudge=tempNode->canMsg.dataBuf[0]+1;
            else ParityJudge=tempNode->canMsg.dataBuf[0]-1;
            //ParityJudge=(tempNode->canMsg.dataBuf[0])%2;
            if(TrackCount[tempNode->canMsg.dataBuf[0]]==TrackCount[ParityJudge])//如果说轨道和其对号相等，是直接通过指令
            {
                NewMissionNode->NextMission=NULL;
                NewMissionNode->CarNum=tempNode->id.idBit.sendDeviceId;//获得小车的编号（设备号）
                NewMissionNode->InitialPoint=tempNode->canMsg.dataBuf[0];//获得小车的起始位置
                NewMissionNode->PreMission=0;//预动标志置1
                NewMissionNode->MissionMark=1;//是直接通过任务
                NewMissionNode->TerminalPoint=tempNode->canMsg.dataBuf[0];//起始位和终点位置一样
                NewMissionNode->FarmID=tempNode->id.canId;
            }
            else//如果不相等，就相当于一个轨道转换任务，发生轨道切换，给小车发的将不是直接过命令
            {
                NewMissionNode->NextMission=NULL;
                NewMissionNode->CarNum=tempNode->id.idBit.sendDeviceId;//获得小车的编号（设备号）
                NewMissionNode->InitialPoint=tempNode->canMsg.dataBuf[0];//获得小车的起始位置
                NewMissionNode->PreMission=0;//预动标志清0
                NewMissionNode->MissionMark=2;//是轨道切换任务
                NewMissionNode->TerminalPoint=tempNode->canMsg.dataBuf[1];//获得小车的终点位置
                NewMissionNode->FarmID=(tempNode->id.canId&0xffffff00)|CarApplyChange;

            }
        }


        /*轨道变换*/
        else if(tempNode->id.idBit.index==CarApplyChange&&tempNode->id.idBit.MasterslaveBit==ManyMaster)//如果任务是轨道转换任务
        {
            NewMissionNode->NextMission=NULL;
            NewMissionNode->CarNum=tempNode->id.idBit.sendDeviceId;//获得小车的编号（设备号）
            NewMissionNode->InitialPoint=tempNode->canMsg.dataBuf[0];//获得小车的起始位置
            NewMissionNode->PreMission=0;//预动标志清0
            NewMissionNode->MissionMark=2;//是轨道切换任务
            NewMissionNode->TerminalPoint=tempNode->canMsg.dataBuf[1];//获得小车的终点位置
            NewMissionNode->FarmID=tempNode->id.canId;
        }

        /********************************************如果是找零帧******************************************************************/
        else if(tempNode->id.idBit.index==TransFindzero)//如果是找零任务
        {
            /*2019-7-5王凯增加：对于域发送的多主协议，无需获得域设备号，且找零任务只需要知道找零标记即可*/
            NewMissionNode->NextMission=NULL;
            if(tempNode->id.idBit.index==TransFindzero)
            {
                NewMissionNode->MissionMark=3;//是找零任务
            }

            NewMissionNode->PreMission=0;//预动标志0
            NewMissionNode->FarmID=tempNode->id.canId;
        }


        else if(tempNode->id.MasteridBit.Subindex==ControlCheck)//自检任务
        {
            NewMissionNode->MissionMark=4;//是自检任务
            NewMissionNode->PreMission=0;//预动标志0
            NewMissionNode->FarmID=tempNode->id.canId;
        }


        /*如果是轨道定位任务//zhucong*/
        else if(tempNode->id.idBit.index==TransLocation&&tempNode->id.idBit.MasterslaveBit==MasterSlave)
        {
            NewMissionNode->NextMission=NULL;
            NewMissionNode->InitialPoint=tempNode->canMsg.dataBuf[0];//需要到的定轨号码
            NewMissionNode->TerminalPoint=tempNode->canMsg.dataBuf[1];//选中的活轨号码
            NewMissionNode->PreMission=0;//预动标志清0
            NewMissionNode->MissionMark=5;//是轨道定位任务
            NewMissionNode->FarmID=tempNode->id.canId;
        }
        /**轨道移动任务*/
        else if(tempNode->id.idBit.index==TransMove&&tempNode->id.idBit.MasterslaveBit==MasterSlave)
        {
            NewMissionNode->NextMission=NULL;
            NewMissionNode->InitialPoint=tempNode->canMsg.dataBuf[0];//高低
            NewMissionNode->TerminalPoint=tempNode->canMsg.dataBuf[1];
            NewMissionNode->PreMission=0;//预动标志清0
            NewMissionNode->MissionMark=6;//是轨道移动任务
            NewMissionNode->FarmID=tempNode->id.canId;
        }

        /*2019-07-05王凯：增加：当任务节点不是合法的索引，将申请的内存释放后，返回*/
        else
        {
            OS_ENTER_CRITICAL();
            myfree(SRAMIN,NewMissionNode);
            OS_EXIT_CRITICAL();
            return;
        }
        NewMissionNode->TimeMark=OSTimeGet();//获得时间标志
        TempMissionNode=g_CarApplyChangedata;//临时结点指向头结点
        while(TempMissionNode->NextMission!=NULL)//把新的结点接在链表的最后
        {
            TempMissionNode=TempMissionNode->NextMission;
        }
        TempMissionNode->NextMission=NewMissionNode;
        NewMissionNode->NextMission=NULL;
				//printf("任务已经入队列表\r\n");
    }
    else
    {
        OS_ENTER_CRITICAL();
        myfree(SRAMIN,NewMissionNode);
        OS_EXIT_CRITICAL();

    }
}

/*******************************************************************************
** 函数名称: Mission_Analyse
** 功能描述: 转轨器任务链表分析函数,在这个函数中，对转轨器变换的任务链表进行统一分配
** 参数说明: headmissionnode: [输入/出]
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-07-01
********************************************************************************/
void Mission_Analyse(CAR_CHANGEMISSION_DATA * headmissionnode)
{
    OS_CPU_SR cpu_sr;
    CAR_CHANGEMISSION_DATA *TempMissionNode=NULL;//临时结点，指向申请转轨任务头结点
    CAR_CHANGEMISSION_DATA *DeleteNode=NULL;//分析完成需要删除free的结点
    CAN_DATA_FRAME *CopyTempNode0;
    TempMissionNode = headmissionnode;//指向头结点

    if(TempMissionNode->NextMission!=NULL)
    {
        /***********域控制器发来的预动命令***************/
        if(TempMissionNode->NextMission->MissionMark==0)//预动任务
        {
            OSMboxPost(TranPreMissionMbox,TempMissionNode->NextMission);
            TransStatus.TrackUse.Usebit.ExeCommands=T_Yes;//正在执行命令
            DeleteNode=TempMissionNode->NextMission;
            TempMissionNode->NextMission=DeleteNode->NextMission;//链表中删除结点，结点在执行线程中释放

        }

        /*******************************小车直接通过的指令已经不用**************************************/
        else if(TempMissionNode->NextMission->MissionMark==1)//如果为小车直接通过任务
        {
            OSMboxPost(CarApplyPassTransMbox,TempMissionNode->NextMission);
            TransStatus.TrackUse.Usebit.ExeCommands=T_Yes;//正在执行命令
            DeleteNode=TempMissionNode->NextMission;
            TempMissionNode->NextMission=DeleteNode->NextMission;//链表中删除结点，结点在执行线程中释放
        }

        /***********************************小车需要转换的命令********************************************/
        else if(TempMissionNode->NextMission->MissionMark==2)//如果为轨道转换任务
        {
            OSMboxPost(CarApplyChangeTranMbox,TempMissionNode->NextMission);
 //printf(" 小车%d号从%d号轨道到%d号轨道任务开始执行\r\n",TempMissionNode->CarNum,TempMissionNode->InitialPoint,TempMissionNode->TerminalPoint);
            TransStatus.TrackUse.Usebit.ExeCommands=T_Yes;//正在执行命令
            DeleteNode=TempMissionNode->NextMission;
            TempMissionNode->NextMission=DeleteNode->NextMission;//链表中删除结点，结点在执行线程中释放
        }

        /********************************************找零任务，貌似可以通用****************************************************/
        else if(TempMissionNode->NextMission->MissionMark==3)//如果是找零任务（在主从模式下，等于3，为了和多主模式区别）
        {
            OS_ENTER_CRITICAL();
            CopyTempNode0 = (CAN_DATA_FRAME *)mymalloc(SRAMIN,sizeof(CAN_DATA_FRAME));
            OS_EXIT_CRITICAL();
            memset(CopyTempNode0,0,sizeof(CAN_DATA_FRAME));
            CopyTempNode0->id.canId = TempMissionNode->NextMission->FarmID;
            OSQPost(TransLocationQeue,CopyTempNode0);//发送
            // OSMboxPost(TransFindzeroMbox,TempMissionNode->NextMission);
            TransStatus.TrackUse.Usebit.FindzeroValid=T_Yes;
            TransStatus.TrackUse.Usebit.ExeCommands=T_Yes;//正在执行命令

            DeleteNode=TempMissionNode->NextMission;
            TempMissionNode->NextMission=DeleteNode->NextMission;
            OS_ENTER_CRITICAL();
            myfree(SRAMIN,DeleteNode);
            OS_EXIT_CRITICAL();
        }

        /****************************自检任务，校零再到最远处再到零号位置******************************************/
        else if(TempMissionNode->NextMission->MissionMark==4)//如果自检
        {
            //OSMboxPost(TransFindzeroMbox,TempMissionNode->NextMission);//jiaoling
            OS_ENTER_CRITICAL();
            CopyTempNode0 = (CAN_DATA_FRAME *)mymalloc(SRAMIN,sizeof(CAN_DATA_FRAME));
            if(CopyTempNode0!=NULL)
            {
                OS_EXIT_CRITICAL();
                memset(CopyTempNode0,0,sizeof(CAN_DATA_FRAME));
                CopyTempNode0->id.canId = TempMissionNode->NextMission->FarmID;
                OSQPost(TransLocationQeue,CopyTempNode0);//发送
            }

            TransStatus.TrackUse.Usebit.FindzeroValid=T_Yes;
            TransStatus.TrackUse.Usebit.ExeCommands=T_Yes;//正在执行命令

            DeleteNode=TempMissionNode->NextMission;
            TempMissionNode->NextMission=DeleteNode->NextMission;
            OS_ENTER_CRITICAL();
            myfree(SRAMIN,DeleteNode);
            OS_EXIT_CRITICAL();
        }


        /*************************************定位任务，目前只考虑单T*****************************************************/
        else if(TempMissionNode->NextMission->MissionMark==5)//是定位任务
        {
            TransStatus.TrackUse.Usebit.ExeCommands=T_Yes;//正在执行命令
            OS_ENTER_CRITICAL();
            CopyTempNode0 = (CAN_DATA_FRAME *)mymalloc(SRAMIN,sizeof(CAN_DATA_FRAME));
            if(CopyTempNode0!=NULL)
            {
                OS_EXIT_CRITICAL();
                memset(CopyTempNode0,0,sizeof(CAN_DATA_FRAME));
                CopyTempNode0->canMsg.dataBuf[0]=TempMissionNode->NextMission->InitialPoint;//需要到达的定轨号
                CopyTempNode0->canMsg.dataBuf[1]=TempMissionNode->NextMission->TerminalPoint;//选中的活轨号

                CopyTempNode0->id.canId = TempMissionNode->NextMission->FarmID;
                OSQPost(TransLocationQeue,CopyTempNode0);//发送
                DeleteNode=TempMissionNode->NextMission;
                TempMissionNode->NextMission=DeleteNode->NextMission;//链表中删除结点，结点在执行线程中释放
                OS_ENTER_CRITICAL();
                myfree(SRAMIN,DeleteNode);
                OS_EXIT_CRITICAL();
            }
        }


        /**********************************************移动任务，移动多少个编码数，正负之分*******************************************************/
        else if(TempMissionNode->NextMission->MissionMark==6)//是移动任务
        {
            OS_ENTER_CRITICAL();
            CopyTempNode0 = (CAN_DATA_FRAME *)mymalloc(SRAMIN,sizeof(CAN_DATA_FRAME));
            if(CopyTempNode0!=NULL)
            {
                OS_EXIT_CRITICAL();
                memset(CopyTempNode0,0,sizeof(CAN_DATA_FRAME));
                CopyTempNode0->canMsg.dataBuf[0]=TempMissionNode->NextMission->InitialPoint;//移动的编码数
                CopyTempNode0->canMsg.dataBuf[1]=TempMissionNode->NextMission->TerminalPoint;//移动的编码数
                if((TempMissionNode->NextMission->TerminalPoint)>>7==0)//是正数
                {
                    CopyTempNode0->canMsg.dataBuf[2]=0x00;
                    CopyTempNode0->canMsg.dataBuf[3]=0x00;
                }
                else if((TempMissionNode->NextMission->TerminalPoint)>>7==1)//是负数
                {
                    CopyTempNode0->canMsg.dataBuf[2]=0xff;
                    CopyTempNode0->canMsg.dataBuf[3]=0xff;
                }

                CopyTempNode0->id.canId = TempMissionNode->NextMission->FarmID;
                OSQPost(TransLocationQeue,CopyTempNode0);//发送
                DeleteNode=TempMissionNode->NextMission;
                TempMissionNode->NextMission=DeleteNode->NextMission;//链表中删除结点，结点在执行线程中释放
                OS_ENTER_CRITICAL();
                myfree(SRAMIN,DeleteNode);
                OS_EXIT_CRITICAL();
            }
        }


        /*2019-07-05王凯：增加else语句，当不是合法的任务，将这个节点从链表中移除并且释放*/
        else
        {
            DeleteNode=TempMissionNode->NextMission;
            TempMissionNode->NextMission=DeleteNode->NextMission;
            OS_ENTER_CRITICAL();
            myfree(SRAMIN,DeleteNode);
            OS_EXIT_CRITICAL();
        }
//        DeleteNode=TempMissionNode->NextMission;
//        TempMissionNode->NextMission=DeleteNode->NextMission;//链表中删除结点，结点在执行线程中释放
        /*此处千万不能释放，释放以后在执行线程中如果调用子函数的话，就会认为这一块内存已经释放，从而覆盖了任务的数据*/
//			OS_ENTER_CRITICAL();
//      myfree(SRAMIN,DeleteNode);
//      OS_EXIT_CRITICAL();
//
    }
    else
    {
        DeleteNode=TempMissionNode->NextMission;
        TempMissionNode->NextMission=DeleteNode->NextMission;
        OS_ENTER_CRITICAL();
        myfree(SRAMIN,DeleteNode);
        OS_EXIT_CRITICAL();
        return;//结束这个函数
    }
    return;//结束这个函数!!!!!!!!!!!!!!!
}

/*******************************************************************************
** 函数名称: TrackCount_Init
** 功能描述:从W25Q中装载转轨器标定参数
** 参数说明: None
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-07-03
********************************************************************************/
bool TrackCount_Load(void)
{
    INT8U W25QCount=0;
    INT8U MovingCount=0;

    W25QXX_Read(W25QXXReceiveBuff,0,sizeof(W25QXXReceiveBuff));
//	   for(W25QCount=0; W25QCount<40; W25QCount++)
//	{
//		if(W25QXXReceiveBuff[W25QCount]==0xff)
//	   W25QXXReceiveBuff[W25QCount]=0;
//	}
//    W25QCount=0;
//在主区域和备份区域校验之后，从主数据区域开始,读砏25QXXReceiveBuff个字节
    for(MovingCount=0; MovingCount<MAX_MovingTrack; MovingCount++) //最大动轨，有几就有几组数据
    {
        for(W25QCount=0; W25QCount<MAX_FixedTrack; W25QCount++)
        {
            TrackCount[MovingCount][W25QCount]=W25QXXReceiveBuff[32*MovingCount+W25QCount*2]+
                                               (W25QXXReceiveBuff[32*MovingCount+W25QCount*2+1]<<8);
        }
    }

//		t=crc16(W25QXXReceiveBuff,sizeof(TrackCount));
//      if((W25QXXReceiveBuff[32]<<8)+W25QXXReceiveBuff[33]==t)
//			{
    return true;
//			}
//			else
//				return false;
}

/*******************************************************************************
** 函数名称: TrackCount_Save
** 功能描述: 将转轨器的轨道编码数保存到flash中
** 参数说明: None
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-07-06
********************************************************************************/
void TrackCount_Save(void)
{
    INT8U W25QCount=0;
    INT8U MovingCount=0;

    memset(W25QXXSendBuff,0,sizeof(W25QXXSendBuff));

    for(MovingCount=0; MovingCount<MAX_MovingTrack; MovingCount++) //最大动轨，有几就有几组数据
    {
        for(W25QCount=0; W25QCount<MAX_FixedTrack; W25QCount++)//最大定轨，有几个每组就有多少个数据
        {
            W25QXXSendBuff[32*MovingCount+W25QCount*2]=TrackCount[MovingCount][W25QCount];//低八位给第一个字节
            W25QXXSendBuff[32*MovingCount+W25QCount*2+1]=TrackCount[MovingCount][W25QCount]>>8;//高八位给第二个字节
        }
    }
    W25QXXSendBuff[128]=(crc16(W25QXXSendBuff,sizeof(TrackCount)))>>8;//crc校验码高八位
    W25QXXSendBuff[129]=crc16(W25QXXSendBuff,sizeof(TrackCount));//CRC校验码低八位

    W25QXX_Write((INT8U*)W25QXXSendBuff,0,sizeof(TrackCount)+2);//将编码数备份到主区域//主区域4k大小，一个扇区
    W25QXX_Write((INT8U*)W25QXXSendBuff,FLASH_SECTOR_SIZE,sizeof(TrackCount)+2);//将编码书备份到备份区//4k，一个扇区
}

/*******************************************************************************
** 函数名称: FindZeroFuction
** 功能描述: 找零功能封装
** 参数说明: None
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-07-18
********************************************************************************/
void FindZeroFuction(u8 mode)
{
    INT8U oserr;
	  TransStatus.DockedNumber=0xffff;
    if ((GPIOB->IDR & 0x02) == 0x00)//如果一上电就在零点开关上，反向走2000个脉冲
    {
        if(ZeroDirction==clockwise)
        {
            StepMotor_Run(100,PCtrl,1500);//z转//函数中有PPEND，会让出调度，不会引起阻塞
        }
        else
        {
            StepMotor_Run(100,NCtrl,1500);////函数中有PPEND，会让出调度，不会引起阻塞//顺时针旋转
        }
        OSSemPend(arrivePosSem,300,&oserr);
    }
    TIM_ITConfig(TIM3,TIM_IT_CC3|TIM_IT_CC4,ENABLE);
    while(FindZeroCtrl!=ISOnZero)//如果说零点开关没有被压住，就一直靠近零点开关走
    {
        if(ZeroDirction==clockwise)//如果是顺时针校零
        {
            StepMotor_NCtrl(100);////函数中有PPEND，会让出调度，不会引起阻塞//顺时针旋转
        }
        else
        {
            StepMotor_PCtrl(100);//z转//函数中有PPEND，会让出调度，不会引起阻塞
        }

        /*判断刹车信号是否有效*/
        if(TranferSpeed==TranferStop)//停止标志位0 立刻停止
        {
            TranferSpeed=1;//停止
            EnableStopButton=T_Yes;//是否急停按键按下
            TIM_ITConfig(TIM3,TIM_IT_CC3|TIM_IT_CC4,DISABLE);
            break;//跳出执行，停止
        }
        /*判断找零开关是否有效*/
        if(TransStatus.ErrorCode==MotorOverCurrent)//如果这时候发生电机过流现象,说明卡在最后了，说明教令开关失效
        {
            TransStatus.ErrorCode=CantFindZero;//错误类型细分为找零失效
            TIM_ITConfig(TIM3,TIM_IT_CC3|TIM_IT_CC4,DISABLE);
            return;
        }
    }
    // TIM_ITConfig(TIM3,TIM_IT_CC3|TIM_IT_CC4,DISABLE);

    OSSemPend(arrivePosSem,300,&oserr);
//TIM_ITConfig(TIM3,TIM_IT_CC3|TIM_IT_CC4,ENABLE);
    if(EnableStopButton!=T_Yes)
    {
        while(FindZeroCtrl!=ISAwayZero)//如果此时教令开关没有松开，一直远离零点走
        {
            if(ZeroDirction==clockwise)
            {
                StepMotor_PCtrl(150);//z转//函数中有PPEND，会让出调度，不会引起阻塞
            }
            else
            {
                StepMotor_NCtrl(150);////函数中有PPEND，会让出调度，不会引起阻塞//顺时针旋转
            }
            /*判断是否存在急停按键按下的情况*/
            if(TranferSpeed==TranferStop)//停止标志位0 立刻停止
            {
                TranferSpeed=1;//停止
                EnableStopButton=T_Yes;//是否急停按键按下
                TIM_ITConfig(TIM3,TIM_IT_CC3|TIM_IT_CC4,DISABLE);

                break;//跳出执行，停止
            }
            //TIM_ITConfig(TIM3,TIM_IT_CC3|TIM_IT_CC4,ENABLE);
            /*判断找零开关是否有效*/
            if(TransStatus.ErrorCode==MotorOverCurrent)//如果这时候发生电机过流现象，说明教令开关失效
            {
                TransStatus.ErrorCode=CantFindZero;//错误类型细分为找零失效
                TIM_ITConfig(TIM3,TIM_IT_CC3|TIM_IT_CC4,DISABLE);
                return;
            }
        }
        TIM_ITConfig(TIM3,TIM_IT_CC3|TIM_IT_CC4,DISABLE);
        OSSemPend(arrivePosSem,300,&oserr);
    }
    FindZeroGlob=0;
    BeepChirptimes++;//交一次
    // TransStatus = DefoultTransStatus;//转轨器状态变成初始化态
    if(EnableStopButton==T_Yes)
    {
        TransStatus.DockedNumber=0xffff;//改变正好对准的轨道号(急停情况下未知位置)
        TransStatus.TrackUse.TrackStatus=0x01;


        TransStatus.EncoderCount=0;

    }
    else
    {
        TransStatus.DockedNumber=0xfffe;//-1
        TransStatus.EncoderCount=0;
        TransStatus.ErrorCode=0;
        TransStatus.TrackUse.TrackStatus=0x01;
        TransStatus.WarningCode=0;
    }
    if(mode==Need_Back&&EnableStopButton!=T_Yes)
    {
        Choose_TransLocate_Change(0xff,Find_NearestTrack());//find the nearest track.
    }
    else
    {
			EnableStopButton=T_No;
        return;
    }
		        

}

/*******************************************************************************
** 函数名称: TrackLockFunction
** 功能描述: 轨道锁操作函数，锁定或者解锁轨道号
** 参数说明: lockorun: 锁定还是解锁
**			 trackNumber: 轨道编码号，直接写编码数
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-07-18
********************************************************************************/
u8 TrackLockFunction(u8 lockorun,u8 trackNumber)
{
    if(lockorun == LockTrack)
        TransStatus.TrackUse.TrackStatus=((TransStatus.TrackUse.TrackStatus)|(0x04<<(trackNumber/2)));//锁定轨道置1
    else if(lockorun == UnlockTrack)
        TransStatus.TrackUse.TrackStatus=((TransStatus.TrackUse.TrackStatus)&(~(0x04<<(trackNumber/2))));//解锁轨道清0
    else return 0;

    return TransStatus.TrackUse.TrackStatus;

}
/*******************************End of File************************************/

