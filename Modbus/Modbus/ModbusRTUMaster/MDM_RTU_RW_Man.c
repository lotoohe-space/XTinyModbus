/********************************************************************************
 * @File name: MD_RTU_RW_Man.c
 * @Author: zspace
 * @Email: 1358745329@qq.com
 * @Version: 1.0
 * @Date: 2020-10-22
 * @Description: ????
 ********************************************************************************/

#include "MDM_RTU_RW_Man.h"
#include "MDM_RTU_Fun.h"
#include "MD_RTU_Tool.h"

MDM_RW_Ctrl MDM_RW_CtrlList[MDM_RW_CTRL_LIST_SIZE] = { 0 };

/*******************************************************
 *
 * Function name: MDM_RW_CtrlNew
 * Description: Create a new control block
 * Parameter:
 *         None
 * Return: New control block
 **********************************************************/
static PMDM_RW_Ctrl MDM_RW_CtrlNew(void) {
    uint16 i;
    for (i = 0; i < MDM_RW_CTRL_LIST_SIZE; i++) {
        if (!MD_GET_BIT(MDM_RW_CtrlList[i].flag, 0)) {
            MD_SET_BIT(MDM_RW_CtrlList[i].flag, 0);
            return &(MDM_RW_CtrlList[i]);
        }
    }
    return NULL;
}

/*******************************************************
 *
 * Function name: MDM_RW_CtrlFindByFunAddr
 * Description: Find the control block by the function address
 * Parameter:
 *        @cbFun	A function pointer
 * Return: Found function control block
 ******************************************************/
static PMDM_RW_Ctrl MDM_RW_CtrlFindByFunAddr(MDMSendReadCallBack cbFun) {
    uint16 i;
    for (i = 0; i < MDM_RW_CTRL_LIST_SIZE; i++) {
        if (MD_GET_BIT(MDM_RW_CtrlList[i].flag, 0)) {
            if (MDM_RW_CtrlList[i].MDMSendReadFun == cbFun) {
                return &(MDM_RW_CtrlList[i]);
            }
        }
    }
    return NULL;
}

/*******************************************************
 *
 * Function name: MDM_RW_CtrlDelRW
 * Description: Delete a RW interface
 * Parameter:
 *         @pMDM_RW_Ctrl	Which control block's interface needs to be removed
 * Return: None
 ******************************************************/
void MDM_RW_CtrlDelRW(PMDM_RW_Ctrl pMDM_RW_Ctrl) {
    if (pMDM_RW_Ctrl == NULL) {
        return;
    }
    pMDM_RW_Ctrl->arg = NULL;
    pMDM_RW_Ctrl->flag = 0;
    pMDM_RW_Ctrl->MDMSendReadFun = NULL;
    pMDM_RW_Ctrl->RWCtrlName = NULL;
}

/*******************************************************
 *
 * Function name: MDM_RW_CtrlAddRW
 * Description: Add a read-write
 * Parameter:
 *         @cbFun		A function that is called in a loop
 *         @arg		The parameters passed to the function
 *         @RWCtrlName		Name of the control block
 * Return: Returns a control block to the user
 ******************************************************/
PMDM_RW_Ctrl MDM_RW_CtrlAddRW(MDMSendReadCallBack cbFun, void *arg, const char *RWCtrlName) {
    PMDM_RW_Ctrl pMDM_RW_Ctrl = MDM_RW_CtrlNew();
    if (pMDM_RW_Ctrl == NULL) {
        return NULL;
    }
    pMDM_RW_Ctrl->MDMSendReadFun = cbFun;
    pMDM_RW_Ctrl->RWCtrlName = RWCtrlName;
    pMDM_RW_Ctrl->arg = arg;
    return pMDM_RW_Ctrl;
}

/*******************************************************
 *
 * Function name: MDM_RW_CtrlSetRWOnceFlag
 * Description: Set whether to send once or cyclically
 * Parameter:
 *        @pMDM_RW_Ctrl        A pointer to a control block object
 *				@flag									Whether read only or write once
 * Return: None
 ******************************************************/
void MDM_RW_CtrlSetRWOnceFlag(PMDM_RW_Ctrl pMDM_RW_Ctrl, BOOL flag) {
    if (pMDM_RW_Ctrl == NULL) {
        return;
    }
    if (flag) {
        MD_SET_BIT(pMDM_RW_Ctrl->flag, 1);
    } else {
        MD_CLR_BIT(pMDM_RW_Ctrl->flag, 1);
    }
}

/*******************************************************
 *
 * Function name: MDM_RW_CtrlResetRetranFlag
 * Description: Reset slave offline flag
 * Parameter:
 *        @pMDM_RW_Ctrl        A pointer to a control block object
 * Return: None
 ******************************************************/
void MDM_RW_CtrlResetRetranFlag(PMDM_RW_Ctrl pMDM_RW_Ctrl) {
    if (pMDM_RW_Ctrl == NULL) {
        return;
    }
    MD_CLR_BIT(pMDM_RW_Ctrl->flag, 7);
}

/*******************************************************
 *
 * Function name: MDM_RW_CtrlLoop
 * Description: This function cyclically processes the host bare-metal transceiver control function
 * Parameter:
 *         None
 * Return: None
 ******************************************************/
void MDM_RW_CtrlLoop(void) {
    uint16 i;
    MDM_RW_CtrlErr res;
    for (i = 0; i < MDM_RW_CTRL_LIST_SIZE; i++) {
        if (MD_GET_BIT(MDM_RW_CtrlList[i].flag,0) && (!MD_GET_BIT(MDM_RW_CtrlList[i].flag, 7)) // If the slave is disconnected or a single transmission is completed, it will no longer be traversed
        ) {
            if (MDM_RW_CtrlList[i].MDMSendReadFun == NULL) {
                continue;
            }

            res = MDM_RW_CtrlList[i].MDMSendReadFun(MDM_RW_CtrlList[i].arg); // Loop call
            if (res == RW_ERR) { // Do not send if sending fails
                MD_SET_BIT(MDM_RW_CtrlList[i].flag, 7);
            }
            if (res != RW_NONE) { // Single send and send success or failure
                if (MD_GET_BIT(MDM_RW_CtrlList[i].flag, 1)) { // Single send
                    MD_SET_BIT(MDM_RW_CtrlList[i].flag, 7);
                }
            }
        }
    }
}
