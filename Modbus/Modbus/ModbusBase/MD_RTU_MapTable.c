/**@file  MD_RTU_MapTable.c
* @brief       Modbus RTU Discrete mapping management module
* @author      zspace
* @date        2020-4-10
* @version     V1.0
**********************************************************************************
* @par Open source address
*	https://github.com/lotoohe-space/XTinyModbus
* @par modify log:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020-4-10  <td>1.0      <td>zspace  		<td>First version
* </table>
*
**********************************************************************************
*/
/*********************************HEAD FILE************************************/
#include "MD_RTU_MapTable.h"
/*********************************END******************************************/

/**
* @brief Add a mapping to master or slave
* @param[in] obj	Slave or host object pointer
* @param[in] pRegCoilItem    Slave or host object pointer
* @return	
* - TRUE success 
* - FALSE fail
*/
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

/**
* @brief Delete a mapping to master or slave
* @param[in] obj	Slave or host object pointer
* @param[in] pRegCoilItem    A mapping deleted,reference @see :: PMapTableItem
* @return	
* - TRUE success 
* - FALSE fail
*/
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
