#include "Can_Dri.h"

void SendTransStatistics(CAN_DATA_FRAME *tempNode);
//void UpdataNowLoction(void);
void SendTransStatus(CAN_DATA_FRAME *CopyTempNode);
void SendTransConfig(CAN_DATA_FRAME *tempNode);
u32 ReadMemeryData(u32  Add);
void WriteMemeryData(u32 Add,u32 data);
void SendDeviceOnlyID(CAN_DATA_FRAME *tempNode);
void SetCANIDByARMID(CAN_DATA_FRAME *tempNode);
void SentCANCommitMode(CAN_DATA_FRAME *tempNode);
void SetCANCommitMode(CAN_DATA_FRAME *tempNode);
void SentControlConfig(CAN_DATA_FRAME *tempNode);
void ModBusHandleFunc(void);
void Calibration_feature(u8 T,u8 Num);
void Feature_Clean(u8 T,u8 Num);
void Update_Limit_Count(void);
void Self_Function(void);//�����У��������ܵ��Ĺ����һ��
void Load_One_FindZero(void);//�������б��������һ����������
unsigned char Find_NearestTrack(void);//�ҵ������������Ĺ�����
u8 Mode_Change(u8 mode);


