/********************************Copyright (c)**********************************\
**                   (c) Copyright 2019, Main, China
**                           All Rights Reserved
**                           By(��ʵ����ҽ�ƿƼ����޹�˾)
**----------------------------------�ļ���Ϣ------------------------------------
** �ļ�����: myconfig.c
** ������Ա: ����
** ��������: 2019-06-21
** �ĵ�����: ȫ�ֱ����������壬�����ջ���壬���ú������¼����������񴴽�����
*******************************End of Head************************************/
#include "myconfig.h"
/*�����ջ*/
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
** ��������: ParameterInit
** ��������: ������ʼ��
** ����˵��: None
** ����˵��: None
** ������Ա: ����
** ��������: 2019-07-17
********************************************************************************/
void ParameterInit(void)
{
    TranferSpeed=1;//��������˶�
    sn0 = *(vu32*)(0x1FFF7A10); //ʹ��SN0����//Ψһ��ʶ��
    ControlMessage.ProductSeries='G';//��Ʒϵ��
    ControlMessage.HardwareVersion=0x0006;//Ӳ���汾
    ControlMessage.SoftwareVersion=0x0005;//����汾
    ControlMessage.ARMOnlyOneId=sn0;//ARMΨһ��ʶ��
    ControlMessage.ManufactureData=20190812;//��������
    ControlMessage.UseData=20190812;//ʹ������

	  EquipmentType=CAN_TRANSFER_MAININDEX;//���豸��������ת����
    CANCommunicatMode.CanMode.CanModeMask=0xff;//CAN�ķ���ģʽ��ʼ��Ϊȫ������

    memset(TrackCount,0xffff,sizeof(TrackCount));//��ʼ������������
    memset(TempTrackCount,0xffff,sizeof(TempTrackCount));//��ʼ����ʱ�궨��������


    TransStatus.TrackUse.Usebit.CheakSelf=1;//�Լ��־����Ҫ�Լ��ʱ��Ϊһ���ϵ�֮����Ҫ�Լ�
}
/*******************************************************************************
** ��������: HardwareInit
** ��������: ����Ӳ����ʼ������
** ����˵��: None
** ����˵��: None
** ������Ա: ����
** ��������: 2019-06-21
********************************************************************************/
void HardwareInit(void)
{
    delay_init(168);//��ʼ����ʱ����
    Beep_Init();//��������ʼ��
    Led_Init();
    TIM10_Cap_Init(0xFFFF,167);//��λ�ź�//�ĳ�ʱ��
    TIM6_Int_Init(420-1,0);//�������5us
    TIM7_Int_Init(8400-1,10000-1);//����֡��ʱ
    Timer2_Init(84-1,1000-1);//Modbus
    TIM9_Cap_Init(0xFFFF,167);
    TIM3_Cap_Init(0xFFFF,83);//��㿪���ź�
    Mosbus_Init();
    my_mem_init(SRAMIN);//��ʼ���ڲ��ڴ��
    my_mem_init(SRAMCCM);//��ʼ��CCRAM  64k
    W25QXX_Init();//W25QXX��ʼ��
    ReadFlash();//��ȡflash
    TrackCount_Load();//��W25Q��ȡ�궨�ı��벢��ʼ������
    CAN_Software_Init();
    CarMission_Software_Init();
    CAN_HardwareInit(CAN1_CHANNEL,CAN_BPS_20K,CAN_Mode_Normal,CAN1_RX0_INT_ENABLE);///<��ʼ��CAN1�������������豸ͨѶ
    CAN_HardwareInit(CAN2_CHANNEL,CAN_BPS_333K,CAN_Mode_Normal,CAN2_RX0_INT_ENABLE);///<��ʼ��CAN2������С��ͨѶ
    CANSEND_Init();//�ѷ����жϷ��ڴ˴�����CAN�������ö����ú��ٽ������á�������������
    StepMotor_Init();
    Update_Limit_Count();//������λֵ
    W25QXX_Read(&ZeroDirction,FLASH_SECTOR_SIZE*2,sizeof(ZeroDirction));//��ȡ���㷽��
    W25QXX_Read(&ThisTransitionNumber,FLASH_SECTOR_SIZE*2+4,sizeof(ThisTransitionNumber));//���豸��
    Load_One_FindZero();//�������б������һ������������WΪ�ϵ�����
    delay_ms(800);
    TIM_ITConfig(TIM9,TIM_IT_CC1,ENABLE);//��ʼ��ʱ��ر��˱����źŵĲ�׽����ϵͳ�ȶ�֮�����¿���
    USART_Cmd(USART3, ENABLE);  //ʹ�ܴ���3
    BeepOFF;//�����ʼ���ɹ���ֹͣ����
  	RTRbuf=(CAN_DATA_FRAME *)mymalloc(SRAMIN,sizeof(CAN_DATA_FRAME));
		printf("��ʼ���ɹ�");
}

