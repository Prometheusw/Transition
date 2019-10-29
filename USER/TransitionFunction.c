#include "myconfig.h"

/*******************************************************************************
** ��������: GetTransStatisticsSend
** ��������: ���ͳ������//�˺�����Ҫ�����ڴ洫���β�Ϊ�ڴ�ָ��
** ����˵��: CopyTempNode: [����/��]
** ����˵��: None
** ������Ա: ����
** ��������: 2019-07-10
********************************************************************************/
void SendTransStatistics(CAN_DATA_FRAME *tempNode)
{
    CAN_DATA_FRAME *CopyTempNode;
    OS_CPU_SR cpu_sr;
    OS_ENTER_CRITICAL();
    CopyTempNode = (CAN_DATA_FRAME *)mymalloc(SRAMIN,sizeof(CAN_DATA_FRAME));//�����ڴ�
    if(CopyTempNode!=NULL)
    {
        OS_EXIT_CRITICAL();
        memcpy(CopyTempNode,tempNode,sizeof(CAN_DATA_FRAME));//����֡���ݵ���ʱ�ڴ���
        memset(CopyTempNode->canMsg.dataBuf,0,sizeof(CAN_MSG));

        memcpy(&(CopyTempNode->canMsg.dataBuf[0]),&TransStatistics.WarningTimes,sizeof(TransStatistics.WarningTimes));
        memcpy(&(CopyTempNode->canMsg.dataBuf[4]),&TransStatistics.ErrTimes,sizeof(TransStatistics.ErrTimes));
        CopyTempNode->dataLen=8;
        CAN2_Single_Send(CopyTempNode);//��֡���ͺ���

        memcpy(&(CopyTempNode->canMsg.dataBuf[0]),&TransStatistics.CommWarning,sizeof(TransStatistics.CommWarning));
        memcpy(&(CopyTempNode->canMsg.dataBuf[4]),&TransStatistics.CommErr,sizeof(TransStatistics.CommErr));
        CopyTempNode->dataLen=8;
        CAN2_Single_Send(CopyTempNode);//��֡���ͺ���

        memcpy(&(CopyTempNode->canMsg.dataBuf[0]),&TransStatistics.FindzeroTimes,sizeof(TransStatistics.FindzeroTimes));
        memcpy(&(CopyTempNode->canMsg.dataBuf[4]),&TransStatistics.MoveTimes,sizeof(TransStatistics.MoveTimes));
        CopyTempNode->dataLen=8;
        CAN2_Single_Send(CopyTempNode);//��֡���ͺ���

        memcpy(&(CopyTempNode->canMsg.dataBuf[0]),&TransStatistics.PassTimes,sizeof(TransStatistics.PassTimes));
        memcpy(&(CopyTempNode->canMsg.dataBuf[4]),&TransStatistics.ShuntTimes,sizeof(TransStatistics.ShuntTimes));
        CopyTempNode->dataLen=8;
        CAN2_Single_Send(CopyTempNode);//��֡���ͺ���
        OS_ENTER_CRITICAL();
        myfree(SRAMIN,CopyTempNode);
        OS_EXIT_CRITICAL();
    }
    else
    {
        TransStatus.ErrorCode=RAMSpillover;//���ϴ�����ʾ�ڴ����
        TransStatistics.ErrTimes++;
        GPIO_ResetBits(GPIOF,GPIO_Pin_9);//ָʾ�Ƴ���
    }

}

/*******************************************************************************
** ��������: SendTransStatus
** ��������: �ϴ�ת����״̬����Ϊ����ģʽ������ģʽ
** ����˵��: CopyTempNode: Ҫ���͵�����֡
** ����˵��: None
** ������Ա: ����
** ��������: 2019-07-10
********************************************************************************/
void SendTransStatus(CAN_DATA_FRAME *tempNode)
{
    CAN_DATA_FRAME *CopyTempNode;
    OS_CPU_SR cpu_sr;
    OS_ENTER_CRITICAL();
    CopyTempNode = (CAN_DATA_FRAME *)mymalloc(SRAMIN,sizeof(CAN_DATA_FRAME));//�����ڴ�
    if(CopyTempNode!=NULL)
    {
        OS_EXIT_CRITICAL();
        memcpy(CopyTempNode,tempNode,sizeof(CAN_DATA_FRAME));//����֡���ݵ���ʱ�ڴ���
        memset(CopyTempNode->canMsg.dataBuf,0,sizeof(CAN_MSG));
        CopyTempNode->dataLen=8;
        CopyTempNode->canMsg.dataBuf[0]=TransStatus.DeviceMode;//���ֽ��ǹ���ģʽ
        CopyTempNode->canMsg.dataBuf[1]=TransStatus.WarningCode;//�������
        CopyTempNode->canMsg.dataBuf[2]=TransStatus.ErrorCode;//�������
        CopyTempNode->canMsg.dataBuf[3]=TransStatus.DockedNumber;//��׼�Ĺ����//�ɲ�Ҫ
        CopyTempNode->canMsg.dataBuf[4]=TransStatus.TrackUse.TrackStatus;//�����
        CopyTempNode->canMsg.dataBuf[5]=(INT8U)TransStatus.EncoderCount;//�Ͱ�λ//��ǰ������
        CopyTempNode->canMsg.dataBuf[6]=(TransStatus.EncoderCount)>>8;//�߰�λ//��ǰ������
        CopyTempNode->canMsg.dataBuf[7]=TransStatus.TrackUse.TrackStatus;//
        CAN2_Single_Send(CopyTempNode);//��֡���ͺ����������õ����ݷ��͸�2
        OS_ENTER_CRITICAL();
        myfree(SRAMIN,CopyTempNode);//�ͷ��ڴ�
        OS_EXIT_CRITICAL();
    }
    else
    {
        TransStatus.ErrorCode=RAMSpillover;//���ϴ�����ʾ�ڴ����
        TransStatistics.ErrTimes++;
        //GPIO_ResetBits(GPIOF,GPIO_Pin_9);//ָʾ�Ƴ���
    }
}
/*******************************************************************************
** ��������: SendTransConfig
** ��������: ����ת����������Ϣ����
** ����˵��: tempNode: [����/��]
** ����˵��: None
** ������Ա: ����
** ��������: 2019-07-10
********************************************************************************/
void SendTransConfig(CAN_DATA_FRAME *tempNode)
{
    INT8U GetConfigCount;
    INT8U GetConfigTimes;
    INT8U GetConfigTrackNum;
    CAN_DATA_FRAME *CopyTempNode;
    OS_CPU_SR cpu_sr;
    OS_ENTER_CRITICAL();
    CopyTempNode = (CAN_DATA_FRAME *)mymalloc(SRAMIN,sizeof(CAN_DATA_FRAME));//�����ڴ�
    if(CopyTempNode!=NULL)
    {
        OS_EXIT_CRITICAL();
        memcpy(CopyTempNode,tempNode,sizeof(CAN_DATA_FRAME));//����֡���ݵ���ʱ�ڴ���
        memset(CopyTempNode->canMsg.dataBuf,0,sizeof(CAN_MSG));
        CopyTempNode->dataLen=8;
        for(GetConfigTrackNum=0; GetConfigTrackNum<MAX_MovingTrack; GetConfigTrackNum++) //�ܹ������
        {
            for(GetConfigTimes=0; GetConfigTimes<1; GetConfigTimes++)//ÿ�����������Ҫ������֡
            {
                for(GetConfigCount=0; GetConfigCount<4; GetConfigCount++)//���Ĵ���������can֡������λ
                {
                    CopyTempNode->canMsg.dataBuf[2*GetConfigCount]=TrackCount[GetConfigTrackNum][GetConfigTimes*4+GetConfigCount];//�Ͱ�λ
                    CopyTempNode->canMsg.dataBuf[2*GetConfigCount+1]=TrackCount[GetConfigTrackNum][GetConfigTimes*4+GetConfigCount]>>8;//�߰�λ
                }
                CAN2_Single_Send(CopyTempNode);//��֡���ͺ���
            }
        }

        OS_ENTER_CRITICAL();
        myfree(SRAMIN,CopyTempNode);
        OS_EXIT_CRITICAL();
    }
    else
    {
        TransStatus.ErrorCode=RAMSpillover;//���ϴ�����ʾ�ڴ����
        TransStatistics.ErrTimes++;
        GPIO_ResetBits(GPIOF,GPIO_Pin_9);//ָʾ�Ƴ���
    }
}
/*******************************************************************************
** ��������: SendDeviceOnlyID
** ��������: �����豸ΨһARM��ź�ID  �����豸CANID
** ����˵��: tempNode: [����/��]
** ����˵��: None
** ������Ա: ����
** ��������: 2019-07-17
********************************************************************************/
void SendDeviceOnlyID(CAN_DATA_FRAME *tempNode)
{
    CAN_DATA_FRAME *CopyTempNode;
    OS_CPU_SR cpu_sr;
    OS_ENTER_CRITICAL();
    CopyTempNode = (CAN_DATA_FRAME *)mymalloc(SRAMIN,sizeof(CAN_DATA_FRAME));//�����ڴ�
    if(CopyTempNode!=NULL)
    {
        OS_EXIT_CRITICAL();
        memset(CopyTempNode,0,sizeof(CAN_DATA_FRAME));
        memcpy(CopyTempNode,tempNode,sizeof(CAN_DATA_FRAME));//����֡���ݵ���ʱ�ڴ���
        memset(CopyTempNode->canMsg.dataBuf,0,sizeof(CAN_MSG));
        CopyTempNode->dataLen=8;
        CopyTempNode->canMsg.dataBuf[0]=sn0;//ARM��ʶ����Ͱ�λ
        CopyTempNode->canMsg.dataBuf[1]=sn0>>8;//9-16
        CopyTempNode->canMsg.dataBuf[2]=sn0>>16;//17-24
        CopyTempNode->canMsg.dataBuf[3]=sn0>>24;//25-32
        CopyTempNode->canMsg.dataBuf[4]=ThisTransitionNumber;//���豸��ID
        CAN2_Single_Send(CopyTempNode);//��֡���ͺ���//ID in this function to change
        OS_ENTER_CRITICAL();
        myfree(SRAMIN,CopyTempNode);//release the memory
        OS_EXIT_CRITICAL();
    }
    else
    {
        TransStatus.ErrorCode=RAMSpillover;//���ϴ�����ʾ�ڴ����
        TransStatistics.ErrTimes++;
        GPIO_ResetBits(GPIOF,GPIO_Pin_9);//ָʾ�Ƴ���
    }

}

