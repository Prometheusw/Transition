#include "myconfig.h"
INT16U TrackCount[MAX_MovingTrack][MAX_FixedTrack]={0};//�궨������
INT16U TempTrackCount[MAX_MovingTrack][MAX_FixedTrack]={0};//�궨������ʱ����
u8 LastFramFlag=0;
u8 TranferSpeed;//ת�����ٶ�
u8 EnableStopButton;//�Ƿ���ɲ���������µ�ͣ����־
u8 FindZeroCtrl=ISAwayZero;//������Ʊ�־λ����ʼֵ�ǲ������㿪�������
u32 sn0;//�豸ΨһARM��־
u8 ThisTransitionNumber;//���豸�豸��
u8 EquipmentType;//�豸���ͺ��룻
T_Control_Message  ControlMessage;//�������������Ϣ
u8 Burnin=0;//�ϻ����Ա�־λ
CAN_DATA_FRAME *RTRbuf=NULL;
uint64_t printfcount=0;//��ӡ���̼��������ڲ鿴
void * TransLocationQeutb[10];//��������λ��Ϣ����
u16 MAX_Count;//������
u16 MIN_Count;//��С����
volatile u8 Now_direction;//��ǰ�������
u8 IsMotorRun=T_No;//����Ƿ��˶���

u8 ThresholdValue;//��λ����ִ�еĴ�����ֵ����һ������û�����㣬��ֵ���Ｋ�ޣ�ǿ��У��
/*ͳ�����ݣ�����������*/
T_Trans_Statistics TransStatistics= {
    0x10203032,
    0x10203033,
    0x10203034,
    0x10203035,
    0x10203036,
    0x10203037,
    0x10203038,
    0x10203039,
};//ת����ͳ�ƽṹ

u8 alreadlyuptrack=0;

INT8U W25QXXSendBuff[130];//W25QXX�ķ��ͻ���
INT8U W25QXXReceiveBuff[130];//���ջ���
volatile  u8 FindZeroGlob=0;//�����Ƿ񴥷�����־
T_CAN_Communicat_Mode CANCommunicatMode;//CANͨѶ��־

OS_EVENT * TransLocationQeue;//�����λ�ź�����
OS_EVENT * CarAlreadyPassSem;//С���Ѿ�ͨ���ź���
OS_EVENT * CarAlreadyUpMbox;//С���Ѿ��Ϲ��ź���
OS_EVENT * CarAlreadyDownSem;//С���Ѿ��¹���ź���
OS_EVENT * TransFindzeroSem;//�������
OS_EVENT * AgreeChangeModeSem;//��ͬ��ı�ģʽ
OS_EVENT * ApplyChangeModeBox;//����ģʽ����ź�����
/************************************�ź���ָ��******************************************/
OS_EVENT *can1RecvSem;	 //CAN1�����ź���ָ��
OS_EVENT *can2RecvSem;	 //CAN2�����ź���ָ��
OS_EVENT *can1ActSem;	   //CAN1����֡�����ź���ָ��
OS_EVENT *can2ActSem;	   //CAN2����֡�����ź���ָ��
OS_EVENT *can1AckSem;	   //CAN1Ӧ��֡�ź���ָ��
OS_EVENT *can2AckSem;	   //CAN1Ӧ��֡�ź���ָ��
OS_EVENT *can1CtlAckSem; //CAN1����Ӧ�����ź���ָ��
OS_EVENT *can2CtlAckSem; //CAN2����Ӧ�����ź���ָ��
OS_EVENT *can1CtlResSem;  //CANָ�ӽ���ź���
OS_EVENT *can1InfoAckSem;  //CAN1��ϢӦ���ź���
OS_EVENT *can2InfoAckSem;  //CAN2��ϢӦ���ź���
OS_EVENT *can2AckSem;
OS_EVENT *can1SendSem;
OS_EVENT *can2SendSem;


OS_EVENT *Can2Find0AckSem;//����ɹ�������ͽ���ź�����Ȼ����ظ��յ����ź���
/************************************��Ϣ����******************************************/
OS_EVENT *can1Mbox;
OS_EVENT *can2Mbox;
OS_EVENT *canCtlResMbox;  //CANָ�ӽ������
OS_EVENT *StepRunMbox;//�����������
T_Trans_Status DefoultTransStatus={
    .WarningCode=0,
    .ErrorCode=0,
    .DockedNumber=0,
    .TrackUse.Usebit.FindzeroValid=0,
    .TrackUse.Usebit.ExeCommands=T_No,
    .TrackUse.Usebit.TrackUse0=T_Unlock,
    .TrackUse.Usebit.TrackUse1=T_Unlock,
    .TrackUse.Usebit.TrackUse2=T_Unlock,
    .TrackUse.Usebit.TrackUse3=T_Unlock,
    .TrackUse.Usebit.Monopolize=T_Unlock,
    .EncoderCount=0,
};//����ȫ�ֱ�����ת����״̬�ṹ//���������ΪУ�������״̬��Ĭ����
T_Trans_Status TransStatus= {
    .DeviceMode=OperatingMode,
    .WarningCode=0,
    .ErrorCode=0,
    .DockedNumber=0,
    .NowLoction=0,
    .TrackUse.Usebit.FindzeroValid=0,
    .TrackUse.Usebit.ExeCommands=T_No,
    .TrackUse.Usebit.TrackUse0=T_Unlock,
    .TrackUse.Usebit.TrackUse1=T_Unlock,
    .TrackUse.Usebit.TrackUse2=T_Unlock,
    .TrackUse.Usebit.TrackUse3=T_Unlock,
    .TrackUse.Usebit.Monopolize=T_Unlock,
	  .TrackUse.Usebit.CheakSelf=1,
    .EncoderCount=0,
};//ת����״̬�ṹ