/*******************************************************************************
** ��������: EventCreate
** ��������: ��������������Ҫ���¼��ṹ��ע������ź����������Ҫע����osconfig������
** ����˵��: None
** ����˵��: None
** ������Ա: ����
** ��������: 2019-06-21
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
    stepFreeSem = OSSemCreate(0);//����ͷ��ź���
    // CarApplyPassTransSem = OSSemCreate(0);//С���������ź���
    CarAlreadyPassSem = OSSemCreate(0);//С���Ѿ�ͨ���ź���
    CarAlreadyUpMbox = OSMboxCreate((void*)0);//С���Ѿ��Ϲ��ź���
    CarAlreadyDownSem = OSSemCreate(0);
    ModBusFlagSem=OSSemCreate(0);
    AgreeChangeModeSem=OSSemCreate(0);
    Can2Find0AckSem = OSSemCreate(0);//����ɹ�������ͽ���ź�����Ȼ����ظ��յ����ź���
    TransFindzeroSem = OSSemCreate(0);//����xinhaoliang

    can1Mbox = OSMboxCreate((void*)0);
    can2Mbox = OSMboxCreate((void*)0);
    TransLocationQeue = OSQCreate(&TransLocationQeutb[0],10);//������λ������Ϣ����
    TranPreMissionMbox = OSMboxCreate((void*)0);
    StepRunMbox = OSMboxCreate((void*)0);
    CarApplyPassTransMbox=OSMboxCreate((void*)0);
    CarApplyChangeTranMbox=OSMboxCreate((void*)0);
    ApplyChangeModeBox=OSMboxCreate((void*)0);//����ģʽ����ź�����
}

/*******************************************************************************
** ��������: TaskCreate
** ��������: ���񴴽�����
** ����˵��: None
** ����˵��: None
** ������Ա: ����
** ��������: 2019-06-21
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
    OSTaskCreate(HEARBEAT_TASK,(void *)0,&HEARTBEAT_TASK_STK[HEARTBEAT_TASK_SIZE-1], HEARTBEAT_PRIO);//��������
    OSTaskCreate(MODBUS_Task,(void *)0,&MODBUS_TASK_STK[MODBUS_TASK_SIZE-1],MODBUS_PRIO);//MODBUS��������
    OSTaskCreate(STATE_Task,(void *)0,  &STATE_TASK_STK[STATE_TASK_SIZE-1],STATE_PRIO);//״̬�����߳�
    //OSTaskCreate(ModeChange_Task,(void *)0,&MODE_TASK_STK[MODE_TASK_SIZE-1],MODE_PRIO);//ģʽ��������߳�
}
/*******************************************************************************
** ��������: StateQuery_Index_judegment
** ��������: ����Ӧ���֡  ֱ�ӻظ����
** ����˵��: tempNode: [����/��]
** ����˵��: None
** ������Ա: ����
** ��������: 2019-07-24
********************************************************************************/
void StateQuery_Index_judegment(CAN_DATA_FRAME * tempNode)
{
    u8 Sub_index=tempNode->id.MasteridBit.Subindex;
    switch(Sub_index)//�ж�������
    {
    case GetTransStatus://��ȡת����״̬����01 ���ڼ�ʱ���� ��������---��������
    {
        SendTransStatus(tempNode);//�ϴ�ת����״̬
    }
    break;
    case GetTransConfig://��ȡת��������03 ���ڼ�ʱ���� ��������---��������
    {
        SendTransConfig(tempNode);//�ϴ�ת��������
    }
    break;
    case GetTransStatistics://��ȡͳ������05 ���ڼ�ʱ���� ��������---��������
    {
        SendTransStatistics(tempNode);//����ת����ͳ�����ݣ����õ�֡���ͺ���
    }
    break;
    case SetTransConfig://����ת�������������ݣ�0x04 ��������ı�궨�ı�����
    {
        ACKSendFram(CAN2_CHANNEL,tempNode);//���̻�Ӧ������
        /*�˴���Ҫ�������Э�̣�ȷ��֡���ͻ����ĸ�ʽ*/
    }
    break;

    case GetControlConfig://��ȡ������������Ϣ  0x27 ���ڼ�ʱ���� ��������---��������
    {
        SentControlConfig(tempNode);
    }
    break;
    }
}

