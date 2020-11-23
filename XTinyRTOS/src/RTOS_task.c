/********************************************************************************
* @File name: RTOS_task.c
* @Author: zspace
* @Version: 1.0
* @Date: 2020-4-10
* @Description: RTOS 任务管理相关模块
********************************************************************************/

/*********************************头文件包含************************************/
#include "RTOS_task.h"
#include "RTOS_mem.h"
#include "RTOS_timer.h"
/*********************************结束******************************************/

/*********************************全局变量************************************/
volatile 	uint32 sysClkCount = 0;								/*系统运行时间计数*/
PTASK_TCB TASK_TCB_LIST[TASK_TCB_NUM] = {0};		/*任务控制块列表*/
uint32 **	OSLastTaskMem = NULL;									/*上一次运行的堆栈地址*/
PTASK_TCB TCBIng = NULL;												/*正在运行的任务*/
uint32 		exclusiveTemp=0;					
uint8 		exclusiveCount=0;											/*独占模式计数*/
BOOL			OSScheduleSW=TRUE;												/*OS的调度开关*/
/*********************************结束******************************************/

/*********************************函数申明************************************/
void OSIdle(void *prg);
/*********************************结束******************************************/

/******************************************************
*
* Function name :OSFindTasckCB
* Description        :从任务的tcb控制列表中找到能够使用的任务块.
* Parameter         :
*        @CRC     上次计算的CRC16值    
*        @byte    需要参与计算的一字节
* Return          : 获取到能用的下标，-1表示为获取到
**********************************************************/
static int32 OSFindTasckCB(void){
	uint32 i;
	for(i=0;i<TASK_TCB_NUM;i++){
		if(TASK_TCB_LIST[i]==NULL){
			return i;
		}
	}
	return -1;
}

/*******************************************************
*
* Function name :	OSEnterExclusiveMode
* Description   : 进入独占模式，不会被中断打断
* Parameter     : 无
* Return        : 无
**********************************************************/
void OSEnterExclusiveMode(void){
	/*如果没有进入过临界区，则进入*/
	exclusiveTemp=DisableInterrupt();
	exclusiveCount++;
}

/*******************************************************
*
* Function name :	OSExitExclusiveMode
* Description   : 退出独占模式
* Parameter     : 无
* Return        : 无
**********************************************************/
void OSExitExclusiveMode(void){
	if(exclusiveCount>0){
		exclusiveCount--;
		if(!exclusiveCount){
			EnableInterrupt(exclusiveTemp);
		}
	}
}

