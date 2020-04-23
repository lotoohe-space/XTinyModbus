/********************************************************************************
* @File name: MD_RTU_APP.c
* @Author: zspace
* @Version: 1.0
* @Date: 2020-4-10
* @Description: Modbus RTU 用户APP模块
********************************************************************************/

/*********************************头文件包含************************************/
#include "MDM_RTU_APP.h"
#include "MDM_RTU_Fun.h"
#include "MDM_RTU_Serial.h"
#include "MDM_RTU_User_Fun.h"
/*********************************结束******************************************/

/*********************************全局变量************************************/
uint16 regCoilDataMaster0[4]={0};
RegCoilItem regCoilItemMaster0={
	.modbusAddr=0x0000,							/*MODBUS中的地址*/
	.modbusData=regCoilDataMaster0,	/*映射的内存单元*/
	.modbusDataSize=64,							/*映射的大小*/
	.addrType=COILS_TYPE						/*映射的类型*/
};
uint16 regDataMaster1[32]={1,2,3,4,5,6,7,8,9,10,11,12};
RegCoilItem regCoilItemMaster1={
	.modbusAddr=0x0000,							/*MODBUS中的地址*/
	.modbusData=regDataMaster1,			/*映射的内存单元*/
	.modbusDataSize=32,							/*映射的大小*/
	.addrType=HOLD_REGS_TYPE				/*映射的类型*/
};

Modbus_RTU modbus_RTU = {0};
Modbus_RTU_CB modbusRWRTUCB = {0};
Modbus_RTU_CB modbusRWRTUCB1 = {0};
Modbus_RTU_CB modbusRWRTUCB2 = {0};
Modbus_RTU_CB modbusRWRTUCB3 = {0};
Modbus_RTU_CB modbusRWRTUCB4 = {0};
/*********************************结束******************************************/

/*******************************************************
*
* Function name :MDM_RTU_APPInit
* Description        :主机用户APP初始化函数
* Parameter         :无
* Return          : TRUE success , FALSE fail
**********************************************************/
BOOL MDM_RTU_APPInit(void){
	if(MDM_RTU_Init(&modbus_RTU,MDMInitSerial,9600,8,1,0)!=ERR_NONE){
		return FALSE;
	}
	
	if(MDM_RTU_AddMapItem(&modbus_RTU,&regCoilItemMaster0)==FALSE){
		return FALSE;
	}
	if(MDM_RTU_AddMapItem(&modbus_RTU,&regCoilItemMaster1)==FALSE){
		return FALSE;
	}
	
	/*RW控制块*/
	MDM_RTU_CB_Init(&modbusRWRTUCB,&modbus_RTU,0,25000,3);
	MDM_RTU_CB_Init(&modbusRWRTUCB1,&modbus_RTU,0,25000,3);
	MDM_RTU_CB_Init(&modbusRWRTUCB2,&modbus_RTU,0,25000,3);
	MDM_RTU_CB_Init(&modbusRWRTUCB3,&modbus_RTU,0,25000,3);
	MDM_RTU_CB_Init(&modbusRWRTUCB4,&modbus_RTU,0,25000,3);
	return TRUE;
}
uint16	temp=~(0x5555);
uint16	temp2=0x5555;
uint16	temp1=1234;
uint16	data1[]={1,2,3,4,5,6};
#define MD_NB_MODE_TEST 1
/*用户读取数据*/
static void MDM_RTUUserRead(void){
	
	uint16 resTemp;
	#if MD_NB_MODE_TEST
	MDError res;
	res = MDM_RTU_NB_ReadCoil(&modbusRWRTUCB,0x1,0,16);
	if(res != ERR_IDLE){
		if(res != ERR_RW_FIN){/*出现错误*/
			if(res == ERR_RW_OV_TIME_ERR){/*超时了*/
				/*使能重传*/
				MDM_RTU_CB_OverTimeReset(&modbusRWRTUCB);
			}
		}else {
			/*读成功*/
			MDM_RTU_ReadBits(modbusRWRTUCB.pModbus_RTU,0x0000,16, (uint8*)&resTemp,COILS_TYPE);
			resTemp=resTemp;
		}	
	}
	#else 
		
		if(MDM_RTU_ReadCoil(&modbusRWRTUCB,0x1,0x0000,16)==ERR_RW_FIN){
			MDM_RTU_ReadBits(modbusRWRTUCB.pModbus_RTU,0x0000,16, (uint8*)&resTemp,COILS_TYPE);
			resTemp=resTemp;
		}
	#endif

}

static void MDM_RTUUserWrite(void){
	MDError res;
	#if MD_NB_MODE_TEST
	res = MDM_RTU_NB_WriteCoils(&modbusRWRTUCB1,0x1,0,16,(uint8*)(&temp));
	if(res != ERR_IDLE){
		if(res != ERR_RW_FIN){/*出现错误*/
			if(res == ERR_RW_OV_TIME_ERR){/*超时了*/
				/*使能重传*/
				MDM_RTU_CB_OverTimeReset(&modbusRWRTUCB1);
			}
		}
	}
	res = MDM_RTU_NB_WriteCoils(&modbusRWRTUCB4,0x1,0,16,(uint8*)(&temp2));
	if(res != ERR_IDLE){
		if(res != ERR_RW_FIN){/*出现错误*/
			if(res == ERR_RW_OV_TIME_ERR){/*超时了*/
				/*使能重传*/
				MDM_RTU_CB_OverTimeReset(&modbusRWRTUCB4);
			}
		}
	}
	#else 
		MDM_RTU_WriteCoils(&modbusRWRTUCB1,0x1,0,16,(uint8*)(&temp));
		MDM_RTU_WriteCoils(&modbusRWRTUCB4,0x1,0,16,(uint8*)(&temp2));
	#endif
}
/*用户数据的读写*/
static void MDM_RTU_UserUpdate(void){
	MDM_RTUUserRead();
	MDM_RTUUserWrite();
}

/*循环调用*/
void MDM_RTU_Loop(void){
	MDM_RTU_UserUpdate();
}
