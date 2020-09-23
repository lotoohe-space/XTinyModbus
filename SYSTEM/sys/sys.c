#include "sys.h"


//********************************************************************************  
//THUMB instruction does not support assembly inline
//Use the following method to implement the assembly instruction WFI  
void WFI_SET(void)
{
	__ASM volatile("wfi");		  
}
//Close all interrupts
void INTX_DISABLE(void)
{		  
	__ASM volatile("cpsid i");
}
//Enable all interrupts
void INTX_ENABLE(void)
{
	__ASM volatile("cpsie i");		  
}
//Set the top address of the stack
//addr: stack top address
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}
