/********************************************************************************
* @File name: MD_RTU_Fun.h
* @Author: zspace
* @Version: 1.0
* @Date: 2020-4-10
* @Description: MModbus RTU Slave receiving function module.
********************************************************************************/

#ifndef _MD_RTU_FUN_H__
#define _MD_RTU_FUN_H__
/*********************************HEAD FILE************************************/
#include "MD_RTU_Queue.h"
#include "MD_RTU_MapTable.h"
#include "MD_RTU_Type.h"
#include "MD_RTU_Tool.h"
#include "MD_RTU_Config.h"
/*********************************END******************************************/

/*********************************VARIABLE TYPE DECLARATION***********************************/
typedef void (*MDSWriteFunciton)(void* obj,uint16 modbusAddr,uint16 wLen,AddrType addrType);

typedef struct{	
	ModbusBase											modbusBase;													/*Inherit modbusBase*/
	MDSWriteFunciton								mdsWriteFun;												/*Slave write callback function*/
	MDSqQueue 											mdSqQueue;													/*Data receiving queue*/
	MDSqQueue												mdMsgSqQueue;												/*Message processing queue*/
	uint8														salveAddr;													/*Slave address*/
	
	uint8														serialReadCache[MDS_RTU_CMD_SIZE];	/*Single instruction read queue*/
	uint16													serialReadCount;										/*Instruction length*/
#if MDS_USE_SEND_CACHE
	uint8														serialSendCache[MDS_RTU_SEND_CACHE_SIZE];	/*Send buffer*/
	uint16													serialSendCount;											/*Number of bytes sent*/
#endif
	PMapTableItem 									pMapTableList[MDS_REG_COIL_ITEM_NUM];		/*Register registry*/
	
	/*The time of the last reception, 0xFFFFFFFF means that the detection frame has not started*/
	uint32 lastTimesTick;
	/*Current real-time time unit 100US*/
	uint32 timesTick;
	
	/*Last sent time*/
	uint32 lastSendTimes;
	
	/*Frame interval time 3.5T*/
	uint16 frameIntervalTime;
	
	/*CRC16 received*/
	uint16 CRC16Update;
	
}*PModbusS_RTU,ModbusS_RTU;

/*Exception code*/
typedef enum{
	READ_COIL_ANL=0x81,
	READ_INPUT_ANL=0x82,
	READ_HOLD_REGS=0x83,
	READ_INPUT_REGS=0X84,
	WRITE_SIN_COIL=0X85,
	WRITE_SIN_REG=0x86,
	WRITE_COILS=0X8F,
	WRITE_REGS=0X90
}ANLCode;

/*error code*/
typedef enum{
	ILLEGAL_FUN=0x01,				/*Not yet supported*/
	ILLEGAL_DAT_ADDR=0x2,
	ILLEGAL_DAT_VAL=0x3,
	SLAVE_DEV_FAILURE=0x4,	/*Not yet supported*/
	/*There is an error code behind, but it is not used yet*/
}ErrorCode;
/*********************************END******************************************/

/*********************************MACRO DEFINITION****************************************/
#define MDS_RTU_FUN_CODE(a)					(((PModbusS_RTU)(a))->serialReadCache[1])
#define MDS_RTU_CRC16(a)						(((((PModbusS_RTU)(a))->serialReadCache[a->serialReadCount-1])<<8)|\
(((PModbusS_RTU)(a))->serialReadCache[a->serialReadCount]))

#define MDS_RTU_START_REG(a)				(((((PModbusS_RTU)(a))->serialReadCache[2])<<8)|\
(((PModbusS_RTU)(a))->serialReadCache[3]))
#define MDS_RTU_REGS_NUM(a)					(((((PModbusS_RTU)(a))->serialReadCache[4])<<8)|\
(((PModbusS_RTU)(a))->serialReadCache[5]))
#define MDS_RTU_BYTES_NUM(a)				((a)->serialReadCache[6])


#if	MDS_USE_SEND_CACHE 
	/*Start sending*/
#define MDS_START_SEND(a)		{uint16 CRC16=0xFFFF;\
	a->serialSendCount=0
	/*Send a byte*/
#define MDS_SEND_BYTE(a,b)	CRC16=MD_CRC16Update(CRC16,(b));\
	a->serialSendCache[a->serialSendCount++]=b
	/*End sending*/
#define MDS_SEND_END(a)			a->serialSendCache[a->serialSendCount++]=(uint8)(CRC16);\
	a->serialSendCache[a->serialSendCount++]=(uint8)(CRC16>>8);\
(TO_MDBase(a))->mdRTUSendBytesFunction(a->serialSendCache,a->serialSendCount);}
#else
/*The following macro is used to send modbus data and calculate the check at the same time*/
#define MDS_START_SEND(a)		{uint8 CRC16Temp;uint16 CRC16=0xFFFF;
#define MDS_SEND_BYTE(a,b)	CRC16=MD_CRC16Update(CRC16,(b));MDS_RTU_SendByte(a,b)
#define MDS_SEND_END(a)			CRC16Temp=CRC16&0xFF;\
	(TO_MDBase(a))->mdRTUSendBytesFunction((uint8*)(&(CRC16Temp)),1);\
	CRC16Temp=(CRC16>>8)&0xFF;\
	(TO_MDBase(a))->mdRTUSendBytesFunction((uint8*)(&(CRC16Temp)),1);\
}
#endif
/*********************************END******************************************/


/*********************************FUNCTION DECLARATION************************************/
void MDS_RTU_Init(PModbusS_RTU pModbusRTU,MD_RTU_SerialInit mdRTUSerialInitFun,uint8 salveAddr,uint32 baud,uint8 dataBits,uint8 stopBit,uint8 parity);
void MDS_RTU_SetWriteListenFun(PModbusS_RTU pModbus_RTU,MDSWriteFunciton wFun);
BOOL MDS_RTU_AddMapItem(PModbusS_RTU pModbusRTU,PMapTableItem pRegCoilItem);
void MDS_RTU_Process(PModbusS_RTU pModbus_RTU);
/*********************************END******************************************/

#endif
