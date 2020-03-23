/**
* @file 		MD_RTU_CRC16.c
* @brief		无
* @details	无
* @author		zspace
* @date		2020/3/23
* @version	A001
* @par History:  无       
*/
#include "MD_RTU_CRC16.h"

///*CRC16校验*/
//uint16 MDS_RTU_CRC16_CAL(uint8 *addr,uint16 num)
//{
//	uint16 crc=0xFFFF;
//	for(;num > 0;num--)
//	{
//		crc = crc^(*addr++);
//		for(int i = 0;i<8;i++)
//		{
//				if(crc & 0x0001)
//						crc = (crc>>1)^0xa001;
//				else
//						crc >>= 1;
//		}
//	}
//  return crc;
//}
uint16 crc16_update(uint16 crc, uint8 a)
{
  int i;

  crc ^= a;
  for (i = 0; i < 8; ++i)
  {
    if (crc & 1)
      crc = (crc >> 1) ^ 0xA001;
    else
      crc = (crc >> 1);
  }

  return crc;
}

