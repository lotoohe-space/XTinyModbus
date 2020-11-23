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

/*Host's non-blocking mode sending controller*/
typedef struct{
	
	MDMSendReadCallBack MDMSendReadFun;	/*Read and write functions called cyclically*/
	void *arg;													/*parameter*/
	const char *RWCtrlName;							/*Send control name*/ 
	uint8	flag;													/*Flag 
																				bit:0 Is it used 
																				bit:1 Single send 
																				bit:7 The slave is disconnected or a single transmission is completed.
																			*/
}*PMDM_RW_Ctrl,MDM_RW_Ctrl;

PMDM_RW_Ctrl MDM_RW_CtrlAddRW(MDMSendReadCallBack cbFun,void *arg,const char *RWCtrlName);
void MDM_RW_CtrlSetRWOnceFlag(PMDM_RW_Ctrl pMDM_RW_Ctrl,BOOL flag);
void MDM_RW_CtrlResetRetranFlag(PMDM_RW_Ctrl pMDM_RW_Ctrl);
void MDM_RW_CtrlSetRWOnceFlag(PMDM_RW_Ctrl pMDM_RW_Ctrl,BOOL flag);
void MDM_RW_CtrlLoop(void);

#endif
