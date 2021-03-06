/********************************Copyright (c)**********************************\
**                   (c) Copyright 2019, Main, China
**                           All Rights Reserved
**                           By(达实久信医疗科技有限公司)
**----------------------------------文件信息------------------------------------
** 文件名称: TrackMission_APP.c
** 创建人员: 王凯
** 创建日期: 2019-06-29
** 文档描述: 轨道任务
******************************************************************************/
#include "myconfig.h"
/*******************************************************************************
** 函数名称: TranPerMission_Task
** 功能描述: 预动任务，这个任务由域控制器的索引帧引发，预动到一个指定的轨道，并将此轨道锁定
轨道锁在2019-07-18号作出改变。由单编码变成双编码  0 1号对应0号轨道  2 3对应1   4 5对应2  6 7---3
需要预动的轨道号/2 锁定
** 参数说明: pdata: [输入/出]
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-07-18
**改动2019年8月13日10:49:08：轨道锁改成定轨锁，轨道转换函数返回值改成八位动轨号+八位定轨号
********************************************************************************/
void TranPerMission_Task(void *pdata)
{   CAR_CHANGEMISSION_DATA *TempMissionNode=NULL;
    CAN_SEND_FRAME *TempSendFram;
    pdata=pdata;
    INT8U oserr;
    INT16U Num;
    u8 Target;
    u8 chooseTrack;//选择的轨道号
    u8 ErgodicTrack;//遍历轨道号
    u16 D_Value;//差值
    u16 MIN_count=0xffff;//The Min Count;
    OS_CPU_SR cpu_sr;
    pdata=pdata;
    while(1)
    {
        TempMissionNode = (CAR_CHANGEMISSION_DATA *)mymalloc(SRAMIN,sizeof(CAR_CHANGEMISSION_DATA));//任务结点
        if(TempMissionNode!=NULL)//节点申请成功
        {
            TempMissionNode=OSMboxPend(TranPreMissionMbox,0,&oserr);//等待由分析任务发送过来的信号邮箱
            TempSendFram = (CAN_SEND_FRAME *)mymalloc(SRAMIN,sizeof(CAN_SEND_FRAME));//通知的结果帧
            chooseTrack=0xff;//为了防止所有的动轨都到不了这个定轨，一般不会出现这种情况，一般可能是标定的数据丢失
            MIN_count=0xffff;//最小编码筛选变量，设置为最大以便和其他的数进行比较

            /*选择最优的动轨编号*/
            for(ErgodicTrack=0; ErgodicTrack<MAX_MovingTrack; ErgodicTrack++)
            {
                if(TrackCount[ErgodicTrack][TempMissionNode->InitialPoint]!=0xffff)
                {
                    D_Value=abs(TrackCount[ErgodicTrack][TempMissionNode->InitialPoint]-TransStatus.EncoderCount);
                    if(D_Value<MIN_count)
                    {
                        MIN_count=D_Value;
                        chooseTrack=ErgodicTrack;
                    }
                }
            }
            if(chooseTrack==0xff)//如果选择不到动轨，说明目标轨道没有标定或者数据丢失或者域控制器数据没有更新
            {
                OS_ENTER_CRITICAL();
                myfree(SRAMIN,TempSendFram);
                myfree(SRAMIN,TempMissionNode);
                OS_EXIT_CRITICAL();
                TransStatus.TrackUse.Usebit.ExeCommands=T_No;//命令执行完毕
                continue;//下面的函数不再执行
            }
						printf("进入预动线程，预动到 %d 号轨道。\r\n",TempMissionNode->InitialPoint);
					  TrackLockFunction(LockTrack,chooseTrack);//轨道锁//Num的高八位是定位时选择的动轨编号
            Num=Choose_TransLocate_Change(chooseTrack,TempMissionNode->InitialPoint);//调用转轨函数转换轨道
            printf("预动到 %d 号轨道成功！\r\n",TempMissionNode->InitialPoint);
            Target=(u8)Num;//Num低八位是转换后对准的定轨编号
            if(TempMissionNode->InitialPoint==Target)//成功
            {
							//调试注释不发结果帧
                BeepChirptimes++;
                memset(TempSendFram,0,sizeof(CAN_SEND_FRAME));
                TempSendFram->id=TempMissionNode->FarmID;
                TempSendFram->len=1;
                TempSendFram->canMsg.dataBuf[0]=No_Err;
                TempSendFram->nextMsg=NULL;
                CAN_Post_Queue(CAN2_CHANNEL,TempSendFram);//压入主动帧链表，发送通知小车通过的帧，并在发送线程中等带对方的回应量

                TransStatus.TrackUse.Usebit.ExeCommands=T_No;//命令执行完毕
                OS_ENTER_CRITICAL();
                myfree(SRAMIN,TempSendFram);
                myfree(SRAMIN,TempMissionNode);
                OS_EXIT_CRITICAL();
            }
            else
            {
                TransStatus.ErrorCode=Comexefailed;//命令接受到，但是执行错误
                TransStatistics.ErrTimes++;//错误统计数据+1
                OS_ENTER_CRITICAL();
                myfree(SRAMIN,TempSendFram);
                myfree(SRAMIN,TempMissionNode);
                OS_EXIT_CRITICAL();
            }
        }
        else
        {
            TransStatus.ErrorCode=RAMSpillover;//故障代码显示内存溢出
            TransStatistics.ErrTimes++;
            OS_ENTER_CRITICAL();
            myfree(SRAMIN,TempSendFram);
            myfree(SRAMIN,TempMissionNode);
            OS_EXIT_CRITICAL();
        }

    }

}


