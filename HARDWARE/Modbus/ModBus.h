#include "stm32f4xx.h"
typedef struct
{
 u8 myadd;//本设备的地址
 u8 rcbuf[100]; //MODBUS接收缓冲区
 u16 timout;//MODbus的数据断续时间	
 u8 recount;//MODbus端口已经收到的数据个数
 u8 timrun;//MODbus定时器是否计时的标志
 u8  reflag;//收到一帧数据的标志
 u8 Sendbuf[100]; //MODbus发送缓冲区	

}MODBUS;


extern MODBUS modbus;
extern u16 Reg[];

void Mosbus_Init(void);
void Modbud_fun3(void);  //3号功能码处理  ---主机要读取本从机的寄存器
void Modbud_fun6(void);  //6号功能码处理
void Mosbus_Event(void);
void ModBus_TempTrackCount_UpData(void);//标定标志更新函数
void ModBus_TrackCount_UpData(void);//有效轨道定位函数



