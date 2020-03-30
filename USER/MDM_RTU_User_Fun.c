#include "MDM_RTU_User_Fun.h"
#include "MDM_RTU_Fun.h"
/*读取bits,可以读取一个，也可以读取多个*/
BOOL MDM_RTU_ReadBits(void* obj,uint16 modbusAddr,uint16 numOf, uint8 *res){
	uint16 i;
	PModbus_RTU pModbusS_RTU = obj;
	if(pModbusS_RTU==NULL){return FALSE;}
	for(i=0;i<MDM_REG_COIL_ITEM_NUM;i++){
		if(pModbusS_RTU->pRegCoilList[i]==NULL){
			continue;
		}
		if(pModbusS_RTU->pRegCoilList[i]->modbusAddr<=modbusAddr&&
		(pModbusS_RTU->pRegCoilList[i]->modbusAddr+pModbusS_RTU->pRegCoilList[i]->modbusDataSize)>=(modbusAddr+numOf)
		){
			if(pModbusS_RTU->pRegCoilList[i]->addrType==BIT_TYPE){/*必须是BIT类型*/
				uint16 	j;
				uint16 offsetAddr=modbusAddr-MDS_RTU_REG_COIL_ITEM_ADDR(pModbusS_RTU->pRegCoilList[i]);
				for(j=offsetAddr; j<offsetAddr+numOf; j++){		
					if(
						MD_GET_BIT(
						MDS_RTU_REG_COIL_ITEM_DATA(pModbusS_RTU->pRegCoilList[i])[j>>4],j%16)
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
BOOL MDM_RTU_ReadRegs(void* obj,uint16 modbusAddr,uint16 numOf, uint16 *res){
	uint16 i;
	PModbus_RTU pModbusS_RTU = obj;
	if(pModbusS_RTU==NULL){return FALSE;}
	for(i=0;i<MDM_REG_COIL_ITEM_NUM;i++){
		if(pModbusS_RTU->pRegCoilList[i]==NULL){
			continue;
		}
		if(pModbusS_RTU->pRegCoilList[i]->modbusAddr<=modbusAddr&&
		(pModbusS_RTU->pRegCoilList[i]->modbusAddr+pModbusS_RTU->pRegCoilList[i]->modbusDataSize)>=(modbusAddr+numOf)
		){
			if(pModbusS_RTU->pRegCoilList[i]->addrType==REG_TYPE){/*必须是BIT类型*/
				uint16 	j;
				uint16 offsetAddr=modbusAddr-MDS_RTU_REG_COIL_ITEM_ADDR(pModbusS_RTU->pRegCoilList[i]);
				for(j=0;j<numOf;j++){
					res[j]=MDS_RTU_REG_COIL_ITEM_DATA(pModbusS_RTU->pRegCoilList[i])[offsetAddr+j];
				}
				return TRUE;
			}
		}
	}
	return FALSE;
}
