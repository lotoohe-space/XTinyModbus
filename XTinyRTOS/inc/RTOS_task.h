#ifndef _RTOS_TASK_H__
#define _RTOS_TASK_H__
#ifdef __cplusplus
 extern "C" {
#endif 
/*********************************头文件包含************************************/
#include "RTOS_type.h"
#include "RTOS_core.h"
/*********************************结束******************************************/

/*********************************配置宏************************************/
#define OS_TICKS_PER_SEC 	1000		/*RTOS调度频率*/
#define TASK_MIN_LEVEL 		10			/*设置最小优先级 */
#define TASK_TCB_NUM 			16			/*最多个任务 */
/*********************************结束******************************************/

/*********************************自定义类型************************************/
typedef enum {
	TASK_MODE_THREAD=0,	/** 当前任务线程模式 */
	TASK_MODE_TIMER=1		/** 当前任务定时器模式 */
}TaskMode;

typedef void(*TaskRunFunction)(void* prg);
typedef struct {
	/** 堆栈地址该指针会随程序运行发生变化 */
	uint32 *memTask;
	/** 堆栈的栈低指针，当任务终结时用于内存释放 */
	uint32 *memLowTask;
	/** 堆栈的栈顶指针 */
	uint32 *memBottomTask;
	/** 运行时间计数 */
	uint32 runCount;
	/** 延时计数 */
	uint32 delayCount;
	/** 进程id */
	uint32 taskID;
	/** 当前状态 */
	uint8 status;
	/** 任务优先级 */
	uint8 level;
	/** 当前任务的模式 */
	TaskMode taskMode;
}*PTASK_TCB,TASK_TCB;
/*********************************结束******************************************/

/*********************************函数申明************************************/
uint32 		OSCreateBaseManual(TASK_TCB *mTASK_TCB,TaskRunFunction taskFun,void *prg,uint8 level,uint32 *mem,uint32 mem_num,	TaskMode mode);
TASK_TCB *OSCreateBase(TaskRunFunction taskFun,void *prg,uint8 level,uint32 task_num,	TaskMode mode);
void 			OSTaskEnd(void);
	
/*下面用户使用*/
void 			OSInit(void);
void 			OSStart(void);
void 			OSTaskDelay(uint32 count);
void	 		OSTaskDel(void);
uint32 		OSSystickCount(void);
TASK_TCB *OSCreateTask(TaskRunFunction taskFun,void *prg,uint8 level,uint32 task_num);
void 			OSSchedule(void);
void 			OSScheduleCTRL(BOOL sw);
BOOL 			OSIsRun(void);

void 			OSEnterExclusiveMode(void);
void 			OSExitExclusiveMode(void);
/*********************************结束******************************************/	
#ifdef __cplusplus
}
#endif
#endif