/*******************************************************
*
* Function name :	OSTaskEnd
* Description   : 当一个任务结束时，会执行此函数，在这个函数中删除当前任务
* Parameter     : 无
* Return        : 无
**********************************************************/
void OSTaskEnd(void){
	if(TCBIng->taskMode==TASK_MODE_THREAD){
		OSTaskDel();
	}else{
		if(((PTIMER_CB)TCBIng)->mode==TIMER_MODE_ONCE){
			OSTaskDel();
		}else{
			int32 dTimer;
			dTimer=((PTIMER_CB)TCBIng)->backCallTimer - TCBIng->runCount;
			//计算运行的时间差，然后进行延时
			if(dTimer>0){
				OSTaskDelay(dTimer);
			}
			TimerReset(((PTIMER_CB)TCBIng));
		}
	}
}
/*******************************************************
*
* Function name :OSCreateBaseManual
* Description        :手动传入tcb控制块,创建一个任务，并且处于就绪状态,该函数可选择创建为定时器任务还是，线程任务
* Parameter         :
* @mTASK_TCB   指向任务的指针   
* @fun_poi   给任务传递的参数
* @prg	  任务分配的多少字长的堆栈
* @level   优先级
* @mem   		栈顶指针
* @mem_num   栈大小
* @mem_low   栈底指针
* @mode   	创建的模式，有两种task和timer模式
* Return          : TRUE success , FALSE fail
**********************************************************/
uint32 OSCreateBaseManual(
	TASK_TCB *mTASK_TCB,TaskRunFunction taskFun,void *prg,uint8 level,uint32 *mem,uint32 mem_num,TaskMode mode
){
	/*栈低指针*/
	uint32 *memLow;
	/*栈顶指针*/
	uint32 *memBottom;
	int32 usableTask;
	if(mTASK_TCB == NULL){return FALSE;}
	if(mem == NULL){return FALSE;}
	/*获取可用的资源*/
	usableTask=OSFindTasckCB();
	if(usableTask<0){return FALSE;}
	/* 计算低位的地址 */
	memLow=mem-mem_num;
	/*保存栈顶的指针*/
	memBottom=mem;

	/* Registers stacked as if auto-saved on exception */
	*(mem) = (uint32)0x01000000L; /* xPSR */
	*(--mem) = ((uint32)taskFun); /* Entry Point */
	/* R14 (LR) (init value will cause fault if ever used)*/
	*(--mem) = (uint32)OSTaskEnd;
	*(--mem) = (uint32)0x12121212L; /* R12*/
	*(--mem) = (uint32)0x03030303L; /* R3 */
	*(--mem) = (uint32)0x02020202L; /* R2 */
	*(--mem) = (uint32)0x01010101L; /* R1 */
	*(--mem) = (uint32)prg; 				/* R0 : argument */
	/* Remaining registers saved on process stack */
	*(--mem) = (uint32)0x11111111L; /* R11 */
	*(--mem) = (uint32)0x10101010L; /* R10 */
	*(--mem) = (uint32)0x09090909L; /* R9 */
	*(--mem) = (uint32)0x08080808L; /* R8 */
	*(--mem) = (uint32)0x07070707L; /* R7 */
	*(--mem) = (uint32)0x06060606L; /* R6 */
	*(--mem) = (uint32)0x05050505L; /* R5 */
	*(--mem) = (uint32)0x04040404L; /* R4 */	
	/*设置堆栈的地址*/
	mTASK_TCB->memTask=mem;
	/*初始化任务控制块*/
	mTASK_TCB->runCount=0;
	mTASK_TCB->delayCount=0;
	/*任务控制块的地址作为进程id*/
	mTASK_TCB->taskID=(uint32)mTASK_TCB;
	mTASK_TCB->status=TRUE;
	/*限制最大的优先级*/
	if(level>TASK_MIN_LEVEL){level=TASK_MIN_LEVEL;};
	/*设置任务的优先级*/
	mTASK_TCB->level=level;
	/*保存栈低指针*/
	mTASK_TCB->memLowTask=memLow;
	/*保存栈顶的指针*/
	mTASK_TCB->memBottomTask=memBottom;	
	/*当前任务的模式*/
	mTASK_TCB->taskMode=mode;
	/*保存任务控制块*/
	TASK_TCB_LIST[usableTask]=mTASK_TCB;
	return TRUE;
}

/*******************************************************
*
* Function name :OSCreateBase
* Description        :创建一个任务，并且处于就绪状态,该函数可选择创建为定时器任务还是，线程任务
* Parameter         :
*        @fun_poi        指向任务的指针    
*        @prg    给任务传递的参数
*        @level    优先级
*        @task_num    任务分配的多少字长的堆栈
*        @mode    创建的模式，有两种task timer
* Return          : 失败返回NULL，否则返回任务控制块
**********************************************************/
TASK_TCB *OSCreateBase(
	TaskRunFunction taskFun,void *	prg,uint8 level,uint32 	task_num,TaskMode mode
){
	uint32 	*mem;
	TASK_TCB *mTASK_TCB;	
	/*申请控制块的内存*/
	mTASK_TCB=(TASK_TCB *)OSMalloc(sizeof(TASK_TCB));
	if(mTASK_TCB==NULL){return NULL;}
	/*申请堆栈的内存*/
	mem=(uint32 *)OSMalloc(sizeof(uint32)*task_num);
	if(mem==NULL){OSFree(mTASK_TCB);return NULL;}		
	/*创建任务*/
	if(
		OSCreateBaseManual(mTASK_TCB,taskFun,prg,level,&mem[task_num-1],task_num,TASK_MODE_THREAD)
		==FALSE
	){
		/*任务表满了，所以失败了*/
		OSFree(mem);
		OSFree(mTASK_TCB);
		return NULL;
	}
	return mTASK_TCB;
}

