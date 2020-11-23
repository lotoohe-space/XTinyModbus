/********************************************************************************
* @File name: RTOS_mutex.c
* @Author: zspace
* @Version: 1.0
* @Date: 2020-4-10
* @Description: RTOS 互斥锁
********************************************************************************/

/*********************************头文件包含************************************/
#include "RTOS_mutex.h"
#include "RTOS_mem.h"
#include "RTOS_task.h"
/*********************************结束******************************************/

/*******************************************************
*
* Function name :MutexCreate
* Description        :互斥量的创建函数
* Parameter         :无
* Return          : 失败返回NULL 
**********************************************************/
PMUTEX_CB MutexCreate(void){
	PMUTEX_CB pMutexCB;
	pMutexCB = (PMUTEX_CB)OSMalloc(sizeof(MUTEX_CB));
	if(pMutexCB == NULL){return NULL;}
	pMutexCB->mutexFlag=0;
	return pMutexCB;
}
/*******************************************************
*
* Function name :MutexFree
* Description        :互斥量的释放函数
* Parameter         :
*        @pMUTEX        信号量对象指针 
* Return          : 无
**********************************************************/
void MutexFree(PMUTEX_CB pMUTEX){
	if(pMUTEX==NULL){return ;}
	OSFree(pMUTEX);
}
/*******************************************************
*
* Function name :MutexUnlock
* Description        :解锁
* Parameter         :
*        @pMUTEX        信号量对象指针 
* Return          : 无
**********************************************************/
uint32 MutexUnlock(PMUTEX_CB pMUTEX){
	if(pMUTEX==NULL){return FALSE;}
	OSEnterExclusiveMode();
	pMUTEX->mutexFlag=0;
	OSExitExclusiveMode();
	return TRUE;
}

/*******************************************************
*
* Function name :MutexUnlock
* Description        :加锁
* Parameter         :
*        @pMUTEX        信号量对象指针 
*        @mutexWaitDelay        等待时间 
* Return          : TRUE or FALSE
**********************************************************/
uint32 MutexLock(PMUTEX_CB pMUTEX,uint32 mutexWaitDelay){
	if(pMUTEX==NULL){return FALSE;}
	if(mutexWaitDelay==MUTEX_WAIT_ENDLESS){
		while(pMUTEX->mutexFlag);
		OSEnterExclusiveMode();
		pMUTEX->mutexFlag=1;
		OSExitExclusiveMode();
		return TRUE;
	}else{
		if(mutexWaitDelay!=0){
			OSTaskDelay(mutexWaitDelay);
		}
		if(pMUTEX->mutexFlag == 0){
			OSEnterExclusiveMode();
			pMUTEX->mutexFlag=1;
			OSExitExclusiveMode();
			return TRUE;
		}
	}
	return FALSE;
}
