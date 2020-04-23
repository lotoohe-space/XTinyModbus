/********************************************************************************
* @File name: MD_RTU_RegCoil.c
* @Author: zspace
* @Version: 1.0
* @Date: 2020-4-10
* @Description: Modbus RTU 离散映射管理模块
********************************************************************************/

/*********************************头文件包含************************************/
#include "MD_RTU_RegCoil.h"
/*********************************结束******************************************/

/*******************************************************
*
* Function name :RegCoilListAdd
* Description        :添加一个映射到主机或从机
* Parameter         :
*        @obj       	从机或者主机对象指针
*        @pRegCoilItem    添加某个 映射,参见[PRegCoilItem]
* Return          : TRUE success , FALSE fail
**********************************************************/
uint8 RegCoilListAdd(void* obj,PRegCoilItem pRegCoilItem,uint16 tabSize){
	uint16 i=0;
	PRegCoilItem *pRegCoilList=obj;
	if(!obj){return FALSE;}
	for(i=0;i<tabSize;i++){
		if(pRegCoilList[i]==NULL){
			pRegCoilList[i]=pRegCoilItem;
			return TRUE;
		}
	}
	return FALSE;
}
/*******************************************************
*
* Function name :RegCoilListDel
* Description        :删除一个映射到主机或从机
* Parameter         :
*        @obj       	从机或者主机对象指针
*        @pRegCoilItem    删除某个映射,参见[PRegCoilItem]
* Return          : TRUE success , FALSE fail
**********************************************************/
uint8 RegCoilListDel(void* obj,PRegCoilItem pRegCoilItem,uint16 tabSize){
	uint16 i=0;
	PRegCoilItem *pRegCoilList=obj;
	if(!obj){return FALSE;}
	for(i=0;i<tabSize;i++){
		if(pRegCoilList[i]==pRegCoilItem){
			pRegCoilList[i]=NULL;
			return TRUE;
		}
	}
	return FALSE;
}



