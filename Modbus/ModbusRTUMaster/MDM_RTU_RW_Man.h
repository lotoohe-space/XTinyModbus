#ifndef _MDM_RTU_RW_MAN_H__
#define _MDM_RTU_RW_MAN_H__

#include "MD_RTU_Type.h"
#include "MD_RTU_Config.h"

typedef enum{
	RW_NONE=0,
	RW_ERR,
	RW_OK
}MDM_RW_CtrlErr;

typedef MDM_RW_CtrlErr (*MDMSendReadCallBack)(void*arg);

/*主机的非阻塞模式发送控制器*/
typedef struct{
	
	MDMSendReadCallBack MDMSendReadFun;	/*循环调用的读写函数*/
	void *arg;													/*传递的参数*/
	const char *RWCtrlName;							/*发送控制的名字*/ 
	uint8	flag;													/*标志 
																			bit:0 是否被使用 
																			bit:1 单次发送 
																			bit:7 从机掉线或者单次发送完成
																			*/
	
}*PMDM_RW_Ctrl,MDM_RW_Ctrl;

PMDM_RW_Ctrl MDM_RW_CtrlAddRW(MDMSendReadCallBack cbFun,void *arg,const char *RWCtrlName);
void MDM_RW_CtrlSetRWOnceFlag(PMDM_RW_Ctrl pMDM_RW_Ctrl,BOOL flag);
void MDM_RW_CtrlResetRetranFlag(PMDM_RW_Ctrl pMDM_RW_Ctrl);
void MDM_RW_CtrlSetRWOnceFlag(PMDM_RW_Ctrl pMDM_RW_Ctrl,BOOL flag);
void MDM_RW_CtrlLoop(void);

#endif
