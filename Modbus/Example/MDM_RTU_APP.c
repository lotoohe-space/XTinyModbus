/********************************************************************************
* @File name: MD_RTU_APP.c
* @Author: zspace
* @Emial: 1358745329@qq.com
* @Version: 1.0
* @Date: 2020-4-10
* @Description: Modbus RTU User APP module
* Open source address: https://github.com/lotoohe-space/XTinyModbus
********************************************************************************/

/*********************************HEAD FILE************************************/
#include "MDM_RTU_APP.h"
#include "MDM_RTU_Fun.h"
#include "MDM_RTU_Serial.h"
#include "MDM_RTU_User_Fun.h"
#include "MDM_RTU_RW_Man.h"
/*********************************END******************************************/

/*********************************GLOBAL VARIABLE************************************/
static uint16 mapTableDataMaster0[4]={0};
static MapTableItem mapTableItemMaster0={
	.modbusAddr=0x0000,								/*Address in MODBUS*/
	.modbusData=mapTableDataMaster0,	/*Mapped memory unit*/
	.modbusDataSize=64,								/*The size of the map*/
	.addrType=COILS_TYPE,							/*Type of mapping*/
	.devAddr=1,												/*Which slave is used*/
};
static uint16 regDataMaster1[32]={1,2,3,4,5,6,7,8,9,10,11,12};
static MapTableItem mapTableItemMaster1={
	.modbusAddr=0x0000,								/*Address in MODBUS*/
	.modbusData=regDataMaster1,				/*Mapped memory unit*/
	.modbusDataSize=32,								/*The size of the map*/
	.addrType=HOLD_REGS_TYPE,					/*Type of mapping*/
	.devAddr=1,												/*Which slave is used*/
};
static uint16 regDataMaster2[32]={1,2,3,4,5,6,7,8,9,10,11,12};
static MapTableItem mapTableItemMaster2={
	.modbusAddr=0x0000,								/*Address in MODBUS*/
	.modbusData=regDataMaster2,				/*Mapped memory unit*/
	.modbusDataSize=32,								/*The size of the map*/
	.addrType=HOLD_REGS_TYPE,					/*Type of mapping*/
	.devAddr=2,												/*Which slave is used*/
};

Modbus_RTU modbus_RTU = {0};

Modbus_RTU_CB modbusRWRTUCB = {0};
Modbus_RTU_CB modbusRWRTUCB1 = {0};
Modbus_RTU_CB modbusRWRTUCB2 = {0};
Modbus_RTU_CB modbusRWRTUCB3 = {0};
Modbus_RTU_CB modbusRWRTUCB4 = {0};

static uint8 MDSRecvQueueData[MDM_RTU_QUEUE_SIZE+1]={0};
/*********************************END******************************************/

/*********************************FUNCTION DECLARATION************************************/
static MDM_RW_CtrlErr MDM_RTU_NB_RW_CtrlTest0(void* arg);
static MDM_RW_CtrlErr MDM_RTU_NB_RW_CtrlTest1(void* arg);
/*********************************END******************************************/

/*******************************************************
*
* Function name :MDM_RTU_APPInit
* Description        :Host user APP initialization function
* Parameter         :None
* Return          : TRUE success , FALSE fail
**********************************************************/
BOOL MDM_RTU_APPInit(void){
	if(MDM_RTU_Init(&modbus_RTU,MDMInitSerial,9600,8,1,0)!=ERR_NONE){
		return FALSE;
	}
	MDM_RTU_QueueInit(&modbus_RTU,MDSRecvQueueData,sizeof(MDSRecvQueueData));
	
	if(MDM_RTU_AddMapItem(&modbus_RTU,&mapTableItemMaster0)==FALSE){
		return FALSE;
	}
	if(MDM_RTU_AddMapItem(&modbus_RTU,&mapTableItemMaster1)==FALSE){
		return FALSE;
	}
	if(MDM_RTU_AddMapItem(&modbus_RTU,&mapTableItemMaster2)==FALSE){
		return FALSE;
	}
	/*RW control block, the user controls the read and write time interval, retransmission timeout and retransmission timeout times*/
	MDM_RTU_CB_Init(&modbusRWRTUCB,&modbus_RTU,0,30000,3);
	MDM_RTU_CB_Init(&modbusRWRTUCB1,&modbus_RTU,0,30000,3);
	MDM_RTU_CB_Init(&modbusRWRTUCB2,&modbus_RTU,0,30000,3);
	MDM_RTU_CB_Init(&modbusRWRTUCB3,&modbus_RTU,0,30000,3);
	MDM_RTU_CB_Init(&modbusRWRTUCB4,&modbus_RTU,0,30000,3);
	
	
	MDM_RW_CtrlAddRW(MDM_RTU_NB_RW_CtrlTest0,NULL,"rw_test_0");
	MDM_RW_CtrlAddRW(MDM_RTU_NB_RW_CtrlTest1,NULL,"rw_test_1");
	
	return TRUE;
}
uint16	temp=0xAAAA;
uint16	temp2=0x5555;
uint16	temp1=1234;
uint16	data1[]={1,2,3,4,5,6};
uint16	data2[]={6,5,4,3,2,1};
#define MD_NB_MODE_TEST 1
/*User read data*/
static void MDM_RTUUserRead(void){
	
	uint16 resTemp;
	#if MD_NB_MODE_TEST
	MDError res;
	res = MDM_RTU_NB_ReadCoil(&modbusRWRTUCB,0x1,0,16);
	if(res != ERR_IDLE){
		if(res != ERR_RW_FIN){/*An error occurred*/
			if(res == ERR_RW_OV_TIME_ERR){/*Timed out*/
				/*Enable retransmission*/
				MDM_RTU_CB_OverTimeReset(&modbusRWRTUCB);
			}
		}else {
			/*Read successfully*/
			MDM_RTU_ReadBits(modbusRWRTUCB.pModbus_RTU,0x0000,16, (uint8*)&resTemp,COILS_TYPE,0x1);
			resTemp=resTemp;
		}	
	}
	#else 
		
		if(MDM_RTU_ReadCoil(&modbusRWRTUCB,0x1,0x0000,16)==ERR_RW_FIN){
			MDM_RTU_ReadBits(modbusRWRTUCB.pModbus_RTU,0x0000,16, (uint8*)&resTemp,COILS_TYPE,0x1);
			resTemp=resTemp;
		}
	#endif

}