/*******************************************************************************
** ��������: SetCANIDByARMID
** ��������: ����ΨһARM��ID���룬����CANID��Ҳ�������ñ����豸��
** ����˵��: tempNode: [����/��]
** ����˵��: None
** ������Ա: ����
** ��������: 2019-07-17
********************************************************************************/
void SetCANIDByARMID(CAN_DATA_FRAME *tempNode)
{
    CAN_DATA_FRAME *CopyTempNode;
    u32 ARMOnlyID;
    OS_CPU_SR cpu_sr;
    OS_ENTER_CRITICAL();
    CopyTempNode = (CAN_DATA_FRAME *)mymalloc(SRAMIN,sizeof(CAN_DATA_FRAME));//�����ڴ�
    if(CopyTempNode!=NULL)
    {
        OS_EXIT_CRITICAL();
        memset(CopyTempNode,0,sizeof(CAN_DATA_FRAME));
        memcpy(CopyTempNode,tempNode,sizeof(CAN_DATA_FRAME));//����֡���ݵ���ʱ�ڴ���
        // memset(CopyTempNode->canMsg.dataBuf,0,sizeof(CAN_MSG));
        ARMOnlyID=(CopyTempNode->canMsg.dataBuf[3]<<24)+(CopyTempNode->canMsg.dataBuf[2]<<16)+(CopyTempNode->canMsg.dataBuf[1]<<8)+CopyTempNode->canMsg.dataBuf[0];
        if(ARMOnlyID!=sn0)
        {
            OS_ENTER_CRITICAL();
            myfree(SRAMIN,CopyTempNode);//release the memory
            OS_EXIT_CRITICAL();
            return;
        }
        else
        {
            ThisTransitionNumber=CopyTempNode->canMsg.dataBuf[4];
            OS_ENTER_CRITICAL();
            myfree(SRAMIN,CopyTempNode);//release the memory
            OS_EXIT_CRITICAL();
        }
    }
    else//�ڴ����
    {
        TransStatus.ErrorCode=RAMSpillover;//���ϴ�����ʾ�ڴ����
        TransStatistics.ErrTimes++;
        GPIO_ResetBits(GPIOF,GPIO_Pin_9);//ָʾ�Ƴ���
    }
    return;
}
/*******************************************************************************
** ��������: GetCANCommitMode
** ��������: ���CANͨѶģʽ,�ϴ�Can��ͨѶģʽ
** ����˵��: tempNode: [����/��]
** ����˵��: None
** ������Ա: ����
** ��������: 2019-07-17
********************************************************************************/
void SentCANCommitMode(CAN_DATA_FRAME *tempNode)
{
    CAN_DATA_FRAME *CopyTempNode;
    OS_CPU_SR cpu_sr;
    OS_ENTER_CRITICAL();
    CopyTempNode = (CAN_DATA_FRAME *)mymalloc(SRAMIN,sizeof(CAN_DATA_FRAME));//�����ڴ�
    if(CopyTempNode!=NULL)
    {
        OS_EXIT_CRITICAL();
        // memset(CopyTempNode,0,sizeof(CAN_DATA_FRAME));
        memcpy(CopyTempNode,tempNode,sizeof(CAN_DATA_FRAME));//����֡���ݵ���ʱ�ڴ���
        memset(CopyTempNode->canMsg.dataBuf,0,sizeof(CAN_MSG));
        CopyTempNode->dataLen=1;
        CopyTempNode->canMsg.dataBuf[0]=CANCommunicatMode.CanMode.CanModeMask;//����һ���ֽ����ΪCanͨѶģʽ
        CAN2_Single_Send(CopyTempNode);//��֡���ͺ���//ID in this function to change
        OS_ENTER_CRITICAL();
        myfree(SRAMIN,CopyTempNode);//release the memory
        OS_EXIT_CRITICAL();
    }
}
/*******************************************************************************
** ��������: SetCANCommitMode
** ��������: ����CanͨѶģʽ//�����Ҫ����������룬��ʹ��������ڴ棬ֱ�ӽ�tempNode
����λ�ĵ�һλ��CAn����״̬��
** ����˵��: tempNode: [����/��]
** ����˵��: None
** ������Ա: ����
** ��������: 2019-07-17
********************************************************************************/
void SetCANCommitMode(CAN_DATA_FRAME *tempNode)
{
    CAN_DATA_FRAME *CopyTempNode;
    OS_CPU_SR cpu_sr;
    OS_ENTER_CRITICAL();
    CopyTempNode = (CAN_DATA_FRAME *)mymalloc(SRAMIN,sizeof(CAN_DATA_FRAME));//�����ڴ�
    if(CopyTempNode!=NULL)
    {
        OS_EXIT_CRITICAL();
        memset(CopyTempNode,0,sizeof(CAN_DATA_FRAME));
        memcpy(CopyTempNode,tempNode,sizeof(CAN_DATA_FRAME));//����֡���ݵ���ʱ�ڴ���
        memset(CopyTempNode->canMsg.dataBuf,0,sizeof(CAN_MSG));
        CANCommunicatMode.CanMode.CanModeMask=CopyTempNode->canMsg.dataBuf[0];
        /*..............*/
        OS_ENTER_CRITICAL();
        myfree(SRAMIN,CopyTempNode);//release the memory
        OS_EXIT_CRITICAL();
    }


}

