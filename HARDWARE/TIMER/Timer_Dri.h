#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sys.h"
#include "delay.h"

typedef struct _HEARTBEAT{
	u8 BeatTime;//������ʱ
	u8 BeatSend;//����֡���ͱ�־
}T_HeartBeat;

enum _SENGFlAG{
	 AlreadySend,
	 NotSend,
	};

extern T_HeartBeat HeartBeat;//������־

void TIM7_Int_Init(u16 arr,u16 psc);
void Timer2_Init(u16 arr,u16 psc);    //1ms����1�θ����¼�




