#ifndef _MDM_RTU_USER_FUN_H_
#define _MDM_RTU_USER_FUN_H_

#include "MD_RTU_Type.h"
#include "MD_RTU_MapTable.h"

BOOL MDM_RTU_ReadBits(void* obj,uint16 modbusAddr,uint16 numOf, uint8 *res, AddrType opAddrType,uint8 devAddr);
BOOL MDM_RTU_ReadRegs(void* obj,uint16 modbusAddr,uint16 numOf, uint16 *res, AddrType opAddrType,uint8 devAddr);

#endif
