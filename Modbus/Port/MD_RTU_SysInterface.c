#include "MD_RTU_SysInterface.h"
#include "tim3.h"

#if MD_RTU_USED_OS

uint8 MD_RTU_CreateThread(PModbusBase pModbusBase,void(*funTask)(void* arg),MD_RTU_TASK_HANDLE *pTaskObj){
	if(pModbusBase==NULL){
		return FALSE;
	}
	*pTaskObj=OSCreateTask(funTask,pModbusBase,MD_RTU_TASK_LEVEL,MD_RTU_TASK_STACK_SIZE);
	return (*pTaskObj)!=NULL;;
}
void MD_RTU_Delay(uint32 ms){
	OSTaskDelay(ms);
}
uint8 MD_RTU_CreateMsg(PModbusBase pModbusBase, MD_RTU_MSG_HANDLE *pMsgObj,
	uint32 msgBoxLength
){
	if(pModbusBase==NULL){
		return FALSE;
	}
	*pMsgObj=MsgCreate();
	return (*pMsgObj)!=NULL;
}
uint8 MD_RTU_MsgPut(PModbusBase pModbusBase, MD_RTU_MSG_HANDLE pMsgObj,void* msg,uint32 msgPutDelay){
	return MsgPut(pMsgObj,msg,msgPutDelay);
}
uint8 MD_RTU_MsgGet(PModbusBase pModbusBase, MD_RTU_MSG_HANDLE pMsgObj,void **msg,uint32 msgGetDelay){
	return MsgGet(pMsgObj,msg,msgGetDelay);
}
/**
* @brief Create the locked object
* @param[in] pModbusBase modbus object
* @param[in] pLockObj Lock object pointer
* @return 
* - TRUE success
* - FALSE fail
*/
uint8 MD_RTU_CreateLock(PModbusBase pModbusBase, MD_RTU_LOCK_HANDLE *pLockObj){
	if(pModbusBase==NULL){
		return FALSE;
	}
	*pLockObj=MutexCreate();
	return (*pLockObj)!=NULL;
}
/**
* @brief Lock the modbus rtu object
* @param[in] pModbusBase modbus object
* @param[in] lockObj lock Object
* @return 
* - TRUE success
* - FALSE fail
*/
uint8 MD_RTU_Lock(PModbusBase pModbusBase, MD_RTU_LOCK_HANDLE lockObj,uint32 mutexWaitDelay){
	return MutexLock(lockObj,mutexWaitDelay);
}
/**
* @brief Unlock the modbus rtu object
* @param[in] pModbusBase modbus object
* @param[in] lockObj lock Object
* @return 
* - TRUE success
* - FALSE fail
*/
uint8 MD_RTU_Unlock(PModbusBase pModbusBase, MD_RTU_LOCK_HANDLE lockObj){
	return MutexUnlock(lockObj);
}
/**
* @brief Modbus RTU get system tick (100us).
* @return system tick.
*/
uint32 MD_RTU_GetSysTick(void){
	return sys_tick_100us;
}
#endif

