/********************************Copyright (c)**********************************\
**                   (c) Copyright 2019, Main, China
**                           All Rights Reserved
**                           By(达实久信医疗科技有限公司)
**----------------------------------文件信息------------------------------------
** 文件名称: myconfig.c
** 创建人员: 王凯
** 创建日期: 2019-06-21
** 文档描述: 全局变量声明定义，任务堆栈定义，配置函数，事件创建和任务创建函数
*******************************End of Head************************************/
#include "myconfig.h"
/*任务堆栈*/
OS_STK START_TASK_STK[START_STK_SIZE];
__align(32) OS_STK RUN_TASK_STK[RUN_TASK_SIZE]                        __attribute__((at(NotUsedCCMBASE(0))));
__align(32) OS_STK CAN2_TASK_STK[CAN2_TASK_SIZE]                      __attribute__((at(NotUsedCCMBASE(512))));
__align(32) OS_STK CAN2ACK_TASK_STK[CAN2ACK_TASK_SIZE]                __attribute__((at(NotUsedCCMBASE(512*2))));
__align(32) OS_STK CAN2ACT_TASK_STK[CAN2ACT_TASK_SIZE]                __attribute__((at(NotUsedCCMBASE(512*3))));
__align(32) OS_STK STEPMOTOR_TASK_STK[STEPMOTOR_TASK_SIZE]            __attribute__((at(NotUsedCCMBASE(512*4))));
__align(32) OS_STK LED_TASK_STK[LED_TASK_SIZE]                        __attribute__((at(NotUsedCCMBASE(512*5))));
__align(32) OS_STK TRANSLOCATION_TASK_STK[TRANSLOCATION_TASK_SIZE]    __attribute__((at(NotUsedCCMBASE(512*6))));
__align(32) OS_STK MISSIONANALYSE_TASK_STK[MISSIONANALYSE_TASK_SIZE]  __attribute__((at(NotUsedCCMBASE(512*7))));
__align(32) OS_STK CARAPPLYPASS_TASK_STK[CARAPPLYPASS_TASK_SIZE]      __attribute__((at(NotUsedCCMBASE(512*8))));
__align(32) OS_STK CAN2Send_TASK_STK[CAN2Send_TASK_SIZE]              __attribute__((at(NotUsedCCMBASE(512*9))));
__align(32) OS_STK CARAPPLYCHANGE_TASK_STK[CARAPPLYCHANGE_TASK_SIZE]  __attribute__((at(NotUsedCCMBASE(512*10))));
__align(32) OS_STK FINDZERO_TASK_STK[FINDZERO_TASK_SIZE]              __attribute__((at(NotUsedCCMBASE(512*11))));
__align(32) OS_STK TRANSPERMI_TASK_STK[TRANSPERMI_TASK_SIZE]          __attribute__((at(NotUsedCCMBASE(512*12))));
__align(32) OS_STK HEARTBEAT_TASK_STK[HEARTBEAT_TASK_SIZE]            __attribute__((at(NotUsedCCMBASE(512*13))));
__align(32) OS_STK MODBUS_TASK_STK[MODBUS_TASK_SIZE]                  __attribute__((at(NotUsedCCMBASE(512*14))));
__align(32) OS_STK STATE_TASK_STK[STATE_TASK_SIZE]                    __attribute__((at(NotUsedCCMBASE(512*15))));
__align(32) OS_STK MODE_TASK_STK[MODE_TASK_SIZE]                      __attribute__((at(NotUsedCCMBASE(512*16))));