/*******************************************************************************
** ��������: SentControlConfig
** ��������: ��ȡ������������Ϣ//���Ϳ�����������Ϣ//��������������Ϣ�ֳ���֡����
** ����˵��: tempNode: [����/��]
** ����˵��: None
** ������Ա: ����
** ��������: 2019-07-17
********************************************************************************/
void SentControlConfig(CAN_DATA_FRAME *tempNode)
{
    CAN_DATA_FRAME *CopyTempNode;
    OS_CPU_SR cpu_sr;
    OS_ENTER_CRITICAL();
    CopyTempNode = (CAN_DATA_FRAME *)mymalloc(SRAMIN,sizeof(CAN_DATA_FRAME));//�����ڴ�
    if(CopyTempNode!=NULL)
    {
        OS_EXIT_CRITICAL();
        memset(CopyTempNode,0,sizeof(CAN_DATA_FRAME));//�����뵽���ڴ���������
        memcpy(CopyTempNode,tempNode,sizeof(CAN_DATA_FRAME));//����֡���ݵ���ʱ�ڴ���
        memset(CopyTempNode->canMsg.dataBuf,0,sizeof(CAN_MSG));//�����ݲ�������
        CopyTempNode->canMsg.dataBuf[0]=ControlMessage.ProductSeries;//��һλ��Ϊ��Ʒϵ��
        memcpy(&CopyTempNode->canMsg.dataBuf[1],&ControlMessage.HardwareVersion,4);//�ڴ濽������ControlMessageһ����������ڵ�
        memset(&CopyTempNode->canMsg.dataBuf[5],0,3);//�ڵ�ʣ�µĲ�������
        CopyTempNode->dataLen=8;

        CAN2_Single_Send(CopyTempNode);//��֡���ͺ���//ID in this function to change//First Frame

        memset(CopyTempNode->canMsg.dataBuf,0,sizeof(CAN_MSG));//�����ݲ����ڴ�ȫ������
        memcpy(&CopyTempNode->canMsg.dataBuf[0],&ControlMessage.SoftwareVersion,8);//�ڴ濽������ControlMessage������������ڵ�
        CopyTempNode->dataLen=8;
        CAN2_Single_Send(CopyTempNode);//��֡���ͺ���//ID in this function to change//Secend Fram

        memset(CopyTempNode->canMsg.dataBuf,0,sizeof(CAN_MSG));//���ݲ�������
        memcpy(&CopyTempNode->canMsg.dataBuf[0],&ControlMessage.ManufactureData,8);//�ڴ濽������ControlMessage������������ڵ�
        CopyTempNode->dataLen=8;
        CAN2_Single_Send(CopyTempNode);//��֡���ͺ���//ID in this function to change//

        OS_ENTER_CRITICAL();
        myfree(SRAMIN,CopyTempNode);//release the memory
        OS_EXIT_CRITICAL();
    }
}

/*******************************************************************************
** ��������: UpdataNowLoction
** ��������: ���µ�ǰλ����Ϣ�������1�źͶ���֮�����12 ������ö�׼2�ž���22
** ����˵��: None
** ����˵��: None
** ������Ա: ����
** ��������: 2019-07-10
********************************************************************************/
//void UpdataNowLoction(void)
//{
//    if(TransStatus.EncoderCount==TrackCount[0])
//        TransStatus.NowLoction=0x00;
//    else if(TransStatus.EncoderCount==TrackCount[1])
//        TransStatus.NowLoction=0x11;
//    else if(TransStatus.EncoderCount==TrackCount[2])
//        TransStatus.NowLoction=0x22;
//    else if(TransStatus.EncoderCount==TrackCount[3])
//        TransStatus.NowLoction=0x33;

//    else if(TransStatus.EncoderCount>TrackCount[0]&&TransStatus.EncoderCount<TrackCount[1])
//        TransStatus.NowLoction=0x01;
//    else if(TransStatus.EncoderCount>TrackCount[1]&&TransStatus.EncoderCount<TrackCount[2])
//        TransStatus.NowLoction=0x12;
//    else if(TransStatus.EncoderCount>TrackCount[2]&&TransStatus.EncoderCount<TrackCount[3])
//        TransStatus.NowLoction=0x23;
//    else TransStatus.NowLoction=0xff;
//}


/*******************************************************************************
** ��������: ReadMemeryData
** ��������: ��ȡָ���ڴ������
** ����˵��: Add: ��ַ
** ����˵��: None
** ������Ա: ����
** ��������: 2019-07-17
********************************************************************************/
u32 ReadMemeryData(u32  Add)
{
    static u32 TempDataBuff;
    TempDataBuff=*(vu32 *)Add;
    return TempDataBuff;
}

/*******************************************************************************
** ��������: WriteMemeryData
** ��������: ��ָ���ڴ�����д����
** ����˵��: Add: [����/��]
**			 data: [����/��]
** ����˵��: None
** ������Ա: ����
** ��������: 2019-07-17
********************************************************************************/
void WriteMemeryData(u32 Add,u32 data)
{
    u32 *TempPoint=(u32 *)Add;
    *TempPoint=data;
    return;
}

