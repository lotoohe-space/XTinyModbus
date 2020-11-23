/********************************************************************************
* @File name: MD_RTU_Queue.c
* @Author: zspace
* @Emial: 1358745329@qq.com
* @Version: 1.0
* @Date: 2020-4-10
* @Description: Modbus RTU Queue function module
********************************************************************************/
#ifndef _SQQUEUE_H__
#define _SQQUEUE_H__

/*********************************HEAD FILE************************************/
#include "MD_RTU_Type.h"
#include "MD_RTU_Config.h"
/*********************************END******************************************/

/*********************************TYPE DEFINITION**************************************/
//typedef uint8 MDQueueDateType;
typedef void MDQueueDataType;
typedef struct {
	//MDQueueDateType data[MD_RTU_QUEUE_SIZE+1];
	MDQueueDataType *data;
	uint16 maxVal;
	int16 front, rear;
	uint8 valid;
}*PMDSqQueue, MDSqQueue;
/*********************************END******************************************/

/*user used data type.*/
typedef enum{
	CHAR_TYPE=0,
	UINT8_TYPE,
	SHORT_TYPE,
	INT_TYPE,
	FLOAT_TYPE
}*PSqQueueType,SqQueueType;

/*********************************FUNCTION DECLARATION**************************************/
uint8 MDInitQueue(MDSqQueue *q,MDQueueDataType* data,uint16 maxVal);
uint16 MDQueueLength(MDSqQueue* q);
void 	MDResetQueue(MDSqQueue* q);
uint8 MDQueueEmpty(MDSqQueue* q);
uint8 MDenQueueH(MDSqQueue* q, MDQueueDataType *e,SqQueueType type);
uint8 MDenQueue(MDSqQueue* q, MDQueueDataType *e,SqQueueType type);
uint8 MDdeQueue(MDSqQueue* q, MDQueueDataType *e,SqQueueType type);
uint8 MDdeQueueF(MDSqQueue* q, MDQueueDataType *e,SqQueueType type);
uint8 MDgetTailQueue(MDSqQueue* q, MDQueueDataType* e,SqQueueType type);
/*********************************END******************************************/

#endif