/*******************************************************************************
** 函数名称: ParameterInit
** 功能描述: 参数初始化
** 参数说明: None
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-07-17
********************************************************************************/
void ParameterInit(void)
{
    TranferSpeed=1;//电机可以运动
    sn0 = *(vu32*)(0x1FFF7A10); //使用SN0即可//唯一标识符
    ControlMessage.ProductSeries='G';//产品系列
    ControlMessage.HardwareVersion=0x0006;//硬件版本
    ControlMessage.SoftwareVersion=0x0005;//软件版本
    ControlMessage.ARMOnlyOneId=sn0;//ARM唯一标识号
    ControlMessage.ManufactureData=20190812;//生产日期
    ControlMessage.UseData=20190812;//使用日期

	  EquipmentType=CAN_TRANSFER_MAININDEX;//此设备的类型是转轨器
    CANCommunicatMode.CanMode.CanModeMask=0xff;//CAN的发送模式初始化为全部开启

    memset(TrackCount,0xffff,sizeof(TrackCount));//初始化编码数数组
    memset(TempTrackCount,0xffff,sizeof(TempTrackCount));//初始化临时标定编码数组


    TransStatus.TrackUse.Usebit.CheakSelf=1;//自检标志，需要自检的时候为一，上电之后需要自检
}
/*******************************************************************************
** 函数名称: HardwareInit
** 功能描述: 环境硬件初始化函数
** 参数说明: None
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-06-21
********************************************************************************/
void HardwareInit(void)
{
    delay_init(168);//初始化延时函数
    Beep_Init();//蜂鸣器初始化
    Led_Init();
    TIM10_Cap_Init(0xFFFF,167);//到位信号//改成时间
    TIM6_Int_Init(420-1,0);//步进电机5us
    TIM7_Int_Init(8400-1,10000-1);//心跳帧定时
    Timer2_Init(84-1,1000-1);//Modbus
    TIM9_Cap_Init(0xFFFF,167);
    TIM3_Cap_Init(0xFFFF,83);//零点开关信号
    Mosbus_Init();
    my_mem_init(SRAMIN);//初始化内部内存池
    my_mem_init(SRAMCCM);//初始化CCRAM  64k
    W25QXX_Init();//W25QXX初始化
    ReadFlash();//读取flash
    TrackCount_Load();//从W25Q读取标定的编码并初始化函数
    CAN_Software_Init();
    CarMission_Software_Init();
    CAN_HardwareInit(CAN1_CHANNEL,CAN_BPS_20K,CAN_Mode_Normal,CAN1_RX0_INT_ENABLE);///<初始化CAN1用于与区域内设备通讯
    CAN_HardwareInit(CAN2_CHANNEL,CAN_BPS_333K,CAN_Mode_Normal,CAN2_RX0_INT_ENABLE);///<初始化CAN2用于与小车通讯
    CANSEND_Init();//把发送中断放在此处，等CAN其他配置都配置好再进行配置。否则会产生问题
    StepMotor_Init();
    Update_Limit_Count();//更新限位值
    W25QXX_Read(&ZeroDirction,FLASH_SECTOR_SIZE*2,sizeof(ZeroDirction));//获取找零方向
    W25QXX_Read(&ThisTransitionNumber,FLASH_SECTOR_SIZE*2+4,sizeof(ThisTransitionNumber));//获设备号
    Load_One_FindZero();//在任务列表里面加一个找零任务，做W为上电找零
    delay_ms(2000);
    TIM_ITConfig(TIM9,TIM_IT_CC1,ENABLE);//初始化时候关闭了报警信号的捕捉，在系统稳定之后，重新开启
		TIM_Cmd(TIM9,ENABLE); //使能定时器9
    USART_Cmd(USART3, ENABLE);  //使能串口3
    BeepOFF;//软件初始化成功后停止鸣叫
  	RTRbuf=(CAN_DATA_FRAME *)mymalloc(SRAMIN,sizeof(CAN_DATA_FRAME));//收到的应答帧识别标识内存区
		printf("初始化成功");
}

