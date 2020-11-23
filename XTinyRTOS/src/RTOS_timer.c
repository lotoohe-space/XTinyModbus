/********************************************************************************
* @File name: RTOS_timer.c
* @Author: zspace
* @Version: 1.0
* @Date: 2020-4-10
* @Description: RTOS 定时器模块
********************************************************************************/

/*********************************头文件包含************************************/
#include "RTOS_timer.h"
#include "RTOS_mem.h"
#include "RTOS_task.h"
/*********************************结束******************************************/

/*******************************************************
*
* Function name :TimerCreate
* Description        :创建一个定时器，立刻开始运行
* Parameter         :
*        @taskFun       定时器函数
*        @prg    			传递的参数
*        @taskNum    栈大小
*        @timerMode    任务模式
*        @timerSysCount    定时器时间
* Return          : 错误返回NULL
**********************************************************/
PTIMER_CB TimerCreate(
	TaskRunFunction taskFun,void *prg,uint8 level,uint32 taskNum,TimerRunMode timerMode,uint32 timerSysCount
){
	PTIMER_CB pTimerCB;
	uint32 *mem;
	/*申请控制块的内存*/
	pTimerCB=(PTIMER_CB)OSMalloc(sizeof(TIMER_CB));
	if(pTimerCB==NULL){
		return NULL;
	}
	/*申请堆栈的内存*/
	mem=(uint32 *)OSMalloc(sizeof(uint32)*taskNum);
	if(mem == NULL){
		OSFree(pTimerCB);
		return NULL;
	}
	/*创建任务*/
	if(
		OSCreateBaseManual((TASK_TCB *)pTimerCB,taskFun,prg,level,&mem[taskNum-1],taskNum,TASK_MODE_TIMER)==FALSE
	){
		/*任务表满了，所以失败了*/
		OSFree(mem);
		OSFree(pTimerCB);
		return NULL;
	}
	OSEnterExclusiveMode();
	pTimerCB->backCallTimer=timerSysCount;		/*多少时间回调一次,一来就设置延时*/
	pTimerCB->mode=timerMode;									/*定时器模式*/
	pTimerCB->resetFlag=0;										/*设置未复位模式*/
	pTimerCB->mTASK_TCB.status=FALSE;					/*定时器开始处于停止状态*/
	pTimerCB->funAddr=taskFun;								/*函数的地址*/
	OSExitExclusiveMode();
	return pTimerCB;
}

/*******************************************************
*
* Function name :TimerSetCycleVal
* Description        :设置定时器的定时值
* Parameter         :
*        @pTIMER       定时器对象指针
*        @val    			循环值
* Return          : TRUE or FALSE
**********************************************************/
uint32 TimerSetCycleVal(PTIMER_CB pTIMER,uint32 val){
	if(pTIMER==NULL){return FALSE;}
	OSEnterExclusiveMode();
	pTIMER->backCallTimer=val;
	OSExitExclusiveMode();
	return TRUE;		
}
/*******************************************************
*
* Function name :TimerStop
* Description        :停止定时器
* Parameter         :
*        @pTIMER       定时器对象指针
* Return          : TRUE or FALSE
**********************************************************/
uint32 TimerStop(PTIMER_CB pTIMER){
	if(pTIMER==NULL){return FALSE;}
	OSEnterExclusiveMode();
	pTIMER->mTASK_TCB.status=FALSE;
	OSExitExclusiveMode();
	return TRUE;	
}

/*******************************************************
*
* Function name :TimerStart
* Description        :启动这定时器
* Parameter         :
*        @pTIMER       定时器对象指针
* Return          : TRUE or FALSE
**********************************************************/
uint32 TimerStart(PTIMER_CB pTIMER){
	if(pTIMER==NULL){return FALSE;}
	OSEnterExclusiveMode();
	pTIMER->mTASK_TCB.status=TRUE;
	OSExitExclusiveMode();
	return TRUE;
}
/*******************************************************
*
* Function name :TimerDel
* Description        :删除定时器
* Parameter         :无
* Return          : 无
**********************************************************/
void TimerDel(void){
	/* 这里直接调用task的删除函数，因为继承了task */
	OSTaskDel();
}
/** 
 * 复位定时器到最开始的状态
 * @param[in] mTIMER 定时器的指针
 * @retval  无
 */
/*******************************************************
*
* Function name :TimerReset
* Description        :复位定时器到最开始的状态
* Parameter         :
*        @pTIMER       定时器对象指针
* Return          : 无
**********************************************************/
void TimerReset(PTIMER_CB pTIMER){
	uint32 *mem;
	OSEnterExclusiveMode();
	pTIMER->mTASK_TCB.memTask=pTIMER->mTASK_TCB.memBottomTask;	/*复位栈顶的指针*/
	pTIMER->mTASK_TCB.delayCount=pTIMER->backCallTimer;	/*复位定时值*/
	pTIMER->mTASK_TCB.runCount=0;	/*复位运行的时间*/
	pTIMER->resetFlag=1;		/*设置为复位模式*/
	mem=pTIMER->mTASK_TCB.memTask;
	/* Registers stacked as if auto-saved on exception */
	*(mem) = (uint32)0x01000000L; /* xPSR */
	*(--mem) = (uint32)(pTIMER->funAddr); /* Entry Point */
	/* R14 (LR) (init value will cause fault if ever used)*/
	*(--mem) = (uint32)OSTaskEnd;
	*(--mem) = (uint32)0x12121212L; /* R12*/
	*(--mem) = (uint32)0x03030303L; /* R3 */
	*(--mem) = (uint32)0x02020202L; /* R2 */
	*(--mem) = (uint32)0x01010101L; /* R1 */
	*(--mem) = (uint32)0x00000000L; /* R0 : argument */
	/* Remaining registers saved on process stack */
	*(--mem) = (uint32)0x11111111L; /* R11 */
	*(--mem) = (uint32)0x10101010L; /* R10 */
	*(--mem) = (uint32)0x09090909L; /* R9 */
	*(--mem) = (uint32)0x08080808L; /* R8 */
	*(--mem) = (uint32)0x07070707L; /* R7 */
	*(--mem) = (uint32)0x06060606L; /* R6 */
	*(--mem) = (uint32)0x05050505L; /* R5 */
	*(--mem) = (uint32)0x04040404L; /* R4 */
	pTIMER->mTASK_TCB.memTask=mem;
	OSExitExclusiveMode();
	OpenSchedule();
}
