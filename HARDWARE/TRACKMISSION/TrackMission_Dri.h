#include "sys.h"
#include "includes.h"
#include "Can_Dri.h"
typedef struct _CAR_CHANGEMISSION_DATA{
	
	INT8U CarNum;
	INT8U PreMission;//Ԥ����־
	INT8U MissionMark;//������
	INT8U InitialPoint;//��ʼλ
	INT8U TerminalPoint;//�յ�λ
	INT32U FarmID;//֡ID
	INT32U TimeMark;//ʱ���־
	struct _CAR_CHANGEMISSION_DATA * NextMission;
	
}CAR_CHANGEMISSION_DATA;//С��ת������������������ݽṹ
/*����������ǽ���*/
enum _TrackLockOrNot{
	LockTrack=0x00,
	UnlockTrack,
};

//extern OS_EVENT * CarApplyPassTransSem;
extern OS_EVENT * CarApplyPassTransMbox;//�����������ΪԤ�������͵��ź����䣬���͵�����ͨ��ת��������
extern OS_EVENT * CarApplyChangeTranMbox;//�����������Ϊ�仯��������͵��ź����䣬���͵�����ת������

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

