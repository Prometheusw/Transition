/********************************Copyright (c)**********************************\
**                   (c) Copyright 2019, Main, China
**                           All Rights Reserved
**                           By(达实久信医疗科技有限公司)
**----------------------------------文件信息------------------------------------
** 文件名称: StepMotor_Dir.c
** 创建人员: 王凯
** 创建日期: 2019-06-21
** 文档描述: 步进电机驱动层函数，接口初始化和定时器初始化
******************************************************************************/
#include "myconfig.h"
OS_EVENT *stepMotorCtlSem;//单步信号量
OS_EVENT *arrivePosSem;//到达目标位置信号量
OS_EVENT *stepFreeSem;//电机释放信号量
INT8U stepMotorCtlTime;//每一步之间间隔5us为一个单位
u8 ZeroDirction=0xff;//零点开关方向,初始未知，从flash中读取或者modbus告知
u8 PCtrl=0;//逆
u8 NCtrl=1;//顺
/*找零方向选择函数*/
void Change_diriction(u8 findzerodirction)
{
    if(findzerodirction==anticlockwise)//如果逆时针校零
    {
        ZeroDirction=anticlockwise;//找零方向是逆时针
    }
    else if(findzerodirction==clockwise)//如果电机顺时针校零
    {
        ZeroDirction=clockwise;//找零方向是顺时针
    }
    W25QXX_Write((INT8U*)&ZeroDirction,FLASH_SECTOR_SIZE*2,sizeof(ZeroDirction));//将找零方向存在flash中
}
/*******************************************************************************
** 函数名称: TIM10_Cap_Init
** 功能描述: 定时器10捕捉 到位信号，初始化
** 参数说明: arr: 自动重装值
**			 psc：时钟预分频数
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-06-14
********************************************************************************/
void TIM10_Cap_Init(u32 arr,u16 psc)
{
    TIM_ICInitTypeDef  TIM10_ICInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10,ENABLE);  	//TIM10时钟使能
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE); 	//使能PORTF时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //GPIOF
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOF,&GPIO_InitStructure); //初始化GPIOF
    GPIO_PinAFConfig(GPIOF,GPIO_PinSource6,GPIO_AF_TIM10); //PF6复用位定时器10
    TIM_TimeBaseStructure.TIM_Prescaler=psc;  //定时器分频
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
    TIM_TimeBaseStructure.TIM_Period=arr;   //自动重装载值
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM10,&TIM_TimeBaseStructure);

    TIM10_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM10_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;	//下降沿捕获
    TIM10_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI3上
    TIM10_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频
    TIM10_ICInitStructure.TIM_ICFilter = 0x00;//配置输入滤波器
    TIM_ICInit(TIM10, &TIM10_ICInitStructure);

    TIM_ClearITPendingBit(TIM10,TIM_IT_CC1);
    TIM_ITConfig(TIM10,TIM_IT_CC1|TIM_IT_CC1,ENABLE);/**/
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=11;//抢占优先级2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
    TIM_Cmd(TIM10,ENABLE); //使能定时器10
}
/*******************************************************************************
** 函数名称: TIM9_Cap_Init
** 功能描述: 捕捉报警信号，初始化
** 参数说明: arr: 自动重装值
**			 psc: 时钟预分频数
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-06-21
********************************************************************************/
void TIM9_Cap_Init(u32 arr,u16 psc)
{
    TIM_ICInitTypeDef  TIM9_ICInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);  	//TIM3时钟使能
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); 	//使能PORTE时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; //GPIOE
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOE,&GPIO_InitStructure); //初始化GPIOE
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource5,GPIO_AF_TIM9); //PE5复用位定时器9
    TIM_TimeBaseStructure.TIM_Prescaler=psc;  //定时器分频
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
    TIM_TimeBaseStructure.TIM_Period=arr;   //自动重装载值
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM9,&TIM_TimeBaseStructure);
    TIM9_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM9_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;	//下降沿捕获
    TIM9_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
    TIM9_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频
    TIM9_ICInitStructure.TIM_ICFilter = 0x00;//配置输入滤波器
    TIM_ICInit(TIM9, &TIM9_ICInitStructure);
    TIM_ClearITPendingBit(TIM9,TIM_IT_CC1);
    TIM_ITConfig(TIM9,TIM_IT_CC1,DISABLE);
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=12;//抢占优先级1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
    TIM_Cmd(TIM9,DISABLE); //使能定时器9
}
/****************************************************************
功能：定时器3捕获初始化    零点开关信号
入口参数：u32 arr-预装载值,u16 psc-分频值
返回值：无
*****************************************************************/
void TIM3_Cap_Init(u32 arr,u16 psc)
{
    TIM_ICInitTypeDef  TIM3_ICInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  	//TIM3时钟使能
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 	//使能PORTB时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1; //GPIOB
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB,&GPIO_InitStructure); //初始化GPIOB
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource0,GPIO_AF_TIM3); //PB0复用位定时器3
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource1,GPIO_AF_TIM3); //PB1复用位定时器3
    TIM_TimeBaseStructure.TIM_Prescaler=psc;  //定时器分频
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
    TIM_TimeBaseStructure.TIM_Period=arr;   //自动重装载值
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
    TIM3_ICInitStructure.TIM_Channel = TIM_Channel_3;
    //TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;	//双边沿捕获
    TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;	//双边沿捕获 wwwwwkkkkk

    TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI3上
    TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频
    TIM3_ICInitStructure.TIM_ICFilter = 0x00;//配置输入滤波器
    TIM_ICInit(TIM3, &TIM3_ICInitStructure);
    TIM3_ICInitStructure.TIM_Channel = TIM_Channel_4;
    //TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;	//上升沿捕获
    TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;	//上升沿捕获wwwwwkkkkk

    TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI4上
    TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频
    TIM3_ICInitStructure.TIM_ICFilter = 0x00;//配置输入滤波器
    TIM_ICInit(TIM3, &TIM3_ICInitStructure);
    TIM_ClearITPendingBit(TIM3,TIM_IT_CC3|TIM_IT_CC4);
    TIM_ITConfig(TIM3,TIM_IT_CC3|TIM_IT_CC4,DISABLE);
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=13;//抢占优先级1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
    TIM_Cmd(TIM3,ENABLE); //使能定时器3
}
/****************************************************************
//通用定时器6中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器5!时基为5us
//此定时器用于步进电机的控制
*****************************************************************/
void TIM6_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);  ///使能TIM6时钟
    TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分频
    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
    TIM_TimeBaseInitStructure.TIM_Period=arr;   //自动重装载值
    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM6,&TIM_TimeBaseInitStructure);
    TIM_ClearITPendingBit(TIM6,TIM_IT_Update);  //清除中断标志位防止启动就进入中断
    TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE); //允许定时器6更新中断/*kkw*/
    TIM_Cmd(TIM6,ENABLE); //使能定时器6
    NVIC_InitStructure.NVIC_IRQChannel=TIM6_DAC_IRQn; //定时器6中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=10; //抢占优先级1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0; //子优先级1
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
** 函数名称: StepMotor_Init
** 功能描述: 步进电机驱动用到的IO口初始化，G6是正反控制信号，G7是步进脉冲输出信号
** 参数说明: None
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-06-21
********************************************************************************/
void StepMotor_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);//使能GPIOG时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOG, &GPIO_InitStructure);//初始化GPIO

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//使能GPIOG时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIO
    GPIO_ResetBits(GPIOC,GPIO_Pin_12);//gao

}


