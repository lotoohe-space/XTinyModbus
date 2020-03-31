/**
* @file 		MD_RTU_APP.c
* @brief		无
* @details	无
* @author		zspace
* @date		2020/3/23
* @version	A001
* @par History:  无       
*/
#include "MD_RTU_APP.h"
#include "MD_RTU_RegCoil.h"
#include "MD_RTU_Fun.h"
#include "MD_RTU_User_Fun.h"
#include "MD_RTU_Serial.h"
#define SALVE_ADDR	0x01

void MDSAPPWriteFunciton(void* obj,uint16 modbusAddr,uint16 wLen,AddrType addrType);

uint16 regCoilData0[32]={1,2,3,4,5,6,7,8,9,10,11,12};
RegCoilItem regCoilItem0={
	.modbusAddr=0x0000,				/*MODBUS中的地址*/
	.modbusData=regCoilData0,	/*映射的内存单元*/
	.modbusDataSize=32,				/*映射的大小*/
	.addrType=REG_TYPE				/*映射的类型*/
};
uint16 regCoilData1[4]={0};
RegCoilItem regCoilItem1={
	.modbusAddr=0x0000,				/*MODBUS中的地址*/
	.modbusData=regCoilData1,	/*映射的内存单元*/
	.modbusDataSize=64,				/*映射的大小*/
	.addrType=BIT_TYPE				/*映射的类型*/
};


ModbusS_RTU modbusS_RTU={0};

BOOL MDS_RTU_APPInit(void){
	
	MDS_RTU_Init(&modbusS_RTU,MDSInitSerial,SALVE_ADDR,115200,8,1,0);
	
	/*添加一个地址映射*/
	if(RegCoilListAdd(&(modbusS_RTU.pRegCoilList), &regCoilItem0)==FALSE){
		return FALSE;
	}
	if(RegCoilListAdd(&(modbusS_RTU.pRegCoilList), &regCoilItem1)==FALSE){
		return FALSE;
	}
	MDS_RTU_SetWriteListenFun(&modbusS_RTU,MDSAPPWriteFunciton);
	return TRUE;
}
/*用户可以更新某个地址的值*/
void MDS_RTU_UserUpdate(void){
	uint16 temp=0xff;
	uint16 temp1[]={1,2,3,4,5};
	MDS_RTU_WriteBits(&modbusS_RTU,1,5,&temp);
	MDS_RTU_WriteReg(&modbusS_RTU,11, temp);
	MDS_RTU_WriteRegs(&modbusS_RTU,5,5, temp1,0);
}
/*写入回调函数*/
void MDSAPPWriteFunciton(void* obj,uint16 modbusAddr,uint16 wLen,AddrType addrType){
	uint16 data[8];
	if((&modbusS_RTU)!=obj){return ;}
	
	if(addrType==BIT_TYPE){
		MDS_RTU_ReadBits(obj,modbusAddr,wLen, (uint8*)data);
		data[0]=data[0];
	}else if(addrType==REG_TYPE){
		MDS_RTU_ReadRegs(obj,modbusAddr,wLen<8?wLen:8, data);
		data[0]=data[0];
	}
}

void MDS_RTU_Loop(void){
	MDS_RTU_Process(&modbusS_RTU);
	MDS_RTU_UserUpdate();
}

