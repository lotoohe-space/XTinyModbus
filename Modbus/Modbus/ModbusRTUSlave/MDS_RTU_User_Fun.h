/********************************************************************************
* @File name: MD_RTU_User_Fun.h
* @Author: zspace
* @Version: 1.0
* @Date: 2020-4-10
* @Description: Modbus RTU host user calling function
********************************************************************************/

#ifndef _MD_RTU_USER_FUN_H__
#define _MD_RTU_USER_FUN_H__

/*********************************HEAD FILE************************************/
#include "MD_RTU_Type.h"
#include "MD_RTU_MapTable.h"
/*********************************END******************************************/

/*********************************Function declaration************************************/
/*General users do not need to call the following functions*/
BOOL MDS_RTU_ReadBits(void* obj,uint16 modbusAddr,uint16 numOf, uint8 *res, AddrType opAddrType);
BOOL MDS_RTU_ReadRegs(void* obj,uint16 modbusAddr,uint16 numOf, uint16 *res, AddrType opAddrType);
BOOL MDS_RTU_WriteBit(void* obj,uint16 modbusAddr,uint8 bit, AddrType opAddrType);
BOOL MDS_RTU_WriteBits(void* obj,uint16 modbusAddr,uint16 numOf, uint16 *bit, AddrType opAddrType);
BOOL MDS_RTU_WriteReg(void* obj,uint16 modbusAddr,uint16 reg, AddrType opAddrType);
BOOL MDS_RTU_WriteRegs(void* obj,uint16 modbusAddr,uint16 numOf, uint16 *reg,uint8 isBigE, AddrType opAddrType);

/*The following function user calls*/
BOOL MDS_RTU_WriteCoil(void* obj,uint16 modbusAddr,uint8 bit);
BOOL MDS_RTU_WriteInput(void* obj,uint16 modbusAddr,uint8 bit);
BOOL MDS_RTU_WriteCoils(void* obj,uint16 modbusAddr,uint16 numOf, uint16 *bit);
BOOL MDS_RTU_WriteInputs(void* obj,uint16 modbusAddr,uint16 numOf, uint16 *bit);
BOOL MDS_RTU_WriteHoldReg(void* obj,uint16 modbusAddr,uint16 reg);
BOOL MDS_RTU_WriteHoldRegs(void* obj,uint16 modbusAddr,uint16 numOf, uint16 *reg);
BOOL MDS_RTU_WriteInputReg(void* obj,uint16 modbusAddr,uint16 reg);
BOOL MDS_RTU_WriteInputRegs(void* obj,uint16 modbusAddr,uint16 numOf, uint16 *reg);
BOOL MDS_RTU_ReadCoils(void *obj,uint16 modbusAddr,uint16 numOf,uint8 *res);
BOOL MDS_RTU_ReadInput(void *obj,uint16 modbusAddr,uint16 numOf,uint8 *res);
BOOL MDS_RTU_ReadHoldRegs(void* obj,uint16 modbusAddr,uint16 numOf, uint16 *res);
BOOL MDS_RTU_ReadInputRegs(void* obj,uint16 modbusAddr,uint16 numOf, uint16 *res);
/*********************************END******************************************/

#endif
