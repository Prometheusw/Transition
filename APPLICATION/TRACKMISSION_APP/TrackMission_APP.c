/********************************Copyright (c)**********************************\
**                   (c) Copyright 2019, Main, China
**                           All Rights Reserved
**                           By(��ʵ����ҽ�ƿƼ����޹�˾)
**----------------------------------�ļ���Ϣ------------------------------------
** �ļ�����: TrackMission_APP.c
** ������Ա: ����
** ��������: 2019-06-29
** �ĵ�����: �������
******************************************************************************/
#include "myconfig.h"
/*******************************************************************************
** ��������: TranPerMission_Task
** ��������: Ԥ����������������������������֡������Ԥ����һ��ָ���Ĺ���������˹������
�������2019-07-18�������ı䡣�ɵ�������˫����  0 1�Ŷ�Ӧ0�Ź��  2 3��Ӧ1   4 5��Ӧ2  6 7---3
��ҪԤ���Ĺ����/2 ����
** ����˵��: pdata: [����/��]
** ����˵��: None
** ������Ա: ����
** ��������: 2019-07-18
**�Ķ�2019��8��13��10:49:08��������ĳɶ����������ת����������ֵ�ĳɰ�λ�����+��λ�����
********************************************************************************/
void TranPerMission_Task(void *pdata)
{   CAR_CHANGEMISSION_DATA *TempMissionNode=NULL;
    CAN_SEND_FRAME *TempSendFram;
    pdata=pdata;
    INT8U oserr;
    INT16U Num;
    u8 Target;
    u8 chooseTrack;//ѡ��Ĺ����
    u8 ErgodicTrack;//���������
    u16 D_Value;//��ֵ
    u16 MIN_count=0xffff;//The Min Count;
    OS_CPU_SR cpu_sr;
    pdata=pdata;
    while(1)
    {
        TempMissionNode = (CAR_CHANGEMISSION_DATA *)mymalloc(SRAMIN,sizeof(CAR_CHANGEMISSION_DATA));//������
        if(TempMissionNode!=NULL)//�ڵ�����ɹ�
        {
            TempMissionNode=OSMboxPend(TranPreMissionMbox,0,&oserr);//�ȴ��ɷ��������͹������ź�����
            TempSendFram = (CAN_SEND_FRAME *)mymalloc(SRAMIN,sizeof(CAN_SEND_FRAME));//֪ͨ�Ľ��֡
            chooseTrack=0xff;//Ϊ�˷�ֹ���еĶ��춼������������죬һ�㲻��������������һ������Ǳ궨�����ݶ�ʧ
            MIN_count=0xffff;//��С����ɸѡ����������Ϊ����Ա�������������бȽ�

            /*ѡ�����ŵĶ�����*/
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
            if(chooseTrack==0xff)//���ѡ�񲻵����죬˵��Ŀ����û�б궨�������ݶ�ʧ���������������û�и���
            {
                OS_ENTER_CRITICAL();
                myfree(SRAMIN,TempSendFram);
                myfree(SRAMIN,TempMissionNode);
                OS_EXIT_CRITICAL();
                TransStatus.TrackUse.Usebit.ExeCommands=T_No;//����ִ�����
                continue;//����ĺ�������ִ��
            }
						printf("����Ԥ���̣߳�Ԥ���� %d �Ź����\r\n",TempMissionNode->InitialPoint);
					  TrackLockFunction(LockTrack,chooseTrack);//�����//Num�ĸ߰�λ�Ƕ�λʱѡ��Ķ�����
            Num=Choose_TransLocate_Change(chooseTrack,TempMissionNode->InitialPoint);//����ת�캯��ת�����
            printf("Ԥ���� %d �Ź���ɹ���\r\n",TempMissionNode->InitialPoint);
            Target=(u8)Num;//Num�Ͱ�λ��ת�����׼�Ķ�����
            if(TempMissionNode->InitialPoint==Target)//�ɹ�
            {
							//����ע�Ͳ������֡
                BeepChirptimes++;
                memset(TempSendFram,0,sizeof(CAN_SEND_FRAME));
                TempSendFram->id=TempMissionNode->FarmID;
                TempSendFram->len=1;
                TempSendFram->canMsg.dataBuf[0]=No_Err;
                TempSendFram->nextMsg=NULL;
                CAN_Post_Queue(CAN2_CHANNEL,TempSendFram);//ѹ������֡��������֪ͨС��ͨ����֡�����ڷ����߳��еȴ��Է��Ļ�Ӧ��

                TransStatus.TrackUse.Usebit.ExeCommands=T_No;//����ִ�����
                OS_ENTER_CRITICAL();
                myfree(SRAMIN,TempSendFram);
                myfree(SRAMIN,TempMissionNode);
                OS_EXIT_CRITICAL();
            }
            else
            {
                TransStatus.ErrorCode=Comexefailed;//������ܵ�������ִ�д���
                TransStatistics.ErrTimes++;//����ͳ������+1
                OS_ENTER_CRITICAL();
                myfree(SRAMIN,TempSendFram);
                myfree(SRAMIN,TempMissionNode);
                OS_EXIT_CRITICAL();
            }
        }
        else
        {
            TransStatus.ErrorCode=RAMSpillover;//���ϴ�����ʾ�ڴ����
            TransStatistics.ErrTimes++;
            OS_ENTER_CRITICAL();
            myfree(SRAMIN,TempSendFram);
            myfree(SRAMIN,TempMissionNode);
            OS_EXIT_CRITICAL();
        }

    }

}


