/********************************************************************************
 * @File name: RTOS_core.c
 * @Author: zspace
 * @Version: 1.0
 * @Date: 2020-4-10
 * @Description: RTOS Core 模块
 ********************************************************************************/

/*********************************头文件包含************************************/
#include "RTOS_core.h"
/*********************************结束******************************************/

/*********************************函数申明************************************/
void OSScheduleProcess(void);
/*********************************结束******************************************/

/*******************************************************
*
* Function name :DisableInterrupt
* Description        :关闭芯片的中断响应
* Parameter         :无
* Return          : PRIMASK的值
**********************************************************/
__asm uint32 DisableInterrupt(void){
		REQUIRE8    					; 加这两条对齐伪指令防止链接器报错
		PRESERVE8   					; 8 字对齐	
    MRS     R0, PRIMASK  	; 读取PRIMASK到R0,R0为返回值 
    CPSID   I							; PRIMASK=1,关中断(NMI和硬件FAULT可以响应)
    BX      LR			    	; 返回
}
/*******************************************************
*
* Function name :EnableInterrupt
* Description        :开启芯片的中断响应
* Parameter         :
*        @temp        之前读到的PRIMASK值
* Return          : 无
**********************************************************/
__asm void EnableInterrupt(uint32 temp){
    MSR     PRIMASK, R0	  ;读取R0到PRIMASK中,R0为参数
    BX      LR						;返回
}
			
/*******************************************************
*
* Function name :StartRTOS
* Description        :开启操作系统
* Parameter         :无
* Return          : 无
**********************************************************/
__asm void StartRTOS(void){
			CPSID   I 
			;首先设置pendsv为最低优先级
			;设置pendsv的中断优先级
			ldr r0,=0xE000ED22
			;最低优先级
			ldr r1,=0xff
			;设置
			strb r1,[r0]
			;设置psp为0,用于判断是否第一次任务调度
			MOVS R0, #0 ;R0 = 0
			MSR PSP, R0 ;PSP = R0
			;开启pendsv中断
			LDR R0, =0xE000ED04 ;R0 = 0xE000ED04
			LDR R1, =0x10000000 ;R1 = 0x10000000
			;设置触发
			STR.w R1, [R0] ;*(uint32_t *)NVIC_INT_CTRL = NVIC_PENDSVSET
			;打开中断
			CPSIE I ;
			;死循环
os_loop 
			B os_loop
}

/*******************************************************
*
* Function name :StartRTOS
* Description        :触发pendsv中断，以便进行操作系统调度
* Parameter         :无
* Return          : 无
**********************************************************/
__asm void OpenSchedule(void){
				;push {r0-r1,lr}
				LDR R0, =0xE000ED04
				LDR R1, =0x10000000 
				;进入pendsv中断
				STR R1, [R0]
				;pop	  {r0-r1,pc}
				bx lr
}
/*******************************************************
*
* Function name :PendSV_Handler
* Description        :pendsv中断函数，该函数种进行任务切换
* Parameter         :无
* Return          : 无
**********************************************************/
__asm void PendSV_Handler(void){
		
				IMPORT OSTaskSW
				IMPORT OSLastTaskMem
	
        CPSID   I
				;获得sp指针的值
				MRS R0, PSP ;R0 = PSP
					
				;如果第一次执行,则执行一次中断调度
				CBZ R0, thread_change 
				;不是第一次则保护r4-r11
				SUBS R0, R0, #0x20 ;R0 -= 0x20
				STM R0, {R4-R11} ;		
				
				;保存本次的栈顶地;
				;修改上次任务的栈顶值
				;为了出现保证下次切换不出现错误
				ldr r1,=OSLastTaskMem
				ldr r1,[r1]
				str R0,[r1]					
thread_change	;任务调度
				push {lr}
				ldr.w r0,=OSTaskSW
				blx r0
				pop  {lr}

				LDM R0, {R4-R11} ;恢复新的r4-r11的值
				ADDS R0, R0, #0x20 ;R0 += 0x20
				MSR PSP, R0
				;切换到用户线程模式
				;lr 的第2位为1时自动切换
				ORR LR, LR, #0x04  
				;开中断
				cpsie I
				BX 	LR	
}
/*******************************************************
*
* Function name :SysTick_Handler
* Description        :systick的中断函数
* Parameter         :无
* Return          :无
**********************************************************/
void SysTick_Handler(void){
	OSScheduleProcess();
}

