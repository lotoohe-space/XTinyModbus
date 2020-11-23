/********************************************************************************
* @File name: RTOS_Core.h
* @Author: zspace
* @Version: 1.0
* @Date: 2020-4-10
* @Description: RTOS Core 模块
********************************************************************************/
#ifndef _RTOS_CORE_H__
#define _RTOS_CORE_H__
#ifdef __cplusplus
 extern "C" {
#endif 
/*********************************头文件包含************************************/
#include "RTOS_type.h"
/*********************************结束******************************************/

/*********************************函数申明************************************/
void 		StartRTOS(void);
uint32 	DisableInterrupt(void);
void 		EnableInterrupt(uint32 temp);
void 		OpenSchedule(void);
/*********************************结束******************************************/

#ifdef __cplusplus
}
#endif
#endif
