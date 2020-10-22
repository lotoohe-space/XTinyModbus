/********************************************************************************
* @File name: MD_RTU_RegCoil.h
* @Author: zspace
* @Emial: 1358745329@qq.com
* @Version: 1.0
* @Date: 2020-4-10
* @Description: Modbus RTU Discrete mapping management module
********************************************************************************/
#ifndef _MD_RTU_REGCOIL_H__
#define _MD_RTU_REGCOIL_H__
/*********************************HEAD FILE************************************/
#include "MD_RTU_Type.h"
/*********************************END******************************************/
	
/*********************************CUSTOM DATA TYPE************************************/
typedef enum{		
	/*¿‡–Õ*/
	COILS_TYPE=1,
	INPUT_TYPE=2,
	HOLD_REGS_TYPE=3,
	INPUT_REGS_TYPE=4
}AddrType;

typedef struct{
	uint16 		modbusAddr;			/*Modbus address*/
	uint16*		modbusData;			/*Stored data*/
	uint16		modbusDataSize;	/*Map size*/
	AddrType	addrType;				/*Address type,Reference[AddrType]*/
	uint8			devAddr;				/*Indicate which device the mapping item belongs to. For the time being, only the master uses this variable and the slave ignores it.*/
}*PMapTableItem,MapTableItem;
/*********************************END******************************************/

/*********************************FUNCTION DECLARATION************************************/
uint8 MapTableAdd(void* obj,PMapTableItem pMapTableItem,uint16 tabSize);
uint8 MapTableDel(void* obj,PMapTableItem pMapTableItem,uint16 tabSize);
/*********************************END******************************************/

#endif