/*******************************************************************************
** ��������: ModBusHandleFunc
** ��������: ModBus�Ĵ�����ַ�ı�����Ĺ���
** ����˵��: : [����/��]
** ����˵��: None
** ������Ա: ����
** ��������: 2019-08-01
********************************************************************************/
void ModBusHandleFunc(void)
{
    CAN_DATA_FRAME * tempNode;
    OS_CPU_SR cpu_sr;
    CAR_CHANGEMISSION_DATA *TempMissionNode=NULL;//��ʱ��㣬ָ������ת������ͷ���
    CAR_CHANGEMISSION_DATA *DeleteNode=NULL;//���������Ҫɾ��free�Ľ��
    TempMissionNode =g_CarApplyChangedata;
    INT32S Count;
    CAN_DATA_FRAME *TempNode;
    u8 oserr;
    u32 * mode;//��Ҫ���䴫�ݵ�ģʽ���
    /**********************************αװ�ƶ����ƶ��Ĳ����Ǳ�������������֮�֣�������������˶�������******************************/
    if(Reg[ModBus_MoveCount]!=0)//�ƶ�������
    {
        Count=(INT32S)Reg[ModBus_MoveCount];
        if(Count==31)
        {
            if(ZeroDirction==anticlockwise)//�������ʱ��У��
            {
                StepMotor_Run(20,1,31);//��ط����ٶȲ���̫С������û�е�λ�ź�
            }
            else
            {
                StepMotor_Run(20,0,31);//��ط����ٶȲ���̫С������û�е�λ�ź�
            }
            OSSemPend(arrivePosSem,0,&oserr);
            Reg[ModBus_MoveCount]=0;
        }
        else if(Count==(INT16S)-31)
        {
            if(ZeroDirction==clockwise)//�����shunʱ��У��
            {
                StepMotor_Run(20,1,31);//��ط����ٶȲ���̫С������û�е�λ�ź�
            }
            else
            {
                StepMotor_Run(20,0,31);//��ط����ٶȲ���̫С������û�е�λ�ź�
            }
            OSSemPend(arrivePosSem,0,&oserr);
            Reg[ModBus_MoveCount]=0;
        }
        else
        {
            /*���Ĵ����е�ֵαװ��CAN֡����֡Id�еĽ����豸��ַ��Ϊ�Ǳ����豸��ַ��ֵ���Ա�����*/
            OS_ENTER_CRITICAL();
            tempNode=(CAN_DATA_FRAME *)mymalloc(SRAMIN,sizeof(CAN_DATA_FRAME));
            if(tempNode!=NULL)
            {
                OS_EXIT_CRITICAL();
                memset(tempNode,0,sizeof(CAN_DATA_FRAME));
                tempNode->id.canId=0x04fe0212;
                tempNode->canMsg.dataBuf[0]=Count;
                tempNode->canMsg.dataBuf[1]=Count>>8;
                tempNode->nextMsg=NULL;
                Apply_Change_Mission(tempNode);
                Reg[ModBus_MoveCount]=0;
                OS_ENTER_CRITICAL();
                myfree(SRAMIN,tempNode);
                OS_EXIT_CRITICAL();
            }
        }
    }

    /*******************************************αװ��λ*******************************************************/
    if(Reg[ModBus_LocNumer]!=0xffff)
    {
        OS_ENTER_CRITICAL();
        tempNode=(CAN_DATA_FRAME *)mymalloc(SRAMIN,sizeof(CAN_DATA_FRAME));
        OS_EXIT_CRITICAL();
        if(tempNode!=NULL)
        {
            memset(tempNode,0,sizeof(CAN_DATA_FRAME));
            tempNode->id.canId=0x04fe0210;
            tempNode->canMsg.dataBuf[0]=Reg[ModBus_LocNumer];
            tempNode->canMsg.dataBuf[1]=Reg[ModBus_LocNumer]>>8;
            tempNode->nextMsg=NULL;
            Apply_Change_Mission(tempNode);
            Reg[ModBus_LocNumer]=0xffff;
            OS_ENTER_CRITICAL();
            myfree(SRAMIN,tempNode);
            OS_EXIT_CRITICAL();
        }
    }
    /*************************************************αװУ��**********************************************************/
    if(Reg[ModBus_FindZero]!=0xffff)
    {
        OS_ENTER_CRITICAL();
        tempNode=(CAN_DATA_FRAME *)mymalloc(SRAMIN,sizeof(CAN_DATA_FRAME));
        if(tempNode!=NULL)
        {
            OS_EXIT_CRITICAL();
            memset(tempNode,0,sizeof(CAN_DATA_FRAME));
            tempNode->id.canId=0x04fe0211;
            tempNode->nextMsg=NULL;
            Apply_Change_Mission(tempNode);
            Reg[ModBus_FindZero]=0xffff;
            OS_ENTER_CRITICAL();
            myfree(SRAMIN,tempNode);
            OS_EXIT_CRITICAL();
        }
    }

    /***************************************αװֹͣ��ֹͣ��ǰ����ִ�е����� �������ʱ����Ҫ���ɲ��***************************************/
    if(Reg[ModBus_Stop]==0x0000)
    {
        /*Ԥ����ΰ���ֹͣ�����������쳣*/
        if(TransStatus.TrackUse.Usebit.ExeCommands==T_Yes||IsMotorRun==T_Yes)//ֻ��ת�������ڹ�����ʱ�򣬲Ż�����ֹͣ
        {
            TranferSpeed=TranferStop;//ֹͣ��־λ
        }
        Reg[ModBus_Stop]=0xffff;
    }
    /**************************************����ɲ�� ������е�����������ֹͣ***********************************************/
    if(Reg[ModBus_Stop]==0x0001)
    {
        while(TempMissionNode->NextMission!=NULL)
        {
            DeleteNode=TempMissionNode->NextMission;
            TempMissionNode->NextMission=DeleteNode->NextMission;
            myfree(SRAMIN,DeleteNode);
        }
        TempMissionNode =g_CarApplyChangedata;
        if(TransStatus.TrackUse.Usebit.ExeCommands==T_Yes||IsMotorRun==T_Yes)//ֻ��ת�������ڹ�����ʱ�򣬲Ż�����ֹͣ
        {
            TranferSpeed=TranferStop;//ֹͣ��־λ
        }
        Reg[ModBus_Stop]=0xffff;
        BeepChirptimes=BeepChirptimes+3;
    }
    /***********************************����������,����*******************************************/
    if(Reg[ModBus_Stop]==0x0002)
    {
        /* while(TempMissionNode->NextMission!=NULL)
         {
             DeleteNode=TempMissionNode->NextMission;
             TempMissionNode->NextMission=DeleteNode->NextMission;
             myfree(SRAMIN,DeleteNode);
         }
         TempMissionNode =g_CarApplyChangedata;
         TranferSpeed=TranferStop;//ֹͣ��־λ*/
        clear_MOTOR_Alarm;
        OSTimeDlyHMSM(0,0,0,100);
        Back_MOTOR_Alarm;
        Reg[ModBus_Stop]=0xffff;//
    }
    /******************************************�����ϻ�����ģʽ********************************************/
    if(Reg[ModBus_Stop]==0x0003)
    {
        Burnin=1;//1�����ϻ�
        Reg[ModBus_Stop]=0xffff;
    }
    /****************************************ͣ�����е�BEEP������***************************************/
    if(Reg[ModBus_Stop]==0x0004)
    {
        BeepOFFORON=1-BeepOFFORON;
        Reg[ModBus_Stop]=0xffff;

    }
    /******************************************�˳��ϻ�����ģʽ****************************************/
    if(Reg[ModBus_Stop]==0x0005)
    {
        Burnin=0;//0
        Reg[ModBus_Stop]=0xffff;
    }



    /***************************************�ı�ģʽΪ�ֶ�DebugMode************************************/
    if(Reg[ModBus_Stop]==0x0006)
    {
        /*��������Զ�ģʽ����ֹ�ఴ������������ߣ�����˵��can�������⣬û������*/
        if(TransStatus.DeviceMode!=DebugMode&&TransStatus.DeviceMode!=OffLine)
        {
            /*��ǰ״̬������ѻ�״̬������Ҫ�ȴ�can��ʱ��,Ȼ����һ֡�����۲��Ƿ�CAN������*/
            if(TransStatus.DeviceMode==standAlone)
            {
                RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);//��������CAN��ʱ��
                OSTimeDlyHMSM(0,0,0,150);
                /*�����������֡�۲�can���Ƿ�����*/
                OS_ENTER_CRITICAL();
                TempNode = (CAN_DATA_FRAME *)mymalloc(SRAMIN,sizeof(CAN_DATA_FRAME));//�����ڴ�
                OS_EXIT_CRITICAL();
//            memset(TempNode->canMsg.dataBuf,0,sizeof(CAN_MSG));
                memset(TempNode,0,sizeof(CAN_DATA_FRAME));
                TempNode->dataLen=0;
                TempNode->id.MasteridBit.Subindex=0x00;//������
                TempNode->id.MasteridBit.MainIndex=0x00;//������
                TempNode->id.MasteridBit.deviceid=0x00;//�豸��//��ĵ�ַ
                TempNode->id.MasteridBit.ReserveBit=0;//����λ
                TempNode->id.MasteridBit.RetryBit=0;//�ط���־
                TempNode->id.MasteridBit.ackBit=0;//Ӧ��֡��־
                TempNode->id.MasteridBit.SubframeBit=0;//��֡��־
                TempNode->id.MasteridBit.MasterslaveBit=0;//������־
                CAN2_Single_Send(TempNode);//��֡���ͺ���//�������������ʧ�ܣ��豸״̬�ͻ�ı�Ϊ����զ״̬
                OS_ENTER_CRITICAL();
                myfree(SRAMIN,TempNode);
                OS_EXIT_CRITICAL();
            }
            /*�������֡���ͳɹ���������ת��ǰ�豸״̬�����ѻ�״̬**/
            if(TransStatus.DeviceMode!=OffLine)//��������ߵĻ�˵��can�߳������⣬���ʱ��Ͳ�Ҫ�ٽ��б任��
            {
                OS_ENTER_CRITICAL();
                mode=(u32 *)mymalloc(SRAMIN,sizeof(mode));
                OS_EXIT_CRITICAL();
                if(mode!=NULL)
                {
                    *mode=DebugMode+1;//��һ��Ϊ�˷�ֹ���������䴫����ֵ
                    OSMboxPost(ApplyChangeModeBox,mode);
                }
//                OS_ENTER_CRITICAL();
//                myfree(SRAMIN,mode);
//                OS_EXIT_CRITICAL();
            }
            Reg[ModBus_Stop]=0xffff;

        }
        else
        {
            Reg[ModBus_Stop]=0xffff;
        }
    }
