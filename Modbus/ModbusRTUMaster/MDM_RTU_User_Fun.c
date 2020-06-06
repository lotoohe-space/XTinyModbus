/********************************************************************************
* @File name: MD_RTU_User_Fun.c
* @Author: zspace
* @Version: 1.0
* @Date: 2020-4-10
* @Description: Modbus RTU 主机用户调用函数
* 开源地址: https://github.com/lotoohe-space/XTinyModbus
********************************************************************************/

/*********************************头文件包含************************************/
#include "MDM_RTU_User_Fun.h"
#include "MDM_RTU_Fun.h"
/*********************************结束******************************************/

/*******************************************************
*
* Function name :MDM_RTU_ReadBits
* Description        :读取离散映射的bits,可以读取一个，也可以读取多个
* Parameter         :
*        @obj        主机对象
*        @modbusAddr        modbus的地址    
*        @numOf    需要读取的个数
*        @opAddrType    地址类型(COILS_TYPE,INPUT_TYPE)，参见[AddrType]
*        @devAddr    需要读取的从机号
* Return          : 
*        @res    返回的值
*				 函数返回 TRUE success , FALSE fail
**********************************************************/
BOOL MDM_RTU_ReadBits(void* obj,uint16 modbusAddr,uint16 numOf, uint8 *res, AddrType opAddrType,uint8 devAddr){
	uint16 i;
	PModbus_RTU pModbusS_RTU = obj;
	if(pModbusS_RTU==NULL){return FALSE;}
		if(opAddrType != COILS_TYPE && opAddrType != INPUT_TYPE){return FALSE;}
	
	for(i=0;i<MDM_REG_COIL_ITEM_NUM;i++){
		if(pModbusS_RTU->pMapTableList[i]==NULL){
			continue;
		}
		/*检查设备号*/
		if(devAddr!=pModbusS_RTU->pMapTableList[i]->devAddr){continue;}
		
		if(pModbusS_RTU->pMapTableList[i]->modbusAddr<=modbusAddr&&
		(pModbusS_RTU->pMapTableList[i]->modbusAddr+pModbusS_RTU->pMapTableList[i]->modbusDataSize)>=(modbusAddr+numOf)
		){
			if(pModbusS_RTU->pMapTableList[i]->addrType==opAddrType){/*必须是BIT类型*/
				uint16 	j;
				uint16 offsetAddr=modbusAddr-MDS_RTU_REG_COIL_ITEM_ADDR(pModbusS_RTU->pMapTableList[i]);
				for(j=offsetAddr; j<offsetAddr+numOf; j++){		
					if(
						MD_GET_BIT(
						MDS_RTU_REG_COIL_ITEM_DATA(pModbusS_RTU->pMapTableList[i])[j>>4],j%16)
					){
							MD_SET_BIT(res[j>>3],j%8);
					}else{
							MD_CLR_BIT(res[j>>3],j%8);
					}
				}
				return TRUE;
			}
		}
	}
	return FALSE;
}
/*******************************************************
*
* Function name :MDM_RTU_ReadRegs
* Description        :读取离散映射的寄存器,可以读取一个，也可以读取多个
* Parameter         :
*        @obj        主机对象
*        @modbusAddr        modbus的地址    
*        @numOf    需要读取的个数
*        @opAddrType    地址类型(HOLD_REGS_TYPE,INPUT_REGS_TYPE)，参见[AddrType]
*        @devAddr    需要读取的从机号
* Return          : 
*        @res    返回的值
*				 函数返回 TRUE success , FALSE fail
**********************************************************/
BOOL MDM_RTU_ReadRegs(void* obj,uint16 modbusAddr,uint16 numOf, uint16 *res, AddrType opAddrType,uint8 devAddr){
	uint16 i;
	PModbus_RTU pModbusS_RTU = obj;
	if(pModbusS_RTU==NULL){return FALSE;}
	if(opAddrType != HOLD_REGS_TYPE && opAddrType != INPUT_REGS_TYPE){return FALSE;}
	
	for(i=0;i<MDM_REG_COIL_ITEM_NUM;i++){
		if(pModbusS_RTU->pMapTableList[i]==NULL){
			continue;
		}
				/*检查设备号*/
		if(devAddr!=pModbusS_RTU->pMapTableList[i]->devAddr){continue;}
		if(pModbusS_RTU->pMapTableList[i]->modbusAddr<=modbusAddr&&
		(pModbusS_RTU->pMapTableList[i]->modbusAddr+pModbusS_RTU->pMapTableList[i]->modbusDataSize)>=(modbusAddr+numOf)
		){
			if(pModbusS_RTU->pMapTableList[i]->addrType==opAddrType){/*必须是BIT类型*/
				uint16 	j;
				uint16 offsetAddr=modbusAddr-MDS_RTU_REG_COIL_ITEM_ADDR(pModbusS_RTU->pMapTableList[i]);
				for(j=0;j<numOf;j++){
					res[j]=MDS_RTU_REG_COIL_ITEM_DATA(pModbusS_RTU->pMapTableList[i])[offsetAddr+j];
				}
				return TRUE;
			}
		}
	}
	return FALSE;
}
