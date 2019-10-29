/********************************Copyright (c)**********************************\                    
**
**----------------------------------�ļ���Ϣ------------------------------------
** �ļ�����: led.c
** ������Ա: ����
** ��������: 2019-06-13
** �ĵ�����: ת������LED�Ƴ�ʼ������ LED0--PF9��LED1--PF10��LED2--PF11��LED3--PG8��LED4--PG9��
********************************************************************************/
#include "myconfig.h"
/*���Ƶ�ģʽΪconst���������ɸı�*/
T_led_blink LED_MODE_2= {50,950,1,0,}; //����һ��
T_led_blink LED_MODE_3= {800,200,1,1,}; // ָʾ�����Ƿ���������
//T_led_blink LED_MODE_4= {20,0,1,0,}; //����˸����ֹcanͨ�����ʹ��������ڴ�й© ָʾcanͨ��
T_led_blink LED_MODE_5= {1000,0,1,0}; //���� ����
T_led_blink LED_MODE_4= {300,0,1,0};

T_led_bTimes LED_BlinkTime= {0,0,0,0,0}; //LED��˸�����ṹ�壬��ʼ��Ϊ0
/*******************************************************************************
** ��������: Led_Init
** ��������: LED�����ų�ʼ��
** ����˵��: None
** ����˵��: None
** ������Ա: ����
** ��������: 2019-06-20
********************************************************************************/
void Led_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//ʹ��GPIOFʱ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);//ʹ��GPIOGʱ��

    //GPIOF9,F10,F11��ʼ������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;//LED0\1\2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(GPIOF, &GPIO_InitStructure);//��ʼ��GPIO
    //GPIOG8 G9��ʼ������//������Աߵı�������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;//LED0\1\2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(GPIOG, &GPIO_InitStructure);//��ʼ��GPIO

    GPIO_SetBits(GPIOF,GPIO_Pin_9 | GPIO_Pin_10|GPIO_Pin_11);//GPIOF9,F10��������������
    GPIO_SetBits(GPIOG,GPIO_Pin_8 | GPIO_Pin_9);//GPIOG8 G9��������������

}

/*******************************************************************************
** ��������: LED_CLEAR
** ��������: ���LED��
** ����˵��: num: [����/��]
** ����˵��: None
** ������Ա: ����
** ��������: 2019-06-03
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
** ��������: Led_Blink_Mode
** ��������: LED��˸ģʽ
** ����˵��: num: �Ʊ��
**			 mode: 1:����ʱ�� 2������ʱ�� 3�����Ĵ��� 4����Ĵ���
** ����˵��: None
** ������Ա: ����
** ��������: 2019-06-03
ע����һ������Ҫ��ɶ������Ƶ�ͬʱ����  �˺�����������  �Ľ��ƻ���ʹ�üĴ��� ����IO�ڵĳ�ʼ������
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
** ��������: Led_Blink_Times
** ��������: LED���ƴ������ж������벢�й�ϵ
** ����˵��: NONE
** ����˵��: None
** ������Ա: ����
** ��������: 2019-06-20
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





