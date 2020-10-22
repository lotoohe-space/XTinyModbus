/********************************************************************************
* @File name: MD_RTU_User_Fun.c
* @Author: zspace
* @Emial: 1358745329@qq.com
* @Version: 1.0
* @Date: 2020-4-10
* @Description: Modbus RTU Host user call function
* Open source address: https://github.com/lotoohe-space/XTinyModbus
********************************************************************************/

/*********************************HEAD FILE************************************/
#include "MDM_RTU_User_Fun.h"
#include "MDM_RTU_Fun.h"
/*********************************END******************************************/

/*******************************************************
*
* Function name :MDM_RTU_ReadBits
* Description        :Read discretely mapped bits, you can read one or multiple
* Parameter         :
*        @obj        Host object pointer
*        @modbusAddr        modbus address    
*        @numOf    The number to be read
*        @opAddrType    Address type(COILS_TYPE,INPUT_TYPE),See[AddrType]
*        @devAddr    Slave number to be read
* Return          : 
*        @res    Returned value TRUE success , FALSE fail
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
		/*Check the device number*/
		if(devAddr!=pModbusS_RTU->pMapTableList[i]->devAddr){continue;}
		
		if(pModbusS_RTU->pMapTableList[i]->modbusAddr<=modbusAddr&&
		(pModbusS_RTU->pMapTableList[i]->modbusAddr+pModbusS_RTU->pMapTableList[i]->modbusDataSize)>=(modbusAddr+numOf)
		){
			if(pModbusS_RTU->pMapTableList[i]->addrType==opAddrType){/*Must be Bit type*/
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
* Description        :Read discretely mapped registers, you can read one or multiple
* Parameter         :
*        @obj        Host object pointer
*        @modbusAddr       modbus address   
*        @numOf    The number to be read
*        @opAddrType    Address type(HOLD_REGS_TYPE,INPUT_REGS_TYPE),See[AddrType]
*        @devAddr    Slave number to be read
* Return          : 
*        @res    TRUE success , FALSE fail
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
		/*Check the device number*/
		if(devAddr!=pModbusS_RTU->pMapTableList[i]->devAddr){continue;}
		if(pModbusS_RTU->pMapTableList[i]->modbusAddr<=modbusAddr&&
		(pModbusS_RTU->pMapTableList[i]->modbusAddr+pModbusS_RTU->pMapTableList[i]->modbusDataSize)>=(modbusAddr+numOf)
		){
			if(pModbusS_RTU->pMapTableList[i]->addrType==opAddrType){/*Must be BIT type*/
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
