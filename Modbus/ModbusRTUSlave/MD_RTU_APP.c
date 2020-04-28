/**
* @File name: MD_RTU_APP.c
* @Author: zspace
* @Version: 1.0
* @Date: 2020-4-28
* @Description: Modbus RTU Slave从机应用模块。    
* 开源地址: https://github.com/lotoohe-space/XTinyModbus
*/
#include "MD_RTU_APP.h"
#include "MD_RTU_MapTable.h"
#include "MD_RTU_Fun.h"
#include "MD_RTU_User_Fun.h"
#include "MD_RTU_Serial.h"
#define SALVE_ADDR	0x01

void MDSAPPWriteFunciton(void* obj,uint16 modbusAddr,uint16 wLen,AddrType addrType);

uint16 mapTableData0[32]={1,2,3,4,5,6,7,8,9,10,11,12};
MapTableItem mapTableItem0={
	.modbusAddr=0x0000,				/*MODBUS中的地址*/
	.modbusData=mapTableData0,	/*映射的内存单元*/
	.modbusDataSize=32,				/*映射的大小*/
	.addrType=HOLD_REGS_TYPE				/*映射的类型*/
};
uint16 mapTableData2[32]={11,21,31,41,51,61,71,81,91,101,111,121};
MapTableItem mapTableItem2={
	.modbusAddr=0x0000,				/*MODBUS中的地址*/
	.modbusData=mapTableData2,	/*映射的内存单元*/
	.modbusDataSize=32,				/*映射的大小*/
	.addrType=INPUT_REGS_TYPE				/*映射的类型*/
};

uint16 mapTableData1[4]={0};
MapTableItem mapTableItem1={
	.modbusAddr=0x0000,				/*MODBUS中的地址*/
	.modbusData=mapTableData1,	/*映射的内存单元*/
	.modbusDataSize=64,				/*映射的大小*/
	.addrType=COILS_TYPE				/*映射的类型*/
};

uint16 mapTableData3[4]={0x5555,0x5555};
MapTableItem mapTableItem3={
	.modbusAddr=0x0000,				/*MODBUS中的地址*/
	.modbusData=mapTableData3,	/*映射的内存单元*/
	.modbusDataSize=64,				/*映射的大小*/
	.addrType=INPUT_TYPE				/*映射的类型*/
};

ModbusS_RTU modbusS_RTU={0};

BOOL MDS_RTU_APPInit(void){
	
	MDS_RTU_Init(&modbusS_RTU,MDSInitSerial,SALVE_ADDR,9600,8,1,0);
	
	if(MDS_RTU_AddMapItem(&modbusS_RTU,&mapTableItem0)==FALSE){
		return FALSE;
	}
	if(MDS_RTU_AddMapItem(&modbusS_RTU,&mapTableItem1)==FALSE){
		return FALSE;
	}
	if(MDS_RTU_AddMapItem(&modbusS_RTU,&mapTableItem2)==FALSE){
		return FALSE;
	}
	if(MDS_RTU_AddMapItem(&modbusS_RTU,&mapTableItem3)==FALSE){
		return FALSE;
	}

	MDS_RTU_SetWriteListenFun(&modbusS_RTU,MDSAPPWriteFunciton);
	return TRUE;
}
/*用户可以更新某个地址的值*/
void MDS_RTU_UserUpdate(void){
	uint16 temp=0xff;
	uint16 temp1[]={1,2,3,4,5};
	MDS_RTU_WriteCoils(&modbusS_RTU,1,5,&temp);
	MDS_RTU_WriteHoldReg(&modbusS_RTU,11, temp);
	MDS_RTU_WriteHoldRegs(&modbusS_RTU,5,5,temp1);
}
/*写入回调函数*/
void MDSAPPWriteFunciton(void* obj,uint16 modbusAddr,uint16 wLen,AddrType addrType){
	uint16 data[8];
	if((&modbusS_RTU)!=obj){return ;}
	
	switch(addrType){
		case COILS_TYPE:
			MDS_RTU_ReadCoils(obj,modbusAddr,wLen, (uint8*)data);
			data[0]=data[0];
			break;
		case INPUT_TYPE:
			break;
		case HOLD_REGS_TYPE:
			MDS_RTU_ReadHoldRegs(obj,modbusAddr,wLen<8?wLen:8, data);
			//data[0]=data[0];
			uint8 temp=MD_H_BYTE(data[0]);
			temp=MD_L_BYTE(data[0]);
			temp=temp;
			break;
		case INPUT_REGS_TYPE:
			break;
	}
}

void MDS_RTU_Loop(void){
	MDS_RTU_Process(&modbusS_RTU);
	MDS_RTU_UserUpdate();
}

