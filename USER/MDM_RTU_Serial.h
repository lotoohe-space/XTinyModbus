#ifndef _MDM_RTU_SERIAL_H__
#define _MDM_RTU_SERIAL_H__

#include "MD_RTU_Type.h"

/*硬件初始化函数，可在此初始化串口*/
void MDMInitSerial(void* obj,uint32 baud,uint8 dataBits,uint8 stopBit,uint8 parity);
/*bsp层中断接收调用这个函数*/
void MDMSerialRecvByte(uint8 byte);
/*定时器中调用该函数*/
void MDMTimeHandler100US(uint32 times);

#endif