/*******************************************************************************
** 函数名称: StepMotor_PCtrl
** 功能描述:步进电机逆时针转动
** 参数说明: speed: 速度值，us
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-06-14
********************************************************************************/
void StepMotor_PCtrl(u32 speed)
{
    u8 err=0;

    MOTOR_DIR_CTRL = 0;//PG6=0，正转
    stepMotorCtlTime = speed;
    MOTOR_STEP_CTRL = 1;//PG7拉高
    TIM_Cmd(TIM6,ENABLE);//拉高之后开启定时器6
    OSSemPend(stepMotorCtlSem,0,&err);
    /*等待信号量stepMotorCtlSem，这个信号量在定时器中断6函数中发出*/
    stepMotorCtlTime = speed;
    MOTOR_STEP_CTRL = 0;//PG7拉低，低电平有效，走一步
    TIM_Cmd(TIM6,ENABLE);//再开启定时器6，wangkai：此处为什么要开两次？？
    OSSemPend(stepMotorCtlSem,0,&err);
    /*等待信号量stepMotorCtlSem，这个信号量在定时器中断6函数中发出*/
    Now_direction=anticlockwise;//当前方向是逆时针

}

/****************************************************************
功能：电机顺时针转动
入口参数：u32 speed-速度值
返回值：无
*****************************************************************/
void StepMotor_NCtrl(u32 speed)
{
    u8 err=0;

    MOTOR_DIR_CTRL = 1;
    stepMotorCtlTime = speed;
    MOTOR_STEP_CTRL = 1;
    TIM_Cmd(TIM6,ENABLE);
    OSSemPend(stepMotorCtlSem,0,&err);
    stepMotorCtlTime = speed;
    MOTOR_STEP_CTRL = 0;
    TIM_Cmd(TIM6,ENABLE);
    OSSemPend(stepMotorCtlSem,0,&err);
    Now_direction=clockwise;//当前方向是顺时针

}

