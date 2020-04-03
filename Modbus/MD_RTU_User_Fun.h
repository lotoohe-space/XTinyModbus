/**
* @file 		MD_RTU_User_Fun.h
* @brief		无
* @details	无
* @author		zspace
* @date		2020/3/23
* @version	A001
* @par History:  无       
*/
#ifndef _MD_RTU_USER_FUN_H__
#define _MD_RTU_USER_FUN_H__
#include "MD_RTU_Type.h"
#include "MD_RTU_RegCoil.h"

/*下面函数一般用户不需要调用*/
BOOL MDS_RTU_ReadBits(void* obj,uint16 modbusAddr,uint16 numOf, uint8 *res, AddrType opAddrType);
BOOL MDS_RTU_ReadRegs(void* obj,uint16 modbusAddr,uint16 numOf, uint16 *res, AddrType opAddrType);
BOOL MDS_RTU_WriteBit(void* obj,uint16 modbusAddr,uint8 bit, AddrType opAddrType);
BOOL MDS_RTU_WriteBits(void* obj,uint16 modbusAddr,uint16 numOf, uint16 *bit, AddrType opAddrType);
BOOL MDS_RTU_WriteReg(void* obj,uint16 modbusAddr,uint16 reg, AddrType opAddrType);
BOOL MDS_RTU_WriteRegs(void* obj,uint16 modbusAddr,uint16 numOf, uint16 *reg,uint8 isBigE, AddrType opAddrType);

/*下面函数用户调用*/
BOOL MDS_RTU_WriteCoil(void* obj,uint16 modbusAddr,uint8 bit);
BOOL MDS_RTU_WriteInput(void* obj,uint16 modbusAddr,uint8 bit);
BOOL MDS_RTU_WriteCoils(void* obj,uint16 modbusAddr,uint16 numOf, uint16 *bit);
BOOL MDS_RTU_WriteMulInput(void* obj,uint16 modbusAddr,uint16 numOf, uint16 *bit);
BOOL MDS_RTU_WriteHoldReg(void* obj,uint16 modbusAddr,uint16 reg);
BOOL MDS_RTU_WriteHoldRegs(void* obj,uint16 modbusAddr,uint16 numOf, uint16 *reg);
BOOL MDS_RTU_WriteInputReg(void* obj,uint16 modbusAddr,uint16 reg);
BOOL MDS_RTU_WriteInputRegs(void* obj,uint16 modbusAddr,uint16 numOf, uint16 *reg);
BOOL MDS_RTU_ReadCoils(void *obj,uint16 modbusAddr,uint16 numOf,uint8 *res);
BOOL MDS_RTU_ReadInput(void *obj,uint16 modbusAddr,uint16 numOf,uint8 *res);
BOOL MDS_RTU_ReadHoldRegs(void* obj,uint16 modbusAddr,uint16 numOf, uint16 *res);
BOOL MDS_RTU_ReadInputRegs(void* obj,uint16 modbusAddr,uint16 numOf, uint16 *res);


#endif
