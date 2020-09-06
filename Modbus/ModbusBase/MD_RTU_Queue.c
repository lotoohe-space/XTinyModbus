/********************************************************************************
* @File name: MD_RTU_Queue.c
* @Author: zspace
* @Emial: 1358745329@qq.com
* @Version: 1.0
* @Date: 2020-4-10
* @Description: Modbus RTU 队列功能模块
* 开源地址: https://github.com/lotoohe-space/XTinyModbus
********************************************************************************/

/*********************************头文件包含************************************/
#include "MD_RTU_Queue.h"
/*********************************结束******************************************/

/*******************************************************
*
* Function name: MDInitQueue
* Description        :初始化一个队列
* Parameter         :
*        @q        队列结构体指针   
* Return          : TRUE success , FALSE fail
**********************************************************/
uint8 MDInitQueue(MDSqQueue *q){
//	MDQueueDateType* data;
	if (q == NULL) { return FALSE; }
	q->maxVal = MD_RTU_QUEUE_SIZE+1;
	q->front = q->rear = 0;
	q->valid = TRUE;
	return TRUE;
}
/*******************************************************
*
* Function name :MDQueueLength
* Description        :获取队列中数据的长度
* Parameter         :
*        @q        队列结构体指针 
* Return          : 队列中数据的长度
**********************************************************/
uint16 MDQueueLength(MDSqQueue* q) {
	if (q == NULL) { return 0; }
	return (q->rear - q->front + q->maxVal) % q->maxVal;
}
/*******************************************************
*
* Function name: MDResetQueue
* Description        :队列复位 （清零）
* Parameter         :
*        @q        队列结构体指针   
* Return          : 无
**********************************************************/
void MDResetQueue(MDSqQueue* q) {
	if (q == NULL) { return ; }
	q->front = q->rear = 0;
}
/*******************************************************
*
* Function name: MDQueueEmpty
* Description        :队列是否为空
* Parameter         :
*        @q        队列结构体指针   
* Return          : TRUE 空 , FALSE 不空
**********************************************************/
uint8 MDQueueEmpty(MDSqQueue* q) {
	if (q == NULL) { return 1; }
	return (q->front == q->rear);
}
/*******************************************************
*
* Function name: MDenQueue
* Description        :队列尾部插入
* Parameter         :
*        @q        队列结构体指针   
*        @e        插入的元素 
* Return          : TRUE success , FALSE fail
**********************************************************/
uint8 MDenQueue(MDSqQueue* q, MDQueueDateType e) {
	if ((q->rear + 1) % q->maxVal == q->front) {
		return FALSE;
	}
	q->rear = (q->rear + 1) % q->maxVal;
	q->data[q->rear] = e;
	return TRUE;
}
/*******************************************************
*
* Function name: MDenQueueH
* Description        :队列头部插入
* Parameter         :
*        @q        队列结构体指针   
*        @e        插入的元素 
* Return          : TRUE success , FALSE fail
**********************************************************/
uint8 MDenQueueH(MDSqQueue* q, MDQueueDateType e){		
	if((q->front - 1 + q->maxVal) % q->maxVal == q->rear)
			return FALSE;
	
	q->data[q->front] = e;
	q->front = (q->front - 1 + q->maxVal) % q->maxVal;
	return TRUE;
}
/*******************************************************
*
* Function name: MDdeQueue
* Description        :队列头部出队列
* Parameter         :
*        @q        队列结构体指针   
*        @e        获取到的元素 
* Return          : TRUE success , FALSE fail
**********************************************************/
uint8 MDdeQueue(MDSqQueue* q, MDQueueDateType *e) {
	if (q->front == q->rear) { /*空了，则返回错误*/
		return FALSE;
	}
	q->front = (q->front + 1) % q->maxVal;
	*e = q->data[q->front];
	return TRUE;
}
/*******************************************************
*
* Function name: MDdeQueueF
* Description        :队列尾部出队列
* Parameter         :
*        @q        队列结构体指针   
*        @e        获取到的元素 
* Return          : TRUE success , FALSE fail
**********************************************************/
uint8 MDdeQueueF(MDSqQueue* q, MDQueueDateType *e) {
	if(q->front == q->rear){
			 return FALSE;	
	}
	*e = q->data[q->rear];
	q->rear = (q->rear - 1 + q->maxVal) % q->maxVal;
	return TRUE;
}
/*******************************************************
*
* Function name: MDgetTailQueue
* Description        :获取队列尾部元素，不影响队列，只取元素
* Parameter         :
*        @q        队列结构体指针   
*        @e        获取到的元素 
* Return          : TRUE success , FALSE fail
**********************************************************/
uint8 MDgetTailQueue(MDSqQueue* q, MDQueueDateType* e) {
	if (q->front == q->rear) { /*空了，则返回错误*/
		return FALSE;
	}
	*e = q->data[(q->front + 1) % q->maxVal];
	return TRUE;
}