/*******************************************************************************
** 函数名称: StepMotor_Run
** 功能描述: 步进电机转动函数
** 参数说明: speed: 最小速度
**			 direction: 运行行方向
**			 count: 脉冲数
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-06-26
********************************************************************************/
bool StepMotor_Run(u32 speed,u8 direction,u32 count)
{
    u8 Stepcount=0;
    u8 SpeedUpMax=100;
    //u8 SpeedDownMin=speed;
    int8_t MAXFlag=-1;//权值
    u32 LevelCount;
    TransStatus.DockedNumber=0xffff;//位置未知
    if(speed>SpeedUpMax)
    {
        LevelCount=(count/(speed-SpeedUpMax))/2;//每LevelCount个编码，speed升一级
    }
    IsMotorRun=T_Yes;
    if(direction==PCtrl)//逆时针
    {
        for(count=count; count!=0; count--) //count步数
        {
            if(TranferSpeed==TranferStop)/*如果这里有刹车信号，直接跳出循环*/
            {
                TranferSpeed=1;
//                EnableStopButton=T_Yes;
                break;
            }

            for(Stepcount=0; Stepcount<2; Stepcount++) //2脉冲为一步
            {
                StepMotor_PCtrl(speed);//100是速度
            }
            /*峰值加减速*/
//            if(speed>SpeedUpMax&&count%LevelCount==0)//说明编码数已经走了一个等级,短路条件
//            {
//                speed=speed+MAXFlag;//加速一个,此时Falg为-1
//                if(speed==SpeedUpMax+1)///当到达最大速度的时候
//                {
//                    MAXFlag=1;
//                }
//            }

            if(ZeroDirction==clockwise)//如果是顺指针找零，这个时候是逆时针转动，所以编码数加一
            {
                TransStatus.EncoderCount++;//编码数加一
                //if(TransStatus.EncoderCount>=MAX_Count) break;
            }
            else //如果是逆时针找零，此时逆时转动，编码数应该减一
            {
                TransStatus.EncoderCount--;//编码数-1
                // if(TransStatus.EncoderCount<=0) break;
            }
        }
        IsMotorRun=T_No;
        return true;
    }
    else if(direction==NCtrl)//顺指针
    {
        for(count=count; count!=0; count--) //step是步数
        {
            if(TranferSpeed==TranferStop)/*如果这里有刹车信号，直接跳出循环*/
            {
                TranferSpeed=1;
                EnableStopButton=T_Yes;
                break;
            }

            for(Stepcount=0; Stepcount<2; Stepcount++)
            {
                StepMotor_NCtrl(speed);//100是速度
            }
//            /*峰值加减速*/
//            if(speed>SpeedUpMax&&count%LevelCount==0)//说明编码数已经走了一个等级,短路条件
//            {
//                speed=speed+MAXFlag;//加速一个,此时Falg为-1
//                if(speed==SpeedUpMax+1)///当到达最大速度的时候
//                {
//                    MAXFlag=1;
//                }
//            }

            if(ZeroDirction==clockwise)//如果是顺指针找零，这个时候是shun时针转动，所以编码数-1
            {
                TransStatus.EncoderCount--;
                //if(TransStatus.EncoderCount<=0) break;

            }
            else
            {
                TransStatus.EncoderCount++;
                //if(TransStatus.EncoderCount>=MAX_Count) break;

            }
            if(TranferSpeed==TranferStop)//停止标志位0 立刻停止
            {
                TranferSpeed=1;//
                break;//跳出执行，停止
            }
        }
        IsMotorRun=T_No;
        return true;
    }
    else return false;


}
/*******************************************************************************
** 函数名称: TIM6_DAC_IRQHandler
** 功能描述: 中断服务函数里面发送走一步的信号量
** 参数说明: None
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-06-14
********************************************************************************/
void TIM6_DAC_IRQHandler(void)
{
    static u16 timer = 0;

    OSIntEnter();

    if(TIM_GetITStatus(TIM6,TIM_IT_Update)==SET) //溢出中断
    {
        timer += 5;
        if(2>=stepMotorCtlTime-timer)//此处的2可以是0--5之间的任何数，不包含0--5。
        {
            timer=0;
            TIM_Cmd(TIM6,DISABLE);//wangkai：此处关闭TIM6有必要，防止正转函数中还没执行到对SPeed的赋值就进中断服务函数
            OSSemPost(stepMotorCtlSem);//当把stepMotorCtlTime减完后，发送到位信号量
        }
    }
    TIM_ClearITPendingBit(TIM6,TIM_IT_Update);//清除中断标志位

    OSIntExit();
}

