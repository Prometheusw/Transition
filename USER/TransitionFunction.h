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
void Self_Function(void);//自运行，把所有能到的轨道走一遍
void Load_One_FindZero(void);//在任务列表里面加入一个找零任务
unsigned char Find_NearestTrack(void);//找到距离零点最近的轨道编号
u8 Mode_Change(u8 mode);