/*******************************************************************************
** ��������: Broadcast_Judegment
** ��������: �Թ㲥֡�Ĵ���
** ����˵��: canChan: [����/��]
**			 tempNode: [����/��]
** ����˵��: None
** ������Ա: ����
** ��������: 2019-07-27
********************************************************************************/
void Broadcast_Judegment(u8 canChan,CAN_DATA_FRAME * tempNode)
{
    u8 Sub_index=tempNode->id.MasteridBit.Subindex;
    switch(Sub_index)
    {
    case GetUniqueID://��ȡARMΨһID 0x10 ���ڼ�ʱ���� ��������---��������  �㲥
    {
        SendDeviceOnlyID(tempNode);//����ARMΨһ��ID
    }
    break;
    case GetCanComMod://��ȡCAN��ͨ��ģʽ 0x12 ���ڼ�ʱ���� ��������---��������  �㲥
    {
        SentCANCommitMode(tempNode);
    }
    break;
    case SetCANID://����ΨһARM��ID���룬0x17 ����CANID��Ҳ�������ñ����豸��  �㲥
    {
        // ACKSendFram(CAN2_CHANNEL,tempNode);//���̻�Ӧ������
        SetCANIDByARMID(tempNode);
    }
    break;
    case SetCanComMod://����CanͨѶģʽ 0x19 //�����Ҫ����������룬��ʹ��������ڴ� �㲥
    {
        SetCANCommitMode(tempNode);
    }
    break;
    }
}
/*******************************************************************************
** ��������: Master_Index_judegment
** ��������: ����ģʽ���ж����ʹ�����
** ����˵��: canChan: [����/��]
**			 tempNode: [����/��]
**			 frameBuf: [����/��]
** ����˵��: None
** ������Ա: ����
** ��������: 2019-07-17
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
//        case SetRestart://0x20,//�������������
//        {
//            __set_FAULTMASK(1);//�����жϹر�
//            NVIC_SystemReset();//�����λ
//        }
//        break;

        case ControlCheck://0x22,//�������Լ�
        {
            ACKSendFram(CAN2_CHANNEL,tempNode);//���̻�Ӧ������
            Apply_Change_Mission(tempNode);
        }
        break;

        case TransFindzero://ת����У��//�Զ�ģʽ�£�ֻ����������ܸ�ת����У�������11
        {
            ACKSendFram(CAN2_CHANNEL,tempNode);//���̻�Ӧ������
            Apply_Change_Mission(tempNode);
        }
        break;

        case TranPerMission://Ԥ������fe
        {
            if((TransStatus.TrackUse.TrackStatus&0x3c)!=0x00)//�Ѿ�����������//Ԥ�������Ѿ�����
            {
                tempNode->dataLen=1;
                tempNode->id.canId=CAN_TRANSFER_MAININDEX<<8|ThisTransitionNumber<<16|TranPerMission|0x04<<24;//�����ŷ���������
                tempNode->canMsg.dataBuf[0]=Comexefailed;//˵���д��󣬹���Ѿ�������//�������Ǵ������
                tempNode->nextMsg=NULL;
                CAN2_Single_Send(tempNode);//��֡���ͺ���
							printf("Ԥ���̱߳�ռ�ã�������Ԥ���̡߳�\r\n");
            }
            else
            {
            ACKSendFram(CAN2_CHANNEL,tempNode);//���̻�Ӧ������
            Apply_Change_Mission(tempNode);
            }
        }
        break;

        case TransLocation://ת������λ10
        {
            ACKSendFram(CAN2_CHANNEL,tempNode);//���̻�Ӧ������
            Apply_Change_Mission(tempNode);
        }
        break;

        case TransMove://ת�����ƶ� 12
        {
            ACKSendFram(CAN2_CHANNEL,tempNode);//���̻�Ӧ������
            Apply_Change_Mission(tempNode);
        }
        break;

        case TransStop://ɲ����ֹͣת�� fc ֹͣ�����ʱ���Ͳ����ٽ��лָ���
        {
            //CAN_SEND_FRAME *TempSendFram;
            //TempSendFram = (CAN_SEND_FRAME *)mymalloc(SRAMIN,sizeof(CAN_SEND_FRAME));//֪ͨ�Ľ��֡
            ACKSendFram(CAN2_CHANNEL,tempNode);//���̻�Ӧ������
            if(TransStatus.TrackUse.Usebit.ExeCommands==T_Yes)//ֻ��ת�������ڹ�����ʱ�򣬲Ż�����ֹͣ
            {
                TranferSpeed=TranferStop;//ֹͣ��־λ
            }//            memset(TempSendFram,0,sizeof(CAN_SEND_FRAME));
//            TempSendFram->id=tempNode->id.canId;
//            TempSendFram->len=1;
//            TempSendFram->canMsg.dataBuf[0]=No_Err;
//            TempSendFram->nextMsg=NULL;
//            CAN_Post_Queue(CAN2_CHANNEL,TempSendFram);//ѹ������֡����
        }
        break;
        case AgreeChangeMode:
        {
            ACKSendFram(CAN2_CHANNEL,tempNode);//���̻�Ӧ������
            OSSemPost(AgreeChangeModeSem);
        }
        break;


        }
    }
    break;
    }
}
/*******************************************************************************
** ��������: Index_Judegment
** ��������: �����������жϣ�ִ�в�ͬ������
** ����˵��: canChan: canͨ��
**			 id:֡id
**			 index:����
**			 tempNode:����֡��
**			 frameBuf:
** ����˵��: None
** ������Ա: ����
** ��������: 2019-06-27
�Ķ�˵����2019-07-05 �������˴���tempNodeָ����ԭ����ACT���������е�ָ�룬������������
��act�̱߳�����һ������ʱ�򣬽�tenonodeָ�뷢�͵���е��������ִ�����̣߳���ʱ����ACT�߳�
�����tempNode�����ͷţ�������ʱ������ִ�����̻߳�û����ɽ��ָ�������񣬾ͻ��������
�Ķ���memcpy
********************************************************************************/
void Index_Judegment(u8 canChan,u32 id,u8 index,CAN_DATA_FRAME * tempNode,CAN_DATA_FRAME *frameBuf)
{
//    INT8U oserr;
//    CAN_DATA_FRAME * CopyTempNode;
	  
    enum _INDEX AgreeMentIndex;
    AgreeMentIndex=(enum _INDEX)index;




    /*ת�����������Զ�ģʽ�£�����ģʽ�£�С���ܸ�ת�������ȣ��ѻ����ֶ�״̬�����ɵ���*/
    if(TransStatus.DeviceMode==OperatingMode)
    {
        switch(AgreeMentIndex)
        {

        case GetTransStatus://��ȡת����״̬����01 ���ڼ�ʱ���� ��������---��������
        {
            SendTransStatus(tempNode);//�ϴ�ת����״̬
        }
        break;

        case ReStart://ʵ�������λff
        {
            __set_FAULTMASK(1);//�����жϹر�
            NVIC_SystemReset();//�����λ
        }
        break;
        case CarApplyPass://С������ͨ��16
        {
            ACKSendFram(CAN2_CHANNEL,tempNode);//���̻�Ӧ������
            Apply_Change_Mission(tempNode);
            //������������棬����һ���ڴ汣����tempnode�����ݣ�ִ����֮��Ż��������ִ�У����Բ�����ָ���ִ�е�����
        }
        break;
        case CarAlreadyPass://С���Ѿ�ͨ��18
        {
            ACKSendFram(CAN2_CHANNEL,tempNode);//���̻�Ӧ������
            OSSemPost(CarAlreadyPassSem);
            //CarApplyPass_Taskִ���߳��л�ȴ�����ź�����ֻ������ź����������Ż��ͷ�����߳�
        }
        break;
        case CarApplyChange://С������ת��10
        {
					printfcount++;
					printf("\r\n<����ID��%llu>��1/14���Ѿ����յ� %d ��С������ת��֡����%d�Ź����%d�Ź����\r\n",printfcount,
					tempNode->id.idBit.sendDeviceId,
					tempNode->canMsg.dataBuf[0],tempNode->canMsg.dataBuf[1]);
					if(LastFramFlag != tempNode->id.idBit.sendDeviceId)
					{
						ACKSendFram(CAN2_CHANNEL,tempNode);//���̻�Ӧ������
            Apply_Change_Mission(tempNode);
						printf("<����ID��%llu>��2/14���Ѿ���%d��С������ת��֡���л�Ӧ��\r\n",printfcount,tempNode->id.idBit.sendDeviceId);
						LastFramFlag=tempNode->id.idBit.sendDeviceId;
					}
					else
					{
						printf("<����ID��%llu>��ERR���յ�%d��С������ת��֡�������������л�Ӧ��\r\n",printfcount,tempNode->id.idBit.sendDeviceId);
					}
						 
            //������������棬����һ���ڴ汣����tempnode�����ݣ�ִ����֮��Ż��������ִ�У����Բ�����ָ���ִ�е�����
        }
        break;

        case CarAlreadyUpTrack://С���Ѿ��Ϲ��12
        {
					u8 *carnum;
					printf("<����ID��%llu>��6/14���Ѿ����յ� %d ��С���Ѿ��Ϲ�֡����ʱ�Ѿ��Ϲ��\r\n",printfcount,tempNode->id.idBit.sendDeviceId);
					if((TransStatus.TrackUse.TrackStatus&0x3c)!=0x00&&alreadlyuptrack==0)//ֻ�е������������ʱ��Ż�����Ѿ��Ϲ�����֡����������Ѿ��Ϲ��ۼ��������
					{
					  ACKSendFram(CAN2_CHANNEL,tempNode);//���̻�Ӧ������
            printf("<����ID��%llu>��7/14���Ѿ���%d��С���Ѿ��Ϲ���л�Ӧ\r\n",printfcount,tempNode->id.idBit.sendDeviceId);
						alreadlyuptrack=1;
					  carnum=(u8 *)mymalloc(SRAMIN,1);
						*carnum=tempNode->id.idBit.sendDeviceId;
            OSMboxPost(CarAlreadyUpMbox,carnum);//��ʼת��
					}
					else
					{
						printf("<����ID��%llu>��ERR���յ�%d��С���Ѿ��Ϲ�֡���������л�Ӧ\r\n",printfcount,tempNode->id.idBit.sendDeviceId);
					}
        }
        break;

        case CarAlreadyDownTrack://С���Ѿ��¹��14
        {
					printf("<����ID��%llu>��12/14���Ѿ����յ� %d ��С���Ѿ��¹�֡���¹��",printfcount,tempNode->id.idBit.sendDeviceId	);
					if((TransStatus.TrackUse.TrackStatus&0x3c)!=0x00)//ֻ�е������������ʱ��Ż�����Ѿ��Ϲ�����֡����������Ѿ��Ϲ��ۼ��������
					{
						ACKSendFram(CAN2_CHANNEL,tempNode);//���̻�Ӧ������
						printf("<����ID��%llu>��13/14���Ѿ���%d��С���¹�֡���л�Ӧ\r\n",printfcount,tempNode->id.idBit.sendDeviceId);
            OSSemPost(CarAlreadyDownSem);//����
					}
					else
					{
						printf("<����ID��%llu>��ERR���յ�%d��С���Ѿ��¹�֡���������л�Ӧ\r\n",printfcount,tempNode->id.idBit.sendDeviceId);
					}
        }
        break;

        case ResetTransStatus://����ת����״̬02
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
//        case TransLocation://ת������λ10
//        {
//            /*2019-07-05�������޸ģ�ʹ����Ϣ����ʱ�����븴���ڴ棬�����ڴ�ᱻ�ͷţ��κ�ʱ������ͬ��ֵ*/
//            CopyTempNode = (CAN_DATA_FRAME *)mymalloc(SRAMIN,sizeof(CAN_DATA_FRAME));
//            memcpy(CopyTempNode,tempNode,sizeof(CAN_DATA_FRAME));
//            OSQPost(TransLocationQeue,CopyTempNode);
//            /*void TransLocation_TASK(void *pdata)�н�������ź���*/
//        }
//        break;
//        case TransFindzero://ת����У��11
//        {
//            CopyTempNode = (CAN_DATA_FRAME *)mymalloc(SRAMIN,sizeof(CAN_DATA_FRAME));
//            memcpy(CopyTempNode,tempNode,sizeof(CAN_DATA_FRAME));
//            OSQPost(TransLocationQeue,CopyTempNode);
//        }
//        break;


//        case ResetTransStatus://����ת����״̬ 0Ϊ����״̬ 02
//        {
//            TransStatus.DeviceMode=tempNode->canMsg.dataBuf[0];
//            TransStatus.WarningCode=tempNode->canMsg.dataBuf[1];
//            TransStatus.ErrorCode=tempNode->canMsg.dataBuf[2];
//        }
//        break;



//        case TransMove://ת�����ƶ� 12
//        {
//            /*�ڵ㶯ʱ������������⿪�̵߳ķ����������׶���������ֱ�ӵ���ִ�к���*/
//            /*ԭ�������ٶ�̫��  ���¶���*/
//            if (tempNode->canMsg.dataBuf[0]+(tempNode->canMsg.dataBuf[1]<<8)==1)
//            {
//                StepMotor_Run(20,0,1);//��ط����ٶȲ���̫С������û�е�λ�ź�
//                OSSemPend(arrivePosSem,0,&oserr);
//            }
//            else if ((INT16S)(tempNode->canMsg.dataBuf[0]+(tempNode->canMsg.dataBuf[1]<<8))==-1)
//            {
//                StepMotor_Run(20,1,1);//��ط����ٶȲ���̫С������û�е�λ�ź�
//                OSSemPend(arrivePosSem,0,&oserr);
//            }
//            else
////�����������̵߳�ִ�У�������߳��лᷢ��һ������ͷ��ź����������������ź�������������Ӱ�������̵߳�ִ��
//            {
//                OSMboxPost(StepRunMbox,tempNode);
//                OSSemPend(stepFreeSem,0,&oserr);//�ȴ��������ִ���̷߳��ص��ź���
//            }
//        }
//        break;

//        case CurrentPreset://���õ�ǰλΪԤ��λ13
//        {
//            TrackCount[(u8)(Reg[ModBus_Calibration]>>8)][(u8)(Reg[ModBus_Calibration])]=TransStatus.EncoderCount;//�������ȫ�ֱ�������Ϊ��ǰ��׼�ı�����
//            TransStatus.DockedNumber=ModBus_Calibration;//���ö�׼�Ĺ���Ÿı�
//        }
//        break;
//        case LoadTransParameters://װ��ת��������14
//        {
//            ReadFlash();//CRCУ�飬У�������Ǳ��������ݵ���������
//            TrackCount_Load(); //��W25Q��ȡ�궨�ı��벢��ʼ������
//        }
//        break;

//        case SaveTransParameters://����ת��������15
//        {
//            TrackCount_Save();
//        }
//        break;
        case GetTransStatus://��ȡת����״̬����01 ���ڼ�ʱ���� ��������---��������
        {
            SendTransStatus(tempNode);//�ϴ�ת����״̬
        }
        break;

        default:
            break;

        }
    }

}
/*******************************************************************************
** ��������: ReadFlash
** ��������: FLASH�д洢�Ĺؼ����ݶ�ȡ���
** ����˵��: None
** ����˵��: None
** ������Ա: ����
** ��������: 2019-07-04
********************************************************************************/
bool ReadFlash(void)
{
    u16 cnt;
    uint8_t i;
    OS_CPU_SR cpu_sr;
    uint8_t *ptr;
    uint8_t *ptr_bak;
    OS_ENTER_CRITICAL();
    ptr  = (uint8_t*)mymalloc(SRAMIN,FLASH_SECTOR_SIZE);//�����������ڴ棬һ�������Ĵ�С4k
    OS_EXIT_CRITICAL();
    if(ptr==NULL) return false;
    OS_ENTER_CRITICAL();
    ptr_bak = (uint8_t*)mymalloc(SRAMIN,FLASH_SECTOR_SIZE);//���뱸���ڴ�����һ��������С4k
    OS_EXIT_CRITICAL();
    if(ptr_bak==NULL)
    {
        OS_ENTER_CRITICAL();
        myfree(SRAMIN,ptr);
        OS_EXIT_CRITICAL();
        return false;
    }
    W25QXX_Read(ptr,0,FLASH_SECTOR_SIZE); //���������ȡһ��������С�����ݵ�ptr������
    for(cnt=0; cnt<FLASH_SECTOR_SIZE; cnt++) //����������е�����
    {
        if(ptr[cnt]!=0xFF)
            break;//�������FF˵����ȡ����������Ч���������ж�cnt
    }
    if(cnt==FLASH_SECTOR_SIZE)
    {
        return false;//cnt����������С˵��һ������ȫ��FF��û��д����ֱ�ӷ���
    }
    for (i=0; i<MAX_READ_TIME; i++) //
    {
        W25QXX_Read(ptr,0,FLASH_SECTOR_SIZE);         //��MAX_READ_TIME��
        if(CRC16(ptr,FLASH_SECTOR_SIZE)==0)
        {
            break;//CRCУ���������������ʱ���i<MAX_READ_TIME
        }
        else
        {
            OSTimeDlyHMSM(0,0,0,10);  //�д�����ʱ10ms ���ʱ���i=MAX_READ_TIME
        }
    }
    if (i<MAX_READ_TIME)//������CRCУ��û�д���
    {
        W25QXX_Read(ptr_bak,FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE); //��ȡ������������ݵ��������ݼĴ���
        if(memcmp(ptr,ptr_bak,FLASH_SECTOR_SIZE))  //�Ƚϣ���ͬΪ0����ͬΪ1
        {
            //�������ͬ��������������ݸ��ǵ���������
            W25QXX_Write(ptr,FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
        }
        OS_ENTER_CRITICAL();
        myfree(SRAMIN,ptr);
        myfree(SRAMIN,ptr_bak);
        OS_EXIT_CRITICAL();
        return true;
    }
    else //������CRCУ���д���
    {
        //ֱ�Ӷ�ȡ��������
        for (i=0; i<MAX_READ_TIME; i++)
        {
            W25QXX_Read(ptr_bak,FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);  //��ȡ��������
            if(CRC16(ptr_bak,FLASH_SECTOR_SIZE)==0)//�����������У��û�д���
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
            //���У��û��������
            W25QXX_Write(ptr_bak,0,FLASH_SECTOR_SIZE);//���������ݸ��ǵ������ݣ������е�����ط�ʱ��˵������������������û��ͨ��У��
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



