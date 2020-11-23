#ifndef _MD_RTU_TOOL_H__
#define _MD_RTU_TOOL_H__

#include "MD_RTU_Type.h"

typedef	void (*MDS_RTU_TimeHandlerFunction)(void* obj
	,uint32 times
	);
typedef void (*MDS_RTU_SendBytesFunction)(uint8 *byte,uint16 num);
typedef void (*MDS_RTU_RecByteFunction)(void* obj, uint8 byte);
typedef void (*MDS_RTU_RecSendConv)(uint8 mode);

typedef void (*MD_RTU_SerialInit)(void* obj,uint32 baud,uint8 dataBits,uint8 stopBit,uint8 parity);

typedef struct{
	/*Data sending and receiving related functions*/
	MDS_RTU_SendBytesFunction				mdRTUSendBytesFunction;
	MDS_RTU_RecByteFunction					mdRTURecByteFunction;
	MDS_RTU_RecSendConv							mdRTURecSendConv;
	/*Timer callback function*/
	MDS_RTU_TimeHandlerFunction			mdRTUTimeHandlerFunction;		
}*PModbusBase,ModbusBase;

/*Convert to ModbusBase*/
#define TO_MDBase(a)		((PModbusBase)(a))

/*Get the variables in RegCoilItem*/
#define MDS_RTU_REG_COIL_LIST(a)						((a)->pRegCoilList)
#define MDS_RTU_REG_COIL_ITEM_ADDR(a)				((a)->modbusAddr)
#define MDS_RTU_REG_COIL_ITEM_DATA(a)				((a)->modbusData)
#define MDS_RTU_REG_COIL_ITEM_Data_Size(a)	((a)->modbusDataSize)
#define MDS_RTU_REG_COIL_ITEM_ADDR_TYPE(a)	((a)->addrType)

/*The following macros are used to manipulate the bits of variables*/
#define MD_GET_BIT(a,b) (((a)>>(b))&0x1)
#define MD_SET_BIT(a,b) (a)|=(1<<(b))
#define MD_CLR_BIT(a,b) (a)&=(~(1<<(b)))

/*Transceiver mode conversion*/
#define MD_RTU_SEND_MODE(a)	while((a)->lastTimesTick!=0xFFFFFFFF);\
if((a)->modbusBase.mdRTURecSendConv)(a)->modbusBase.mdRTURecSendConv(TRUE)
#define MD_RTU_RECV_MODE(a)	while((a)->lastTimesTick!=0xFFFFFFFF);\
if((a)->modbusBase.mdRTURecSendConv)(a)->modbusBase.mdRTURecSendConv(FALSE)

/*Swap the high and low bytes of a halfword*/
#define MD_SWAP_HL(a) (a)=((((a)&0xff)<<8)|(((a)>>8)&0xff))

/*Take high and low byte*/
#define MD_H_BYTE(a)	(((a)>>8)&0xff)
#define MD_L_BYTE(a)	((a)&0xff)

#endif
