/********************************Copyright (c)**********************************\
**                   (c) Copyright 2019, Main, China
**                           All Rights Reserved
**                           By(��ʵ����ҽ�ƿƼ����޹�˾)
**----------------------------------�ļ���Ϣ------------------------------------
** �ļ�����: StepMotor_Dir.h
** ������Ա: ����
** ��������: 2019-06-21
** �ĵ�����: ������������㺯������
*******************************End of Head************************************/
#include "includes.h"
#include "sys.h"

#define MOTOR_DIR_CTRL PGout(6)       //0:��ת  1:��ת
#define MOTOR_STEP_CTRL PGout(7)
#define clear_MOTOR_Alarm   GPIO_SetBits(GPIOC,GPIO_Pin_12)//���߾��ǵ͵�ƽ
#define Back_MOTOR_Alarm    GPIO_ResetBits(GPIOC,GPIO_Pin_12)//���;��Ǹߵ�ƽ
#define anticlockwise  1
#define clockwise    0
extern u8 PCtrl;
extern u8 NCtrl;
extern INT8U stepMotorCtlTime;
extern u8 ZeroDirction;//��㿪�ط���

/*�ⲿ���� �ź����ź�����*/
extern OS_EVENT * can1Mbox;

extern OS_EVENT *stepMotorCtlSem;
extern OS_EVENT *arrivePosSem;//����Ŀ��λ���ź���
extern OS_EVENT *stepFreeSem;//����ͷ��ź���

void TIM10_Cap_Init(u32 arr,u16 psc);
void TIM9_Cap_Init(u32 arr,u16 psc);
void TIM6_Int_Init(u16 arr,u16 psc);
void TIM3_Cap_Init(u32 arr,u16 psc);
void StepMotor_Init(void);
void StepMotor_PCtrl(u32 speed);
void StepMotor_NCtrl(u32 speed);
void TIM6_DAC_IRQHandler(void);
void TIM1_UP_TIM10_IRQHandler(void);
void TIM1_BRK_TIM9_IRQHandler(void);
bool StepMotor_Run(u32 speed,u8 direction,u32 count);
void Change_diriction(u8 findzerodirction);

