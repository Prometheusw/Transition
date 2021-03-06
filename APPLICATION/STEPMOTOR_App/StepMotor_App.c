/********************************Copyright (c)**********************************\
**                   (c) Copyright 2019, Main, China
**                           All Rights Reserved
**                           By(达实久信医疗科技有限公司)
**----------------------------------文件信息------------------------------------
** 文件名称: StepMotor_App.c
** 创建人员: 王凯
** 创建日期: 2019-06-21
** 文档描述: 步进电机应用任务
******************************************************************************/
#include "myconfig.h"
void STEPMOTOR_Task(void *pdata)
{
    INT32S StepMotorCount;
    INT32U MotorCount;
    CAN_DATA_FRAME * tempNode;
    INT8U dirction;
    INT8U oserr;
    OS_CPU_SR cpu_sr;
    pdata=pdata;
    while(1)
    {
        tempNode = (CAN_DATA_FRAME *)OSMboxPend(StepRunMbox,0,&oserr);//等待正反转步数信号邮箱
        /*解析步数和方向*/
        StepMotorCount=((tempNode->canMsg.dataBuf[3])<<24)+((tempNode->canMsg.dataBuf[2])<<16)+
                       ((tempNode->canMsg.dataBuf[1])<<8)+(tempNode->canMsg.dataBuf[0]);
        dirction=(StepMotorCount&0x80000000)>>31;
        if(StepMotorCount<0)
            MotorCount=(~StepMotorCount+1);
        else
            MotorCount=StepMotorCount;
        if(ZeroDirction==anticlockwise)//如果是逆时针校零
        {
            dirction=1-dirction;//步数为正的时候，dir是0，1是顺时针，步数为负的时候，dir是1，0是逆时针
        }
        else if(ZeroDirction==clockwise)//如果是顺时针找零
        {
            dirction=dirction;//步数为正应该电机逆时针0旋转，步数为负应该电机顺指针旋转1
        }
        /*执行电机运动函数*/
        StepMotor_Run(100,dirction,MotorCount);
        OSSemPend(arrivePosSem,1000,&oserr);//等待到位信号，在定时器10中断服务函数中发送10
        if(oserr==OS_ERR_TIMEOUT)//超时//和预期的时间不符合，说明可能丢步，重试
        {
            BeepAlwaysON=1;//蜂鸣器一直响
            // BeepChirptimes=BeepChirptimes+20;
            TransStatus.ErrorCode=MotorOverTime;//电机卡死超时错误
            StepMotor_Run(200,1-dirction,1000);//这时候反向走1000脉冲
            OSSemPend(arrivePosSem,1000,&oserr);//等待到位信号，如果还是超时，说明反向不能走，是车子把转轨器卡死
            if(oserr!=OS_ERR_NONE)////任然超时
            {
                TransStatus.ErrorCode=BeStuckByCar;//转轨器被车卡主
            }
            else//如果反向走成功
            {
                StepMotor_Run(200,dirction,1000);//再正向走
            }
            OSSemPend(arrivePosSem,0,&oserr);//等待到位信号，这里无限等待，如果等不到，说明除了严重故障，人工处理
            TransStatus.ErrorCode=No_Err;//如果等到说明故障能自恢复
            BeepAlwaysON=0;//关掉一直响
            OSSemPost(stepFreeSem);//执行完机械动作
        }
        else
        {
            OSSemPost(stepFreeSem);//执行完机械动作
        }
//					  StepMotor_Run(200,1-dirction,500);//重试，反向移动500
//					  OSSemPend(arrivePosSem,0,&oserr);//等待到位信号，在定时器10中断服务函数中发送
        //StepMotor_Run(200,dirction,500);//正向500
        // OSSemPend(arrivePosSem,1000,&oserr);//等待到位信号，在定时器10中断服务函数中发送
//            if(oserr==OS_ERR_NONE)//通过重试后正常
//						{
//							GPIO_ResetBits(GPIOF,GPIO_Pin_8);//关掉beep
//              FindZeroFuction();//找零功能函数
//						}
//						else if(oserr==OS_ERR_TIMEOUT)
//						{
//						 GPIO_SetBits(GPIOF,GPIO_Pin_8);
//						}
//
////            while(1)//让电机运行线程死在这里，通知手动
////            {
////                CAN2_Single_Send(tempNode);
////                GPIO_SetBits(GPIOF,GPIO_Pin_8);
////                OSTimeDlyHMSM(0,0,0,1000);
////                GPIO_ResetBits(GPIOF,GPIO_Pin_8);
////                OSTimeDlyHMSM(0,0,0,1000);
////                __set_FAULTMASK(1);//所有中断关闭
////                NVIC_SystemReset();//软件复位
////            }
//        }
        /*此处做错误判断，对oserr进行判断执行不同的发送函数*/
        //CAN2_Single_Send(tempNode);//调用单帧回复函数回复一帧数据
        OS_ENTER_CRITICAL();
        myfree(SRAMIN,tempNode);//释放由信号邮箱传过来的结点
        OS_EXIT_CRITICAL();
    }
}

