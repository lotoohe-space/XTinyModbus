/********************************************************************************
* @File name: RTOS_sem.c
* @Author: zspace
* @Version: 1.0
* @Date: 2020-4-10
* @Description: RTOS 信号量模块
********************************************************************************/

/*********************************头文件包含************************************/
#include "RTOS_sem.h"
#include "RTOS_core.h"
#include "RTOS_task.h"
#include "RTOS_mem.h"
/*********************************结束******************************************/

/*******************************************************
*
* Function name :SemCreate
* Description        :创建一个信号量
* Parameter         :
*        @initVal   信号量初始值    
*        @maxVal    信号量最大值
* Return          : 失败返回NULL，否则返回信号量对象
**********************************************************/
PSEM_CB SemCreate(uint32 initVal,uint32 maxVal){
	PSEM_CB pSEM=(PSEM_CB)OSMalloc(sizeof(SEM_CB));
	if(pSEM==NULL){return NULL;}
	pSEM->semVal=initVal;
	pSEM->semMaxVal=maxVal;
	return pSEM;
}

/*******************************************************
*
* Function name :SemFree
* Description        :释放一个信号量
* Parameter         :
*        @PSEM_CB   信号量对象指针
* Return          : 无
**********************************************************/
void SemFree(PSEM_CB pSEM){
	OSFree(pSEM);
}
/*******************************************************
*
* Function name :		SemPost
* Description        :发送一个信号量
* Parameter         :
*        @PSEM_CB   信号量对象指针
* Return          : TRUE or FALSE
**********************************************************/
uint32 SemPost(PSEM_CB pSEM){
	if(pSEM == NULL){ return FALSE; }
	OSEnterExclusiveMode();
	pSEM->semVal++;
	if(pSEM->semVal>pSEM->semMaxVal){
		/*限制不超过最大值*/
		pSEM->semVal=pSEM->semMaxVal;
	}
	OSExitExclusiveMode();
	return TRUE;
}
/*******************************************************
*
* Function name :		SemPend
* Description        :接收一个信号量
* Parameter         :
*        @PSEM_CB   信号量对象指针
*        @semWaitDelay   等待时间
* Return          : TRUE or FALSE
**********************************************************/
uint32 SemPend(PSEM_CB pSEM,uint32 semWaitDelay){
	if(pSEM==NULL){return FALSE;}

	if(semWaitDelay==SEM_WAIT_ENDLESS){
		/*一直等*/
		while(pSEM->semVal==0);
		OSEnterExclusiveMode();
		pSEM->semVal--;
		OSExitExclusiveMode();
		return TRUE;
	}else{
		/*延时后在查看*/
		if(semWaitDelay!=0){
			OSTaskDelay(semWaitDelay);
		}
		if(pSEM->semVal!=0){
			/*有信号量*/
			OSEnterExclusiveMode();
			pSEM->semVal--;
			OSExitExclusiveMode();
			return TRUE;
		}
	}
	return FALSE;
}

