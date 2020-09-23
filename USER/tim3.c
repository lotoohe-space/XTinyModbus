#include "tim3.h"
#include "Sys_Config.h"
#if MD_USD_SALVE
#include "MDS_RTU_Serial.h"
#include "MDS_RTU_Serial_1.h"
#else 
#include "MDM_RTU_Serial.h"
#endif

vu32 sys_tick_100us=0;

//General purpose timer 3 interrupt initialization
//The clock here is 2 times that of APB1, and APB1 is 36M
//arr: automatic reload value.
//psc: clock prescaler number
//Timer 3 is used here!
void TIM3_Int_Init(u16 arr,u16 psc){
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 

	TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler =psc;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 

	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); 

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);  

	TIM_Cmd(TIM3, ENABLE);                          
}


void TIM3_IRQHandler(void)   												
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) 	
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  		 
		sys_tick_100us++;
		#if MD_USD_SALVE
		MDSTimeHandler100US();
		MDSTimeHandler100US_1();
		#else
		MDMTimeHandler100US();
		#endif
	}
}
