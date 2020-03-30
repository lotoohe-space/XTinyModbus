#ifndef _MD_RTU_TOOL_H__
#define _MD_RTU_TOOL_H__

#include "MD_RTU_Type.h"

typedef	void (*MDS_RTU_TimeHandlerFunction)(void* obj,uint32 times);
typedef void (*MDS_RTU_SendBytesFunction)(uint8 *byte,uint16 num);
typedef void (*MDS_RTU_RecByteFunction)(void* obj, uint8 byte);
typedef void (*MDS_RTU_RecSendConv)(uint8 mode);

typedef void (*MD_RTU_SerialInit)(void* obj,uint32 baud,uint8 dataBits,uint8 stopBit,uint8 parity);

typedef struct{
	/*数据发送接收有关的函数*/
	MDS_RTU_SendBytesFunction				mdRTUSendBytesFunction;
	MDS_RTU_RecByteFunction					mdRTURecByteFunction;
	MDS_RTU_RecSendConv							mdRTURecSendConv;
	/*定时器回调函数*/
	MDS_RTU_TimeHandlerFunction			mdRTUTimeHandlerFunction;		
}*PModbusBase,ModbusBase;

/*转换为ModbusBase*/
#define TO_MDBase(a)		((PModbusBase)(a))

/*获取RegCoilItem中的变量*/
#define MDS_RTU_REG_COIL_LIST(a)						((a)->pRegCoilList)
#define MDS_RTU_REG_COIL_ITEM_ADDR(a)				((a)->modbusAddr)
#define MDS_RTU_REG_COIL_ITEM_DATA(a)				((a)->modbusData)
#define MDS_RTU_REG_COIL_ITEM_Data_Size(a)	((a)->modbusDataSize)
#define MDS_RTU_REG_COIL_ITEM_ADDR_TYPE(a)	((a)->addrType)

/*下面宏用来对变量的位进行操作*/
#define MD_GET_BIT(a,b) (((a)>>(b))&0x1)
#define MD_SET_BIT(a,b) (a)|=(1<<(b))
#define MD_CLR_BIT(a,b) (a)&=(~(1<<(b)))

/*交换一个半字的高低字节*/
#define MD_SWAP_HL(a) (a)=((((a)&0xff)<<8)|(((a)>>8)&0xff))

#define MD_H_BYTE(a)	(((a)>>8)&0xff)
#define MD_L_BYTE(a)	((a)&0xff)

#endif