/*******************************************************************************
** 函数名称: CarApplyPass_Task
** 功能描述: 在多轨道并且轨道有锁定的情况下，这个线程会被启动，并行处理所有的能直接通过的
** 小车任务。
** 参数说明: pdata: [输入/出]
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-08-26
********************************************************************************/
void CarApplyPass_Task(void *pdata)
{
    CAR_CHANGEMISSION_DATA *TempMissionNode=NULL;
    CAN_SEND_FRAME *TempFramdown=NULL;//通知小车下轨的主动帧结点
    pdata=pdata;
    INT8U sserr;
    OS_CPU_SR cpu_sr;
    OS_ENTER_CRITICAL();
    TempFramdown = (CAN_SEND_FRAME *)mymalloc(SRAMIN,sizeof(CAN_SEND_FRAME));//通知小车下轨的主动帧
    OS_EXIT_CRITICAL();
    while(1)
    {
        while((TransStatus.TrackUse.TrackStatus&0x3c)!=0x00)
        {
            /*********************************遍历任务链表，找到其中的所有能直接通过的锁定任务，并行处理********************************/
            CAR_CHANGEMISSION_DATA *ergodicMissionNode=NULL;//临时结点，指向申请转轨任务头结点
            ergodicMissionNode = g_CarApplyChangedata;//指向头结点
            CAR_CHANGEMISSION_DATA *DeleteNode=NULL;//分析完成需要删除free的结点
            u8 choseMoving=0xff;
            u8 SuitMissionCount=0;
            u8 ergodicMoving=0;
            while(ergodicMissionNode->NextMission!=NULL)
            {
                if(ergodicMissionNode->NextMission->MissionMark==2)//如果是轨道转换任务
                {
                    for(ergodicMoving=0; ergodicMoving<MAX_MovingTrack; ergodicMoving++)//遍历四条轨道
                    {
                        /*当有活动轨能连接目标轨道和起轨道，即可以直接通过*/
                        if((TrackCount[ergodicMoving][ergodicMissionNode->NextMission->InitialPoint]==TrackCount[ergodicMoving][ergodicMissionNode->NextMission->TerminalPoint])&&
                                (TrackCount[ergodicMoving][ergodicMissionNode->NextMission->InitialPoint]==TransStatus.EncoderCount)&&
                                (TrackCount[ergodicMoving][ergodicMissionNode->NextMission->InitialPoint]!=0xffff)&&
                                (TrackCount[ergodicMoving][ergodicMissionNode->NextMission->TerminalPoint]!=0xffff))
                        {
                            choseMoving=ergodicMoving;
                        }
                    }
                }
                if(choseMoving!=0xff)
                {
                    TrackLockFunction(LockTrack,choseMoving);//轨道锁
                    memset(TempFramdown,0,sizeof(CAN_SEND_FRAME));
                    TempFramdown->len=2;
                    TempFramdown->id=ergodicMissionNode->NextMission->CarNum<<16|ThisTransitionNumber<<8|CarCanDownTrack|0x14<<24;//索引号要改成通知小车可以下轨道的索引
                    TempFramdown->canMsg.dataBuf[0]=ergodicMissionNode->NextMission->InitialPoint;//起始位
                    TempFramdown->nextMsg=NULL;
                    CAN_Post_Queue(CAN2_CHANNEL,TempFramdown);//压入主动帧链表，发送通知小车下轨的帧，并在发送线程中等带对方的回应量
                    choseMoving=0xff;
                    SuitMissionCount++;
                }
                DeleteNode=ergodicMissionNode->NextMission;
                ergodicMissionNode->NextMission=DeleteNode->NextMission;
                OS_ENTER_CRITICAL();
                myfree(SRAMIN,DeleteNode);
                OS_EXIT_CRITICAL();
            }
            for(SuitMissionCount=SuitMissionCount+1; SuitMissionCount!=0; SuitMissionCount--)
            {
                OSSemPend(CarAlreadyDownSem,0,&sserr);//等待小车已经下轨信号量
                TrackLockFunction(UnlockTrack,TempMissionNode->TerminalPoint);//轨道锁
            }
        }
    }
}

