/**
* @file 		MD_RTU_Type.h
* @brief		None
* @details	None
* @author		zspace
* @date		2020/3/23
* @version	A001
* @par History:  None       
*/
#ifndef _MD_RTU_TYPE_H__
#define _MD_RTU_TYPE_H__

#include "MD_RTU_Config.h"

#if MD_RTU_USED_OS
#include "RTOS_Type.h"
#else
typedef unsigned char uchar;
typedef unsigned char uint8;
typedef char int8;
typedef unsigned int uint32;
typedef int int32;
typedef unsigned short uint16;
typedef short int16;

typedef uint8 BOOL;
#define FALSE 0
#define TRUE 1

#ifndef NULL
#define NULL 0
#endif

#define BOOL uint8
#endif

#endif