/*******************************************************************************
** 函数名称: TransLocation_TASK
** 功能描述: 轨道定位任务,包括手动模式下的找零任务，统一使用消息队列
** 参数说明: pdata: [输入/出]
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-06-28
********************************************************************************/
void TransLocation_TASK(void *pdata)
{
    CAN_DATA_FRAME * tempNode2;
    CAN_SEND_FRAME *TempResultFram;
    INT8U oserr;
    INT16U Num;
    OS_CPU_SR cpu_sr;
    pdata=pdata;
    while(1)
    {
        tempNode2=(CAN_DATA_FRAME *)OSQPend(TransLocationQeue,0,&oserr);//接受定位帧
        OS_ENTER_CRITICAL();
        TempResultFram=(CAN_SEND_FRAME *)mymalloc(SRAMIN,sizeof(CAN_SEND_FRAME));//结果帧
        OS_EXIT_CRITICAL();
        memset(TempResultFram,0,sizeof(CAN_SEND_FRAME));//申请到的内存清零
        /************************如果是定位任务，数据位******************************************/
        if(tempNode2->id.idBit.index==TransLocation)//定位帧
        {
            Num=tempNode2->canMsg.dataBuf[0] | (tempNode2->canMsg.dataBuf[1]<<8);//解析需要定位的定轨轨道号
            //调用轨道改变函数并且返回值位正确的	neiyou等待步进电机执行线程返回的信号量
            if(Choose_TransLocate_Change(0,tempNode2->canMsg.dataBuf[0])==(u8)Num)
                // if(Choose_TransLocate_Change(tempNode2->canMsg.dataBuf[1],tempNode2->canMsg.dataBuf[0])==Num)

            {
                BeepChirptimes++;
                if(tempNode2->id.MasteridBit.deviceid==ThisTransitionNumber)//如果是外部定位 非modbus 需要回复
                {
                    /*填充结果回复发送帧的数据*/
                    TempResultFram->len=1;
                    TempResultFram->id=(tempNode2->id.canId);//
                    TempResultFram->canMsg.dataBuf[0]=0x55;
                    TempResultFram->nextMsg=NULL;
                    CAN_Post_Queue(CAN2_CHANNEL,TempResultFram);//入发送队列
                }
                /*2019-07-05王凯:修改：给返回帧添加错误代码，错误代码是数据的第一个字节*/
//                tempNode2->canMsg.dataBuf[0]=No_Err;//
//                CAN2_Single_Send(tempNode2);//返送结果帧
//							  OSSemPend(can2InfoAckSem,0,&sserr);//等待域已经接收到找零成功信息的回应信号
                OS_ENTER_CRITICAL();
                myfree(SRAMIN,TempResultFram);
                myfree(SRAMIN,tempNode2);
                OS_EXIT_CRITICAL();
                TransStatus.TrackUse.Usebit.ExeCommands=T_No;//正在执行命令
            }
            else//命令通信成功，但是执行失败
            {
                tempNode2->canMsg.dataBuf[0]=Comexefailed;
                tempNode2->dataLen=1;
                CAN2_Single_Send(tempNode2);//返送结果帧
                OS_ENTER_CRITICAL();
                myfree(SRAMIN,TempResultFram);
                myfree(SRAMIN,tempNode2);
                OS_EXIT_CRITICAL();
                TransStatus.TrackUse.Usebit.ExeCommands=T_No;//正在执行命令释放
            }
        }

        /*****************************************找零任务*****************************************************************/
        else if(tempNode2->id.idBit.index==TransFindzero)//找零任务
        {
            FindZeroFuction(Need_Back);//找零函数
            if(tempNode2->id.MasteridBit.deviceid==ThisTransitionNumber)
            {
                TempResultFram->len=1;
                TempResultFram->id=tempNode2->id.canId;//
                TempResultFram->canMsg.dataBuf[0]=No_Err;
                TempResultFram->nextMsg=NULL;
                CAN_Post_Queue(CAN2_CHANNEL,TempResultFram);//入发送队列
            }
            OS_ENTER_CRITICAL();
            myfree(SRAMIN,TempResultFram);
            myfree(SRAMIN,tempNode2);
            OS_EXIT_CRITICAL();
            TransStatus.TrackUse.Usebit.ExeCommands=T_No;//正在执行命令释放
        }
        /********************************************如果是自检任务***********************************************/
        else if(tempNode2->id.MasteridBit.MainIndex==2&&tempNode2->id.MasteridBit.Subindex==ControlCheck)
        {
            u8 MovingErdoc=0;
            u8 StaticErdoc=0;
            u8 chooseErdoc;
            u8 twiceErdoc;
            FindZeroFuction(Need_Back);//先找零
            for(StaticErdoc=0; StaticErdoc<MAX_FixedTrack; StaticErdoc++) //遍历count
            {
                if(TrackCount[MovingErdoc][StaticErdoc]!=0xffff)//不是0xffff就是有效的可以到达的
                {
                    chooseErdoc=StaticErdoc;//
                }
            }
            if(TrackCount[0][0]!=0xffff)//如果零号是有效的，就定位到零号
            {
                twiceErdoc=0;
            }
            else//如果不能定位，就定位到一号
            {
                twiceErdoc=1;
            }
            if(Choose_TransLocate_Change(0,chooseErdoc)==(chooseErdoc|(0x00<<8)))//调用TransLocate（轨道号）轨道改变函数并且返回值位正确的	neiyou等待步进电机执行线程返回的信号量
            {
                {
                    if(Choose_TransLocate_Change(0,twiceErdoc)==(twiceErdoc|(0x00<<8)))
                        /*2019-07-05王凯:修改：给返回帧添加错误代码，错误代码是数据的第一个字节*/
                        //memset(tempNode2->canMsg.dataBuf,0,sizeof(CAN_MSG));
                        if(tempNode2->id.MasteridBit.deviceid==ThisTransitionNumber)
                        {
                            TempResultFram->len=8;
                            TempResultFram->id=tempNode2->id.canId;//
                            TempResultFram->canMsg.dataBuf[0]=TransStatus.DeviceMode;//最字节是工作模式
                            TempResultFram->canMsg.dataBuf[1]=TransStatus.WarningCode;//警告代码
                            TempResultFram->canMsg.dataBuf[2]=TransStatus.ErrorCode;//错误代码
                            TempResultFram->canMsg.dataBuf[3]=TransStatus.DockedNumber;//对准的轨道号//可不要
                            TempResultFram->canMsg.dataBuf[4]=TransStatus.TrackUse.TrackStatus;//轨道锁
                            TempResultFram->canMsg.dataBuf[5]=(INT8U)TransStatus.EncoderCount;//低八位//当前编码数
                            TempResultFram->canMsg.dataBuf[6]=(TransStatus.EncoderCount)>>8;//高八位//当前编码数
                            TempResultFram->canMsg.dataBuf[7]=TransStatus.TrackUse.TrackStatus;//当前位置//01 12 23 11 22 33 ff
                            TempResultFram->nextMsg=NULL;
                            CAN_Post_Queue(CAN2_CHANNEL,TempResultFram);//入发送队列
                        }
                    OS_ENTER_CRITICAL();
                    myfree(SRAMIN,TempResultFram);
                    myfree(SRAMIN,tempNode2);
                    OS_EXIT_CRITICAL();

                    TransStatus.TrackUse.Usebit.ExeCommands=T_No;//正在执行命令释放
                }
            }
        }
        /*********************************************移动任务************************************************************/
        else if(tempNode2->id.MasteridBit.Subindex==TransMove&&tempNode2->id.MasteridBit.MasterslaveBit==MasterSlave)
        {
            if (tempNode2->canMsg.dataBuf[0]+(tempNode2->canMsg.dataBuf[1]<<8)==1)
            {

                if(ZeroDirction==anticlockwise)//如果是逆时针校零
                {
                    StepMotor_Run(200,1,30);//这地方的速度不可太小，否则没有到位信号
                }
                else
                {
                    StepMotor_Run(200,0,30);//这地方的速度不可太小，否则没有到位信号
                }

                OSSemPend(arrivePosSem,0,&oserr);
            }
            else if ((INT16S)(tempNode2->canMsg.dataBuf[0]+(tempNode2->canMsg.dataBuf[1]<<8))==-1)
            {

                if(ZeroDirction==anticlockwise)//如果是逆时针校零
                {
                    StepMotor_Run(200,0,30);//这地方的速度不可太小，否则没有到位信号
                }
                else
                {
                    StepMotor_Run(200,1,30);//这地方的速度不可太小，否则没有到位信号
                }


                OSSemPend(arrivePosSem,0,&oserr);
            }
            else
//引起电机运行线程的执行，在这个线程中会发送一个电机释放信号量，如果不对这个信号量进行消减会影响其他线程的执行
            {

                OSMboxPost(StepRunMbox,tempNode2);
                OSSemPend(stepFreeSem,0,&oserr);//等待步进电机执行线程返回的信号量
            }
            if(tempNode2->id.MasteridBit.deviceid==ThisTransitionNumber)
            {
                TempResultFram->len=1;
                TempResultFram->id=tempNode2->id.canId;//
                TempResultFram->canMsg.dataBuf[0]=No_Err;
                TempResultFram->nextMsg=NULL;
                CAN_Post_Queue(CAN2_CHANNEL,TempResultFram);//入发送队列
            }
            OS_ENTER_CRITICAL();
            myfree(SRAMIN,TempResultFram);
            myfree(SRAMIN,tempNode2);
            OS_EXIT_CRITICAL();
        }
        else
        {
            tempNode2->canMsg.dataBuf[0]=Comexefailed;
            CAN2_Single_Send(tempNode2);//返送结果帧
            OS_ENTER_CRITICAL();
            myfree(SRAMIN,tempNode2);
            OS_EXIT_CRITICAL();
            TransStatus.TrackUse.Usebit.ExeCommands=T_No;//正在执行命令释放
        }
    }
}



/*******************************End of File************************************/