/*******************************************************************************
** ��������: CarApplyPass_Task
** ��������: �ڶ������ҹ��������������£�����̻߳ᱻ���������д������е���ֱ��ͨ����
** С������
** ����˵��: pdata: [����/��]
** ����˵��: None
** ������Ա: ����
** ��������: 2019-08-26
********************************************************************************/
void CarApplyPass_Task(void *pdata)
{
    CAR_CHANGEMISSION_DATA *TempMissionNode=NULL;
    CAN_SEND_FRAME *TempFramdown=NULL;//֪ͨС���¹������֡���
    pdata=pdata;
    INT8U sserr;
    OS_CPU_SR cpu_sr;
    OS_ENTER_CRITICAL();
    TempFramdown = (CAN_SEND_FRAME *)mymalloc(SRAMIN,sizeof(CAN_SEND_FRAME));//֪ͨС���¹������֡
    OS_EXIT_CRITICAL();
    while(1)
    {
        while((TransStatus.TrackUse.TrackStatus&0x3c)!=0x00)
        {
            /*********************************�������������ҵ����е�������ֱ��ͨ�����������񣬲��д���********************************/
            CAR_CHANGEMISSION_DATA *ergodicMissionNode=NULL;//��ʱ��㣬ָ������ת������ͷ���
            ergodicMissionNode = g_CarApplyChangedata;//ָ��ͷ���
            CAR_CHANGEMISSION_DATA *DeleteNode=NULL;//���������Ҫɾ��free�Ľ��
            u8 choseMoving=0xff;
            u8 SuitMissionCount=0;
            u8 ergodicMoving=0;
            while(ergodicMissionNode->NextMission!=NULL)
            {
                if(ergodicMissionNode->NextMission->MissionMark==2)//����ǹ��ת������
                {
                    for(ergodicMoving=0; ergodicMoving<MAX_MovingTrack; ergodicMoving++)//�����������
                    {
                        /*���л��������Ŀ������������������ֱ��ͨ��*/
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
                    TrackLockFunction(LockTrack,choseMoving);//�����
                    memset(TempFramdown,0,sizeof(CAN_SEND_FRAME));
                    TempFramdown->len=2;
                    TempFramdown->id=ergodicMissionNode->NextMission->CarNum<<16|ThisTransitionNumber<<8|CarCanDownTrack|0x14<<24;//������Ҫ�ĳ�֪ͨС�������¹��������
                    TempFramdown->canMsg.dataBuf[0]=ergodicMissionNode->NextMission->InitialPoint;//��ʼλ
                    TempFramdown->nextMsg=NULL;
                    CAN_Post_Queue(CAN2_CHANNEL,TempFramdown);//ѹ������֡��������֪ͨС���¹��֡�����ڷ����߳��еȴ��Է��Ļ�Ӧ��
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
                OSSemPend(CarAlreadyDownSem,0,&sserr);//�ȴ�С���Ѿ��¹��ź���
                TrackLockFunction(UnlockTrack,TempMissionNode->TerminalPoint);//�����
            }
        }
    }
}

/*****************************-++��������++-*****************************************
** ��������: CarApplyChange_Task
** ��������: С������ת������//ִ���߳�
** ����˵��: pdata: [����/��]
** ����˵��: None
** ������Ա: ����
** ��������: 2019-07-02
********************************************************************************/
void CarApplyChange_Task(void *pdata)
{
    INT8U oserr;
    INT8U sserr;
	  u8 *nowcarnum;
    INT16U Num;
    INT8U InitialTrackNum=0;//��ʼ�����
    INT8U TerminalTrackNum=0;//�յ�����
    u8 ergodicMoving=0;//�����������
    u16 deffValue;//��ֵ
    u16 MIN_Value=0xffff;//��С��ֵ
    u8  BestchoseMoving;//������ѡ�����
    OS_CPU_SR cpu_sr;
    CAR_CHANGEMISSION_DATA *TempMissionNode=NULL;//��ʱ����������������
    CAN_SEND_FRAME *TempFramup=NULL;//֪ͨС���Ϲ������֡���
    CAN_SEND_FRAME *TempFramdown=NULL;//֪ͨС���¹������֡���
	  CAN_SEND_FRAME *CarIsDownTrack=NULL;//֪ͨ�������С�������¹����֡�ڵ㣬���������������·��

    while(1)
    {
        TempFramup = (CAN_SEND_FRAME *)mymalloc(SRAMIN,sizeof(CAN_SEND_FRAME));//֪ͨС���Ϲ������֡
        TempFramdown = (CAN_SEND_FRAME *)mymalloc(SRAMIN,sizeof(CAN_SEND_FRAME));//֪ͨС���¹������֡

        TempMissionNode=OSMboxPend(CarApplyChangeTranMbox,0,&oserr);//�ȴ��ɷ��������͹������ź����䣨˵����ת������
        MIN_Value=0xffff;//���������������Ա���������Ƚ�
        u8 LockedFlag=0;//�Ƿ����ֱ��ͨ���ı�־
        // TransStatus.TrackUse.Usebit.Monopolize=T_Yes;//�������λ��1
        TransStatus.TrackUse.Usebit.ExeCommands=T_Yes;//����ִ������
        for(ergodicMoving=0; ergodicMoving<MAX_MovingTrack; ergodicMoving++)
        {
            /*���л��������Ŀ������������������ֱ��ͨ��*/
            if((TrackCount[ergodicMoving][TempMissionNode->InitialPoint]==TrackCount[ergodicMoving][TempMissionNode->TerminalPoint])&&
                    (TrackCount[ergodicMoving][TempMissionNode->InitialPoint]!=0xffff)&&(TrackCount[ergodicMoving][TempMissionNode->TerminalPoint]!=0xffff))
            {
                LockedFlag=1;//Ϊ1���ǿ���ֱ��ͨ��������
                deffValue=abs(TrackCount[ergodicMoving][TempMissionNode->TerminalPoint]-TransStatus.EncoderCount);
                if(deffValue<MIN_Value)
                {
                    MIN_Value=deffValue;
                    BestchoseMoving=ergodicMoving;
                }
                //  TransStatus.TrackUse.Usebit.Monopolize=T_No;//�������λ��0,˵������
            }
        }

        /*�ж��Ƿ����ֱ��ͨ�����Ƿ�Ϊ��������*/
        if(LockedFlag==0)//û����ֱ��ͨ���Ĺ����˵���Ƕ�������
        {
            MIN_Value=0xffff;//���������������Ա���������Ƚ�
            TransStatus.TrackUse.Usebit.Monopolize=T_Yes;//�������λ��1
            /*���Ŷ�����ѡ���ܣ����ţ�����ǰ���뵽��ʼ������+��ʼ��Ŀ�����ľ���ֵ֮��MIN*/
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

            /*���ת�Ƶ���ʼλ��*��������ֵ�Ƕ���+����*/
            printf("<����ID��%llu>��3/14��%d��ת������ʼת�Ƶ�%d�Ź������ʼ�����!\r\n",printfcount,ThisTransitionNumber,TempMissionNode->InitialPoint);
						//printf("The %d Transition starts to switch to the %d track!\r\n",ThisTransitionNumber,TempMissionNode->InitialPoint);
            TrackLockFunction(LockTrack,BestchoseMoving);//�����
						Num = Choose_TransLocate_Change(BestchoseMoving,TempMissionNode->InitialPoint);//����ת�캯��ת�����������ֵ��ת����Ĺ���ţ������жϹ���Ƿ�ת���ɹ�
            InitialTrackNum=(u8)Num;//��׼�Ķ������
            if(InitialTrackNum == TempMissionNode->InitialPoint)//����Ѿ�ת������ʼλ�óɹ�
            {
							  printf("<����ID��%llu>��4/14��%d��ת����ת�Ƶ�%d�Ź���ɹ���\r\n",printfcount,ThisTransitionNumber,TempMissionNode->InitialPoint);
                memset(TempFramup,0,sizeof(CAN_SEND_FRAME));//��Ϊ��û����������֡���ݣ�ȫ������Ϊ�㣬��ֹ���ݴ���
                TempFramup->len=2;
                TempFramup->id=TempMissionNode->CarNum<<16|ThisTransitionNumber<<8|CarCanUpTrack|0x14<<24;//������Ҫ�ĳ�֪ͨС�������Ϲ��������
                TempFramup->canMsg.dataBuf[0]=TempMissionNode->InitialPoint;//��ʼλ
                TempFramup->canMsg.dataBuf[1]=TempMissionNode->TerminalPoint;//�յ�λ��
                TempFramup->nextMsg=NULL;
                CAN_Post_Queue(CAN2_CHANNEL,TempFramup);//ѹ������֡��������֪ͨС���Ϲ��֡�����ڷ����߳��еȴ��Է��Ļ�Ӧ��
							  printf("<����ID��%llu>��5/14��֪ͨ%d��С���Ϲ����\r\n",printfcount,TempMissionNode->CarNum);
                nowcarnum = (u8 *)OSMboxPend(CarAlreadyUpMbox,0,&sserr);//�ȴ�С���Ѿ��Ϲ��ź�����
//							while(*nowcarnum!=TempMissionNode->CarNum)//������յ��Ĳ��Ǵ����̵�С�����룬�Ǿ���Ϊ�˴������⣬һֱ����
//							{
//									nowcarnum = (u8 *)OSMboxPend(CarAlreadyUpMbox,0,&sserr);//�ȴ�С���Ѿ��Ϲ��ź�����
//							}
							printf("<����ID��%llu>��8/14��%d��С���Ϲ���Ѿ���ɣ�\r\n",printfcount,TempMissionNode->CarNum);
              TrackLockFunction(UnlockTrack,BestchoseMoving);//���������
            }
            /*ת�Ƶ���ʼλ�ò����Ϲ��������*/
            /*���ת�Ƶ�Ŀ��λ��*/
            //TrackLockFunction(LockTrack,TempMissionNode->TerminalPoint);//���������
					  TrackLockFunction(LockTrack,BestchoseMoving);//�����
						printf("<����ID��%llu>��9/14��%d��ת������ʼת�Ƶ�%d�Ź��!���յ�����\r\n",printfcount,ThisTransitionNumber,TempMissionNode->TerminalPoint);
            Num = Choose_TransLocate_Change(BestchoseMoving,TempMissionNode->TerminalPoint);//����ת�캯��ת�����������ֵ��ת����Ĺ���ţ������жϹ���Ƿ�ת���ɹ�
            TerminalTrackNum=(u8)Num;
            if(TerminalTrackNum==TempMissionNode->TerminalPoint)//����任�Ѿ��ɹ�
            {
							  printf("<����ID��%llu>��10/14��%d��ת����ת�Ƶ�%d�Ź���ɹ���\r\n",printfcount,ThisTransitionNumber,TempMissionNode->TerminalPoint);
                memset(TempFramdown,0,sizeof(CAN_SEND_FRAME));
                TempFramdown->len=2;
                TempFramdown->id=TempMissionNode->CarNum<<16|ThisTransitionNumber<<8|CarCanDownTrack|0x14<<24;//������Ҫ�ĳ�֪ͨС�������¹��������
                TempFramdown->canMsg.dataBuf[0]=TempMissionNode->InitialPoint;//��ʼλ
                TempFramdown->nextMsg=NULL;
                CAN_Post_Queue(CAN2_CHANNEL,TempFramdown);//ѹ������֡��������֪ͨС���¹��֡�����ڷ����߳��еȴ��Է��Ļ�Ӧ��
							/*С���ո��¹���͸��������������С���Ѿ��¹�״̬֪ͨ֡*/
							//���
							  CarIsDownTrack = (CAN_SEND_FRAME *)mymalloc(SRAMIN,sizeof(CAN_SEND_FRAME));//֪ͨ���������С���Ѿ��¹����֡
							  memset(CarIsDownTrack,0,sizeof(CAN_SEND_FRAME));//֡���
							  CarIsDownTrack->len=1;//������1
							  CarIsDownTrack->id=0x04<<24|ThisTransitionNumber<<16|CAN_TRANSFER_MAININDEX<<8|CarIsDowning;
							  CarIsDownTrack->canMsg.dataBuf[0]=TempMissionNode->CarNum;
							  CarIsDownTrack->nextMsg=NULL;
							//������
							  CAN_Post_Queue(CAN2_CHANNEL,CarIsDownTrack);//�������������
							/*���ڷ����߳��еȴ�����������Ļ�Ӧ*/
							printf("<����ID��%llu>��11/14��֪ͨ%d��С���¹�\r\n",printfcount,TempMissionNode->CarNum);
								TrackLockFunction(LockTrack,BestchoseMoving);//�����
                OSSemPend(CarAlreadyDownSem,5000,&sserr);//�ȴ�С���Ѿ��¹��ź���
						if(sserr==OS_ERR_NONE)
						{
								printf("<����ID��%llu>��14/14��%d��С���¹�ɹ���%d��С�����̽���\r\n\r\n\r\n",printfcount,TempMissionNode->CarNum,TempMissionNode->CarNum);
						}
						else
						{
							printf("��ʱ\r\n");
						}
						
							/*�˴�Ӧ���и���ҪС��λ�õ�֡*/
                TrackLockFunction(UnlockTrack,BestchoseMoving);//�����
                TransStatus.TrackUse.Usebit.Monopolize=T_No;//�������λ�ͷ�
                TransStatus.TrackUse.Usebit.ExeCommands=T_No;//����ִ�������ͷ�
            }
            alreadlyuptrack=0;//�����Ѿ��Ϲ�
						alreadlydowntrack=0;
            OS_ENTER_CRITICAL();
						myfree(SRAMIN,CarIsDownTrack);
            myfree(SRAMIN,TempFramdown);
            myfree(SRAMIN,TempFramup);
            myfree(SRAMIN,TempMissionNode);
						myfree(SRAMIN,nowcarnum);
            OS_EXIT_CRITICAL();
					  

        }
        else//flag==1�����ǿ���ֱ��ͨ��������
        {

            /*���ת�Ƶ�Ŀ��λ��*/
            //TrackLockFunction(LockTrack,TempMissionNode->TerminalPoint);//���������
            Num = Choose_TransLocate_Change(BestchoseMoving,TempMissionNode->TerminalPoint);//����ת�캯��ת�����������ֵ��ת����Ĺ���ţ������жϹ���Ƿ�ת���ɹ�
            TrackLockFunction(LockTrack,BestchoseMoving);//��������������������Ѿ��г�ͨ��
            TerminalTrackNum=(u8)Num;
            if(TerminalTrackNum==TempMissionNode->TerminalPoint)//����任�Ѿ��ɹ�
            {

                memset(TempFramdown,0,sizeof(CAN_SEND_FRAME));
                TempFramdown->len=2;
                TempFramdown->id=TempMissionNode->CarNum<<16|ThisTransitionNumber<<8|CarCanDownTrack|0x14<<24;//������Ҫ�ĳ�֪ͨС�������¹��������
                TempFramdown->canMsg.dataBuf[0]=TempMissionNode->InitialPoint;//��ʼλ
                TempFramdown->nextMsg=NULL;
                CAN_Post_Queue(CAN2_CHANNEL,TempFramdown);//ѹ������֡��������֪ͨС���¹��֡�����ڷ����߳��еȴ��Է��Ļ�Ӧ��
                /**************************************************************************************************************/
							  OSSemPend(CarAlreadyDownSem,0,&sserr);//�ȴ�С���Ѿ��¹��ź���//�����//Ԥ��ʱ����������û���¹죬��������
							  
							 
							
                TrackLockFunction(UnlockTrack,BestchoseMoving);//���������
                TransStatus.TrackUse.Usebit.Monopolize=T_No;//�������λ�ͷ�
                TransStatus.TrackUse.Usebit.ExeCommands=T_No;//����ִ�������ͷ�
            }
            OS_ENTER_CRITICAL();
            myfree(SRAMIN,TempFramdown);
            myfree(SRAMIN,TempFramup);
            myfree(SRAMIN,TempMissionNode);
            OS_EXIT_CRITICAL();
        }
        /*��ǰ�ѹ�����Ϻã���ֹת���ʱ����ܵ�Ԥ������*/
        //TrackLockFunction(LockTrack,TempMissionNode->InitialPoint);//���������




        OS_ENTER_CRITICAL();
        myfree(SRAMIN,TempFramdown);
        myfree(SRAMIN,TempFramup);
        myfree(SRAMIN,TempMissionNode);
        OS_EXIT_CRITICAL();

    }
}
/*******************************************************************************
** ��������: MissionAnalyse_TASK
** ��������: ת�������������ѯ���񣬵�������������������ת�������е�ʱ�򣬷��������Ͳ�ͬ���ź�����
** ����˵��: pdata: [����/��]
** ����˵��: None
** ������Ա: ����
** ��������: 2019-07-01
********************************************************************************/
void MissionAnalyse_Task(void *pdata)
{
    pdata=pdata;
    while(1)
    {
//        CAR_CHANGEMISSION_DATA *TempMissionNode=NULL;//��ʱ��㣬ָ������ת������ͷ���
//        CAR_CHANGEMISSION_DATA *DeleteNode;
//       TempMissionNode = g_CarApplyChangedata;//ָ��ͷ���
        /*�����ڵ�����ִ��һ������������ʱ����*/

        /*��������Ϊ�ա�����������߶���ʱ���ó�cpuʹ��Ȩ*/
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

        Mission_Analyse(g_CarApplyChangedata);//����ת�������񲢷��Ͳ�ͬ���ź�����
    }
}

/*******************************************************************************
** ��������: TransFindZero_Task
** ��������: �������� ����100
** ����˵��: pdata: [����/��]
** ����˵��: None
** ������Ա: ����
** ��������: 2019-07-03
********************************************************************************/
void TransFindZero_Task(void *pdata)
{
    pdata=pdata;
    while(1)
    {
        //OSSemPend(TransFindzeroSem,0,&oserr);//���޵ȴ������ź���
        if(FindZeroGlob>0)//˵���ж��Ѿ�����
        {
            OSTimeDlyHMSM(0,0,0,10);  //10ms���˲�
            if ((GPIOB->IDR & 0x02) == 0x00)
            {
                FindZeroCtrl=ISOnZero;//��ȡPB1��ֵ�����ʱ�����PB1��0��˵���Ѿ�ѹ��У�㿪��
                FindZeroGlob=0;
            }
            else if((GPIOB->IDR & 0x02) == 0x02)
            {
                FindZeroCtrl=ISAwayZero;//��ȡPB1��ֵ�����ʱ�����PB1��1��˵���Ѿ��뿪У�㿪��
                FindZeroGlob=0;
            }
        }
        else
            OSTimeDlyHMSM(0,0,0,5);  //�ó�CPU

    }
}
/*******************************End of File************************************/





