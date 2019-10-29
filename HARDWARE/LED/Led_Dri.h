#ifndef __LED_H
#define __LED_H
#include "sys.h"
#include "includes.h"
//����LED����״̬�Ľṹ�����Ĵ�������Ĵ����������Σ��𼸴Σ�����Ϊ���ɸı�const������flash
typedef const struct blink {
    INT16U bright;
    INT16U crush;
    INT8U  bri_times;
    INT8U  cru_times;
} T_led_blink; //sizeof 4
//����ת�����ϼ����Ƶ����Ĵ���
typedef struct blinktimes {
    INT16U LED0_Times;
    INT16U LED1_Times;
    INT16U LED2_Times;
    INT16U LED3_Times;
    INT16U LED4_Times;
} T_led_bTimes;

/*�Ƶı�� ��ʱʹ�ô˱�� ��Ҫ�Ľ� �����������ɶԶ���ƵĲ��� �磺NUM_1|NUM_2*/
#define LED_NUM_0   101
#define LED_NUM_1   102
#define LED_NUM_2   103
#define LED_NUM_3   104
#define LED_NUM_4   105
#define NUM_ALL     111

//����˸ģʽ
extern T_led_blink LED_MODE_2;//����˸ģʽ2�ṹ��
extern T_led_blink LED_MODE_3;
extern T_led_blink LED_MODE_4;
extern T_led_blink LED_MODE_5;
//LEDȫ�ֱ�����ָʾ����˸���ٴ�
extern T_led_bTimes LED_BlinkTime;


/*λ���������Ƶ�*/
#define LED0 PFout(9)	// DS0
#define LED1 PFout(10)	// DS1	 

void Led_Init(void);//��ʼ��
void Led_Blink_Mode(INT8U num, T_led_blink* mode);
void Led_Clear(INT8U num);
void Led_Blink_Times(void);

#endif
