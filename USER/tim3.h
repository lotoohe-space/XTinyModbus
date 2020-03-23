#ifndef _TIM3_H__
#define _TIM3_H__
#include "stm32f10x.h"                  // Device header

extern vu32 sys_tick_100us;
void TIM3_Int_Init(u16 arr,u16 psc);

#endif
