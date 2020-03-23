/**
* @file 		MD_RTU_CRC16.h
* @brief		нч
* @details	нч
* @author		zspace
* @date		2020/3/23
* @version	A001
* @par History:  нч       
*/
#ifndef _MD_RTU_CRC16_H__
#define _MD_RTU_CRC16_H__
#include "MD_RTU_Type.h"

//uint16 MDS_RTU_CRC16_CAL(uint8 *addr,uint16 num);
uint16 crc16_update(uint16 crc, uint8 a);

#endif
