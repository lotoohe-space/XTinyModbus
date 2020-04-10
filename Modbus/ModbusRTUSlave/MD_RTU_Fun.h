/********************************************************************************
* @File name: MD_RTU_Fun.h
* @Author: zspace
* @Version: 1.0
* @Date: 2020-4-10
* @Description: Modbus RTU Slave从机接收功能模块。
********************************************************************************/

#ifndef _MD_RTU_FUN_H__
#define _MD_RTU_FUN_H__
/*********************************头文件包含************************************/
#include "MD_RTU_Queue.h"
#include "MD_RTU_RegCoil.h"
#include "MD_RTU_Type.h"
#include "MD_RTU_Tool.h"
/*********************************结束******************************************/

/*********************************配置参数***************************************/
#define 	REG_COIL_ITEM_NUM 	20			/*离散映射最大数量*/
#define 	MDS_RTU_CMD_SIZE		256			/*单条指令长度*/
/*********************************结束******************************************/


/*********************************变量类型申明***********************************/
typedef void (*MDSWriteFunciton)(void* obj,uint16 modbusAddr,uint16 wLen,AddrType addrType);

typedef struct{	
	ModbusBase											modbusBase;													/*继承modbusBase*/
	MDSWriteFunciton								mdsWriteFun;												/*主机写回调函数*/
	MDSqQueue 											mdSqQueue;													/*数据接收队列*/
	MDSqQueue												mdMsgSqQueue;												/*消息处理队列*/
	uint8														salveAddr;													/*从机地址*/
	
	uint8														serialReadCache[MDS_RTU_CMD_SIZE];	/*单指令读取队列*/
	uint16													serialReadCount;										/*指令的长度*/

	PRegCoilItem 										pRegCoilList[REG_COIL_ITEM_NUM];					/*寄存器注册表*/

	/*上次接收的时间,0xFFFFFFF表示未起开始检测帧*/
	uint32 lastTimesTick;
	/*当前的实时时间单位100US*/
	uint32 timesTick;
	
	/*上次发送的时间*/
	uint32 lastSendTimes;
	
	/*帧间隔时间3.5T*/
	uint16 frameIntervalTime;
	
	/*接收的CRC16*/
	uint16 CRC16Update;
	
	/*发送的CRC16*/
	uint16 CRC16SendUpdate;
}*PModbusS_RTU,ModbusS_RTU;

/*异常码*/
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

/*错误码*/
typedef enum{
	ILLEGAL_FUN=0x01,				/*还未支持*/
	ILLEGAL_DAT_ADDR=0x2,
	ILLEGAL_DAT_VAL=0x3,
	SLAVE_DEV_FAILURE=0x4,	/*还未支持*/
	/*后面还有错误码，但是暂时未使用*/
}ErrorCode;
/*********************************结束******************************************/

/*********************************宏定义****************************************/
#define MDS_RTU_FUN_CODE(a)					(((PModbusS_RTU)(a))->serialReadCache[1])
#define MDS_RTU_CRC16(a)						(((((PModbusS_RTU)(a))->serialReadCache[a->serialReadCount-1])<<8)|\
(((PModbusS_RTU)(a))->serialReadCache[a->serialReadCount]))

#define MDS_RTU_START_REG(a)				(((((PModbusS_RTU)(a))->serialReadCache[2])<<8)|\
(((PModbusS_RTU)(a))->serialReadCache[3]))
#define MDS_RTU_REGS_NUM(a)					(((((PModbusS_RTU)(a))->serialReadCache[4])<<8)|\
(((PModbusS_RTU)(a))->serialReadCache[5]))
#define MDS_RTU_BYTES_NUM(a)				((a)->serialReadCache[6])

/*下面宏用来发送modbus数据,并同时计算校验*/
#define MSD_START_SEND(a)		(a)->CRC16SendUpdate=0xFFFF
#define MSD_SEND_BYTE(a,b)	(a)->CRC16SendUpdate=MD_CRC16Update((a)->CRC16SendUpdate,(b));\
MDS_RTU_SendByte(a,b)
#define MSD_SEND_END(a)			(TO_MDBase(a))->mdRTUSendBytesFunction((uint8*)(&((a)->CRC16SendUpdate)),2)
/*********************************结束******************************************/


/*********************************函数申明************************************/
void MDS_RTU_Init(PModbusS_RTU pModbusRTU,MD_RTU_SerialInit mdRTUSerialInitFun,uint8 salveAddr,uint32 baud,uint8 dataBits,uint8 stopBit,uint8 parity);
void MDS_RTU_SetWriteListenFun(PModbusS_RTU pModbus_RTU,MDSWriteFunciton wFun);
void MDS_RTU_Process(PModbusS_RTU pModbus_RTU);
/*********************************结束******************************************/

#endif
