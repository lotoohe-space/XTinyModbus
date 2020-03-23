#ifndef _MD_RTU_REGCOIL_H__
#define _MD_RTU_REGCOIL_H__
#include "MD_RTU_Type.h"

typedef enum{		
	BIT_TYPE=0,
	REG_TYPE=1
}AddrType;

typedef struct{
	uint16 		modbusAddr;	/*modbus的地址*/
	uint16*			modbusData;	/*存储的数据*/
	uint16		modbusDataSize;/*当addrType==BIT_TYPE,为bit数；当addrType==REG_TYPE时为寄存器个数*/
	AddrType	addrType;		/*地址类型*/
												/*MSB:该块被定义为使用标志位*/
}*PRegCoilItem,RegCoilItem;


uint8 RegCoilListAdd(void* obj,PRegCoilItem pRegCoilItem);
uint8 RegCoilListDel(void* obj,PRegCoilItem pRegCoilItem);
//#define REGCOIL_ITEM_USED_FLAG_SET(a) MDS_SET_BIT(((PRegCoilItem)(a))->addrType,7)
//#define REGCOIL_ITEM_USED_FLAG_CLR(a) MDS_CLR_BIT(((PRegCoilItem)(a))->addrType,7)
//#define REGCOIL_ITEM_USED_FLAG_GET(a) MDS_GET_BIT(((PRegCoilItem)(a))->addrType,7)

#endif
