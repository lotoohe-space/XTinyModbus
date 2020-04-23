/********************************************************************************
* @File name: MD_RTU_Fun.h
* @Author: zspace
* @Version: 1.0
* @Date: 2020-4-10
* @Description: Modbus RTU 主机功能模块
********************************************************************************/

#ifndef _MEM_RTU_FUN_H__
#define _MEM_RTU_FUN_H__
/*********************************头文件包含************************************/
#include "MD_RTU_Queue.h"
#include "MD_RTU_Type.h"
#include "MD_RTU_Error.h"
#include "MD_RTU_Tool.h"
#include "MD_RTU_RegCoil.h"
/*********************************结束******************************************/

/*********************************配置宏************************************/
#define MDM_REG_COIL_ITEM_NUM 20		/*离散映射个数*/
/*********************************结束******************************************/

/*********************************头文件包含************************************/
/*modbus rtu 中的 功能码*/
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
	/*继承modbusBase*/
	ModbusBase		modbusBase;
	/*离散映射列表，读取到的数据被存与其中*/
	PRegCoilItem 	pRegCoilList[MDM_REG_COIL_ITEM_NUM];
	/*数据接收队列*/
	MDSqQueue 		mdSqQueue;

	/*上次接收的时间,0xFFFFFFF表示未起开始检测帧*/
	uint32 				lastTimesTick;
	/*当前的实时时间单位100US*/
	uint32 				timesTick;
	
	/*帧间隔时间3.5T*/
	uint16 				frameIntervalTime;
	
	/*接收的CRC16*/
	uint16 				CRC16Update;
	
	/*父类地址，用于在非阻塞读取时的时间片占用区分*/
	void *				parentObj;
	
	/*为1代表接收到一帧的数据*/
	uint8					recvFlag;
}*PModbus_RTU,Modbus_RTU;

/*发送控制块*/
typedef struct{
	PModbus_RTU pModbus_RTU;/*modbus rtu*/
	
	uint16			sendIntervalTime;/*发送的间隔时间，即多久发送一次*/
	
	uint32			sendTimeTick;/*发送时的时间*/
	uint8				RTCount;/*以及重传次数*/
	
	uint32			sendOverTime;/*设定的发送超时时间*/
	
	uint8				RTTimes;/*设定的重传次数 当其为255时表示一直进行重传*/
	uint8				sendFlag;/*发送标志位 0未发送 1发送了 2发送成功 3发送失败*/
}*PModbus_RTU_CB,Modbus_RTU_CB;

/*Modbus RTU 块初始化函数*/
MDError MDM_RTU_Init(
	PModbus_RTU pModbusRTU,
	MD_RTU_SerialInit mdRTUSerialInitFun,
	uint32 baud,
	uint8 dataBits,
	uint8 stopBits,
	uint8 parity
);

/*控制块初始化函数*/
void MDM_RTU_CB_Init(
	 PModbus_RTU_CB 	pModbusRTUCB
	,PModbus_RTU 		pModbusRTU
	,uint16 				sendIntervalTime
	,uint32					sendOverTime/*发送超时时间*/
	,uint8 					RTTimes/*重传次数 当其为255时表示一直进行重传*/
);
/*********************************结束******************************************/

/*********************************宏定义************************************/
#define MEM_RTU_START_EN()	{uint16 CRCUpdate=0xFFFF;
#define MEM_RTU_EN_QUEUE(a,b) MDM_RTU_SendByte((a),(b));\
CRCUpdate=MD_CRC16Update(CRCUpdate,(b))
#define MEM_RTU_END_EN(a)		(TO_MDBase(a))->mdRTUSendBytesFunction((uint8*)(&CRCUpdate),2);}
/*********************************结束******************************************/

/*********************************函数定义************************************/
/*定时器中断函数中调用,时间单位100us*/
void MDM_RTU_TimeHandler(void *obj);
/*串口接收中断函数中调用*/
void MDM_RTU_RecvByte(void *obj,uint8 byte);

/*控制块超时复位*/
void MDM_RTU_CB_OverTimeReset(PModbus_RTU_CB 	pModbusRTUCB);

BOOL MDM_RTU_AddMapItem(PModbus_RTU pModbusRTU,PRegCoilItem pRegCoilItem);

/*从队列中获取数据*/
MDError MDM_RTU_ReadByte(PModbus_RTU pModbusRTU,uint8 *res,uint8 len);
MDError MDM_RTU_ReadUint16(PModbus_RTU pModbusRTU,uint16 *res,uint8 len);

/*非阻塞式读写基函数*/
MDError MDM_RTU_NB_RW(PModbus_RTU_CB pModbus_RTU_CB,ModbusFunCode funCode,uint8 slaveAddr,uint16 startAddr,uint16 numOf,void *wData);
/*阻塞式读写基函数*/
MDError MDM_RTU_RW(PModbus_RTU_CB pModbus_RTU_CB,ModbusFunCode funCode,uint8 slaveAddr,uint16 startAddr,uint16 numOf,void *wData);

/*下面是非阻塞式读写*/
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

/*下面是阻塞式读写*/
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
/*********************************结束******************************************/

#endif
