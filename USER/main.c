#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "tim3.h"

#include "MD_RTU_APP.h"
#include "MDM_RTU_APP.h"
#include "w25qxx.h"
#include "touch.h"
#include "GBK_LibDrive.h"	

#include "bsp_amg88xx.h"

#include <stdlib.h>



 int main(void)
 {	 		    
	delay_init();	    	 //延时函数初始化	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	//uart_init(115200);	 	//串口初始化为115200
	 
 	LED_Init();			     //LED端口初始化
	LCD_Init();          //液晶屏初始化
  W25QXX_Init();			//W25QXX初始化

  POINT_COLOR=RED; 
	
	while(W25QXX_ReadID()!=W25Q16)								//检测不到W25QXX
	{
		LCD_ShowString(30,150,200,16,16,RED,"W25Q16 Check Failed!");
		delay_ms(500);
		LCD_ShowString(30,150,200,16,16,RED,"Please Check!      ");
		delay_ms(500);
		LED1=!LED1;		//DS0闪烁
	}
	LCD_ShowString(30,150,200,16,16,RED,"W25Q16 Ready!"); 
	
  GBK_Lib_Init();       //硬件GBK字库初始化--(如果使用不带字库的液晶屏版本，此处可以屏蔽，不做字库初始化） 
	
	KEY_Init();	//按键初始化
	 
 	tp_dev.init();//触摸屏初始化
	
	amg88xx_init();
	
  LCD_Clear(WHITE);//清除屏幕

	MDS_RTU_APPInit();
	//MDM_RTU_APPInit();
	TIM3_Int_Init(72-1,100-1);
	while(1){
		MDS_RTU_Loop();
		//MDM_RTU_Loop();
		//delay_ms(50);
	}
}
////放到主函数的初始化中初始化
//void Timer1CountInitial(void)
//{
//	NVIC_InitTypeDef NVIC_InitStructure;
//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//	TIM_DeInit(TIM1);	/*复位TIM1定时器*/
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);	/*开时钟*/
//	TIM_TimeBaseStructure.TIM_Period = 100;	/*时钟滴答的次数，够数中断这里是1ms中断一次*/     
//	TIM_TimeBaseStructure.TIM_Prescaler = 720-1;	/* 分频720*/       
//	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  	 
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	/*计数方向向上计数*/
//	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
//	/* Clear TIM1 update pending flag  清除TIM1溢出中断标志]  */
//	TIM_ClearFlag(TIM1, TIM_FLAG_Update);
//	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;	/*溢出中断*/
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;  
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);	/* Enable TIM1 Update interrupt TIM1溢出中断允许*/
//	TIM1->CR1 |= TIM_CR1_CEN;	/*开启Timer1计数*/
//}


//void TIM1_UP_IRQHandler(void)
//{        
//	//TIM_TimeBaseStructure.TIM_Period = 100-1;//自动重装值（此时进中断的周期为100ms）
//	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
//	{  
//		sys_tick++;
//		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
//	}
//}




