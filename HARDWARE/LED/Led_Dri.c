/********************************Copyright (c)**********************************\                    
**
**----------------------------------文件信息------------------------------------
** 文件名称: led.c
** 创建人员: 王凯
** 创建日期: 2019-06-13
** 文档描述: 转轨器板LED灯初始化函数 LED0--PF9，LED1--PF10，LED2--PF11，LED3--PG8，LED4--PG9；
********************************************************************************/
#include "myconfig.h"
/*亮灯的模式为const变量，不可改变*/
T_led_blink LED_MODE_2= {50,950,1,0,}; //亮灭一次
T_led_blink LED_MODE_3= {800,200,1,1,}; // 指示任务是否正常运行
//T_led_blink LED_MODE_4= {20,0,1,0,}; //快闪烁，防止can通信速率过大引起内存泄漏 指示can通信
T_led_blink LED_MODE_5= {1000,0,1,0}; //长亮 备用
T_led_blink LED_MODE_4= {300,0,1,0};

T_led_bTimes LED_BlinkTime= {0,0,0,0,0}; //LED闪烁次数结构体，初始化为0
/*******************************************************************************
** 函数名称: Led_Init
** 功能描述: LED的引脚初始化
** 参数说明: None
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-06-20
********************************************************************************/
void Led_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//使能GPIOF时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);//使能GPIOG时钟

    //GPIOF9,F10,F11初始化设置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;//LED0\1\2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化GPIO
    //GPIOG8 G9初始化设置//方向口旁边的备用引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;//LED0\1\2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOG, &GPIO_InitStructure);//初始化GPIO

    GPIO_SetBits(GPIOF,GPIO_Pin_9 | GPIO_Pin_10|GPIO_Pin_11);//GPIOF9,F10设置上拉，灯灭
    GPIO_SetBits(GPIOG,GPIO_Pin_8 | GPIO_Pin_9);//GPIOG8 G9设置上拉，灯灭

}

/*******************************************************************************
** 函数名称: LED_CLEAR
** 功能描述: 灭掉LED灯
** 参数说明: num: [输入/出]
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-06-03
********************************************************************************/
void Led_Clear(INT8U num)
{
    switch(num)
    {
    case LED_NUM_1:
        GPIO_SetBits(GPIOF,GPIO_Pin_9);
        break;

    case LED_NUM_2:
        GPIO_SetBits(GPIOF,GPIO_Pin_10);
        break;
    case NUM_ALL:
    {
        //GPIO_SetBits(GPIOF,GPIO_Pin_9);
        GPIO_SetBits(GPIOF,GPIO_Pin_10);
        GPIO_SetBits(GPIOF,GPIO_Pin_11);
        GPIO_SetBits(GPIOG,GPIO_Pin_8);
        GPIO_SetBits(GPIOG,GPIO_Pin_9);
    }
    break;

    default:
        break;
    }
}

