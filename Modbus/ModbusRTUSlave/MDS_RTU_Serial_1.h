/********************************************************************************
* @File name: MD_RTU_Serial_1.c
* @Author: zspace
* @Emial: 1358745329@qq.com
* @Version: 1.0
* @Date: 2020-4-10
* @Description: Modbus RTU Serial related modules
********************************************************************************/
#ifndef _MD_RTU_SERIAL_1_H__
#define _MD_RTU_SERIAL_1_H__

/*********************************HEAD FILE************************************/
#include "MD_RTU_Type.h"
/*********************************END******************************************/

/*********************************FUNCTION DECLARATION************************************/
void MDSInitSerial_1(void* obj,uint32 baud,uint8 dataBits,uint8 stopBit,uint8 parity);
void MDSSerialRecvByte_1(uint8 byte);
void MDSTimeHandler100US_1(void);
/*********************************END******************************************/

#endif