/***********************************************�ı�ģʽΪ�Զ�***************************************************************/
    if(Reg[ModBus_Stop]==0x0007)
    {
        /*��������Զ�ģʽ����ֹ�ఴ������������ߣ�����˵��can�������⣬û������*/
        if(TransStatus.DeviceMode!=OperatingMode&&TransStatus.DeviceMode!=OffLine)
        {
            /*��ǰ״̬������ѻ�״̬������Ҫ�ȴ�can��ʱ��,Ȼ����һ֡�����۲��Ƿ�CAN������*/
            if(TransStatus.DeviceMode==standAlone)
            {
                RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);//��������CAN��ʱ��
                OSTimeDlyHMSM(0,0,0,150);
                /*�����������֡�۲�can���Ƿ�����*/
                OS_ENTER_CRITICAL();
                TempNode = (CAN_DATA_FRAME *)mymalloc(SRAMIN,sizeof(CAN_DATA_FRAME));//�����ڴ�
                OS_EXIT_CRITICAL();
//            memset(TempNode->canMsg.dataBuf,0,sizeof(CAN_MSG));
                memset(TempNode,0,sizeof(CAN_DATA_FRAME));
                TempNode->dataLen=0;
                TempNode->id.MasteridBit.Subindex=0x00;//������
                TempNode->id.MasteridBit.MainIndex=0x00;//������
                TempNode->id.MasteridBit.deviceid=0x00;//�豸��//��ĵ�ַ
                TempNode->id.MasteridBit.ReserveBit=0;//����λ
                TempNode->id.MasteridBit.RetryBit=0;//�ط���־
                TempNode->id.MasteridBit.ackBit=0;//Ӧ��֡��־
                TempNode->id.MasteridBit.SubframeBit=0;//��֡��־
                TempNode->id.MasteridBit.MasterslaveBit=0;//������־
                CAN2_Single_Send(TempNode);//��֡���ͺ���//�������������ʧ�ܣ��豸״̬�ͻ�ı�Ϊ����զ״̬
                OS_ENTER_CRITICAL();
                myfree(SRAMIN,TempNode);
                OS_EXIT_CRITICAL();
            }
            /*�������֡���ͳɹ���������ת��ǰ�豸״̬�����ѻ�״̬*/
            if(TransStatus.DeviceMode!=OffLine)//��������ߵĻ�˵��can�߳������⣬���ʱ��Ͳ�Ҫ�ٽ��б任��
            {
                OS_ENTER_CRITICAL();
                mode=(u32 *)mymalloc(SRAMIN,sizeof(mode));
                OS_EXIT_CRITICAL();
                if(mode!=NULL)
                {
                    *mode=OperatingMode+1;//��һ��Ϊ�˷�ֹ���������䴫����ֵ
                    OSMboxPost(ApplyChangeModeBox,mode);
                }
//                OS_ENTER_CRITICAL();
//                myfree(SRAMIN,mode);
//                OS_EXIT_CRITICAL();
            }
            Reg[ModBus_Stop]=0xffff;

        }
        else
        {
            Reg[ModBus_Stop]=0xffff;
        }
    }
