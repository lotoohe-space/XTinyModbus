/********************************************************************************
* @File name: RTOS_timer.c
* @Author: zspace
* @Version: 1.0
* @Date: 2020-4-10
* @Description: RTOS 定时器模块
********************************************************************************/
#ifndef _RTOS_TIMER_H__
#define _RTOS_TIMER_H__
#ifdef __cplusplus
 extern "C" {
#endif 
/*********************************头文件包含************************************/
#include "RTOS_task.h"
/*********************************结束******************************************/


/*********************************配置宏************************************/

/*********************************结束******************************************/

/*********************************自定义类型************************************/
typedef enum{
	TIMER_MODE_ONCE=0,					/** 一次模式 */
	TIMER_MODE_INFINITE_LOOP=1	/** 无线循环模式 */
}TimerRunMode;

typedef struct {
	TASK_TCB 						mTASK_TCB;					/** 继承任务相关的tcb */
	uint32 							backCallTimer;			/** 回调的时间 */
	TaskRunFunction 		funAddr;						/** 函数的回调地址 */
	uint8 							mode;								/** 定时器的模式 */
	uint8 							resetFlag;					/** 复位标记 */
}*PTIMER_CB,TIMER_CB;
/*********************************结束******************************************/

/*********************************函数申明************************************/
PTIMER_CB TimerCreate(
	TaskRunFunction taskFun,void *prg,uint8 level,uint32 taskNum,TimerRunMode timerMode,uint32 timerSysCount
);
void 			TimerReset(PTIMER_CB mTIMER);
uint32 		TimerStart(PTIMER_CB mTIMER);
uint32 		TimerStop(PTIMER_CB mTIMER);
uint32 		TimerSetCycleVal(PTIMER_CB mTIMER,uint32 val);
/*********************************结束******************************************/
#ifdef __cplusplus
}
#endif
#endif
