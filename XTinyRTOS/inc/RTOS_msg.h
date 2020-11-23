#ifndef _RTOS_MSG_H__
#define _RTOS_MSG_H__
#ifdef __cplusplus
 extern "C" {
#endif 
/*********************************头文件包含************************************/
#include "RTOS_type.h"
/*********************************结束******************************************/

/*********************************配置宏************************************/
#define MSG_MAX_LEN 32		/*消息的最大长度*/
/*********************************结束******************************************/

/*********************************宏定义************************************/
#define MSG_WAIT_ENDLESS 0xFFFFFFFF
/*********************************结束******************************************/

/*********************************自定义类型************************************/
typedef struct {
	/*消息id*/
	uint32 		msgID;
	/*队首指针*/
	int32 		front;
	int32 		rear;
	/*消息,存放的指针*/
	void* 		msg[MSG_MAX_LEN];
}*PMSG_CB,MSG_CB;
/*********************************结束******************************************/

/*********************************函数申明************************************/
PMSG_CB MsgCreate(void);
void 		MsgFree(PMSG_CB mTCB_MSG);

uint32 	MsgGet(PMSG_CB mTCB_MSG,void **msg,uint32 msgGetDelay);
uint32 	MsgPut(PMSG_CB mTCB_MSG,void* msg,uint32 msgGetDelay);

BOOL 		MsgIsEmpty(PMSG_CB pMsgCB);
BOOL 		MsgIsFull(PMSG_CB pMsgCB);
uint32 	MsgLen(PMSG_CB pMsgCB);
/*********************************结束******************************************/
#ifdef __cplusplus
}
#endif
#endif