/**************************************************************�ı�ģʽΪ�ѻ�*************************************************************/
    if(Reg[ModBus_Stop]==0x0008)
    {
        if(TransStatus.DeviceMode!=standAlone)
        {
            if(TransStatus.DeviceMode!=OffLine)//�����ǰ״̬��������ģʽ
            {
                OS_ENTER_CRITICAL();
                mode=(u32 *)mymalloc(SRAMIN,sizeof(mode));
                OS_EXIT_CRITICAL();
                if(mode!=NULL)
                {
                    *mode=standAlone+1;//��һ��Ϊ�˷�ֹ���������䴫����ֵ
                    OSMboxPost(ApplyChangeModeBox,mode);
                }
//                OS_ENTER_CRITICAL();
//                myfree(SRAMIN,mode);
//                OS_EXIT_CRITICAL();
            }
            else//�����ǰ�Ѿ�������ģʽ���Ͳ�����Ҫ���룬ֱ�Ӹı�
            {
                TransStatus.DeviceMode=standAlone;
            }

            Reg[ModBus_Stop]=0xffff;
        }
        else
        {
            Reg[ModBus_Stop]=0xffff;
        }
    }
    /**********************************************αװ�Լ�֡**********************************************/
    if(Reg[ModBus_CheakSelf]==0x0)
    {
        OS_ENTER_CRITICAL();
        tempNode=(CAN_DATA_FRAME *)mymalloc(SRAMIN,sizeof(CAN_DATA_FRAME));
        if(tempNode!=NULL)
        {
            OS_EXIT_CRITICAL();
            memset(tempNode,0,sizeof(CAN_DATA_FRAME));
            tempNode->id.canId=0x04fe0222;
            tempNode->nextMsg=NULL;
            Apply_Change_Mission(tempNode);

//            memset(tempNode,0,sizeof(CAN_DATA_FRAME));
//            tempNode->id.canId=0x04fe0210;
//            tempNode->canMsg.dataBuf[0]=0;
//            tempNode->canMsg.dataBuf[1]=0;
//            tempNode->nextMsg=NULL;
//            Apply_Change_Mission(tempNode);

            Reg[ModBus_CheakSelf]=0xffff;
            OS_ENTER_CRITICAL();
            myfree(SRAMIN,tempNode);
            OS_EXIT_CRITICAL();
        }


    }
    /**********************************ѯ�ʸ�֪��У�㿪�ط���0001 У�㷽��Ϊ��ʱ��*******************************/
    if(Reg[ModBus_CheakSelf]==0x0001)
    {
        Change_diriction(anticlockwise);//У�㿪������ʱ�뷽��
        Reg[ModBus_CheakSelf]=0xffff;
    }
    /**********************************ѯ�ʸ�֪��У�㿪�ط���0002 У�㷽��Ϊ˳ʱ��*******************************/

    if(Reg[ModBus_CheakSelf]==0x0002)
    {
        Change_diriction(clockwise);//У�㿪����˳ʱ�뷽��
        Reg[ModBus_CheakSelf]=0xffff;
    }


    /****************************����Ǳ궨ModBus_Calibration �Ĵ���ǰ��λΪ�����ţ����λ�Ƕ�����**************************/
    if(Reg[ModBus_Calibration]!=0xffff)
    {
        if(TempTrackCount[(u8)(Reg[ModBus_Calibration]>>8)][(u8)(Reg[ModBus_Calibration])]==0xffff)
        {
            TempTrackCount[(u8)(Reg[ModBus_Calibration]>>8)][(u8)(Reg[ModBus_Calibration])]=TransStatus.EncoderCount;//�������ȫ�ֱ�������Ϊ��ǰ��׼�ı�����
            TransStatus.DockedNumber=Reg[ModBus_Calibration];//���ö�׼�Ĺ���Ÿı�
            Calibration_feature((u8)(Reg[ModBus_Calibration]>>8),(u8)(Reg[ModBus_Calibration]));//�Ѿ��궨�Ĺ����Ч��־����
        }
        else
        {
            TempTrackCount[(u8)(Reg[ModBus_Calibration]>>8)][(u8)(Reg[ModBus_Calibration])]=0xffff;
            Feature_Clean((u8)(Reg[ModBus_Calibration]>>8),(u8)(Reg[ModBus_Calibration]));//����궨��־λ
        }
        Reg[ModBus_Calibration]=0xffff;
        //    Reg[9]=0xffff;
        BeepChirptimes++;
    }
    /****************************************ȡ�����α궨��������ԭΪδ�궨ǰ������********************************************************/
    if(Reg[ModBus_SetCalibration]==0x0001)
    {
        memcpy(TempTrackCount,TrackCount,sizeof(TempTrackCount));
        ModBus_TempTrackCount_UpData();//�����Ѿ��궨��λ
        Reg[ModBus_SetCalibration]=0xffff;
        BeepChirptimes++;

    }
    /********************************************������еı궨����****************************************************************/
    if(Reg[ModBus_SetCalibration]==0x0002)
    {
        memset(TempTrackCount,0xffff,sizeof(TempTrackCount));
        ModBus_TempTrackCount_UpData();//�����Ѿ��궨��λ
        Reg[ModBus_SetCalibration]=0xffff;
        BeepChirptimes++;
    }
    /*****************************************�궨���ݱ��浽���豸��FLASH*********************************************************/
    if(Reg[ModBus_SetCalibration]==0x0000)
    {
        memcpy(TrackCount,TempTrackCount,sizeof(TrackCount));
        TrackCount_Save();
        Reg[ModBus_SetCalibration]=0xffff;
        BeepChirptimes++;
        ModBus_TempTrackCount_UpData();//�����Ѿ��궨��λ
        Update_Limit_Count();//������λֵ
    }
    /*********************************************˳ָ�����ʱ�����************************************************************/
    if(Reg[ModBus_DirictionTest]!=0x0000)
    {
        if(Reg[ModBus_DirictionTest]==0x0001)//˳ʱ��
        {
            StepMotor_Run(200,NCtrl,500);
            OSSemPend(arrivePosSem,1000,&oserr);//�ȴ���λ�źţ��ڶ�ʱ��10�жϷ������з���
        }
        else if(Reg[ModBus_DirictionTest]==0x0002)
        {
            StepMotor_Run(200,PCtrl,500);
            OSSemPend(arrivePosSem,1000,&oserr);//�ȴ���λ�źţ��ڶ�ʱ��10�жϷ������з���
        }
        Reg[ModBus_DirictionTest]=0x0000;
    }
    /**********************************************�ı��豸��23*****************************************************************/
    if(Reg[ModBus_ThisTransitionNumber_W]!=0xffff)
    {
        ThisTransitionNumber=Reg[ModBus_ThisTransitionNumber_W];
        W25QXX_Write((u8*)&ThisTransitionNumber,FLASH_SECTOR_SIZE*2+4,sizeof(ThisTransitionNumber));//���豸�Ŵ���FLASH_SECTOR_SIZE*2+4��
        Reg[ModBus_ThisTransitionNumber_W]=0xffff;
        BeepChirptimes++;

    }
}