/*******************************************************************************
** 函数名称: EventCreate
** 功能描述: 创建人任务所需要的事件结构，注意事项：信号量最大允许要注意在osconfig中配置
** 参数说明: None
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-06-21
********************************************************************************/
void EventCreate(void)
{
    can1RecvSem = OSSemCreate(0);
    can2RecvSem = OSSemCreate(0);
    can1ActSem  = OSSemCreate(0);
    can2ActSem  = OSSemCreate(0);
    can1AckSem  = OSSemCreate(0);
    can2AckSem  = OSSemCreate(0);
    can1CtlAckSem = OSSemCreate(0);
    can2CtlAckSem = OSSemCreate(0);
    can1CtlResSem = OSSemCreate(0);
    can1InfoAckSem = OSSemCreate(0);
    can2InfoAckSem = OSSemCreate(0);
    can1SendSem = OSSemCreate(0);
    can2SendSem = OSSemCreate(0);
    stepMotorCtlSem = OSSemCreate(0);
    arrivePosSem = OSSemCreate(0);
    stepFreeSem = OSSemCreate(0);//电机释放信号量
    // CarApplyPassTransSem = OSSemCreate(0);//小车有任务信号量
    CarAlreadyPassSem = OSSemCreate(0);//小车已经通过信号量
    CarAlreadyUpMbox = OSMboxCreate((void*)0);//小车已经上轨信号量
    CarAlreadyDownSem = OSSemCreate(0);
    ModBusFlagSem=OSSemCreate(0);
    AgreeChangeModeSem=OSSemCreate(0);
    Can2Find0AckSem = OSSemCreate(0);//找零成功后给域发送结果信号量，然后域回复收到的信号量
    TransFindzeroSem = OSSemCreate(0);//找零xinhaoliang

    can1Mbox = OSMboxCreate((void*)0);
    can2Mbox = OSMboxCreate((void*)0);
    TransLocationQeue = OSQCreate(&TransLocationQeutb[0],10);//创建定位任务消息队列
    TranPreMissionMbox = OSMboxCreate((void*)0);
    StepRunMbox = OSMboxCreate((void*)0);
    CarApplyPassTransMbox=OSMboxCreate((void*)0);
    CarApplyChangeTranMbox=OSMboxCreate((void*)0);
    ApplyChangeModeBox=OSMboxCreate((void*)0);//申请模式变更信号邮箱
}

/*******************************************************************************
** 函数名称: TaskCreate
** 功能描述: 任务创建函数
** 参数说明: None
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-06-21
********************************************************************************/
void TaskCreate(void)
{
    OSTaskCreate(RUN_Task,      (void *)0, &RUN_TASK_STK[RUN_TASK_SIZE-1],         RUN_TASK_PRIO);
    OSTaskCreate(CAN2_Task,     (void *)0, &CAN2_TASK_STK[CAN2_TASK_SIZE-1],       CAN2_TASK_PRIO);
    OSTaskCreate(CAN2_Ack_Task, (void *)0, &CAN2ACK_TASK_STK[CAN2ACK_TASK_SIZE-1],  CAN2ACK_TASK_PRIO);
    OSTaskCreate(CAN2_Act_Task, (void *)0, &CAN2ACT_TASK_STK[CAN2ACT_TASK_SIZE-1],  CAN2ACT_TASK_PRIO);
    OSTaskCreate(LED_Task,      (void *)0, &LED_TASK_STK[LED_TASK_SIZE-1],         LED_TASK_PRIO);
    OSTaskCreate(STEPMOTOR_Task,(void *)0, &STEPMOTOR_TASK_STK[STEPMOTOR_TASK_SIZE-1], STEPMOTOR_TASK_PRIO);
    OSTaskCreate(TransLocation_TASK,(void *)0,&TRANSLOCATION_TASK_STK[TRANSLOCATION_TASK_SIZE-1], TRANSLOCATION_TASK_PRIO);
    OSTaskCreate(MissionAnalyse_Task,(void *)0,&MISSIONANALYSE_TASK_STK[MISSIONANALYSE_TASK_SIZE-1], MISSIONANALYSE_PRIO);
// OSTaskCreate(CarApplyPass_Task,(void *)0,&CARAPPLYPASS_TASK_STK[CARAPPLYPASS_TASK_SIZE-1], CARAPPLYPASS_PRIO);
    OSTaskCreate(CAN2_Send_Task,(void *)0,&CAN2Send_TASK_STK[CAN2Send_TASK_SIZE-1], CAN2Send_PRIO);
    OSTaskCreate(CarApplyChange_Task,(void *)0,&CARAPPLYCHANGE_TASK_STK[CARAPPLYCHANGE_TASK_SIZE-1], CARAPPLYCHANGE_PRIO);
    OSTaskCreate(TransFindZero_Task,(void *)0,&FINDZERO_TASK_STK[FINDZERO_TASK_SIZE-1], FINDZERO_PRIO);
    OSTaskCreate(TranPerMission_Task,(void *)0,&TRANSPERMI_TASK_STK[TRANSPERMI_TASK_SIZE-1], TRANSPERMI_PRIO);
    OSTaskCreate(HEARBEAT_TASK,(void *)0,&HEARTBEAT_TASK_STK[HEARTBEAT_TASK_SIZE-1], HEARTBEAT_PRIO);//心跳任务
    OSTaskCreate(MODBUS_Task,(void *)0,&MODBUS_TASK_STK[MODBUS_TASK_SIZE-1],MODBUS_PRIO);//MODBUS处理任务
    OSTaskCreate(STATE_Task,(void *)0,  &STATE_TASK_STK[STATE_TASK_SIZE-1],STATE_PRIO);//状态处理线程
    //OSTaskCreate(ModeChange_Task,(void *)0,&MODE_TASK_STK[MODE_TASK_SIZE-1],MODE_PRIO);//模式变更处理线程
}
/*******************************************************************************
** 函数名称: StateQuery_Index_judegment
** 功能描述: 无需应答的帧  直接回复结果
** 参数说明: tempNode: [输入/出]
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-07-24
********************************************************************************/
void StateQuery_Index_judegment(CAN_DATA_FRAME * tempNode)
{
    u8 Sub_index=tempNode->id.MasteridBit.Subindex;
    switch(Sub_index)//判断子索引
    {
    case GetTransStatus://获取转轨器状态数据01 属于即时命令 接受命令---发送数据
    {
        SendTransStatus(tempNode);//上传转轨器状态
    }
    break;
    case GetTransConfig://获取转轨器配置03 属于即时命令 接受命令---发送数据
    {
        SendTransConfig(tempNode);//上传转轨器配置
    }
    break;
    case GetTransStatistics://获取统计数据05 属于即时命令 接受命令---发送数据
    {
        SendTransStatistics(tempNode);//返回转轨器统计数据，调用单帧发送函数
    }
    break;
    case SetTransConfig://设置转轨器的配置数据，0x04 这个用来改变标定的编码数
    {
        ACKSendFram(CAN2_CHANNEL,tempNode);//立刻回应，主从
        /*此处需要和域进行协商，确定帧发送回来的格式*/
    }
    break;

    case GetControlConfig://获取控制器基本信息  0x27 属于即时命令 接受命令---发送数据
    {
        SentControlConfig(tempNode);
    }
    break;
    }
}

