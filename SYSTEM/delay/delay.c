#include "delay.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//Receive status flag
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos สนำร	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//STM32F103ZE core board
//Use SysTick's normal counting mode to manage the delay (suitable for STM32F10x series)
//Include delay_us, delay_ms

//////////////////////////////////////////////////////////////////////////////////  

static u8  fac_us=0;							//us delay multiplier			   
static u16 fac_ms=0;							//ms delay multiplier, under ucos, it represents the number of ms per beat
	
	
#if SYSTEM_SUPPORT_OS							//If SYSTEM_SUPPORT_OS is defined, it means that the OS is supported (not limited to UCOS).
//When delay_us/delay_ms needs to support OS, three OS-related macro definitions and functions are needed to support
//First, there are 3 macro definitions:
// delay_osrunning: used to indicate whether the OS is currently running to determine whether the related function can be used
//delay_ostickspersec: used to represent the clock beat set by the OS, delay_init will initialize the systick according to this parameter
// delay_osintnesting: used to indicate the nesting level of OS interrupts, because interrupts cannot be scheduled, delay_ms uses this parameter to determine how to run
//Then 3 functions:
// delay_osschedlock: used to lock OS task scheduling and prohibit scheduling
//delay_osschedunlock: used to unlock OS task scheduling and restart scheduling
// delay_ostimedly: used for OS delay, which can cause task scheduling.

//This routine is only supported by UCOSII and UCOSIII, other OS, please refer to the transplantation by yourself
//Support UCOSII
#ifdef OS_CRITICAL_METHOD //OS_CRITICAL_METHOD is defined, indicating that UCOSII should be supported
#define delay_osrunning OSRunning //Is the OS running flag, 0, not running; 1, running
#define delay_ostickspersec OS_TICKS_PER_SEC //OS clock beats, that is, the number of dispatches per second
#define delay_osintnesting OSIntNesting //Interrupt nesting level, that is, the number of interrupt nesting
#endif

//Support UCOSIII
#ifdef CPU_CFG_CRITICAL_METHOD //CPU_CFG_CRITICAL_METHOD is defined, indicating that UCOSIII should be supported
#define delay_osrunning OSRunning //Is the OS running flag, 0, not running; 1, running
#define delay_ostickspersec OSCfg_TickRate_Hz //OS clock tick, that is, the number of scheduling per second
#define delay_osintnesting OSIntNestingCtr //Interrupt nesting level, that is, the number of interrupt nesting
#endif


//When the us-level delay, turn off task scheduling (to prevent interrupting the us-level delay)
void delay_osschedlock(void)
{
#ifdef CPU_CFG_CRITICAL_METHOD   			
	OS_ERR err; 
	OSSchedLock(&err);						
#else											
	OSSchedLock();							
#endif
}

//When the us-level delay, resume task scheduling
void delay_osschedunlock(void)
{	
#ifdef CPU_CFG_CRITICAL_METHOD   				
	OS_ERR err; 
	OSSchedUnlock(&err);						
#else										
	OSSchedUnlock();							
#endif
}

//Call the delay function that comes with the OS to delay
//ticks: Delayed number of beats
void delay_ostimedly(u32 ticks)
{
#ifdef CPU_CFG_CRITICAL_METHOD
	OS_ERR err; 
	OSTimeDly(ticks,OS_OPT_TIME_PERIODIC,&err);	//UCOSII delay adopts periodic mode
#else
	OSTimeDly(ticks);						
#endif 
}
 
//Systick interrupt service function, used when using ucos
void SysTick_Handler(void)
{	
	if(delay_osrunning==1)						
	{
		OSIntEnter();				
		OSTimeTick();              
		OSIntExit();  
	}
}
#endif

			   
//Initialize the delay function
//When using OS, this function will initialize the clock beat of OS
//SYSTICK clock is fixed at 1/8 of HCLK clock
//SYSCLK: system clock
void delay_init()
{
#if SYSTEM_SUPPORT_OS  							//If you need to support OS.
	u32 reload;
#endif
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//Select external clock HCLK/8
	fac_us=SystemCoreClock/8000000;				//1/8 of the system clock
#if SYSTEM_SUPPORT_OS  							//If you need to support OS.
	reload=SystemCoreClock/8000000; //Counting times per second unit is M
	reload*=1000000/delay_ostickspersec; //Set the overflow time according to delay_ostickspersec
	//reload is a 24-bit register, the maximum value: 16777216, at 72M, about 1.86s
	fac_ms=1000/delay_ostickspersec; //Represents the minimum unit that the OS can delay

	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk; //Enable SYSTICK interrupt
	SysTick->LOAD=reload; //Interrupt once every 1/delay_ostickspersec second
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk; //Enable SYSTICK
#else
	fac_ms=(u16)fac_us*1000;					//Under non-OS, it represents the number of systick clocks required for each ms
#endif
}								    

#if SYSTEM_SUPPORT_OS  							//If you need to support OS.
//Delay nus
//nus is the number of us to be delayed.	    								   
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;				
	ticks=nus*fac_us; 								 
	tcnt=0;
	delay_osschedlock();					
	told=SysTick->VAL;        				
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;			
		}  
	};
	delay_osschedunlock();											    
}
//Delay nms
//nms: the number of ms to delay
void delay_ms(u16 nms)
{	
	if(delay_osrunning&&delay_osintnesting==0)	
	{		 
		if(nms>=fac_ms)							
		{ 
   			delay_ostimedly(nms/fac_ms);	
		}
		nms%=fac_ms;							
	}
	delay_us((u32)(nms*1000));					
}
#else When not using OS
//Delay nus
//nus is the number of us to be delayed.    								   
void delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; 						 
	SysTick->VAL=0x00;        					
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));		
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	
	SysTick->VAL =0X00;      					
}
//Delay nms
//Pay attention to the range of nms
//SysTick->LOAD is a 24-bit register, so the maximum delay is:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK unit is Hz, nms unit is ms
//For 72M conditions, nms<=1864
void delay_ms(u16 nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;				
	SysTick->VAL =0x00;							
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;
	SysTick->VAL =0X00;       				
} 
#endif 








