/*******************************************************************************
** 函数名称: TIM1_UP_TIM10_IRQHandler
** 功能描述: 捕捉到位信号，然后发送到位信号量
** 参数说明: None
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-06-14
********************************************************************************/
void TIM1_UP_TIM10_IRQHandler(void)
{
    OSIntEnter();

    if(TIM_GetITStatus(TIM10, TIM_IT_CC1) != RESET)
    {
        if(arrivePosSem->OSEventCnt==0)//不允许多个到位信号积压
        {
            OSSemPost(arrivePosSem);//当捕捉到到位信号，发送到位信号
            LED_BlinkTime.LED3_Times++;//暂时让灯3亮
        }
    }
    TIM_ClearITPendingBit(TIM10,TIM_IT_CC1|TIM_IT_Update);

    OSIntExit();
}


/*******************************************************************************
** 函数名称: TIM1_BRK_TIM9_IRQHandler
** 功能描述: 报警信号捕捉中断处理函数
** 参数说明: None
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-06-21
********************************************************************************/
void TIM1_BRK_TIM9_IRQHandler(void)
{
    OSIntEnter();
    if(TIM_GetITStatus(TIM9, TIM_IT_CC1) != RESET)
    {
        BeepChirptimes=BeepChirptimes+10;//交10次
        TransStatus.ErrorCode=MotorOverCurrent;//电机过流，卡死在最后
        /*此处对报警信号进行处理*/
        TIM_ClearITPendingBit(TIM9, TIM_IT_CC1);
    }
    OSIntExit();
}
/*******************************************************************************
** 函数名称: TIM3_IRQHandler
** 功能描述:
** 参数说明: None
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-07-03
********************************************************************************/
void TIM3_IRQHandler(void)
{
    OSIntEnter();
    if(TIM_GetITStatus(TIM3, TIM_IT_CC4) != RESET)    //零点开关接在PB1    左侧
    {
        FindZeroGlob++;
        TIM_ClearITPendingBit(TIM3, TIM_IT_CC4);
    }

    if(TIM_GetITStatus(TIM3, TIM_IT_CC3) != RESET)	  //零点开关接在PB0
    {
        FindZeroGlob++;
        TIM_ClearITPendingBit(TIM3, TIM_IT_CC3);
    }
    OSIntExit();
}
/*******************************End of File************************************/



