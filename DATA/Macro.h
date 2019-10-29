/********************************Copyright (c)**********************************\
**                   (c) Copyright 2019, Main, China
**                           All Rights Reserved
**                           By(达实久信医疗科技有限公司)
**----------------------------------文件信息------------------------------------
** 文件名称: Macro.h
** 创建人员: 王凯
** 创建日期: 2019-09-03
** 文档描述: 宏定义数据*/
#define  NotUsedCCMBASE(offset)       (0X10005500+(offset*4))//定义未使用的CCM地址
#define  FLASH_SECTOR_SIZE            4*1024//定义一个扇区的大小4k
#define  MAX_READ_TIME               3//定义重复读取次数 Flash
/*定义定轨数量和动轨数量*/
#define  MAX_FixedTrack         16
#define  MAX_MovingTrack         4
/*最大轨道切换阈值，超过这个阈值将会强制找零*/
#define  MAXThresholdValue       8
/*定义工作模式*/
#define  OperatingMode       1//自动运行模式
#define  DebugMode           0//手动运行模式
#define  OffLine             2//离线，联机状态，通讯离线
#define  standAlone          3//脱机，单机运行忽略通讯状态
/*定义轨道号*/
#define Track0            0
#define Track1            1
#define Track2            2
#define Track3            3
#define Track4            4

/*定义开和关*/
#define  T_ON              1
#define  T_OFF             0
/*定义是和 否*/
#define T_Yes             1
#define T_No              0

/*定义轨道锁定*/
#define  T_Lock           1
#define  T_Unlock         0
/*定义找零模式，是否需要回到最近的轨道*/
#define  Need_Back      1
#define  Needt_Back     0
/*定义自检标志位，1是没有自检，需要自检；0是已经自检*/
#define  Need_Cheak       1
#define  Already_Cheak    0
/*定义故障代码*/
#define No_Err            0x00//无故障
#define Comexefailed      0x35//命令通信成功，但是执行失败
#define RAMSpillover      0x03//内存溢出，内存分配不成功

#define MotorNoPower      0x60//电机无电
#define MotorOverCurrent  0x61//电机过流
#define MotorOverTime     0x62//电机超时；也就是卡死
#define CantFindZero      0x63//无法找零
#define BeStuckByCar      0x64//转轨器被车卡住

#define BeepON     GPIO_SetBits(GPIOF,GPIO_Pin_8)//叫
#define BeepOFF    GPIO_ResetBits(GPIOF,GPIO_Pin_8)

/*与任务相关的四个个必要条件，优先级，堆栈大小，堆栈，任务函数声明*/
#define      RUN_TASK_SIZE                512
#define      CAN2_TASK_SIZE               512
#define      CAN2ACK_TASK_SIZE            512
#define      CAN2ACT_TASK_SIZE            512
#define      STEPMOTOR_TASK_SIZE          512
#define      LED_TASK_SIZE                512
#define      TRANSLOCATION_TASK_SIZE      512
#define      MISSIONANALYSE_TASK_SIZE     512
#define      CARAPPLYPASS_TASK_SIZE       512
#define      CAN2Send_TASK_SIZE           512
#define      CARAPPLYCHANGE_TASK_SIZE     512
#define      FINDZERO_TASK_SIZE           512
#define      TRANSPERMI_TASK_SIZE         512
#define      HEARTBEAT_TASK_SIZE          512
#define      MODBUS_TASK_SIZE             512
#define      STATE_TASK_SIZE              512
#define      MODE_TASK_SIZE             512
/*1任务优先级设置*/
#define START_TASK_PRIO      			4//开始任务 优先级最高
#define CAN2_TASK_PRIO            5//can2分配任务，中断接受之后，在这个任务分配处理
#define CAN2ACK_TASK_PRIO         6//CAn2应答帧处理任务
#define CAN2ACT_TASK_PRIO         7//can2主动帧处理任务，在can2处理任务之后
#define STEPMOTOR_TASK_PRIO       15
#define TRANSLOCATION_TASK_PRIO   8//轨道定位任务
#define RUN_TASK_PRIO             21//指示系统是否正常运行函数。放在最低优先级
#define LED_TASK_PRIO             18
#define MISSIONANALYSE_PRIO       10//转轨器任务分析任务
#define CARAPPLYPASS_PRIO         11//小车要通过转轨器任务
#define CAN2Send_PRIO             14
#define CARAPPLYCHANGE_PRIO       12//小车申请转换轨道任务
#define FINDZERO_PRIO             9
#define TRANSPERMI_PRIO           13

#define HEARTBEAT_PRIO            17
#define MODBUS_PRIO               16

#define STATE_PRIO                19
#define MODE_PRIO                 20
/*2定义任务堆栈大小*/
#define START_STK_SIZE  				  128





#define USEPRINTF   1














/********************************End of Head************************************/
