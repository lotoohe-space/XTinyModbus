#include "MD_RTU_User_Fun.h"
#include "MD_RTU_Fun.h"

/*读取bits,可以读取一个，也可以读取多个*/
BOOL MDS_RTU_ReadBits(void* obj,uint16 modbusAddr,uint16 numOf, uint8 *res){
	uint16 i;
	PModbusS_RTU pModbusS_RTU = obj;
	if(pModbusS_RTU==NULL){return FALSE;}
	for(i=0;i<REG_COIL_ITEM_NUM;i++){
		if(pModbusS_RTU->pRegCoilList[i]==NULL){
			continue;
		}
		if(pModbusS_RTU->pRegCoilList[i]->modbusAddr<=modbusAddr&&
		(pModbusS_RTU->pRegCoilList[i]->modbusAddr+pModbusS_RTU->pRegCoilList[i]->modbusDataSize)>=(modbusAddr+1)
		){
			if(pModbusS_RTU->pRegCoilList[i]->addrType==BIT_TYPE){/*必须是BIT类型*/
				uint16 	j;
				uint16 offsetAddr=modbusAddr-MDS_RTU_REG_COIL_ITEM_ADDR(pModbusS_RTU->pRegCoilList[i]);
				for(j=offsetAddr; j<offsetAddr+numOf; j++){		
					if(
						MDS_GET_BIT(
						MDS_RTU_REG_COIL_ITEM_DATA(pModbusS_RTU->pRegCoilList[i])[j>>4],j%16)
					){
							MDS_SET_BIT(res[j>>3],j%8);
					}else{
							MDS_CLR_BIT(res[j>>3],j%8);
					}
				}
				return TRUE;
			}
		}
	}
	return FALSE;
}
BOOL MDS_RTU_ReadRegs(void* obj,uint16 modbusAddr,uint16 numOf, uint16 *res){
	uint16 i;
	PModbusS_RTU pModbusS_RTU = obj;
	if(pModbusS_RTU==NULL){return FALSE;}
	for(i=0;i<REG_COIL_ITEM_NUM;i++){
		if(pModbusS_RTU->pRegCoilList[i]==NULL){
			continue;
		}
		if(pModbusS_RTU->pRegCoilList[i]->modbusAddr<=modbusAddr&&
		(pModbusS_RTU->pRegCoilList[i]->modbusAddr+pModbusS_RTU->pRegCoilList[i]->modbusDataSize)>=(modbusAddr+1)
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
/*该函数被用户调用，用来设置某地址的状态*/
BOOL MDS_RTU_WriteBit(void* obj,uint16 modbusAddr,uint8 bit){
	uint16 i;
	PModbusS_RTU pModbusS_RTU = obj;
	if(pModbusS_RTU==NULL){return FALSE;}
	for(i=0;i<REG_COIL_ITEM_NUM;i++){
		if(pModbusS_RTU->pRegCoilList[i]==NULL){
			continue;
		}
		if(pModbusS_RTU->pRegCoilList[i]->modbusAddr<=modbusAddr&&
		(pModbusS_RTU->pRegCoilList[i]->modbusAddr+pModbusS_RTU->pRegCoilList[i]->modbusDataSize)>=(modbusAddr+1)
		){
			if(pModbusS_RTU->pRegCoilList[i]->addrType==BIT_TYPE){/*必须是BIT类型*/
				uint16 offsetAddr=modbusAddr-MDS_RTU_REG_COIL_ITEM_ADDR(pModbusS_RTU->pRegCoilList[i]);
				if(bit){
					MDS_SET_BIT(
						MDS_RTU_REG_COIL_ITEM_DATA(pModbusS_RTU->pRegCoilList[i])[offsetAddr>>4],offsetAddr%16);
				}else{
					MDS_CLR_BIT(
						MDS_RTU_REG_COIL_ITEM_DATA(pModbusS_RTU->pRegCoilList[i])[offsetAddr>>4],offsetAddr%16);
				}
				return TRUE;
			}
		}
	}
	return FALSE;
}
BOOL MDS_RTU_WriteBits(void* obj,uint16 modbusAddr,uint16 numOf, uint16 *bit){
	uint16 i;
	PModbusS_RTU pModbusS_RTU = obj;
	if(pModbusS_RTU==NULL){return FALSE;}
	for(i=0;i<REG_COIL_ITEM_NUM;i++){
		if(pModbusS_RTU->pRegCoilList[i]==NULL){
			continue;
		}
		if(pModbusS_RTU->pRegCoilList[i]->modbusAddr<=modbusAddr&&
		(pModbusS_RTU->pRegCoilList[i]->modbusAddr+pModbusS_RTU->pRegCoilList[i]->modbusDataSize)>=(modbusAddr+1)
		){
			if(pModbusS_RTU->pRegCoilList[i]->addrType==BIT_TYPE){/*必须是BIT类型*/
				uint16 offsetAddr=modbusAddr-MDS_RTU_REG_COIL_ITEM_ADDR(pModbusS_RTU->pRegCoilList[i]);
				uint16 j;
				for(j=0;j<numOf;j++){
					uint8 *byteData=(uint8*)bit;
					if(
						MDS_GET_BIT( byteData[j>>3] ,j%8)
					){
						MDS_SET_BIT(
							MDS_RTU_REG_COIL_ITEM_DATA(
							pModbusS_RTU->pRegCoilList[i])[(offsetAddr+j)>>4]
						,(j+offsetAddr)%16);
					}else{
						MDS_CLR_BIT(
							MDS_RTU_REG_COIL_ITEM_DATA(
							pModbusS_RTU->pRegCoilList[i])[(offsetAddr+j)>>4]
						,(j+offsetAddr)%16);
					}
				}
				return TRUE;
			}
		}
	}
	return FALSE;
}
BOOL MDS_RTU_WriteReg(void* obj,uint16 modbusAddr,uint16 reg){
	uint16 i;
	PModbusS_RTU pModbusS_RTU = obj;
	if(pModbusS_RTU==NULL){return FALSE;}
	for(i=0;i<REG_COIL_ITEM_NUM;i++){
		if(pModbusS_RTU->pRegCoilList[i]==NULL){
			continue;
		}
		if(pModbusS_RTU->pRegCoilList[i]->modbusAddr<=modbusAddr&&
		(pModbusS_RTU->pRegCoilList[i]->modbusAddr+pModbusS_RTU->pRegCoilList[i]->modbusDataSize)>=(modbusAddr+1)
		){
			if(pModbusS_RTU->pRegCoilList[i]->addrType==REG_TYPE){/*必须是REG类型*/
				uint16 offsetAddr=modbusAddr-MDS_RTU_REG_COIL_ITEM_ADDR(pModbusS_RTU->pRegCoilList[i]);
				MDS_RTU_REG_COIL_ITEM_DATA(pModbusS_RTU->pRegCoilList[i])[offsetAddr]=reg;
				return TRUE;
			}
		}
	}
	return FALSE;
}
BOOL MDS_RTU_WriteRegs(void* obj,uint16 modbusAddr,uint16 numOf, uint16 *reg,uint8 isBigE){
	uint16 i;
	PModbusS_RTU pModbusS_RTU = obj;
	if(pModbusS_RTU==NULL){return FALSE;}
	for(i=0;i<REG_COIL_ITEM_NUM;i++){
		if(pModbusS_RTU->pRegCoilList[i]==NULL){
			continue;
		}
		if(pModbusS_RTU->pRegCoilList[i]->modbusAddr<=modbusAddr&&
		(pModbusS_RTU->pRegCoilList[i]->modbusAddr+pModbusS_RTU->pRegCoilList[i]->modbusDataSize)>=(modbusAddr+1)
		){
			if(pModbusS_RTU->pRegCoilList[i]->addrType==REG_TYPE){/*必须是REG类型*/
				uint16 offsetAddr=modbusAddr-MDS_RTU_REG_COIL_ITEM_ADDR(pModbusS_RTU->pRegCoilList[i]);
				uint16 j=0;
				for(j=0;j<numOf;j++){
					MDS_RTU_REG_COIL_ITEM_DATA(pModbusS_RTU->pRegCoilList[i])[offsetAddr+j]=
					isBigE?MDS_SWAP_HL(reg[j]):reg[j];
				}		
				return TRUE;
			}
		}
	}
	return FALSE;
}
