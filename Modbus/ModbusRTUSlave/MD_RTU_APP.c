/**
* @file 		MD_RTU_APP.c
* @brief		��
* @details	��
* @author		zspace
* @date		2020/3/23
* @version	A001
* @par History:  ��       
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
	.modbusAddr=0x0000,				/*MODBUS�еĵ�ַ*/
	.modbusData=regCoilData0,	/*ӳ����ڴ浥Ԫ*/
	.modbusDataSize=32,				/*ӳ��Ĵ�С*/
	.addrType=HOLD_REGS_TYPE				/*ӳ�������*/
};
uint16 regCoilData2[32]={11,21,31,41,51,61,71,81,91,101,111,121};
RegCoilItem regCoilItem2={
	.modbusAddr=0x0000,				/*MODBUS�еĵ�ַ*/
	.modbusData=regCoilData2,	/*ӳ����ڴ浥Ԫ*/
	.modbusDataSize=32,				/*ӳ��Ĵ�С*/
	.addrType=INPUT_REGS_TYPE				/*ӳ�������*/
};

uint16 regCoilData1[4]={0};
RegCoilItem regCoilItem1={
	.modbusAddr=0x0000,				/*MODBUS�еĵ�ַ*/
	.modbusData=regCoilData1,	/*ӳ����ڴ浥Ԫ*/
	.modbusDataSize=64,				/*ӳ��Ĵ�С*/
	.addrType=COILS_TYPE				/*ӳ�������*/
};

uint16 regCoilData3[4]={0x5555,0x5555};
RegCoilItem regCoilItem3={
	.modbusAddr=0x0000,				/*MODBUS�еĵ�ַ*/
	.modbusData=regCoilData3,	/*ӳ����ڴ浥Ԫ*/
	.modbusDataSize=64,				/*ӳ��Ĵ�С*/
	.addrType=INPUT_TYPE				/*ӳ�������*/
};

ModbusS_RTU modbusS_RTU={0};

BOOL MDS_RTU_APPInit(void){
	
	MDS_RTU_Init(&modbusS_RTU,MDSInitSerial,SALVE_ADDR,115200,8,1,0);
	
	if(MDS_RTU_AddMapItem(&modbusS_RTU,&regCoilItem0)==FALSE){
		return FALSE;
	}
	if(MDS_RTU_AddMapItem(&modbusS_RTU,&regCoilItem1)==FALSE){
		return FALSE;
	}
	if(MDS_RTU_AddMapItem(&modbusS_RTU,&regCoilItem2)==FALSE){
		return FALSE;
	}
	if(MDS_RTU_AddMapItem(&modbusS_RTU,&regCoilItem3)==FALSE){
		return FALSE;
	}

	MDS_RTU_SetWriteListenFun(&modbusS_RTU,MDSAPPWriteFunciton);
	return TRUE;
}
/*�û����Ը���ĳ����ַ��ֵ*/
void MDS_RTU_UserUpdate(void){
	uint16 temp=0xff;
	uint16 temp1[]={1,2,3,4,5};
	MDS_RTU_WriteCoils(&modbusS_RTU,1,5,&temp);
	MDS_RTU_WriteHoldReg(&modbusS_RTU,11, temp);
	MDS_RTU_WriteHoldRegs(&modbusS_RTU,5,5,temp1);
}
/*д��ص�����*/
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
			data[0]=data[0];
			break;
		case INPUT_REGS_TYPE:
			break;
	}
}

void MDS_RTU_Loop(void){
	MDS_RTU_Process(&modbusS_RTU);
	MDS_RTU_UserUpdate();
}
