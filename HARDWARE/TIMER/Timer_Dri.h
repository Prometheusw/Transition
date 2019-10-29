#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sys.h"
#include "delay.h"

typedef struct _HEARTBEAT{
	u8 BeatTime;//心跳计时
	u8 BeatSend;//心跳帧发送标志
}T_HeartBeat;

enum _SENGFlAG{
	 AlreadySend,
	 NotSend,
	};

extern T_HeartBeat HeartBeat;//心跳标志

void TIM7_Int_Init(u16 arr,u16 psc);
void Timer2_Init(u16 arr,u16 psc);    //1ms产生1次更新事件