/*****************************-++核心任务++-*****************************************
** 函数名称: CarApplyChange_Task
** 功能描述: 小车申请转轨任务//执行线程
** 参数说明: pdata: [输入/出]
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-07-02
********************************************************************************/
void CarApplyChange_Task(void *pdata)
{
    INT8U oserr;
    INT8U sserr;
	  u8 *nowcarnum;
    INT16U Num;
    INT8U InitialTrackNum=0;//起始轨道号
    INT8U TerminalTrackNum=0;//终点轨道号
    u8 ergodicMoving=0;//动轨遍历参数
    u16 deffValue;//差值
    u16 MIN_Value=0xffff;//最小差值
    u8  BestchoseMoving;//最优挑选动轨号
    OS_CPU_SR cpu_sr;
    CAR_CHANGEMISSION_DATA *TempMissionNode=NULL;//临时结点接受任务结点数据
    CAN_SEND_FRAME *TempFramup=NULL;//通知小车上轨的主动帧结点
    CAN_SEND_FRAME *TempFramdown=NULL;//通知小车下轨的主动帧结点
	  CAN_SEND_FRAME *CarIsDownTrack=NULL;//通知域控制器小车正在下轨道的帧节点，便于域控制器计算路径

    while(1)
    {
        TempFramup = (CAN_SEND_FRAME *)mymalloc(SRAMIN,sizeof(CAN_SEND_FRAME));//通知小车上轨的主动帧
        TempFramdown = (CAN_SEND_FRAME *)mymalloc(SRAMIN,sizeof(CAN_SEND_FRAME));//通知小车下轨的主动帧

        TempMissionNode=OSMboxPend(CarApplyChangeTranMbox,0,&oserr);//等待由分析任务发送过来的信号邮箱（说明有转轨任务）
        MIN_Value=0xffff;//将这个数拉成最大，以便和其他数比较
        u8 LockedFlag=0;//是否可以直接通过的标志
        // TransStatus.TrackUse.Usebit.Monopolize=T_Yes;//轨道独享位置1
        TransStatus.TrackUse.Usebit.ExeCommands=T_Yes;//正在执行命令
        for(ergodicMoving=0; ergodicMoving<MAX_MovingTrack; ergodicMoving++)
        {
            /*当有活动轨能连接目标轨道和起轨道，即可以直接通过*/
            if((TrackCount[ergodicMoving][TempMissionNode->InitialPoint]==TrackCount[ergodicMoving][TempMissionNode->TerminalPoint])&&
                    (TrackCount[ergodicMoving][TempMissionNode->InitialPoint]!=0xffff)&&(TrackCount[ergodicMoving][TempMissionNode->TerminalPoint]!=0xffff))
            {
                LockedFlag=1;//为1则是可以直接通过的任务
                deffValue=abs(TrackCount[ergodicMoving][TempMissionNode->TerminalPoint]-TransStatus.EncoderCount);
                if(deffValue<MIN_Value)
                {
                    MIN_Value=deffValue;
                    BestchoseMoving=ergodicMoving;
                }
                //  TransStatus.TrackUse.Usebit.Monopolize=T_No;//轨道独享位置0,说明锁定
            }
        }

        /*判断是否可以直接通过，是否为独享任务*/
        if(LockedFlag==0)//没有能直接通过的轨道，说明是独享任务
        {
            MIN_Value=0xffff;//将这个数拉成最大，以便和其他数比较
            TransStatus.TrackUse.Usebit.Monopolize=T_Yes;//轨道独享位置1
            /*最优动轨挑选功能：最优，即当前编码到起始定轨编号+起始到目标编码的绝对值之和MIN*/
            for(ergodicMoving=0; ergodicMoving<MAX_MovingTrack; ergodicMoving++)
            {
                if(TrackCount[ergodicMoving][TempMissionNode->InitialPoint]!=0xffff&&TrackCount[ergodicMoving][TempMissionNode->TerminalPoint]!=0xffff)
                {
                    deffValue=abs(TransStatus.EncoderCount-TrackCount[ergodicMoving][TempMissionNode->InitialPoint])+
                              abs(TrackCount[ergodicMoving][TempMissionNode->TerminalPoint]-TrackCount[ergodicMoving][TempMissionNode->InitialPoint]);

                    if(deffValue<MIN_Value)
                    {
                        MIN_Value=deffValue;
                        BestchoseMoving=ergodicMoving;
                    }
                }
            }

            /*轨道转移到初始位置*函数返回值是动轨+定轨*/
            printf("<事务ID：%llu>（3/14）%d号转轨器开始转移到%d号轨道（起始轨道）!\r\n",printfcount,ThisTransitionNumber,TempMissionNode->InitialPoint);
						//printf("The %d Transition starts to switch to the %d track!\r\n",ThisTransitionNumber,TempMissionNode->InitialPoint);
            TrackLockFunction(LockTrack,BestchoseMoving);//轨道锁
						Num = Choose_TransLocate_Change(BestchoseMoving,TempMissionNode->InitialPoint);//调用转轨函数转换轨道，返回值是转换后的轨道号，用来判断轨道是否转换成功
            InitialTrackNum=(u8)Num;//对准的定轨号码
            if(InitialTrackNum == TempMissionNode->InitialPoint)//轨道已经转换到初始位置成功
            {
							  printf("<事务ID：%llu>（4/14）%d号转轨器转移到%d号轨道成功！\r\n",printfcount,ThisTransitionNumber,TempMissionNode->InitialPoint);
                memset(TempFramup,0,sizeof(CAN_SEND_FRAME));//因为并没有填满整个帧数据，全部设置为零，防止数据错乱
                TempFramup->len=2;
                TempFramup->id=TempMissionNode->CarNum<<16|ThisTransitionNumber<<8|CarCanUpTrack|0x14<<24;//索引号要改成通知小车可以上轨道的索引
                TempFramup->canMsg.dataBuf[0]=TempMissionNode->InitialPoint;//起始位
                TempFramup->canMsg.dataBuf[1]=TempMissionNode->TerminalPoint;//终点位置
                TempFramup->nextMsg=NULL;
                CAN_Post_Queue(CAN2_CHANNEL,TempFramup);//压入主动帧链表，发送通知小车上轨的帧，并在发送线程中等带对方的回应量
							  printf("<事务ID：%llu>（5/14）通知%d号小车上轨道！\r\n",printfcount,TempMissionNode->CarNum);
                nowcarnum = (u8 *)OSMboxPend(CarAlreadyUpMbox,0,&sserr);//等待小车已经上轨信号邮箱
//							while(*nowcarnum!=TempMissionNode->CarNum)//如果接收到的不是此流程的小车号码，那就认为此处有问题，一直接受
//							{
//									nowcarnum = (u8 *)OSMboxPend(CarAlreadyUpMbox,0,&sserr);//等待小车已经上轨信号邮箱
//							}
							printf("<事务ID：%llu>（8/14）%d号小车上轨道已经完成！\r\n",printfcount,TempMissionNode->CarNum);
              TrackLockFunction(UnlockTrack,BestchoseMoving);//轨道锁解锁
            }
            /*转移到初始位置并且上轨流程完成*/
            /*轨道转移到目标位置*/
            //TrackLockFunction(LockTrack,TempMissionNode->TerminalPoint);//轨道锁加锁
					  TrackLockFunction(LockTrack,BestchoseMoving);//轨道锁
						printf("<事务ID：%llu>（9/14）%d号转轨器开始转移到%d号轨道!（终点轨道）\r\n",printfcount,ThisTransitionNumber,TempMissionNode->TerminalPoint);
            Num = Choose_TransLocate_Change(BestchoseMoving,TempMissionNode->TerminalPoint);//调用转轨函数转换轨道，返回值是转换后的轨道号，用来判断轨道是否转换成功
            TerminalTrackNum=(u8)Num;
            if(TerminalTrackNum==TempMissionNode->TerminalPoint)//轨道变换已经成功
            {
							  printf("<事务ID：%llu>（10/14）%d号转轨器转移到%d号轨道成功！\r\n",printfcount,ThisTransitionNumber,TempMissionNode->TerminalPoint);
                memset(TempFramdown,0,sizeof(CAN_SEND_FRAME));
                TempFramdown->len=2;
                TempFramdown->id=TempMissionNode->CarNum<<16|ThisTransitionNumber<<8|CarCanDownTrack|0x14<<24;//索引号要改成通知小车可以下轨道的索引
                TempFramdown->canMsg.dataBuf[0]=TempMissionNode->InitialPoint;//起始位
                TempFramdown->nextMsg=NULL;
                CAN_Post_Queue(CAN2_CHANNEL,TempFramdown);//压入主动帧链表，发送通知小车下轨的帧，并在发送线程中等带对方的回应量
							/*小车刚刚下轨道就给区域控制器发送小车已经下轨状态通知帧*/
							//组包
							  CarIsDownTrack = (CAN_SEND_FRAME *)mymalloc(SRAMIN,sizeof(CAN_SEND_FRAME));//通知区域控制器小车已经下轨道的帧
							  memset(CarIsDownTrack,0,sizeof(CAN_SEND_FRAME));//帧清空
							  CarIsDownTrack->len=1;//长度是1
							  CarIsDownTrack->id=0x04<<24|ThisTransitionNumber<<16|CAN_TRANSFER_MAININDEX<<8|CarIsDowning;
							  CarIsDownTrack->canMsg.dataBuf[0]=TempMissionNode->CarNum;
							  CarIsDownTrack->nextMsg=NULL;
							//组包完成
							  CAN_Post_Queue(CAN2_CHANNEL,CarIsDownTrack);//给区域控制器发
							/*会在发送线程中等待区域控制器的回应*/
							printf("<事务ID：%llu>（11/14）通知%d号小车下轨\r\n",printfcount,TempMissionNode->CarNum);
								TrackLockFunction(LockTrack,BestchoseMoving);//轨道锁
                OSSemPend(CarAlreadyDownSem,5000,&sserr);//等待小车已经下轨信号量
						if(sserr==OS_ERR_NONE)
						{
								printf("<事务ID：%llu>（14/14）%d号小车下轨成功，%d号小车流程结束\r\n\r\n\r\n",printfcount,TempMissionNode->CarNum,TempMissionNode->CarNum);
						}
						else
						{
							printf("超时\r\n");
						}
						
							/*此处应该有跟域要小车位置的帧*/
                TrackLockFunction(UnlockTrack,BestchoseMoving);//轨道锁
                TransStatus.TrackUse.Usebit.Monopolize=T_No;//轨道独享位释放
                TransStatus.TrackUse.Usebit.ExeCommands=T_No;//正在执行命令释放
            }
            alreadlyuptrack=0;//解锁已经上轨
						alreadlydowntrack=0;
            OS_ENTER_CRITICAL();
						myfree(SRAMIN,CarIsDownTrack);
            myfree(SRAMIN,TempFramdown);
            myfree(SRAMIN,TempFramup);
            myfree(SRAMIN,TempMissionNode);
						myfree(SRAMIN,nowcarnum);
            OS_EXIT_CRITICAL();
					  

        }
        else//flag==1，即是可以直接通过的任务
        {

            /*轨道转移到目标位置*/
            //TrackLockFunction(LockTrack,TempMissionNode->TerminalPoint);//轨道锁加锁
            Num = Choose_TransLocate_Change(BestchoseMoving,TempMissionNode->TerminalPoint);//调用转轨函数转换轨道，返回值是转换后的轨道号，用来判断轨道是否转换成功
            TrackLockFunction(LockTrack,BestchoseMoving);//轨道锁上锁，这条动轨已经有车通过
            TerminalTrackNum=(u8)Num;
            if(TerminalTrackNum==TempMissionNode->TerminalPoint)//轨道变换已经成功
            {

                memset(TempFramdown,0,sizeof(CAN_SEND_FRAME));
                TempFramdown->len=2;
                TempFramdown->id=TempMissionNode->CarNum<<16|ThisTransitionNumber<<8|CarCanDownTrack|0x14<<24;//索引号要改成通知小车可以下轨道的索引
                TempFramdown->canMsg.dataBuf[0]=TempMissionNode->InitialPoint;//起始位
                TempFramdown->nextMsg=NULL;
                CAN_Post_Queue(CAN2_CHANNEL,TempFramdown);//压入主动帧链表，发送通知小车下轨的帧，并在发送线程中等带对方的回应量
                /**************************************************************************************************************/
							  OSSemPend(CarAlreadyDownSem,0,&sserr);//等待小车已经下轨信号量//差错处理//预计时间三倍余量没有下轨，进入差错处理
							  
							 
							
                TrackLockFunction(UnlockTrack,BestchoseMoving);//解锁轨道锁
                TransStatus.TrackUse.Usebit.Monopolize=T_No;//轨道独享位释放
                TransStatus.TrackUse.Usebit.ExeCommands=T_No;//正在执行命令释放
            }
            OS_ENTER_CRITICAL();
            myfree(SRAMIN,TempFramdown);
            myfree(SRAMIN,TempFramup);
            myfree(SRAMIN,TempMissionNode);
            OS_EXIT_CRITICAL();
        }
        /*提前把轨道锁上好，防止转轨的时候接受到预动命令*/
        //TrackLockFunction(LockTrack,TempMissionNode->InitialPoint);//轨道锁加锁




        OS_ENTER_CRITICAL();
        myfree(SRAMIN,TempFramdown);
        myfree(SRAMIN,TempFramup);
        myfree(SRAMIN,TempMissionNode);
        OS_EXIT_CRITICAL();

    }
}
/*******************************************************************************
** 函数名称: MissionAnalyse_TASK
** 功能描述: 转轨器任务链表查询任务，当任务链表中有任务并且转轨器空闲的时候，分析并发送不同的信号邮箱
** 参数说明: pdata: [输入/出]
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-07-01
********************************************************************************/
void MissionAnalyse_Task(void *pdata)
{
    pdata=pdata;
    while(1)
    {
//        CAR_CHANGEMISSION_DATA *TempMissionNode=NULL;//临时结点，指向申请转轨任务头结点
//        CAR_CHANGEMISSION_DATA *DeleteNode;
//       TempMissionNode = g_CarApplyChangedata;//指向头结点
        /*用于在当正在执行一个锁定的任务时，再*/

        /*任务链表为空、轨道上锁或者独享时，让出cpu使用权*/
        while((g_CarApplyChangedata->NextMission==NULL)||
                (TransStatus.TrackUse.Usebit.ExeCommands==T_Yes)||TransStatus.TrackUse.Usebit.Monopolize==T_Yes)
            //while((TransStatus.DeviceMode==DebugMode)||(g_CarApplyChangedata->NextMission==NULL)||((TransStatus.TrackUse.TrackStatus&0x7f)!=0x00))
        {
//            if(TransStatus.TrackUse.TrackStatus!=0x00)
//            {
//                while(TempMissionNode->NextMission!=NULL)
//                {
//                    if(TempMissionNode->NextMission->MissionMark==2)
//                    {

//                    }
//                    TempMissionNode->NextMission=TempMissionNode->NextMission->NextMission;
//                }
//            }
            OSTimeDlyHMSM(0,0,0,10);
        }
        OSTimeDlyHMSM(0,0,0,10);

        Mission_Analyse(g_CarApplyChangedata);//分析转轨器任务并发送不同的信号邮箱
    }
}

/*******************************************************************************
** 函数名称: TransFindZero_Task
** 功能描述: 找零任务 定速100
** 参数说明: pdata: [输入/出]
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-07-03
********************************************************************************/
void TransFindZero_Task(void *pdata)
{
    pdata=pdata;
    while(1)
    {
        //OSSemPend(TransFindzeroSem,0,&oserr);//无限等待找零信号量
        if(FindZeroGlob>0)//说明中断已经进入
        {
            OSTimeDlyHMSM(0,0,0,10);  //10ms的滤波
            if ((GPIOB->IDR & 0x02) == 0x00)
            {
                FindZeroCtrl=ISOnZero;//读取PB1的值，这个时候如果PB1是0，说明已经压到校零开关
                FindZeroGlob=0;
            }
            else if((GPIOB->IDR & 0x02) == 0x02)
            {
                FindZeroCtrl=ISAwayZero;//读取PB1的值，这个时候如果PB1是1，说明已经离开校零开关
                FindZeroGlob=0;
            }
        }
        else
            OSTimeDlyHMSM(0,0,0,5);  //让出CPU

    }
}
/*******************************End of File************************************/





