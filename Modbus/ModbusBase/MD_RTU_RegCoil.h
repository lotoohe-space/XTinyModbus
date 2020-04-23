/********************************************************************************
* @File name: MD_RTU_RegCoil.h
* @Author: zspace
* @Version: 1.0
* @Date: 2020-4-10
* @Description: Modbus RTU 离散映射管理模块
********************************************************************************/
#ifndef _MD_RTU_REGCOIL_H__
#define _MD_RTU_REGCOIL_H__
/*********************************头文件包含************************************/
#include "MD_RTU_Type.h"
/*********************************结束******************************************/

/*********************************自定义类型************************************/
typedef enum{		
	/*类型*/
	COILS_TYPE=1,
	INPUT_TYPE=2,
	HOLD_REGS_TYPE=3,
	INPUT_REGS_TYPE=4
}AddrType;
typedef struct{
	uint16 		modbusAddr;			/*modbus的地址*/
	uint16*			modbusData;		/*存储的数据*/
	uint16		modbusDataSize;	/*参见[AddrType]*/
	AddrType	addrType;				/*地址类型*/
}*PRegCoilItem,RegCoilItem;
/*********************************结束******************************************/

/*********************************函数申明************************************/
uint8 RegCoilListAdd(void* obj,PRegCoilItem pRegCoilItem,uint16 tabSize);
uint8 RegCoilListDel(void* obj,PRegCoilItem pRegCoilItem,uint16 tabSize);
/*********************************结束******************************************/

#endif