static void MDM_RTUUserWrite(void){
	MDError res;
	#if MD_NB_MODE_TEST
	res = MDM_RTU_NB_WriteCoils(&modbusRWRTUCB1,0x1,0,16,(uint8*)(&temp));
	if(res != ERR_IDLE){
		if(res != ERR_RW_FIN){/*An error occurred*/
			if(res == ERR_RW_OV_TIME_ERR){/*Timed out*/
				/*Enable retransmission*/
				MDM_RTU_CB_OverTimeReset(&modbusRWRTUCB1);
			}
		}
	}
	res = MDM_RTU_NB_WriteCoils(&modbusRWRTUCB4,0x1,0,16,(uint8*)(&temp2));
	if(res != ERR_IDLE){
		if(res != ERR_RW_FIN){/*An error occurred*/
			if(res == ERR_RW_OV_TIME_ERR){/*Timed out*/
				/*Enable retransmission*/
				MDM_RTU_CB_OverTimeReset(&modbusRWRTUCB4);
			}
		}
	}
	#else 
		MDM_RTU_WriteCoils(&modbusRWRTUCB1,0x1,0,16,(uint8*)(&temp));
		MDM_RTU_WriteCoils(&modbusRWRTUCB4,0x1,0,16,(uint8*)(&temp2));
	#endif
}
/*Send control function*/
static MDM_RW_CtrlErr MDM_RTU_NB_RW_CtrlTest0(void* arg){
	MDError res;
	#if MD_NB_MODE_TEST
	res = MDM_RTU_NB_WriteRegs(&modbusRWRTUCB2,0x1,0,6,data1);
	if(res != ERR_IDLE){
		if(res != ERR_RW_FIN){/*An error occurred*/
			if(res == ERR_RW_OV_TIME_ERR){/*Timed out*/
				/*Enable retransmission*/
				MDM_RTU_CB_OverTimeReset(&modbusRWRTUCB1);
				return RW_ERR;
			}
		}else{
			return RW_OK; 
		}
	}
	#endif
	return RW_NONE;
}
/*Send control function*/
static MDM_RW_CtrlErr MDM_RTU_NB_RW_CtrlTest1(void* arg){
	MDError res;
	#if MD_NB_MODE_TEST
	res = MDM_RTU_NB_WriteRegs(&modbusRWRTUCB3,0x1,0,6,data2);
	if(res != ERR_IDLE){
		if(res != ERR_RW_FIN){/*An error occurred*/
			if(res == ERR_RW_OV_TIME_ERR){/*Timed out*/
				/*Enable retransmission*/
				MDM_RTU_CB_OverTimeReset(&modbusRWRTUCB1);
				return RW_ERR;
			} 
		}else{
			return RW_OK; 
		}
	}
	#endif
	return RW_NONE;
}


/*User data reading and writing*/
static void MDM_RTU_UserUpdate(void){
	MDM_RTUUserRead();
	MDM_RTUUserWrite();
}

/*Loop call*/
void MDM_RTU_Loop(void){
	MDM_RTU_UserUpdate();
}
