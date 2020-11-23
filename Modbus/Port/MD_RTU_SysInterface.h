#ifndef _MD_SYS_INTERFACE_H__
#define _MD_SYS_INTERFACE_H__
#include "MD_RTU_Config.h"

#if MD_RTU_USED_OS 

#include "MD_RTU_Type.h"
#include "MD_RTU_Tool.h"
#include "MD_RTU_Queue.h"

///< RTOS files
#include "RTOS_msg.h"
#include "RTOS_mutex.h"
#include "RTOS_task.h"

#define MD_RTU_LOCK_HANDLE 	PMUTEX_CB ///< Locked object name 
#define MD_RTU_MSG_HANDLE		PMSG_CB 	///< Msg oobject name
#define MD_RTU_TASK_HANDLE	PTASK_TCB 	///< task oobject name

#define MD_RTU_TASK_LEVEL 			6
#define MD_RTU_TASK_STACK_SIZE	128

#define MD_RTU_WAIT_UNLESS	0xFFFFFFFF ///< always waiting
#define MD_RTU_MSG_BOX_LENGTH 32

uint8 MD_RTU_CreateThread(PModbusBase pModbusBase,void(*funTask)(void* arg),MD_RTU_TASK_HANDLE *pTaskObj);
void  MD_RTU_Delay(uint32 ms);

uint8 MD_RTU_CreateMsg(PModbusBase pModbusBase, MD_RTU_MSG_HANDLE *pMsgObj,
	uint32 msgBoxLength
);
uint8 MD_RTU_MsgPut(PModbusBase pModbusBase, MD_RTU_MSG_HANDLE pMsgObj,void* msg,uint32 msgPutDelay);
uint8 MD_RTU_MsgGet(PModbusBase pModbusBase, MD_RTU_MSG_HANDLE pMsgObj,void **msg,uint32 msgGetDelay);

uint8 MD_RTU_CreateLock(PModbusBase pModbusBase, MD_RTU_LOCK_HANDLE *pLockObj);
uint8 MD_RTU_Lock(PModbusBase pModbusBase, MD_RTU_LOCK_HANDLE lockObj,uint32 mutexWaitDelay);
uint8 MD_RTU_Unlock(PModbusBase pModbusBase, MD_RTU_LOCK_HANDLE lockObj);

uint32 MD_RTU_GetSysTick(void);
#endif

#if MD_RTU_USED_OS
#define MD_RTU_LOCK(a,b) 				MD_RTU_Lock(a,b,MD_RTU_WAIT_UNLESS)
#define MD_RTU_UNLOCK(a,b) 				MD_RTU_Unlock(a,b)
#define MD_RTU_MSG_PUT(a,b,c,d) 	MD_RTU_MsgPut(a,b,c,d)
#define MD_RTU_MSG_GET(a,b,c,d) 	MD_RTU_MsgGet(a,b,c,d)
#else 
#define MD_RTU_LOCK(a,b)
#define MD_RTU_UNLOCK(a,b)
#define MD_RTU_MSG_PUT(a,b,c,d)
#define MD_RTU_MSG_GET(a,b,c,d)
#endif 

#endif
