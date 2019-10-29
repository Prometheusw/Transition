#ifndef __LED_H
#define __LED_H
#include "sys.h"
#include "includes.h"
//定义LED亮灭状态的结构，亮的次数，灭的次数，亮几次，灭几次；定义为不可改变const，放在flash
typedef const struct blink {
    INT16U bright;
    INT16U crush;
    INT8U  bri_times;
    INT8U  cru_times;
} T_led_blink; //sizeof 4
//定义转轨器上几个灯的亮的次数
typedef struct blinktimes {
    INT16U LED0_Times;
    INT16U LED1_Times;
    INT16U LED2_Times;
    INT16U LED3_Times;
    INT16U LED4_Times;
} T_led_bTimes;

/*灯的编号 暂时使用此编号 需要改进 加掩码可以完成对多个灯的操作 如：NUM_1|NUM_2*/
#define LED_NUM_0   101
#define LED_NUM_1   102
#define LED_NUM_2   103
#define LED_NUM_3   104
#define LED_NUM_4   105
#define NUM_ALL     111

//灯闪烁模式
extern T_led_blink LED_MODE_2;//灯闪烁模式2结构体
extern T_led_blink LED_MODE_3;
extern T_led_blink LED_MODE_4;
extern T_led_blink LED_MODE_5;
//LED全局变量，指示灯闪烁多少次
extern T_led_bTimes LED_BlinkTime;


/*位带操作控制灯*/
#define LED0 PFout(9)	// DS0
#define LED1 PFout(10)	// DS1	 

void Led_Init(void);//初始化
void Led_Blink_Mode(INT8U num, T_led_blink* mode);
void Led_Clear(INT8U num);
void Led_Blink_Times(void);

#endif
