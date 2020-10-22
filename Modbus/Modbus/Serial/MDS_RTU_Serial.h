/********************************************************************************
* @File name: MD_RTU_Serial.c
* @Author: zspace
* @Version: 1.0
* @Date: 2020-4-10
* @Description: Modbus RTU serial port related modules
********************************************************************************/
#ifndef _MD_RTU_SERIAL_H__
#define _MD_RTU_SERIAL_H__

/*********************************HEAD FILE************************************/
#include "MD_RTU_Type.h"
/*********************************END******************************************/

/*********************************FUNCTION DECLARATION*************************/
void MDSInitSerial(void* obj,uint32 baud,uint8 dataBits,uint8 stopBit,uint8 parity);
void MDSSerialRecvByte(uint8 byte);
void MDSTimeHandler100US(void);
/*********************************END******************************************/

#endif
