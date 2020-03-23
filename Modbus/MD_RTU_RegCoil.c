/**
* @file 		MD_RTU_RegCoil.c
* @brief		нч
* @details	нч
* @author		zspace
* @date		2020/3/23
* @version	A001
* @par History:  нч       
*/
#include "MD_RTU_RegCoil.h"
#include "MD_RTU_Fun.h"

uint8 RegCoilListAdd(void* obj,PRegCoilItem pRegCoilItem){
	uint16 i=0;
	PModbusS_RTU pModbusRTU=obj;
	if(!obj){return FALSE;}
	for(i=0;i<REG_COIL_ITEM_NUM;i++){
		if(pModbusRTU->pRegCoilList[i]==NULL){
			pModbusRTU->pRegCoilList[i]=pRegCoilItem;
			return TRUE;
		}
	}
	return FALSE;
}
uint8 RegCoilListDel(void* obj,PRegCoilItem pRegCoilItem){
	uint16 i=0;
	PModbusS_RTU pModbusRTU=obj;
	if(!obj){return FALSE;}
	for(i=0;i<REG_COIL_ITEM_NUM;i++){
		if(pModbusRTU->pRegCoilList[i]==pRegCoilItem){
			pModbusRTU->pRegCoilList[i]=NULL;
			return TRUE;
		}
	}
	return FALSE;
}



