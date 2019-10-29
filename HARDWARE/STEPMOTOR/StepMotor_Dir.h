/********************************Copyright (c)**********************************\
**                   (c) Copyright 2019, Main, China
**                           All Rights Reserved
**                           By(达实久信医疗科技有限公司)
**----------------------------------文件信息------------------------------------
** 文件名称: StepMotor_Dir.h
** 创建人员: 王凯
** 创建日期: 2019-06-21
** 文档描述: 步进电机驱动层函数声明
*******************************End of Head************************************/
#include "includes.h"
#include "sys.h"

#define MOTOR_DIR_CTRL PGout(6)       //0:正转  1:反转
#define MOTOR_STEP_CTRL PGout(7)
#define clear_MOTOR_Alarm   GPIO_SetBits(GPIOC,GPIO_Pin_12)//拉高就是低电平
#define Back_MOTOR_Alarm    GPIO_ResetBits(GPIOC,GPIO_Pin_12)//拉低就是高电平
#define anticlockwise  1
#define clockwise    0
extern u8 PCtrl;
extern u8 NCtrl;
extern INT8U stepMotorCtlTime;
extern u8 ZeroDirction;//零点开关方向

/*外部变量 信号量信号邮箱*/
extern OS_EVENT * can1Mbox;

extern OS_EVENT *stepMotorCtlSem;
extern OS_EVENT *arrivePosSem;//到达目标位置信号量
extern OS_EVENT *stepFreeSem;//电机释放信号量

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

