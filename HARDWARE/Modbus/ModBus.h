#include "stm32f4xx.h"
typedef struct
{
 u8 myadd;//���豸�ĵ�ַ
 u8 rcbuf[100]; //MODBUS���ջ�����
 u16 timout;//MODbus�����ݶ���ʱ��	
 u8 recount;//MODbus�˿��Ѿ��յ������ݸ���
 u8 timrun;//MODbus��ʱ���Ƿ��ʱ�ı�־
 u8  reflag;//�յ�һ֡���ݵı�־
 u8 Sendbuf[100]; //MODbus���ͻ�����	

}MODBUS;


extern MODBUS modbus;
extern u16 Reg[];

void Mosbus_Init(void);
void Modbud_fun3(void);  //3�Ź����봦��  ---����Ҫ��ȡ���ӻ��ļĴ���
void Modbud_fun6(void);  //6�Ź����봦��
void Mosbus_Event(void);
void ModBus_TempTrackCount_UpData(void);//�궨��־���º���
void ModBus_TrackCount_UpData(void);//��Ч�����λ����



