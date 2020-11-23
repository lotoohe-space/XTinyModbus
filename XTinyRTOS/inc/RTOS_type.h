#ifndef _RTOS_TYPE_H__
#define _RTOS_TYPE_H__
#ifdef __cplusplus
 extern "C" {
#endif 
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
#ifdef __cplusplus
}
#endif
#endif
