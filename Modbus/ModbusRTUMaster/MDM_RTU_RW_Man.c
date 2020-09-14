#include "MDM_RTU_RW_Man.h"
#include "MDM_RTU_Fun.h"
#include "MD_RTU_Tool.h"

MDM_RW_Ctrl MDM_RW_CtrlList[MDM_RW_CTRL_LIST_SIZE]={0};

static PMDM_RW_Ctrl MDM_RW_CtrlNew(void){
	uint16 i;
	for(i=0;i<MDM_RW_CTRL_LIST_SIZE;i++){
		if(!MD_GET_BIT(MDM_RW_CtrlList[i].flag,0)){
			MD_SET_BIT(MDM_RW_CtrlList[i].flag,0);
			return &(MDM_RW_CtrlList[i]);
		}
	}
	return NULL;
}
static PMDM_RW_Ctrl MDM_RW_CtrlFindByFunAddr(MDMSendReadCallBack cbFun){
	uint16 i;
	for(i=0;i<MDM_RW_CTRL_LIST_SIZE;i++){
		if(MD_GET_BIT(MDM_RW_CtrlList[i].flag,0)){
			if(MDM_RW_CtrlList[i].MDMSendReadFun==cbFun){
				return &(MDM_RW_CtrlList[i]);
			}
		}
	}
	return NULL;
}
/*删除一个RW接口*/
void MDM_RW_CtrlDelRW(PMDM_RW_Ctrl pMDM_RW_Ctrl){
	if(pMDM_RW_Ctrl==NULL){return ;}
	pMDM_RW_Ctrl->arg=NULL;
	pMDM_RW_Ctrl->flag=0;
	pMDM_RW_Ctrl->MDMSendReadFun=NULL;
	pMDM_RW_Ctrl->RWCtrlName=NULL;
}
/*添加一个读写*/
PMDM_RW_Ctrl MDM_RW_CtrlAddRW(MDMSendReadCallBack cbFun,void *arg,const char *RWCtrlName){
	PMDM_RW_Ctrl pMDM_RW_Ctrl=MDM_RW_CtrlNew();
	if(pMDM_RW_Ctrl==NULL){
		return NULL;
	}
	pMDM_RW_Ctrl->MDMSendReadFun=cbFun;
	pMDM_RW_Ctrl->RWCtrlName=RWCtrlName;
	pMDM_RW_Ctrl->arg=arg;
	return pMDM_RW_Ctrl;
}
/*设置是否发送一次，或者循环发送*/
void MDM_RW_CtrlSetRWOnceFlag(PMDM_RW_Ctrl pMDM_RW_Ctrl,BOOL flag){
	if(pMDM_RW_Ctrl==NULL){return ;}
	if(flag){
		MD_SET_BIT(pMDM_RW_Ctrl->flag,1);
	}else{
		MD_CLR_BIT(pMDM_RW_Ctrl->flag,1);
	}
}
/*复位从机掉线标志位*/
void MDM_RW_CtrlResetRetranFlag(PMDM_RW_Ctrl pMDM_RW_Ctrl){
	if(pMDM_RW_Ctrl==NULL){return ;}
	MD_CLR_BIT(pMDM_RW_Ctrl->flag,7);
}

/*******************************************************
*
* Function name :MDM_RW_CtrlLoop
* Description        :该函数对主机裸机收发控制功能进行循环处理
* Parameter         :
*        						无
* Return          : 无
**********************************************************/
void MDM_RW_CtrlLoop(void){
	uint16 i;
	MDM_RW_CtrlErr res;
	for(i=0;i<MDM_RW_CTRL_LIST_SIZE;i++){
		if(MD_GET_BIT(MDM_RW_CtrlList[i].flag,0) &&
			(!MD_GET_BIT(MDM_RW_CtrlList[i].flag,7))/*从机掉线或者单次发送完成则不再被遍历*/
		){
			if(MDM_RW_CtrlList[i].MDMSendReadFun==NULL){
				continue;
			}
			
			res=MDM_RW_CtrlList[i].MDMSendReadFun(MDM_RW_CtrlList[i].arg);/*循环调用*/
			if(res==RW_ERR){/*发送失败则不发送*/
				MD_SET_BIT(MDM_RW_CtrlList[i].flag,7);
			}
			if(res!=RW_NONE){/*单次发送and发送成功或者失败*/
				if(MD_GET_BIT(MDM_RW_CtrlList[i].flag,1)){/*单次发送*/
					MD_SET_BIT(MDM_RW_CtrlList[i].flag,7);
				}
			}
		}
	}
}




