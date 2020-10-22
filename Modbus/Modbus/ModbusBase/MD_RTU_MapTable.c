/********************************************************************************
* @File name: MD_RTU_RegCoil.c
* @Author: zspace
* @Emial: 1358745329@qq.com
* @Version: 1.0
* @Date: 2020-4-10
* @Description: Modbus RTU Discrete mapping management module
* Open source address: https://github.com/lotoohe-space/XTinyModbus
********************************************************************************/

/*********************************HEAD FILE************************************/
#include "MD_RTU_MapTable.h"
/*********************************END******************************************/

/*******************************************************
*
* Function name :RegCoilListAdd
* Description        :Add a mapping to master or slave
* Parameter         :
*        @obj       	Slave or host object pointer
*        @pRegCoilItem    A mapping added,reference[PRegCoilItem]
* Return          : TRUE success , FALSE fail
**********************************************************/
uint8 MapTableAdd(void* obj,PMapTableItem pMapTableItem,uint16 tabSize){
	uint16 i=0;
	PMapTableItem *pMapTableList=obj;
	if(!obj){return FALSE;}
	for(i=0;i<tabSize;i++){
		if(pMapTableList[i]==NULL){
			pMapTableList[i]=pMapTableItem;
			return TRUE;
		}
	}
	return FALSE;
}
/*******************************************************
*
* Function name :RegCoilListDel
* Description        :Delete a mapping to master or slave
* Parameter         :
*        @obj       	Slave or host object pointer
*        @pRegCoilItem    A mapping deleted,reference[PRegCoilItem]
* Return          : TRUE success , FALSE fail
**********************************************************/
uint8 MapTableDel(void* obj,PMapTableItem pMapTableItem,uint16 tabSize){
	uint16 i=0;
	PMapTableItem *pMapTableList=obj;
	if(!obj){return FALSE;}
	for(i=0;i<tabSize;i++){
		if(pMapTableList[i]==pMapTableItem){
			pMapTableList[i]=NULL;
			return TRUE;
		}
	}
	return FALSE;
}



