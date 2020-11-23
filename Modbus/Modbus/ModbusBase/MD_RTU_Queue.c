/**
* @file MD_RTU_Queue.c
* @author zspace
* @emial 1358745329@qq.com
* @version 1.0
* @date 2020-4-10
* @brief Modbus RTU Queue function module
* Open source address: https://github.com/lotoohe-space/XTinyModbus
*/

/*********************************HEAD FILE************************************/
#include "MD_RTU_Queue.h"
/*********************************END******************************************/

/**
* @brief Initialize a queue
* @param[in] q Queue object pointer   
* @param[in] data	Queue cache data  
* @maxVal[in] Queue length
* @return 
* - TRUE success
* - FALSE fail
*/
uint8 MDInitQueue(MDSqQueue *q,MDQueueDataType* data,uint16 maxVal){
	if (q == NULL) { return FALSE; }
	q->data=data;
	q->maxVal = maxVal;
	q->front = q->rear = 0;
	q->valid = TRUE;
	return TRUE;
}
/**
* @brief Get the length of the data in the queue
* @param[in] q Queue object pointer   
* @return The length of valid data in the queue
*/
uint16 MDQueueLength(MDSqQueue* q) {
	if (q == NULL) { return 0; }
	return (q->rear - q->front + q->maxVal) % q->maxVal;
}
/**
* @brief Queue reset (cleared)
* @param q Queue object pointer  
* @return None
*/
void MDResetQueue(MDSqQueue* q) {
	if (q == NULL) { return ; }
	q->front = q->rear = 0;
}
/**
* @brief Whether the queue is empty
* @param q Queue object pointer 
* @return 
* - TRUE empty
* - FALSE Not empty
*/
uint8 MDQueueEmpty(MDSqQueue* q) {
	if (q == NULL) { return 1; }
	return (q->front == q->rear);
}
/**
* @brief Insert at the end of the queue
* @param[in] q  Queue object pointer    
* @param[in] e	Inserted element 
* @return
* - TRUE success
* - FALSE fail
*/
uint8 MDenQueue(MDSqQueue* q, MDQueueDataType *e,SqQueueType type) {
	if ((q->rear + 1) % q->maxVal == q->front) {
		return FALSE;
	}
	q->rear = (q->rear + 1) % q->maxVal;
	switch(type){
		case CHAR_TYPE:
		((char*)(q->data))[q->rear] = *((char*)e);
		break;
		case UINT8_TYPE:
		((uint8*)(q->data))[q->rear] = *((uint8*)e);
		break;
		case SHORT_TYPE:
		((short*)(q->data))[q->rear] = *((short*)e);
		break;
		case INT_TYPE:
		((int*)(q->data))[q->rear] = *((int*)e);
		break;
		case FLOAT_TYPE:
		((float*)(q->data))[q->rear] = *((float*)e);
		break;
	}
	
	return TRUE;
}
/**
* @brief Queue head insertion
* @param[in]  q	Queue object pointer   
* @param[in]  e	Inserted element 
* @return 
* - TRUE success
* - FALSE fail
*/
uint8 MDenQueueH(MDSqQueue* q, MDQueueDataType *e,SqQueueType type){		
	if((q->front - 1 + q->maxVal) % q->maxVal == q->rear)
			return FALSE;

	switch(type){
		case CHAR_TYPE:
		((char*)(q->data))[q->front] = *((char*)e);
		break;
		case UINT8_TYPE:
		((uint8*)(q->data))[q->front] = *((uint8*)e);
		break;
		case SHORT_TYPE:
		((short*)(q->data))[q->front] = *((short*)e);
		break;
		case INT_TYPE:
		((int*)(q->data))[q->front] = *((int*)e);
		break;
		case FLOAT_TYPE:
		((float*)(q->data))[q->front] = *((float*)e);
		break;
	}
	q->front = (q->front - 1 + q->maxVal) % q->maxVal;
	return TRUE;
}
/**
* @brief Queue head out of the queue
* @param[in]	q Queue object pointer   
* @param[in]	e	Obtained elements 
* @return
* - TRUE success
* - FALSE fail
*/
uint8 MDdeQueue(MDSqQueue* q, MDQueueDataType *e,SqQueueType type) {
	if (q->front == q->rear) { /*Empty, return an error*/
		return FALSE;
	}
	q->front = (q->front + 1) % q->maxVal;
	
	switch(type){
		case CHAR_TYPE:
		*((char*)e)=((char*)(q->data))[q->front];
		break;
		case UINT8_TYPE:
		*((uint8*)e)=((uint8*)(q->data))[q->front];
		break;
		case SHORT_TYPE:
		*((short*)e)=((short*)(q->data))[q->front];
		break;
		case INT_TYPE:
		*((int*)e)=((int*)(q->data))[q->front];
		break;
		case FLOAT_TYPE:
		*((int*)e)=((float*)(q->data))[q->front];
		break;
	}
	return TRUE;
}
/**
* @brief	Queue tail out
* @param[in] q        Queue object pointer   
* @param[in] e        Obtained elements 
* @return
* - TRUE success
* - FALSE fail
*/
uint8 MDdeQueueF(MDSqQueue* q, MDQueueDataType *e,SqQueueType type) {
	if(q->front == q->rear){
			 return FALSE;	
	}
	switch(type){
		case CHAR_TYPE:
		*((char*)e)=((char*)(q->data))[q->rear];
		break;
		case UINT8_TYPE:
		*((uint8*)e)=((uint8*)(q->data))[q->rear];
		break;
		case SHORT_TYPE:
		*((short*)e)=((short*)(q->data))[q->rear];
		break;
		case INT_TYPE:
		*((int*)e)=((int*)(q->data))[q->rear];
		break;
		case FLOAT_TYPE:
		*((int*)e)=((float*)(q->data))[q->rear];
		break;
	}
	q->rear = (q->rear - 1 + q->maxVal) % q->maxVal;
	return TRUE;
}
/**
* @brief Get the element at the end of the queue without affecting the queue, only the element
* @param[in] q        Queue object pointer    
* @param[in] e        Obtained elements 
* @return
* - TRUE succes
* - FALSE fail
*/
uint8 MDgetTailQueue(MDSqQueue* q, MDQueueDataType* e,SqQueueType type) {
	if (q->front == q->rear) { /*Empty, return an error*/
		return FALSE;
	}
	
	switch(type){
		case CHAR_TYPE:
		*((char*)e)=((char*)(q->data))[(q->front + 1) % q->maxVal];
		break;
		case UINT8_TYPE:
		*((uint8*)e)=((uint8*)(q->data))[(q->front + 1) % q->maxVal];
		break;
		case SHORT_TYPE:
		*((short*)e)=((short*)(q->data))[(q->front + 1) % q->maxVal];
		break;
		case INT_TYPE:
		*((int*)e)=((int*)(q->data))[(q->front + 1) % q->maxVal];
		break;
		case FLOAT_TYPE:
		*((int*)e)=((float*)(q->data))[(q->front + 1) % q->maxVal];
		break;
	}
	return TRUE;
}


