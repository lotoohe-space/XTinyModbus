/**
* @file 		MD_RTU_Serial.h
* @brief		нч
* @details	нч
* @author		zspace
* @date		2020/3/23
* @version	A001
* @par History:  нч       
*/
#ifndef _MD_RTU_SERIAL_H__
#define _MD_RTU_SERIAL_H__


#include "MD_RTU_Type.h"

void MDSInitSerial(void* obj,uint32 baud,uint8 dataBits,uint8 stopBit,uint8 parity);
void MDSSerialRecvByte(uint8 byte);
void MDSTimeHandler100US(uint32 times);

#endif
