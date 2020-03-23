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

BOOL MDS_RTU_ReadBits(void* obj,uint16 modbusAddr,uint16 numOf, uint8 *res);
BOOL MDS_RTU_ReadRegs(void* obj,uint16 modbusAddr,uint16 numOf, uint16 *res);
/*下面4个函数除了用户调用，还被MD_RTU_Fun.h中调用*/
BOOL MDS_RTU_WriteBit(void* obj,uint16 modbusAddr,uint8 bit);
BOOL MDS_RTU_WriteBits(void* obj,uint16 modbusAddr,uint16 numOf, uint16 *bit);
BOOL MDS_RTU_WriteReg(void* obj,uint16 modbusAddr,uint16 reg);
BOOL MDS_RTU_WriteRegs(void* obj,uint16 modbusAddr,uint16 numOf, uint16 *reg,uint8 isBigE);

#endif
