/********************************************************************************
* @File name: RTOS_msg.c
* @Author: zspace
* @Version: 1.0
* @Date: 2020-4-10
* @Description: RTOS 操作系统的消息邮箱的实现的代码
********************************************************************************/

/*********************************头文件包含************************************/
#include "RTOS_msg.h"
#include "RTOS_mem.h"
#include "RTOS_task.h"
/*********************************结束******************************************/

/*******************************************************
*
* Function name :MsgCreate
* Description        :创建一个消息队列,内存申请失败则返回错误. 
* Parameter         :无
* Return          : 返回NULL失败
**********************************************************/
PMSG_CB MsgCreate(void){
	PMSG_CB pMsgCB=OSMalloc(sizeof(MSG_CB));
	if(pMsgCB==NULL){return NULL;}
	pMsgCB->msgID=(uint32)pMsgCB;
	pMsgCB->front=pMsgCB->rear=0;
	return pMsgCB;
}
/*******************************************************
*
* Function name :MsgFree
* Description        : 释放一个消息队列
* Parameter         :
*        @pMsgCB        消息邮箱对象指针 
* Return          : 无
**********************************************************/
void MsgFree(PMSG_CB pMsgCB){
	OSFree(pMsgCB);
}
/*******************************************************
*
* Function name :MsgEmpty
* Description        : 消息是否为空
* Parameter         :
*        @pMsgCB        消息邮箱对象指针 
* Return          : 无
**********************************************************/
BOOL MsgIsEmpty(PMSG_CB pMsgCB){
	return (pMsgCB->front==pMsgCB->rear);
}
/*******************************************************
*
* Function name :MsgIsFull
* Description        : 消息是否满
* Parameter         :
*        @pMsgCB        消息邮箱对象指针 
* Return          : 无
**********************************************************/
BOOL MsgIsFull(PMSG_CB pMsgCB){
	return ((pMsgCB->rear + 1) % MSG_MAX_LEN == pMsgCB->front);
}
/*******************************************************
*
* Function name :MsgGet
* Description        :从消息队列中获取消息
* Parameter         :
*        @pMsgCB        要获取的消息的消息控制块    
*        @msg    要获取的消息存放地址
*        @msgGetDelay    获取消息的超时时间
* Return          : TRUE success , FALSE fail
**********************************************************/
uint32 MsgGet(PMSG_CB pMsgCB,void **msg,uint32 msgGetDelay){
	if(pMsgCB==NULL){return FALSE;}
	
	if(msgGetDelay==MSG_WAIT_ENDLESS){
		while(MsgIsEmpty(pMsgCB)==TRUE);
		OSEnterExclusiveMode();
		pMsgCB->front=(pMsgCB->front+1) % MSG_MAX_LEN;
		*msg =pMsgCB->msg[pMsgCB->front];
		OSExitExclusiveMode();
		return TRUE;
	} else {
		if(msgGetDelay!=0){
			OSTaskDelay(msgGetDelay);
		}
		if(MsgIsEmpty(pMsgCB)!=TRUE){
			OSEnterExclusiveMode();
			pMsgCB->front=(pMsgCB->front+1) % MSG_MAX_LEN;
			*msg =pMsgCB->msg[pMsgCB->front];
			OSExitExclusiveMode();
			return TRUE;
		}
	}
	return FALSE;
}
/*******************************************************
*
* Function name :MsgPut
* Description        :存放消息
* Parameter         :
*        @pMsgCB        要发送的消息的消息控制块    
*        @msg    要发送的消息存放地址
*        @msgGetDelay    发送消息的超时时间
* Return          : TRUE success , FALSE fail
**********************************************************/
uint32 MsgPut(PMSG_CB pMsgCB,void* msg,uint32 msgGetDelay){
	if(pMsgCB==NULL){return FALSE;}
	
	if(msgGetDelay==MSG_WAIT_ENDLESS){
		while(MsgIsFull(pMsgCB)==TRUE);
		OSEnterExclusiveMode();
		pMsgCB->rear=(pMsgCB->rear+1)%MSG_MAX_LEN;
		pMsgCB->msg[pMsgCB->rear]=msg;
		OSExitExclusiveMode();	
		return TRUE;
	}else{
		if(msgGetDelay!=0){
			OSTaskDelay(msgGetDelay);
		}
		if(MsgIsFull(pMsgCB)==FALSE){
			OSEnterExclusiveMode();
			pMsgCB->rear=(pMsgCB->rear+1)%MSG_MAX_LEN;
			pMsgCB->msg[pMsgCB->rear]=msg;
			OSExitExclusiveMode();
			return TRUE;
		}
	}
	return FALSE;
}
/*******************************************************
*
* Function name :MsgLen
* Description        :获取消息的长度
* Parameter         :
*        @pMsgCB        消息控制块  
* Return          : 消息队列长度
**********************************************************/
uint32 MsgLen(PMSG_CB pMsgCB){
	return (pMsgCB->rear - pMsgCB->front + MSG_MAX_LEN)%MSG_MAX_LEN;
}