/*******************************************************
*
* Function name :OSCreateTask
* Description        :创建一个任务，并且处于就绪状态
* Parameter         :
*        @fun_poi     指向任务的指针       
*        @prg    			给任务传递的参数
*        @level    		任务优先级
*        @task_num    任务分配的多少字长的堆栈
* Return          : TRUE success , FALSE fail
**********************************************************/
TASK_TCB* OSCreateTask(TaskRunFunction taskFun,void *prg,uint8 level,uint32 task_num){
	return OSCreateBase(taskFun,prg,level,task_num,TASK_MODE_THREAD);
}


/*******************************************************
*
* Function name :OSTaskDel
* Description        :删除正在运行的任务
* Parameter         :none
* Return          :none
**********************************************************/
void OSTaskDel(void ){
	uint32 i;
	uint32 lev;
	lev=DisableInterrupt();
	for(i=0;i<TASK_TCB_NUM;i++){
		if(TASK_TCB_LIST[i]==TCBIng){
			OSFree(TCBIng->memLowTask);
			OSFree(TCBIng);
			TASK_TCB_LIST[i] = NULL;
			break;
		}
	}	
	EnableInterrupt(lev);
	OpenSchedule();
}

/*******************************************************
*
* Function name :OSTaskDelay
* Description        :当前任务延时
* Parameter         :
*        @count     延时多少个系统时钟   
* Return          :none
**********************************************************/
void OSTaskDelay(uint32 count){
	uint32 lev;
	lev=DisableInterrupt();
	TCBIng->delayCount=count;
	EnableInterrupt(lev);
	OpenSchedule();
}
/*******************************************************
*
* Function name :TaskSW
* Description        :任务切换，该函数在core.c中调用
* Parameter         :none 
* Return          :none
**********************************************************/
void *OSTaskSW(void){
	uint32 i=0;
	TASK_TCB *max_TASK_TCB=NULL;
	static TASK_TCB *back_task_tcb=NULL;
	/*定时器模式*/
	if(TCBIng->taskMode==TASK_MODE_TIMER){
		/*如果是定时器模式*/
		if(((PTIMER_CB)TCBIng)->resetFlag==1){
			/*退到压栈时候的值*/
			*OSLastTaskMem=TCBIng->memBottomTask-15;
			((PTIMER_CB)TCBIng)->resetFlag=0;
		}
	}
	/*查找没有相同优先级的*/
	if(back_task_tcb!=NULL){
		uint32 spotted=0;
		for(i=0;i<TASK_TCB_NUM;i++){
			if(back_task_tcb==TASK_TCB_LIST[i]){
				spotted=1;
				continue; 
			}
			/*确保是没有被调度过的任务*/
			if(spotted==1){
				if(TASK_TCB_LIST[i]!=NULL&&
					TASK_TCB_LIST[i]->status==TRUE&&
				/*任务没有被延时*/
					TASK_TCB_LIST[i]->delayCount==0&&
					back_task_tcb->level==TASK_TCB_LIST[i]->level){
					max_TASK_TCB=TASK_TCB_LIST[i];
					goto _exit;
				}
			}
		}
	}
	back_task_tcb=NULL;
	for(i=0;i<TASK_TCB_NUM;i++){
		if(TASK_TCB_LIST[i]!=NULL){
			if(TASK_TCB_LIST[i]->status==TRUE&&
				/*任务没有被延时*/
					TASK_TCB_LIST[i]->delayCount==0){
				if(max_TASK_TCB==NULL||(
					max_TASK_TCB->status==FALSE||
				/*任务没有被延时*/
					max_TASK_TCB->delayCount>0)){
					max_TASK_TCB=TASK_TCB_LIST[i];
					continue;
				}
				/*获取优先级最高的*/
				if(max_TASK_TCB->level > TASK_TCB_LIST[i]->level){
					max_TASK_TCB=TASK_TCB_LIST[i];
				}
			}
		}
	}
//	for(i=0;i<TASK_TCB_NUM;i++){
//		if(TASK_TCB_LIST[i]!=null){
//			if(TASK_TCB_LIST[i]->status==true&&
//				//任务没有被延时
//					TASK_TCB_LIST[i]->delay_count==0){
//				//获取优先级最高的
//				if(max_TASK_TCB==NULL){
//					max_TASK_TCB=TASK_TCB_LIST[i];
//				}else if(max_TASK_TCB->level > TASK_TCB_LIST[i]->level){
//					max_TASK_TCB=TASK_TCB_LIST[i];
//				}
//			}
//		}
//	}

_exit:
	OSLastTaskMem=&max_TASK_TCB->memTask;	/*当前的堆栈*/
	TCBIng=max_TASK_TCB;									/*保存当前运行的tcb*/
	back_task_tcb=max_TASK_TCB;						/*保存上次获得的最大优先级*/
	return max_TASK_TCB->memTask;					/*返回堆栈的地址*/
}


