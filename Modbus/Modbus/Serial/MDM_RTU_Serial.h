/********************************************************************************
* @File name: MD_RTU_Serial.h
* @Author: zspace
* @Emial: 1358745329@qq.com
* @Version: 1.0
* @Date: 2020-4-10
* @Description: Modbus RTU Serial related modules
********************************************************************************/

#ifndef _MDM_RTU_SERIAL_H__
#define _MDM_RTU_SERIAL_H__

/*********************************HEAD FILE************************************/
#include "MD_RTU_Type.h"
#include "MD_RTU_Config.h"
/*********************************END******************************************/

/*********************************Function declaration************************************/
/*Hardware initialization function, you can initialize the serial port here*/
void MDMInitSerial(void* obj,uint32 baud,uint8 dataBits,uint8 stopBit,uint8 parity);
/*Bsp layer interrupt receiving call this function*/
void MDMSerialRecvByte(uint8 byte);
/*Call this function in the timer*/
void MDMTimeHandler100US(uint32 times);

void MDMSerialSendBytes(uint8 *bytes,uint16 num);
void MDMSerialSWRecv_Send(uint8 mode);
/*********************************END******************************************/

#endif
