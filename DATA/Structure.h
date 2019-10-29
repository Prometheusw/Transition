/********************************Copyright (c)**********************************\
**                   (c) Copyright 2019, Main, China
**                           All Rights Reserved
**                           By(达实久信医疗科技有限公司)
**----------------------------------文件信息------------------------------------
** 文件名称: structure.h
** 创建人员: 王凯
** 创建日期: 2019-09-03
** 文档描述: */
/*CAN信息主索引分配*/
#include "includes.h"
/********子索引***********/
typedef enum _CAN_MAININDEX
{
	CAN_CONTROLLER_MAININDEX,          //通用控制器
	CAN_CAR_MAININDEX,                 //小车
	CAN_TRANSFER_MAININDEX,            //转轨器02
	CAN_FIERDOOR_MAININDEX,            //防火门
	CAN_POWER_MAININDEX,               //电源
}CAN_MAIN_INDEX;
/*定义CAN的通讯模式*/
typedef struct _CANCOMMOD {
    union
    {
        u8 CanModeMask;
        struct
        {
            u8 HeartBeatSendbit:1,
            ACKFrameSendbit:1,
            ReplyFrameSendbit:1,
            ACTFrameSendbit:1,

            HeartBeatRecebit:1,
            ACKFrameRecebit:1,
            ReplyFrameRecebit:1,
            ACTFrameRecebit:1;
        } CanModebit;
    } CanMode;
} T_CAN_Communicat_Mode;
/*定义Modbus的寄存器*/
enum _ModBusReg{
	ModBus_NowCount=0x00,
	ModBus_NowLoction,//01
	ModBus_WorkMode,//02
	ModBus_MoveCount,//03
	ModBus_LocNumer,//04
	ModBus_FindZero,//05
	ModBus_Stop,//06
	ModBus_CheakSelf,//07
  ModBus_Calibration,//08
	ModBus_SetCalibration,//09
	ModBus_DirictionTest=0x12,//18号，是方向测试编号
	ModBus_WarningCode=0x14,//20号，是警告代码
	ModBus_ErrorCode,//21号，是错误代码
	ModBus_ThisTransitionNumber_W,//22号，写入设备编号
	ModBus_ThisTransitionNumber_R,//23号，读取设备编号
	ModBus_MissionCount,//24号 任务数
	ModBus_MemUse,
};
/*定义控制器信息数据结构*/
typedef struct _CONTROLMESS {
    char	ProductSeries;//产品系列
    u32  HardwareVersion;//硬件版本
    u32  SoftwareVersion;//软件版本
    u32  ARMOnlyOneId;//ARM唯一识别号
    u32  ManufactureData;//生产日期
    u32  UseData;//使用日期
} T_Control_Message;
/*定义子索引*/
enum _SUBINDEX {
    GetUniqueID=0x10,//获取唯一编号
    SetCANID=0x11,//根据唯一编号设置设备ID（CANid)
    GetCanComMod=0x12,//获取CAN通讯模式
    SetCanComMod=0x13,//设置CAN通讯模式
    SetRestart=0x20,//域控制重新启动
    ControlCheck=0x22,//控制器自检
    GetControlConfig=0x27,//获取控制器基本信息
};
/*定义单字节索引信息*/
enum _INDEX {
    UploadTransStatus=0x00,//上传转轨器状态，由自身变化引起
    GetTransStatus,//获取转轨器状态
    ResetTransStatus,//重置转轨器状态
    GetTransConfig,     //获取转轨器配置
    SetTransConfig,     //设置转轨器配置
    GetTransStatistics,  //获取统计数据
    TransLocation=0x10,//转轨器定位
    TransFindzero,     //转轨器校零
    TransMove,           //转轨器移动
    CurrentPreset,       //设置当前位为预置位
    LoadTransParameters, //装载转轨器参数
    SaveTransParameters, //保存转轨器参数


    CarApplyPass,        //小车申请通过
    CarCanPass,          //转轨器通知小车可以通过
    CarAlreadyPass,      //小车通知已经通过
    TranPerMission=0x20,
	  CarIsDowning=0x21,//小车刚刚下轨道，这个用于通知区域控制器方便其计算路径
//    CarApplyChange=0x16,//小车申请转轨
//    CarCanUpTrack,       //转轨器通知小车可以驶入（上轨）
//    CarAlreadyUpTrack,   //小车通知已经驶入
//    CarCanDownTrack,     //转轨器通知小车可以行驶出(xiagui)
//    CarAlreadyDownTrack, //小车通知已经行驶出，通知转轨器释放
    AgreeChangeMode=0xfb,
    TransStop=0xfc,//tingzhi
    ReStart=0xff,//软件复位
};
/**关键任务索引**/
enum _TempINdex{
	 CarApplyChange=0x10,//小车申请转轨
    CarCanUpTrack,       //转轨器通知小车可以驶入（上轨）
    CarAlreadyUpTrack,   //小车通知已经驶入
    CarCanDownTrack,     //转轨器通知小车可以行驶出(xiagui)
    CarAlreadyDownTrack, //小车通知已经行驶出，通知转轨器释放
};
/*转轨器状态结构*/
typedef struct Status {
    INT8U DeviceMode;//工作模式
    INT8U WarningCode;//警告代码
    INT8U ErrorCode;//故障代码
    INT16U DockedNumber;//正好对接的轨道编号
    INT8U NowLoction;//目前的位置//在（前四位）和（后四位）之间//保留
    union
    {
        INT8U TrackStatus;
        struct
        {
            INT8U FindzeroValid:1,/*校零开关是否有效*/
                  ExeCommands:1,/*是否正在执行命令*/
                  TrackUse0:1,/*转轨器轨道1有车通过*/
                  TrackUse1:1,/*转轨器轨道2有车通过*/
                  TrackUse2:1,/*转轨器轨道3有车通过*/
                  TrackUse3:1,/*转轨器轨道4有车通过*/
                  Monopolize:1,/*转轨器独享*/
					        CheakSelf:1;/*自检标志位，上电之后是1，说需要自检，域通过这一位判断是否自检，自检完成后，置0*/
					        
        } Usebit;

    } TrackUse;
    INT16U EncoderCount;//步进编码器计数
} T_Trans_Status;

/*转轨器统计数据结构*/
typedef struct _Statistics {
    INT32U WarningTimes;//警告次数
    INT32U ErrTimes;//故障次数
    INT32U CommWarning;//通讯警告次数
    INT32U CommErr;//通讯故障次数
    INT32U FindzeroTimes;//校零次数
    INT32U MoveTimes;//移动次数
    INT32U PassTimes;//通过次数
    INT32U ShuntTimes;//调车次数
} T_Trans_Statistics;

/*转轨器速度*/
enum _TranferSpeed
{
    TranferStop,
	  StopOnForce,
    TranferSpeedOne,
    TranferSpeedTwo,
    TranferSpeedThree,
    TranferSpeedFour,
};

enum _FindZeroState
{
	ISOnZero,//已经压到零点上面
	ISAwayZero,//离开零点
};
/********************************End of Head************************************/
