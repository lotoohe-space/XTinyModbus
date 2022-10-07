/********************************************************************************
* @File name: MD_RTU_Fun.h
* @Author: zspace
* @Emial: 1358745329@qq.com
* @Version: 1.0
* @Date: 2020-4-10
* @Description: Modbus RTU Host function module
********************************************************************************/

#ifndef _MEM_RTU_FUN_H__
#define _MEM_RTU_FUN_H__
/*********************************HEAD FILE************************************/
#include "MD_RTU_Queue.h"
#include "MD_RTU_Type.h"
#include "MD_RTU_Error.h"
#include "MD_RTU_Tool.h"
#include "MD_RTU_MapTable.h"
#include "MD_RTU_Config.h"
#include "MD_RTU_SysInterface.h"

/*********************************END******************************************/

/*********************************CUSTOM DATA TYPE************************************/
/*Function code in Modbus RTU*/
typedef enum{
	READ_COIL=1,
	READ_INPUT=2,
	READ_HOLD_REG=3,
	READ_INPUT_REG=4,
	WRITE_SIN_COIL=5,
	WRITE_SIN_REG=6,
	WRITE_COILS=15,
	WRITE_REGS=16
}ModbusFunCode;

typedef struct{
	/*Inherit modbusBase*/
	ModbusBase		modbusBase;
	/*Discrete mapping list, the data read is stored in it*/
	PMapTableItem pMapTableList[MDM_REG_COIL_ITEM_NUM];
	/*Data receiving queue*/
	MDSqQueue 		mdSqQueue;
#if MD_RTU_USED_OS
  /**/
	MD_RTU_LOCK_HANDLE mdRTULockHandle;
	MD_RTU_LOCK_HANDLE mdRTULockObjHandle;
	MD_RTU_LOCK_HANDLE mdRTULockObj1Handle;
	MD_RTU_TASK_HANDLE mdRTUTaskHandle;
	MD_RTU_MSG_HANDLE  mdRTUMsgHandle;
#endif
#if MDM_USE_SEND_CACHE
	uint8					serialSendCache[MDM_RTU_SEND_CACHE_SIZE];		/*Send cache*/
	uint16				serialSendCount;														/*Number of bytes sent*/
#endif
	
	/*The time of the last reception, 0xFFFFFFF means that the detection frame has not started*/
	uint32 				lastTimesTick;
	/*Current real-time time unit 100US*/
	uint32 				timesTick;
	
	/*Frame interval time 3.5T*/
	uint16 				frameIntervalTime;
	
	/*CRC16 received*/
	uint16 				CRC16Update;
	
	/*Parent class address, used to distinguish time slice occupation during non-blocking read*/
	void *				parentObj;
	
	/*1 means one frame of data is received*/
	uint8					recvFlag;
	
	/*last mode*/
	uint8					blockMode;
}*PModbus_RTU,Modbus_RTU;

#if MD_RTU_USED_OS
#define MD_RTU_LOCK_HANDLE_ARG(a) 			(a)->mdRTULockHandle
#define MD_RTU_LOCK_OBJ_HANDLE_ARG(a) 	(a)->mdRTULockObjHandle
#define MD_RTU_LOCK_OBJ1_HANDLE_ARG(a) 	(a)->mdRTULockObj1Handle
#define MD_RTU_TASK_HANDLE_ARG(a) 			(a)->mdRTUTaskHandle
#define MD_RTU_MSG_HANDLE_ARG(a) 				(a)->mdRTUMsgHandle
#else
#define MD_RTU_LOCK_HANDLE_ARG(a) 
#define MD_RTU_LOCK_OBJ_HANDLE_ARG(a)
#define MD_RTU_LOCK_OBJ1_HANDLE_ARG(a) 
//#define MD_RTU_SEM_HANDLE_ARG(a)
#define MD_RTU_MSG_HANDLE_ARG(a)
#endif

/*Send control block*/
typedef struct{
	PModbus_RTU pModbus_RTU;/*modbus rtu*/
	
	uint32			sendIntervalTime;/*The sending interval, that is, sending once for a long time.*/
	
	uint32			sendTimeTick;/*Time when sending*/
	uint8				RTCount;/*Number of retransmissions*/
	
	uint32			sendOverTime;/*Set sending timeout time*/
	
	uint8				RTTimes;/*The set number of retransmissions. When it is 255, it means that retransmissions will continue.*/
	uint8				sendFlag;/*Send flag
												0 Unsent 
												1 Already sent 
												2 Sent successfully 
												3 Failed to send 
												*/
	uint8 			flag;			/*
	bit0:Device offline polling flag (when flag is 1, the control block will not be polled when the device is offline).
	bit1:Device offline polling enable
	*/
}*PModbus_RTU_CB,Modbus_RTU_CB;

/*Get device offline flag*/
#define MD_CB_GET_DIS_FLAG(a)	MD_GET_BIT(a->flag,0)
#define MD_CB_SET_DIS_FLAG(a)	MD_SET_BIT(a->flag,0)
#define MD_CB_CLR_DIS_FLAG(a)	MD_CLR_BIT(a->flag,0)

/*Device enable flag*/
#define MD_CB_GET_DIS_FLAG_EN(a)	MD_GET_BIT(a->flag,1)
#define MD_CB_SET_DIS_FLAG_EN(a)	MD_SET_BIT(a->flag,1)
#define MD_CB_CLR_DIS_FLAG_EN(a)	MD_CLR_BIT(a->flag,1)




/*********************************END******************************************/

/*********************************MACRO DEFINITION************************************/
#if MDM_USE_SEND_CACHE

#define MEM_RTU_START_EN(a)	{uint16 CRCUpdate1=0xFFFF;(a)->serialSendCount=0
	