/*�Ѿ��궨��־λ�ú���*/
void Calibration_feature(u8 T,u8 Num)
{
    if(T==0)
    {
        Reg[10]=Reg[10]|(0x0001<<Num);
    }
    else if(T==1)
    {
        Reg[11]=Reg[11]|(0x0001<<Num);
    }
    else if(T==2)
    {
        Reg[12]=Reg[12]|(0x0001<<Num);
    }
    else if(T==3)
    {
        Reg[13]=Reg[13]|(0x0001<<Num);
    }
    else
        return;
    return;
}
/*���ѡ�еı궨λ�󣬽�����Ӧ�ı�־λ����*/
void Feature_Clean(u8 T,u8 Num)
{
    if(T==0)
    {
        Reg[10]=Reg[10]&~(0x0001<<Num);
    }
    else if(T==1)
    {
        Reg[11]=Reg[11]&~(0x0001<<Num);
    }
    else if(T==2)
    {
        Reg[12]=Reg[12]&~(0x0001<<Num);
    }
    else if(T==3)
    {
        Reg[13]=Reg[13]&~(0x0001<<Num);
    }
    else
        return;
    return;
}


/*����Ƿ���к���*/
bool IS_Track_FREE()
{

    if((TransStatus.DeviceMode==DebugMode)||(g_CarApplyChangedata->NextMission==NULL)||(TransStatus.TrackUse.Usebit.ExeCommands==T_Yes))
    {
        return false;
    }

    return true;
}

/*�ҳ����Ʊ��룬��С*/
void Update_Limit_Count(void)
{
    u8 StaticErdoc;//���������
    u8 MovingErdoc=0;
    u16 LimitCount;
    LimitCount=0x0;
    for(StaticErdoc=0; StaticErdoc<MAX_FixedTrack; StaticErdoc++) //����count
    {
        if(TrackCount[MovingErdoc][StaticErdoc]!=0xffff&&TrackCount[MovingErdoc][StaticErdoc]>LimitCount)//����0xffff������Ч�Ŀ��Ե����
        {
            LimitCount=TrackCount[MovingErdoc][StaticErdoc];//�б�LimitCount������ݾ͸��£����һ�ο϶����
        }
    }
    MAX_Count=LimitCount;//������
    LimitCount=0xffff;
    for(StaticErdoc=0; StaticErdoc<MAX_FixedTrack; StaticErdoc++) //����count
    {
        if(TrackCount[MovingErdoc][StaticErdoc]!=0xffff&&TrackCount[MovingErdoc][StaticErdoc]<LimitCount)//����0xffff������Ч�Ŀ��Ե����
        {
            LimitCount=TrackCount[MovingErdoc][StaticErdoc];//�б�LimitCount������ݾ͸��£����һ�ο϶����
        }
    }
    MIN_Count=LimitCount;//
}
/*�����У��������ܵ��Ĺ������һ�飬��������֮��У��*/
void Self_Function(void)//�����У��������ܵ��Ĺ����һ��
{
    CAN_DATA_FRAME * tempNode;//ģ��֡
    OS_CPU_SR cpu_sr;
    u8 MovingErdoc=0;//����
    u8 StaticErdoc=0;//����
//    u8 TimeCount=0;//����������
    while(Burnin==1&&TransStatus.ErrorCode==No_Err)//�����б�Ҫ����������޹��ϲ����������ź���Ч
    {
        for(StaticErdoc=0; StaticErdoc<MAX_FixedTrack; StaticErdoc++) //����count
        {
            if(TrackCount[MovingErdoc][StaticErdoc]!=0xffff)//����0xffff������Ч�Ŀ��Ե����
            {
                OS_ENTER_CRITICAL();//�����ٽ���
                tempNode=(CAN_DATA_FRAME *)mymalloc(SRAMIN,sizeof(CAN_DATA_FRAME));
                if(tempNode!=NULL)
                {
                    OS_EXIT_CRITICAL();//����ɹ����˳��ٽ���
                    memset(tempNode,0,sizeof(CAN_DATA_FRAME));
                    tempNode->id.canId=0x04fe0210;//��λ
                    tempNode->canMsg.dataBuf[0]=StaticErdoc;
                    tempNode->canMsg.dataBuf[1]=MovingErdoc>>8;
                    tempNode->nextMsg=NULL;
                    Apply_Change_Mission(tempNode);
                    OS_ENTER_CRITICAL();
                    myfree(SRAMIN,tempNode);
                    OS_EXIT_CRITICAL();
                    Reg[19]++;
                }
                // BeepChirptimes++;
                OSTimeDlyHMSM(0,0,0,10000);
            }
            if(TransStatus.ErrorCode!=No_Err)
                break;
        }
//        TimeCount++;//ÿ����һ�֣�������һ
//        if(TimeCount==2)
//        {
//            /*У��*/
//            tempNode=(CAN_DATA_FRAME *)mymalloc(SRAMIN,sizeof(CAN_DATA_FRAME));
//            if(tempNode!=NULL)
//            {
//                memset(tempNode,0,sizeof(CAN_DATA_FRAME));
//                tempNode->id.canId=0x04fe0211;
//                tempNode->nextMsg=NULL;
//                Apply_Change_Mission(tempNode);
//                Reg[ModBus_FindZero]=0xffff;
//                myfree(SRAMIN,tempNode);
//                Reg[19]++;
//            }
//            BeepChirptimes++;
//            TimeCount=0;
//            OSTimeDlyHMSM(0,0,0,8000);
//        }
    }
    //Reg[19]=0;

}

/*�������������װ��һ����������,װ��λ���ڶ�����ǰ��*/
void Load_One_FindZero(void)
{
    OS_CPU_SR cpu_sr;
    CAR_CHANGEMISSION_DATA *NewMissionNode;//���������ݽ��
    CAR_CHANGEMISSION_DATA *TempMissionNode;//��ʱ��㣬ָ������ת������ͷ���
//	  CAR_CHANGEMISSION_DATA *BackUpMissionNode;//���ݽڵ�
    OS_ENTER_CRITICAL();//�����ٽ���
    NewMissionNode = (CAR_CHANGEMISSION_DATA*)mymalloc(SRAMIN,sizeof(CAR_CHANGEMISSION_DATA));//�����ڴ�
    OS_EXIT_CRITICAL();
    if(NewMissionNode!=NULL)
    {
        NewMissionNode->NextMission=NULL;
        NewMissionNode->MissionMark=3;//����������
        NewMissionNode->PreMission=0;//Ԥ����־0
        NewMissionNode->FarmID=0x04fe0211;
    }
    NewMissionNode->TimeMark=OSTimeGet();//���ʱ���־
    TempMissionNode=g_CarApplyChangedata;//��ʱ���ָ��ͷ���
    TempMissionNode->NextMission=NewMissionNode;//
    NewMissionNode->NextMission=NULL;
}
/************************************************************************************************
*******************�ҵ����������Ĺ����ţ�������ʱ���ã�����ʱ��,Ҫ�ص������������Ĺ����
*******************����ֵ��ѡ�еĹ����
************************************************************************************************/
unsigned char Find_NearestTrack(void)
{
    u16 MinCount=0xffff;
    u8 TheChooseTrack;
    u8 ErdicCount;
    for(ErdicCount=0; ErdicCount<MAX_FixedTrack; ErdicCount++)
    {
        if(TrackCount[0][ErdicCount]!=0xffff&&TrackCount[0][ErdicCount]<MinCount)
        {
            MinCount=TrackCount[0][ErdicCount];
            TheChooseTrack=ErdicCount;
        }
    }
    return TheChooseTrack;
}

