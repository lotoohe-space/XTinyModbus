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

typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;

#ifndef NULL
#define NULL 0L
#endif

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE (!TRUE)
#endif

#define BOOL uint8

#endif
