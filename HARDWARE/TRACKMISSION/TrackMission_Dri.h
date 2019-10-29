#include "sys.h"
#include "includes.h"
#include "Can_Dri.h"
typedef struct _CAR_CHANGEMISSION_DATA{
	
	INT8U CarNum;
	INT8U PreMission;//预动标志
	INT8U MissionMark;//任务标记
	INT8U InitialPoint;//起始位
	INT8U TerminalPoint;//终点位
	INT32U FarmID;//帧ID
	INT32U TimeMark;//时间标志
	struct _CAR_CHANGEMISSION_DATA * NextMission;
	
}CAR_CHANGEMISSION_DATA;//小车转换轨道任务链表结点数据结构
/*轨道上锁还是解锁*/
enum _TrackLockOrNot{
	LockTrack=0x00,
	UnlockTrack,
};

//extern OS_EVENT * CarApplyPassTransSem;
extern OS_EVENT * CarApplyPassTransMbox;//链表分析任务，为预动任务发送的信号邮箱，发送到申请通过转轨器任务
extern OS_EVENT * CarApplyChangeTranMbox;//链表分析任务，为变化轨道任务发送的信号邮箱，发送到申请转轨任务

extern OS_EVENT * TranPreMissionMbox;

extern CAR_CHANGEMISSION_DATA *g_CarApplyChangedata;

void CarMission_Software_Init(void);
void Apply_Pass_Mission(CAN_DATA_FRAME * tempNode);
void Apply_Change_Mission(CAN_DATA_FRAME * tempNode);
void Mission_Analyse(CAR_CHANGEMISSION_DATA * headmissionnode);
u16 TransLocate_Change(u8 TransNum);
bool TrackCount_Load(void);
void TrackCount_Save(void);
void FindZeroFuction(u8 mode);
u8 TrackLockFunction(u8 lockorun,u8 trackNumber);
u16 Choose_TransLocate_Change(u8 MovingTrack,u8 FixedTrack);