/*******************************************************************************
** 函数名称: Led_Blink_Mode
** 功能描述: LED闪烁模式
** 参数说明: num: 灯编号
**			 mode: 1:灯亮时间 2：灯灭时间 3：亮的次数 4：灭的次数
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-06-03
注：当一条调用要完成对两个灯的同时操作  此函数将不可用  改进计划：使用寄存器 参照IO口的初始化操作
********************************************************************************/
void Led_Blink_Mode(INT8U num, T_led_blink* mode)
{
    INT8U britimes=mode->bri_times;
    INT8U crutimes=mode->cru_times;
    switch(num)
    {
    case LED_NUM_0:
    {
        for(britimes=mode->bri_times; britimes>0; britimes--)
        {
            GPIO_ResetBits(GPIOF,GPIO_Pin_9);
            // GPIOF->BSRRH = GPIO_Pin_9;
            OSTimeDlyHMSM(0,0,0,mode->bright);
            GPIO_SetBits(GPIOF,GPIO_Pin_9);
            OSTimeDlyHMSM(0,0,0,mode->crush);
        }
        for(crutimes=mode->cru_times; crutimes>0; crutimes--)
        {
            GPIO_SetBits(GPIOF,GPIO_Pin_9);
            OSTimeDlyHMSM(0,0,0,mode->bright);
            OSTimeDlyHMSM(0,0,0,mode->crush);
        }
    }
    break;
    case LED_NUM_1:
    {
        for(britimes=mode->bri_times; britimes>0; britimes--)
        {
            GPIO_ResetBits(GPIOF,GPIO_Pin_10);
            OSTimeDlyHMSM(0,0,0,mode->bright);
            GPIO_SetBits(GPIOF,GPIO_Pin_10);
            OSTimeDlyHMSM(0,0,0,mode->crush);
        }
        for(crutimes=mode->cru_times; crutimes>0; crutimes--)
        {
            GPIO_SetBits(GPIOF,GPIO_Pin_10);
            OSTimeDlyHMSM(0,0,0,mode->bright);
            OSTimeDlyHMSM(0,0,0,mode->crush);
        }
    }
    break;

    case LED_NUM_2:
    {
        for(britimes=mode->bri_times; britimes>0; britimes--)
        {
            GPIO_ResetBits(GPIOF,GPIO_Pin_11);
            OSTimeDlyHMSM(0,0,0,mode->bright);
            GPIO_SetBits(GPIOF,GPIO_Pin_11);
            OSTimeDlyHMSM(0,0,0,mode->crush);
        }
        for(crutimes=mode->cru_times; crutimes>0; crutimes--)
        {
            GPIO_SetBits(GPIOF,GPIO_Pin_11);
            OSTimeDlyHMSM(0,0,0,mode->bright);
            OSTimeDlyHMSM(0,0,0,mode->crush);
        }
    }
    break;
    case LED_NUM_3:
    {
        for(britimes=mode->bri_times; britimes>0; britimes--)
        {
            GPIO_ResetBits(GPIOG,GPIO_Pin_8);
            OSTimeDlyHMSM(0,0,0,mode->bright);
            GPIO_SetBits(GPIOG,GPIO_Pin_8);
            OSTimeDlyHMSM(0,0,0,mode->crush);
        }
        for(crutimes=mode->cru_times; crutimes>0; crutimes--)
        {
            GPIO_SetBits(GPIOG,GPIO_Pin_8);
            OSTimeDlyHMSM(0,0,0,mode->bright);
            OSTimeDlyHMSM(0,0,0,mode->crush);
        }
    }
    break;
    case LED_NUM_4:
    {
        for(britimes=mode->bri_times; britimes>0; britimes--)
        {
            GPIO_ResetBits(GPIOG,GPIO_Pin_9);
            OSTimeDlyHMSM(0,0,0,mode->bright);
            GPIO_SetBits(GPIOG,GPIO_Pin_9);
            OSTimeDlyHMSM(0,0,0,mode->crush);
        }
        for(crutimes=mode->cru_times; crutimes>0; crutimes--)
        {
            GPIO_SetBits(GPIOG,GPIO_Pin_9);
            OSTimeDlyHMSM(0,0,0,mode->bright);
            OSTimeDlyHMSM(0,0,0,mode->crush);
        }
    }
    break;

    default:
        break;
    }
}


/*******************************************************************************
** 函数名称: Led_Blink_Times
** 功能描述: LED亮灯次数，判断语句必须并列关系
** 参数说明: NONE
** 返回说明: None
** 创建人员: 王凯
** 创建日期: 2019-06-20
********************************************************************************/
void Led_Blink_Times(void)
{

    while(LED_BlinkTime.LED0_Times|LED_BlinkTime.LED1_Times|LED_BlinkTime.LED2_Times|LED_BlinkTime.LED3_Times|LED_BlinkTime.LED4_Times)
    {
        if(LED_BlinkTime.LED0_Times!=0)
        {
            // Led_Blink_Mode(LED_NUM_0, &LED_MODE_4);
            GPIO_ResetBits(GPIOF,GPIO_Pin_9);
            LED_BlinkTime.LED0_Times--;
        }
        if(LED_BlinkTime.LED1_Times!=0)
        {
            //Led_Blink_Mode(LED_NUM_1, &LED_MODE_4);
            GPIO_ResetBits(GPIOF,GPIO_Pin_10);
            LED_BlinkTime.LED1_Times--;
        }
        if(LED_BlinkTime.LED2_Times!=0)
        {
            // Led_Blink_Mode(LED_NUM_2, &LED_MODE_4);
            GPIO_ResetBits(GPIOF,GPIO_Pin_11);
            LED_BlinkTime.LED2_Times--;
        }
        if(LED_BlinkTime.LED3_Times!=0)
        {
            //Led_Blink_Mode(LED_NUM_3, &LED_MODE_4);
            GPIO_ResetBits(GPIOG,GPIO_Pin_8);
            LED_BlinkTime.LED3_Times--;
        }
        if(LED_BlinkTime.LED4_Times!=0)
        {
            // Led_Blink_Mode(LED_NUM_4, &LED_MODE_4);
            GPIO_ResetBits(GPIOG,GPIO_Pin_9);
            LED_BlinkTime.LED4_Times--;
        }

    }
		OSTimeDlyHMSM(0,0,0,60);
		Led_Clear(NUM_ALL);

}
/********************************End of File************************************/