#define MEM_RTU_EN_QUEUE(a,b) (a)->serialSendCache[(a)->serialSendCount++]=(b);\
CRCUpdate1=MD_CRC16Update(CRCUpdate1,(b))
	
#define MEM_RTU_END_EN(a)		a->serialSendCache[a->serialSendCount++]=(uint8)(CRCUpdate1);\
	a->serialSendCache[a->serialSendCount++]=(uint8)(CRCUpdate1>>8);\
(TO_MDBase(a))->mdRTUSendBytesFunction(a->serialSendCache,a->serialSendCount);}

#else
#define MEM_RTU_START_EN()	{uint16 CRCUpdate=0xFFFF;
#define MEM_RTU_EN_QUEUE(a,b) MDM_RTU_SendByte((a),(b));\
CRCUpdate=MD_CRC16Update(CRCUpdate,(b))
#define MEM_RTU_END_EN(a)	MDM_RTU_SendByte(a,(uint8)CRCUpdate);\
	MDM_RTU_SendByte(a,(uint8)(CRCUpdate>>8));\
}
#endif
/*********************************END******************************************/

/*********************************FUNCTION DEFINITION************************************/

#if MD_RTU_USED_OS	///< used os
void MDM_RTU_SysProcessTask(void *arg);
#endif

/*Modbus RTU block initialization function*/
MDError MDM_RTU_Init(
	PModbus_RTU pModbusRTU,
	MD_RTU_SerialInit mdRTUSerialInitFun,
	uint32 baud,
	uint8 dataBits,
	uint8 stopBits,
	uint8 parity
);
/*Control block initialization function*/
void MDM_RTU_CB_Init(
	 PModbus_RTU_CB 	pModbusRTUCB
	,PModbus_RTU 		pModbusRTU
	,uint32 				sendIntervalTime
	,uint32					sendOverTime/*Send timeout*/
	,uint8 					RTTimes/*The number of retransmissions, when it is 255, it means that retransmissions have been performed.*/
);
void MDM_RTU_QueueInit(PModbus_RTU pModbus_RTU,
	uint8* recvQueueData,
	uint16  recvQueueSize
);

/*Called in the timer interrupt function, the time unit is 100us*/
void MDM_RTU_TimeHandler(void *obj,uint32 timesTick);
/*Called in the serial port receive interrupt function*/
void MDM_RTU_RecvByte(void *obj,uint8 byte);

/*Control block timeout reset*/
void MDM_RTU_CB_OverTimeReset(PModbus_RTU_CB 	pModbusRTUCB);
void MDM_RTU_CB_ClrDisFlag(PModbus_RTU_CB 	pModbusRTUCB);
void MDM_RTU_CB_SetDisPollEnFlag(PModbus_RTU_CB 	pModbusRTUCB,BOOL state);

BOOL MDM_RTU_AddMapItem(PModbus_RTU pModbusRTU,PMapTableItem pRegCoilItem);

/*Get data from the queue*/
MDError MDM_RTU_ReadByte(PModbus_RTU pModbusRTU,uint8 *res,uint8 len);
MDError MDM_RTU_ReadUint16(PModbus_RTU pModbusRTU,uint16 *res,uint8 len);

///*Non-blocking read and write basis functions*/
//MDError MDM_RTU_NB_RW(PModbus_RTU_CB pModbus_RTU_CB,ModbusFunCode funCode,uint8 slaveAddr,uint16 startAddr,uint16 numOf,void *wData);
///*Blocking read and write basis functions*/
//MDError MDM_RTU_RW(PModbus_RTU_CB pModbus_RTU_CB,ModbusFunCode funCode,uint8 slaveAddr,uint16 startAddr,uint16 numOf,void *wData);

/*The following is non-blocking read and write*/
MDError MDM_RTU_NB_ReadCoil(PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf);
MDError MDM_RTU_NB_ReadInput(PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf);
MDError MDM_RTU_NB_ReadHoldReg(PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf);
MDError MDM_RTU_NB_ReadInputReg(PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf);
MDError MDM_RTU_NB_WriteSingleCoil(
	PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,BOOL boolVal);
MDError MDM_RTU_NB_WriteSingleReg(
	PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 val);
MDError MDM_RTU_NB_WriteCoils(
	PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf,uint8* val);
MDError MDM_RTU_NB_WriteRegs(
	PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf,uint16* val);

/*The following is a blocking read and write*/
MDError MDM_RTU_ReadCoil(PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf);
MDError MDM_RTU_ReadInput(PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf);
MDError MDM_RTU_ReadHoldReg(PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf);
MDError MDM_RTU_ReadInputReg(PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf);
MDError MDM_RTU_WriteSingleCoil(
	PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,BOOL boolVal);
MDError MDM_RTU_WriteSingleReg(
	PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 val);
MDError MDM_RTU_WriteCoils(
	PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf,uint8* val);
MDError MDM_RTU_WriteRegs(
	PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf,uint16* val);

/*The following API is used to be blocking, but can be mixed with non-blocking calls,
you may need to set the value of the MDM_MIX_CALL_DELAY parameter.*/
MDError MDM_RTU_MixReadCoil(PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf);
MDError MDM_RTU_MixReadInput(PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf);
MDError MDM_RTU_MixReadHoldReg(PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf);
MDError MDM_RTU_MixReadInputReg(PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf);
MDError MDM_RTU_MixWriteSingleCoil(
	PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,BOOL boolVal);
MDError MDM_RTU_MixWriteSingleReg(
	PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 val);
MDError MDM_RTU_MixWriteCoils(
	PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf,uint8* val);
MDError MDM_RTU_MixWriteRegs(
	PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf,uint16* val);

/*********************************END******************************************/

#endif