/*******************************************************************************
** 函数名称: Broadcast_Judegment
** 功能描述: 对广播帧的处理
** 参数说明: canChan: [输入/出]
**			 tempNode: [输入/出]
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-07-27
********************************************************************************/
void Broadcast_Judegment(u8 canChan,CAN_DATA_FRAME * tempNode)
{
    u8 Sub_index=tempNode->id.MasteridBit.Subindex;
    switch(Sub_index)
    {
    case GetUniqueID://获取ARM唯一ID 0x10 属于即时命令 接受命令---发送数据  广播
    {
        SendDeviceOnlyID(tempNode);//返回ARM唯一的ID
    }
    break;
    case GetCanComMod://获取CAN的通信模式 0x12 属于即时命令 接受命令---发送数据  广播
    {
        SentCANCommitMode(tempNode);
    }
    break;
    case SetCANID://根据唯一ARM的ID号码，0x17 设置CANID，也就是设置本机设备号  广播
    {
        // ACKSendFram(CAN2_CHANNEL,tempNode);//立刻回应，主从
        SetCANIDByARMID(tempNode);
    }
    break;
    case SetCanComMod://设置Can通讯模式 0x19 //如果需要返货错误代码，就使用申请的内存 广播
    {
        SetCANCommitMode(tempNode);
    }
    break;
    }
}
/*******************************************************************************
** 函数名称: Master_Index_judegment
** 功能描述: 主从模式下判断主和从索引
** 参数说明: canChan: [输入/出]
**			 tempNode: [输入/出]
**			 frameBuf: [输入/出]
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-07-17
********************************************************************************/
void Master_Index_judegment(u8 canChan,CAN_DATA_FRAME * tempNode,CAN_DATA_FRAME *frameBuf)
{
    u8 Main_index=tempNode->id.MasteridBit.MainIndex;
    u8 Sub_index=tempNode->id.MasteridBit.Subindex;
    switch(Main_index)
    {
    case CAN_TRANSFER_MAININDEX://0x02
    {
        switch(Sub_index)
        {
//        case SetRestart://0x20,//域控制重新启动
//        {
//            __set_FAULTMASK(1);//所有中断关闭
//            NVIC_SystemReset();//软件复位
//        }
//        break;

        case ControlCheck://0x22,//控制器自检
        {
            ACKSendFram(CAN2_CHANNEL,tempNode);//立刻回应，主从
            Apply_Change_Mission(tempNode);
        }
        break;

        case TransFindzero://转轨器校零//自动模式下，只有域控制器能给转轨器校零的命令11
        {
            ACKSendFram(CAN2_CHANNEL,tempNode);//立刻回应，主从
            Apply_Change_Mission(tempNode);
        }
        break;

        case TranPerMission://预动任务fe
        {
            if((TransStatus.TrackUse.TrackStatus&0x3c)!=0x00)//已经有任务锁定//预动任务已经存在
            {
                tempNode->dataLen=1;
                tempNode->id.canId=CAN_TRANSFER_MAININDEX<<8|ThisTransitionNumber<<16|TranPerMission|0x04<<24;//索引号发给主从域
                tempNode->canMsg.dataBuf[0]=Comexefailed;//说明有错误，轨道已经被锁定//数据域是错误代码
                tempNode->nextMsg=NULL;
                CAN2_Single_Send(tempNode);//单帧发送函数
							printf("预动线程被占用，不进入预动线程。\r\n");
            }
            else
            {
            ACKSendFram(CAN2_CHANNEL,tempNode);//立刻回应，主从
            Apply_Change_Mission(tempNode);
            }
        }
        break;

        case TransLocation://转轨器定位10
        {
            ACKSendFram(CAN2_CHANNEL,tempNode);//立刻回应，主从
            Apply_Change_Mission(tempNode);
        }
        break;

        case TransMove://转轨器移动 12
        {
            ACKSendFram(CAN2_CHANNEL,tempNode);//立刻回应，主从
            Apply_Change_Mission(tempNode);
        }
        break;

        case TransStop://刹车，停止转动 fc 停止命令不耗时，就不用再进行恢复了
        {
            //CAN_SEND_FRAME *TempSendFram;
            //TempSendFram = (CAN_SEND_FRAME *)mymalloc(SRAMIN,sizeof(CAN_SEND_FRAME));//通知的结果帧
            ACKSendFram(CAN2_CHANNEL,tempNode);//立刻回应，主从
            if(TransStatus.TrackUse.Usebit.ExeCommands==T_Yes)//只有转轨器正在工作的时候，才会让他停止
            {
                TranferSpeed=TranferStop;//停止标志位
            }//            memset(TempSendFram,0,sizeof(CAN_SEND_FRAME));
//            TempSendFram->id=tempNode->id.canId;
//            TempSendFram->len=1;
//            TempSendFram->canMsg.dataBuf[0]=No_Err;
//            TempSendFram->nextMsg=NULL;
//            CAN_Post_Queue(CAN2_CHANNEL,TempSendFram);//压入主动帧链表
        }
        break;
        case AgreeChangeMode:
        {
            ACKSendFram(CAN2_CHANNEL,tempNode);//立刻回应，主从
            OSSemPost(AgreeChangeModeSem);
        }
        break;


        }
    }
    break;
    }
}
/*******************************************************************************
** 函数名称: Index_Judegment
** 功能描述: 对索引进行判断，执行不同的任务
** 参数说明: canChan: can通道
**			 id:帧id
**			 index:索引
**			 tempNode:数据帧包
**			 frameBuf:
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-06-27
改动说明：2019-07-05 王凯：此处的tempNode指针是原来的ACT任务链表中的指针，这里会出现问题
当act线程遍历到一个结点的时候，将tenonode指针发送到机械或者其他执行类线程，这时候在ACT线程
里面对tempNode进行释放，如果这个时候其他执行类线程还没有完成结点指明的任务，就会产生问题
改动：memcpy
********************************************************************************/
void Index_Judegment(u8 canChan,u32 id,u8 index,CAN_DATA_FRAME * tempNode,CAN_DATA_FRAME *frameBuf)
{
//    INT8U oserr;
//    CAN_DATA_FRAME * CopyTempNode;
	  
    enum _INDEX AgreeMentIndex;
    AgreeMentIndex=(enum _INDEX)index;




    /*转轨器工作在自动模式下，这种模式下，小车能给转轨器调度，脱机和手动状态都不可调度*/
    if(TransStatus.DeviceMode==OperatingMode)
    {
        switch(AgreeMentIndex)
        {

        case GetTransStatus://获取转轨器状态数据01 属于即时命令 接受命令---发送数据
        {
            SendTransStatus(tempNode);//上传转轨器状态
        }
        break;

        case ReStart://实现软件复位ff
        {
            __set_FAULTMASK(1);//所有中断关闭
            NVIC_SystemReset();//软件复位
        }
        break;
        case CarApplyPass://小车申请通过16
        {
            ACKSendFram(CAN2_CHANNEL,tempNode);//立刻回应，多主
            Apply_Change_Mission(tempNode);
            //在这个函数里面，申请一块内存保存了tempnode的数据，执行完之后才会继续向下执行，所以不会出现覆盖执行的问题
        }
        break;
        case CarAlreadyPass://小车已经通过18
        {
            ACKSendFram(CAN2_CHANNEL,tempNode);//立刻回应，多主
            OSSemPost(CarAlreadyPassSem);
            //CarApplyPass_Task执行线程中会等待这个信号量，只有这个信号量到来，才会释放这个线程
        }
        break;
        case CarApplyChange://小车申请转轨10
        {
					printfcount++;
					printf("\r\n<事务ID：%llu>（1/14）已经接收到 %d 号小车申请转轨帧，从%d号轨道到%d号轨道。\r\n",printfcount,
					tempNode->id.idBit.sendDeviceId,
					tempNode->canMsg.dataBuf[0],tempNode->canMsg.dataBuf[1]);
					ACKSendFram(CAN2_CHANNEL,tempNode);//立刻回应，多主
					printf("<事务ID：%llu>（2/14）已经对%d号小车申请转轨帧进行回应。\r\n",printfcount,tempNode->id.idBit.sendDeviceId);
					if(LastFramFlag != tempNode->id.idBit.sendDeviceId)//为了防止出现连续多次当送通过转轨期指令
					{
            Apply_Change_Mission(tempNode);
						LastFramFlag=tempNode->id.idBit.sendDeviceId;
						printf("<事务ID：%llu>%d号小车申请转轨任务已经建立。\r\n",printfcount,tempNode->id.idBit.sendDeviceId);
					}
					else
					{
						printf("<事务ID：%llu>（ERR）收到%d号小车申请转轨帧，任务重复。\r\n",printfcount,tempNode->id.idBit.sendDeviceId);
					}
						 
            //在这个函数里面，申请一块内存保存了tempnode的数据，执行完之后才会继续向下执行，所以不会出现覆盖执行的问题
        }
        break;

        case CarAlreadyUpTrack://小车已经上轨道12
        {
					u8 *carnum;
					printf("<事务ID：%llu>（6/14）已经接收到 %d 号小车已经上轨帧，此时已经上轨道\r\n",printfcount,tempNode->id.idBit.sendDeviceId);
					ACKSendFram(CAN2_CHANNEL,tempNode);//立刻回应，多主
          printf("<事务ID：%llu>（7/14）已经对%d号小车已经上轨进行回应\r\n",printfcount,tempNode->id.idBit.sendDeviceId);
					if((TransStatus.TrackUse.TrackStatus&0x3c)!=0x00&&alreadlyuptrack==0)//只有当轨道被上锁的时候才会出现已经上轨这种帧，避免出现已经上轨累计这种情况
					{
					  printf("<事务ID：%llu>发送%d号小车已经上轨信号量\r\n",printfcount,tempNode->id.idBit.sendDeviceId);
						alreadlyuptrack=1;
					  carnum=(u8 *)mymalloc(SRAMIN,1);
						*carnum=tempNode->id.idBit.sendDeviceId;
            OSMboxPost(CarAlreadyUpMbox,carnum);//开始转轨
					}
					else
					{
						printf("<事务ID：%llu>（ERR）收到%d号小车已经上轨帧，不发送信号量\r\n",printfcount,tempNode->id.idBit.sendDeviceId);
					}
        }
        break;

        case CarAlreadyDownTrack://小车已经下轨道14
        {
					printf("<事务ID：%llu>（12/14）已经接收到 %d 号小车已经下轨帧，下轨道\r\n",printfcount,tempNode->id.idBit.sendDeviceId	);
					ACKSendFram(CAN2_CHANNEL,tempNode);//立刻回应，多主
			   	printf("<事务ID：%llu>（13/14）已经对%d号小车下轨帧进行回应\r\n",printfcount,tempNode->id.idBit.sendDeviceId);
					if((TransStatus.TrackUse.TrackStatus&0x3c)!=0x00&&alreadlydowntrack==0)//只有当轨道被上锁的时候才会出现已经轨这种帧，避免出现已经上轨累计这种情况
					{
						alreadlyuptrack=1;
						printf("<事务ID：%llu>发送%d号小车已经下轨信号量\r\n",printfcount,tempNode->id.idBit.sendDeviceId);
            OSSemPost(CarAlreadyDownSem);//解锁
					}
					else
					{
						printf("<事务ID：%llu>（ERR）收到%d号小车已经下轨帧，但不发送信号量\r\n",printfcount,tempNode->id.idBit.sendDeviceId);
					}
        }
        break;

        case ResetTransStatus://重置转轨器状态02
        {
            TransStatus.DeviceMode=tempNode->canMsg.dataBuf[0];
            TransStatus.WarningCode=tempNode->canMsg.dataBuf[1];
            TransStatus.ErrorCode=tempNode->canMsg.dataBuf[2];
        }
        break;
        default:
            break;
        }
    }


    else if(TransStatus.DeviceMode==DebugMode)
    {
        switch(AgreeMentIndex)
        {
//        case TransLocation://转轨器定位10
//        {
//            /*2019-07-05王凯：修改：使用消息队列时候，申请复制内存，否则内存会被释放，任何时候都是相同的值*/
//            CopyTempNode = (CAN_DATA_FRAME *)mymalloc(SRAMIN,sizeof(CAN_DATA_FRAME));
//            memcpy(CopyTempNode,tempNode,sizeof(CAN_DATA_FRAME));
//            OSQPost(TransLocationQeue,CopyTempNode);
//            /*void TransLocation_TASK(void *pdata)中接受这个信号量*/
//        }
//        break;
//        case TransFindzero://转轨器校零11
//        {
//            CopyTempNode = (CAN_DATA_FRAME *)mymalloc(SRAMIN,sizeof(CAN_DATA_FRAME));
//            memcpy(CopyTempNode,tempNode,sizeof(CAN_DATA_FRAME));
//            OSQPost(TransLocationQeue,CopyTempNode);
//        }
//        break;


//        case ResetTransStatus://重置转轨器状态 0为调试状态 02
//        {
//            TransStatus.DeviceMode=tempNode->canMsg.dataBuf[0];
//            TransStatus.WarningCode=tempNode->canMsg.dataBuf[1];
//            TransStatus.ErrorCode=tempNode->canMsg.dataBuf[2];
//        }
//        break;



//        case TransMove://转轨器移动 12
//        {
//            /*在点动时候，如果采用另外开线程的方法，很容易丢步，这里直接调用执行函数*/
//            /*原因是起步速度太慢  导致丢步*/
//            if (tempNode->canMsg.dataBuf[0]+(tempNode->canMsg.dataBuf[1]<<8)==1)
//            {
//                StepMotor_Run(20,0,1);//这地方的速度不可太小，否则没有到位信号
//                OSSemPend(arrivePosSem,0,&oserr);
//            }
//            else if ((INT16S)(tempNode->canMsg.dataBuf[0]+(tempNode->canMsg.dataBuf[1]<<8))==-1)
//            {
//                StepMotor_Run(20,1,1);//这地方的速度不可太小，否则没有到位信号
//                OSSemPend(arrivePosSem,0,&oserr);
//            }
//            else
////引起电机运行线程的执行，在这个线程中会发送一个电机释放信号量，如果不对这个信号量进行消减会影响其他线程的执行
//            {
//                OSMboxPost(StepRunMbox,tempNode);
//                OSSemPend(stepFreeSem,0,&oserr);//等待步进电机执行线程返回的信号量
//            }
//        }
//        break;

//        case CurrentPreset://设置当前位为预置位13
//        {
//            TrackCount[(u8)(Reg[ModBus_Calibration]>>8)][(u8)(Reg[ModBus_Calibration])]=TransStatus.EncoderCount;//轨道数的全局变量设置为当前对准的编码数
//            TransStatus.DockedNumber=ModBus_Calibration;//正好对准的轨道号改变
//        }
//        break;
//        case LoadTransParameters://装载转轨器参数14
//        {
//            ReadFlash();//CRC校验，校验区域是编码数备份的主副区域
//            TrackCount_Load(); //从W25Q读取标定的编码并初始化函数
//        }
//        break;

//        case SaveTransParameters://保存转轨器参数15
//        {
//            TrackCount_Save();
//        }
//        break;
        case GetTransStatus://获取转轨器状态数据01 属于即时命令 接受命令---发送数据
        {
            SendTransStatus(tempNode);//上传转轨器状态
        }
        break;

        default:
            break;

        }
    }

}
/*******************************************************************************
** 函数名称: ReadFlash
** 功能描述: FLASH中存储的关键数据读取检测
** 参数说明: None
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-07-04
********************************************************************************/
bool ReadFlash(void)
{
    u16 cnt;
    uint8_t i;
    OS_CPU_SR cpu_sr;
    uint8_t *ptr;
    uint8_t *ptr_bak;
    OS_ENTER_CRITICAL();
    ptr  = (uint8_t*)mymalloc(SRAMIN,FLASH_SECTOR_SIZE);//申请主区域内存，一个扇区的大小4k
    OS_EXIT_CRITICAL();
    if(ptr==NULL) return false;
    OS_ENTER_CRITICAL();
    ptr_bak = (uint8_t*)mymalloc(SRAMIN,FLASH_SECTOR_SIZE);//申请备份内存区域，一个扇区大小4k
    OS_EXIT_CRITICAL();
    if(ptr_bak==NULL)
    {
        OS_ENTER_CRITICAL();
        myfree(SRAMIN,ptr);
        OS_EXIT_CRITICAL();
        return false;
    }
    W25QXX_Read(ptr,0,FLASH_SECTOR_SIZE); //从主区域读取一个扇区大小的数据到ptr缓存中
    for(cnt=0; cnt<FLASH_SECTOR_SIZE; cnt++) //检查主缓存中的数据
    {
        if(ptr[cnt]!=0xFF)
            break;//如果不是FF说明读取到的数据有效，跳出后判断cnt
    }
    if(cnt==FLASH_SECTOR_SIZE)
    {
        return false;//cnt等于扇区大小说明一个扇区全是FF，没有写过，直接返回
    }
    for (i=0; i<MAX_READ_TIME; i++) //
    {
        W25QXX_Read(ptr,0,FLASH_SECTOR_SIZE);         //读MAX_READ_TIME次
        if(CRC16(ptr,FLASH_SECTOR_SIZE)==0)
        {
            break;//CRC校验无误，跳出，这个时候的i<MAX_READ_TIME
        }
        else
        {
            OSTimeDlyHMSM(0,0,0,10);  //有错误延时10ms 这个时候的i=MAX_READ_TIME
        }
    }
    if (i<MAX_READ_TIME)//主区域CRC校验没有错误
    {
        W25QXX_Read(ptr_bak,FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE); //读取备份区域的数据到备份数据寄存器
        if(memcmp(ptr,ptr_bak,FLASH_SECTOR_SIZE))  //比较，相同为0，不同为1
        {
            //如果不相同，将主区域的数据覆盖到备份区域
            W25QXX_Write(ptr,FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
        }
        OS_ENTER_CRITICAL();
        myfree(SRAMIN,ptr);
        myfree(SRAMIN,ptr_bak);
        OS_EXIT_CRITICAL();
        return true;
    }
    else //主区域CRC校验有错误
    {
        //直接读取备份区域
        for (i=0; i<MAX_READ_TIME; i++)
        {
            W25QXX_Read(ptr_bak,FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);  //读取备份区域
            if(CRC16(ptr_bak,FLASH_SECTOR_SIZE)==0)//如果备份区域校验没有错误
            {
                break;
            }
            else
            {
                OSTimeDlyHMSM(0,0,0,10);  //
            }
        }
        if (i<MAX_READ_TIME)
        {
            //如果校验没错误到这里
            W25QXX_Write(ptr_bak,0,FLASH_SECTOR_SIZE);//将备份数据覆盖到主数据，当运行到这个地方时，说明主数据区域有问题没有通过校验
            OS_ENTER_CRITICAL();
            myfree(SRAMIN,ptr);
            myfree(SRAMIN,ptr_bak);
            OS_EXIT_CRITICAL();
            return true;
        }
    }
    OS_ENTER_CRITICAL();
    myfree(SRAMIN,ptr);
    myfree(SRAMIN,ptr_bak);
    OS_EXIT_CRITICAL();
    return false;
}



