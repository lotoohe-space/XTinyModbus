/********************************************************************************
* @File name: MD_RTU_Queue.c
* @Author: zspace
* @Emial: 1358745329@qq.com
* @Version: 1.0
* @Date: 2020-4-10
* @Description: Modbus RTU Queue function module
* Open source address: https://github.com/lotoohe-space/XTinyModbus
********************************************************************************/

/*********************************HEAD FILE************************************/
#include "MD_RTU_Queue.h"
/*********************************END******************************************/

/*******************************************************
*
* Function name: MDInitQueue
* Description        :Initialize a queue
* Parameter         :
*        @q        Queue object pointer   
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
* Description        :Get the length of the data in the queue
* Parameter         :
*        @q        Queue object pointer 
* Return          : The length of the data in the queue.
**********************************************************/
uint16 MDQueueLength(MDSqQueue* q) {
	if (q == NULL) { return 0; }
	return (q->rear - q->front + q->maxVal) % q->maxVal;
}
/*******************************************************
*
* Function name: MDResetQueue
* Description :Queue reset (cleared)
* Parameter :
*        @q         Queue object pointer  
* Return : None
**********************************************************/
void MDResetQueue(MDSqQueue* q) {
	if (q == NULL) { return ; }
	q->front = q->rear = 0;
}
/*******************************************************
*
* Function name: MDQueueEmpty
* Description        :Whether the queue is empty
* Parameter         :
*        @q       Queue object pointer 
* Return          : TRUE empty , FALSE Not empty
**********************************************************/
uint8 MDQueueEmpty(MDSqQueue* q) {
	if (q == NULL) { return 1; }
	return (q->front == q->rear);
}
/*******************************************************
*
* Function name: MDenQueue
* Description        :Insert at the end of the queue
* Parameter         :
*        @q        Queue object pointer    
*        @e        Inserted element 
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
* Description        :Queue head insertion
* Parameter         :
*        @q        Queue object pointer   
*        @e        Inserted element 
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
* Description        :Queue head out of the queue
* Parameter         :
*        @q        Queue object pointer   
*        @e        Obtained elements 
* Return          : TRUE success , FALSE fail
**********************************************************/
uint8 MDdeQueue(MDSqQueue* q, MDQueueDateType *e) {
	if (q->front == q->rear) { /*Empty, return an error*/
		return FALSE;
	}
	q->front = (q->front + 1) % q->maxVal;
	*e = q->data[q->front];
	return TRUE;
}
/*******************************************************
*
* Function name: MDdeQueueF
* Description        :Queue tail out
* Parameter         :
*        @q        Queue object pointer   
*        @e        Obtained elements 
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
* Description        :Get the element at the end of the queue without affecting the queue, only the element
* Parameter         :
*        @q        Queue object pointer    
*        @e        Obtained elements 
* Return          : TRUE success , FALSE fail
**********************************************************/
uint8 MDgetTailQueue(MDSqQueue* q, MDQueueDateType* e) {
	if (q->front == q->rear) { /*Empty, return an error*/
		return FALSE;
	}
	*e = q->data[(q->front + 1) % q->maxVal];
	return TRUE;
}