/*******************************************************
*
* Function name :OSInit
* Description        :操作系统初始化
* Parameter         :无
* Return          :无
**********************************************************/
void OSInit(void){
	InitMem();
	
	OSScheduleSW=FALSE;/*调度开*/
}

/*******************************************************
*
* Function name :OSStart
* Description        :操作系统启动
* Parameter         :无
* Return          :无
**********************************************************/
void OSStart(void){
	/*创建一个空闲任务,空闲任务的优先级为最低优先级*/
	if(OSCreateTask(OSIdle,NULL,TASK_MIN_LEVEL,64)==FALSE){return ;}
	OSScheduleSW=TRUE;
	/*跳转到汇编启动操作系统*/
	StartRTOS();
}

/*******************************************************
*
* Function name :OSStart
* Description        :获取操作系统的运行时钟计数
* Parameter         :无
* Return          :操作系统的运行时间
**********************************************************/
uint32 OSSystickCount(void){
	return sysClkCount;
}

/*******************************************************
*
* Function name : OSIdle
* Description        :操作系统空闲函数，该任务最低优先级
* Parameter         :无
* Return          :无
**********************************************************/
void OSIdle(void *prg){
	prg=prg;
	for(;;){
	}
}
/*******************************************************
*
* Function name :OSScheduleCTRL
* Description        :调度开关设置
* Parameter         :无
* Return          :无
**********************************************************/
void OSScheduleCTRL(BOOL sw){
	OSEnterExclusiveMode();
	OSScheduleSW = sw;
	OSExitExclusiveMode();
}
/*******************************************************
*
* Function name :OSScheduleCTRL
* Description        :OS是否在運行
* Parameter         :无
* Return          :运行状态
**********************************************************/
BOOL OSIsRun(void){
	return OSScheduleSW;
}
/*******************************************************
*
* Function name :OSSchedule
* Description        :调用将触发一次操作系统调度
* Parameter         :无
* Return          :无
**********************************************************/
void OSSchedule(void){
	if(OSScheduleSW){
		/*如果打开了调度，则允许调度*/
		OpenSchedule();
	}
}
/*******************************************************
*
* Function name :TaskLoop
* Description        :使正在休眠的任务的时间减一
* Parameter         :none
* Return          :none
**********************************************************/
void OSProcess(void){
	uint32 i;
	uint32 lev;
	/*系统的运行时间+1*/
	sysClkCount++;
	for(i=0;i<TASK_TCB_NUM;i++){
		/*线程处于活跃状态*/
		if(TASK_TCB_LIST[i] != NULL && TASK_TCB_LIST[i]->status == TRUE){
			/*是否有延时*/
			if(TASK_TCB_LIST[i]->delayCount>0){
				lev=DisableInterrupt();
				TASK_TCB_LIST[i]->delayCount--;
				EnableInterrupt(lev);
			}
			lev=DisableInterrupt();
			/*运行时间+1*/
			TASK_TCB_LIST[i]->runCount++;	
			EnableInterrupt(lev);
		}
	}
}
/*******************************************************
*
* Function name :OSScheduleProcess
* Description        :该函数进行中断调度的处理
* Parameter         :none
* Return          :none
**********************************************************/
void OSScheduleProcess(void){
	if(OSScheduleSW){
		OSProcess();			/*线程休眠时间处理*/
		OSSchedule();			/*开启pendsv中断*/
	}
}
