#include "myconfig.h"
INT16U TrackCount[MAX_MovingTrack][MAX_FixedTrack]={0};//标定编码数
INT16U TempTrackCount[MAX_MovingTrack][MAX_FixedTrack]={0};//标定所用零时数组
u8 LastFramFlag=0;
u8 TranferSpeed;//转轨器速度
u8 EnableStopButton;//是否是刹车按键导致的停车标志
u8 FindZeroCtrl=ISAwayZero;//找零控制标志位，初始值是不在找零开关上面的
u32 sn0;//设备唯一ARM标志
u8 ThisTransitionNumber;//本设备设备号
u8 EquipmentType;//设备类型号码；
T_Control_Message  ControlMessage;//定义控制器的信息
u8 Burnin=0;//老化测试标志位
CAN_DATA_FRAME *RTRbuf=NULL;
uint64_t printfcount=0;//打印流程计数，便于查看
void * TransLocationQeutb[10];//定义轨道定位消息队列
u16 MAX_Count;//最大编码
u16 MIN_Count;//最小编码
volatile u8 Now_direction;//当前电机方向
u8 IsMotorRun=T_No;//电机是否运动中

u8 ThresholdValue;//定位任务执行的次数阈值，当一定次数没有找零，阈值到达极限，强制校零
/*统计数据，仅仅做测试*/
T_Trans_Statistics TransStatistics= {
    0x10203032,
    0x10203033,
    0x10203034,
    0x10203035,
    0x10203036,
    0x10203037,
    0x10203038,
    0x10203039,
};//转轨器统计结构

u8 alreadlyuptrack=0;

INT8U W25QXXSendBuff[130];//W25QXX的发送缓存
INT8U W25QXXReceiveBuff[130];//接收缓存
volatile  u8 FindZeroGlob=0;//找零是否触发零点标志
T_CAN_Communicat_Mode CANCommunicatMode;//CAN通讯标志

OS_EVENT * TransLocationQeue;//轨道定位信号邮箱
OS_EVENT * CarAlreadyPassSem;//小车已经通过信号量
OS_EVENT * CarAlreadyUpMbox;//小车已经上轨信号量
OS_EVENT * CarAlreadyDownSem;//小车已经下轨道信号量
OS_EVENT * TransFindzeroSem;//轨道找零
OS_EVENT * AgreeChangeModeSem;//域同意改变模式
OS_EVENT * ApplyChangeModeBox;//申请模式变更信号邮箱
/************************************信号量指针******************************************/
OS_EVENT *can1RecvSem;	 //CAN1接收信号量指针
OS_EVENT *can2RecvSem;	 //CAN2接收信号量指针
OS_EVENT *can1ActSem;	   //CAN1主动帧处理信号量指针
OS_EVENT *can2ActSem;	   //CAN2主动帧处理信号量指针
OS_EVENT *can1AckSem;	   //CAN1应答帧信号量指针
OS_EVENT *can2AckSem;	   //CAN1应答帧信号量指针
OS_EVENT *can1CtlAckSem; //CAN1控制应答处理信号量指针
OS_EVENT *can2CtlAckSem; //CAN2控制应答处理信号量指针
OS_EVENT *can1CtlResSem;  //CAN指挥结果信号量
OS_EVENT *can1InfoAckSem;  //CAN1信息应答信号量
OS_EVENT *can2InfoAckSem;  //CAN2信息应答信号量
OS_EVENT *can2AckSem;
OS_EVENT *can1SendSem;
OS_EVENT *can2SendSem;


OS_EVENT *Can2Find0AckSem;//找零成功后给域发送结果信号量，然后域回复收到的信号量
/************************************消息邮箱******************************************/
OS_EVENT *can1Mbox;
OS_EVENT *can2Mbox;
OS_EVENT *canCtlResMbox;  //CAN指挥结果邮箱
OS_EVENT *StepRunMbox;//步进电机步数
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
};//定义全局变量，转轨器状态结构//这个变量作为校零和重置状态的默认量
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
};//转轨器状态结构