/***********************************************************************************************
** ��������: Mode_Change
** ��������: ģʽ�ı亯�����ѻ��ֶ��Զ�֮����໥ת�������ߴ���״̬�¿���ֱ�ӱ��Ϊ�ѻ�ģʽ
���Զ��ֶ��ѻ�֮����໥ת��Ҫ�������룬����--Ӧ��--ͬ�⣻���ѻ���Ϊ������Ҫ�ȴ�canȻ������
ģʽ�����ÿ��ģʽ���֮�󣬶��������б���ա�
** ����˵��: mode: [����/��]
** ����˵��: None
** ������Ա: ����
** ��������: 2019-09-26
************************************************************************************************/
u8 Mode_Change(u8 mode)
{
    OS_CPU_SR cpu_sr;
    u8 oser;
    CAR_CHANGEMISSION_DATA *TempMissionNode=NULL;//��ʱ��㣬ָ������ת������ͷ���
    CAR_CHANGEMISSION_DATA *DeleteNode=NULL;//���������Ҫɾ��free�Ľ��
    TempMissionNode =g_CarApplyChangedata;
    CAN_SEND_FRAME *ApplyFram=NULL;
    OS_ENTER_CRITICAL();//�����ٽ���
    ApplyFram = (CAN_SEND_FRAME *)mymalloc(SRAMIN,sizeof(CAN_SEND_FRAME));
    OS_EXIT_CRITICAL();
    memset(ApplyFram,0,sizeof(CAN_SEND_FRAME));
    /*���������ֶ�ģʽ��Ϊ�˷�ֹ�û���ΰ���ģʽ������������������������״̬��CAN���ϲ�ͨ����ʱ�����Ϊ����û������*/
    if(mode==DebugMode&&TransStatus.DeviceMode!=DebugMode&&TransStatus.DeviceMode!=OffLine)
    {
        ApplyFram->len=1;
        ApplyFram->id=0x040002fb|(ThisTransitionNumber<<16);
        ApplyFram->canMsg.dataBuf[0]=DebugMode;//���Ϊ�ֶ�ģʽ
        ApplyFram->nextMsg=NULL;
        CAN_Post_Queue(CAN2_CHANNEL,ApplyFram);//���������֡
        OSSemPend(AgreeChangeModeSem,0,&oser);//�ȴ���Ļ�Ӧ������ط��ĵȴ�Ӧ������
        if(oser==OS_ERR_NONE)//�����Ӧ
        {
            TransStatus.DeviceMode=DebugMode;//�ı�ģʽ

            while(TempMissionNode->NextMission!=NULL)//����������
            {
                DeleteNode=TempMissionNode->NextMission;
                TempMissionNode->NextMission=DeleteNode->NextMission;
                OS_ENTER_CRITICAL();//�����ٽ���
                myfree(SRAMIN,DeleteNode);
                OS_EXIT_CRITICAL();

            }
            TempMissionNode =g_CarApplyChangedata;//����ָ��ͷ���
            //�������ִ���������ͣ��
            if(TransStatus.TrackUse.Usebit.ExeCommands==T_Yes)//ֻ��ת�������ڹ�����ʱ�򣬲Ż�����ֹͣ
            {
                TranferSpeed=TranferStop;//ֹͣ��־λ
            }
        }
    }
		/***********************************************���Ϊ�ѻ�ģʽ**************************************************************/
    else if(mode==standAlone&&TransStatus.DeviceMode!=standAlone)//�����ʱ�����ѻ�ģʽ����Ҫ����ѻ�ģʽ
    {
        ApplyFram->len=1;
        ApplyFram->id=0x040002fb|(ThisTransitionNumber<<16);
        ApplyFram->canMsg.dataBuf[0]=standAlone;//�����ģʽ
        ApplyFram->nextMsg=NULL;
        CAN_Post_Queue(CAN2_CHANNEL,ApplyFram);
        OSSemPend(AgreeChangeModeSem,0,&oser);
        if(oser==OS_ERR_NONE)
        {
            TransStatus.DeviceMode=standAlone;
            OSTimeDlyHMSM(0,0,0,150);//�ó�CPU��״̬�ϴ��̷߳��ͱ任���״̬
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, DISABLE);//�رյ���CAN��ʱ��
            while(TempMissionNode->NextMission!=NULL)
            {
                DeleteNode=TempMissionNode->NextMission;
                TempMissionNode->NextMission=DeleteNode->NextMission;
                OS_ENTER_CRITICAL();//�����ٽ���
                myfree(SRAMIN,DeleteNode);
                OS_EXIT_CRITICAL();
            }
            TempMissionNode =g_CarApplyChangedata;
            if(TransStatus.TrackUse.Usebit.ExeCommands==T_Yes)//ֻ��ת�������ڹ�����ʱ�򣬲Ż�����ֹͣ
            {
                TranferSpeed=TranferStop;//ֹͣ��־λ
            }
        }
    }
    /*���������Զ�ģʽ��Ϊ�˷�ֹ�û���ΰ���ģʽ������������������������״̬��CAN���ϲ�ͨ����ʱ�����Ϊ����û������*/
    else if(mode==OperatingMode&&TransStatus.DeviceMode!=OperatingMode&&TransStatus.DeviceMode!=OffLine)//�����ʱ�����Զ�ģʽ����Ҫ�ı�Ϊ�Զ�ģʽ
    {
        ApplyFram->len=1;
        ApplyFram->id=0x040002fb|(ThisTransitionNumber<<16);
        ApplyFram->canMsg.dataBuf[0]=OperatingMode;//�����ģʽ
        ApplyFram->nextMsg=NULL;
        CAN_Post_Queue(CAN2_CHANNEL,ApplyFram);
        OSSemPend(AgreeChangeModeSem,0,&oser);
        if(oser==OS_ERR_NONE)
        {
            TransStatus.DeviceMode=OperatingMode;

            while(TempMissionNode->NextMission!=NULL)
            {
                DeleteNode=TempMissionNode->NextMission;
                TempMissionNode->NextMission=DeleteNode->NextMission;
                OS_ENTER_CRITICAL();//�����ٽ���
                myfree(SRAMIN,DeleteNode);
                OS_EXIT_CRITICAL();
            }
            TempMissionNode = g_CarApplyChangedata;
            if(TransStatus.TrackUse.Usebit.ExeCommands==T_Yes)//ֻ��ת�������ڹ�����ʱ�򣬲Ż�����ֹͣ
            {
                TranferSpeed=TranferStop;//ֹͣ��־λ
            }
        }
    }
    OS_ENTER_CRITICAL();//�����ٽ���
    myfree(SRAMIN,ApplyFram);
    OS_EXIT_CRITICAL();
    return TransStatus.DeviceMode;

}





