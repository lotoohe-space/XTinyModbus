/********************************************************************************
* @File name: MD_RTU_APP.c
* @Author: zspace
* @Version: 1.0
* @Date: 2020-4-10
* @Description: Modbus RTU �û�APPģ��
********************************************************************************/

/*********************************ͷ�ļ�����************************************/
#include "MDM_RTU_APP.h"
#include "MDM_RTU_Fun.h"
#include "MDM_RTU_Serial.h"
#include "MDM_RTU_User_Fun.h"
/*********************************����******************************************/

/*********************************ȫ�ֱ���************************************/
uint16 regCoilDataMaster0[4]={0};
RegCoilItem regCoilItemMaster0={
	.modbusAddr=0x0000,							/*MODBUS�еĵ�ַ*/
	.modbusData=regCoilDataMaster0,	/*ӳ����ڴ浥Ԫ*/
	.modbusDataSize=64,							/*ӳ��Ĵ�С*/
	.addrType=COILS_TYPE						/*ӳ�������*/
};
uint16 regDataMaster1[32]={1,2,3,4,5,6,7,8,9,10,11,12};
RegCoilItem regCoilItemMaster1={
	.modbusAddr=0x0000,							/*MODBUS�еĵ�ַ*/
	.modbusData=regDataMaster1,			/*ӳ����ڴ浥Ԫ*/
	.modbusDataSize=32,							/*ӳ��Ĵ�С*/
	.addrType=HOLD_REGS_TYPE				/*ӳ�������*/
};

Modbus_RTU modbus_RTU = {0};
Modbus_RTU_CB modbusRWRTUCB = {0};
Modbus_RTU_CB modbusRWRTUCB1 = {0};
Modbus_RTU_CB modbusRWRTUCB2 = {0};
Modbus_RTU_CB modbusRWRTUCB3 = {0};
Modbus_RTU_CB modbusRWRTUCB4 = {0};
/*********************************����******************************************/

/*******************************************************
*
* Function name :MDM_RTU_APPInit
* Description        :�����û�APP��ʼ������
* Parameter         :��
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
	
	/*RW���ƿ�*/
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
/*�û���ȡ����*/
static void MDM_RTUUserRead(void){
	
	uint16 resTemp;
	#if MD_NB_MODE_TEST
	MDError res;
	res = MDM_RTU_NB_ReadCoil(&modbusRWRTUCB,0x1,0,16);
	if(res != ERR_IDLE){
		if(res != ERR_RW_FIN){/*���ִ���*/
			if(res == ERR_RW_OV_TIME_ERR){/*��ʱ��*/
				/*ʹ���ش�*/
				MDM_RTU_CB_OverTimeReset(&modbusRWRTUCB);
			}
		}else {
			/*���ɹ�*/
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
		if(res != ERR_RW_FIN){/*���ִ���*/
			if(res == ERR_RW_OV_TIME_ERR){/*��ʱ��*/
				/*ʹ���ش�*/
				MDM_RTU_CB_OverTimeReset(&modbusRWRTUCB1);
			}
		}
	}
	res = MDM_RTU_NB_WriteCoils(&modbusRWRTUCB4,0x1,0,16,(uint8*)(&temp2));
	if(res != ERR_IDLE){
		if(res != ERR_RW_FIN){/*���ִ���*/
			if(res == ERR_RW_OV_TIME_ERR){/*��ʱ��*/
				/*ʹ���ش�*/
				MDM_RTU_CB_OverTimeReset(&modbusRWRTUCB4);
			}
		}
	}
	#else 
		MDM_RTU_WriteCoils(&modbusRWRTUCB1,0x1,0,16,(uint8*)(&temp));
		MDM_RTU_WriteCoils(&modbusRWRTUCB4,0x1,0,16,(uint8*)(&temp2));
	#endif
}
/*�û����ݵĶ�д*/
static void MDM_RTU_UserUpdate(void){
	MDM_RTUUserRead();
	MDM_RTUUserWrite();
}

/*ѭ������*/
void MDM_RTU_Loop(void){
	MDM_RTU_UserUpdate();
}