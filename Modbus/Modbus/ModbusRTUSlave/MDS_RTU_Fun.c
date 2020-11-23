/********************************************************************************
* @File name: MD_RTU_Fun.c
* @Author: zspace
* @Emial: 1358745329@qq.com
* @Version: 1.0
* @Date: 2020-4-10
* @Description: Modbus RTU Slave slave receiving function.
********************************************************************************/

/*********************************HEAD FILE************************************/
#include "MDS_RTU_Fun.h"
#include "MD_RTU_CRC16.h"
#include "MDS_RTU_Serial.h"
#include "MDS_RTU_User_Fun.h"
/*********************************END******************************************/

/**********************************FUNCTION DECLARATION*************************************/
void MDS_RTU_RecvByte(void *obj,uint8 byte);
void MDS_RTU_TimeHandler(void *obj,uint32 times);

static void MDS_RTU_SendErrorCode(PModbusS_RTU pModbus_RTU,ANLCode anlCode,ErrorCode errCode);
uint8 MDS_RTU_ReadDataProcess(PModbusS_RTU pModbus_RTU,uint16 reg,uint16 regNum,uint8 funCode);
uint8 MDS_RTU_WriteDataProcess(PModbusS_RTU pModbus_RTU,uint16 reg,uint16 regNum,uint8 funCode,uint16* data,uint8 byteCount);
/*********************************END******************************************/

/*******************************************************
*
* Function name :MDS_RTU_Init
* Description        :Initialize a slave
* Parameter         :
*        @pModbusRTU            Slave structure pointer
*        @mdRTUSerialInitFun    Hardware serial port initialization function  
*        @salveAddr            Slave address
*        @baud            Baud rate
*        @dataBits            Data bit
*        @stopBits           Stop bit
*        @parity       Parity bit
* Return          : None
**********************************************************/
void MDS_RTU_Init(PModbusS_RTU pModbusRTU,MD_RTU_SerialInit mdRTUSerialInitFun,uint8 salveAddr,
	uint32 baud,uint8 dataBits,uint8 stopBits,uint8 parity){
	uint8 i;
	float T;
	if(pModbusRTU==NULL){
		return ;
	}
	/*Init is queue.*/
	pModbusRTU->mdMsgSqQueue.data=NULL;
	pModbusRTU->mdMsgSqQueue.maxVal=0;
	pModbusRTU->mdSqQueue.data=NULL;
	pModbusRTU->mdSqQueue.maxVal=0;
	
	pModbusRTU->salveAddr=salveAddr;
	pModbusRTU->serialReadCount=0;
#if MDS_USE_SEND_CACHE
	pModbusRTU->serialSendCount=0;
#endif
	for(i=0;i<MDS_REG_COIL_ITEM_NUM;i++){
		pModbusRTU->pMapTableList[i] = NULL;
	}
	
	TO_MDBase(pModbusRTU)->mdRTUTimeHandlerFunction=MDS_RTU_TimeHandler;
	TO_MDBase(pModbusRTU)->mdRTURecByteFunction=MDS_RTU_RecvByte;
	TO_MDBase(pModbusRTU)->mdRTUSendBytesFunction=NULL;
	TO_MDBase(pModbusRTU)->mdRTURecSendConv=NULL;
	
	pModbusRTU->mdsWriteFun=NULL;
	pModbusRTU->lastTimesTick=0xFFFFFFFF;
	pModbusRTU->lastSendTimes=0x00000000;
	pModbusRTU->timesTick=0x00000000;
	
	T=(1.0/(float)baud)*100000;// 100us
	uint16 time=0;
	time=T*(dataBits+(parity?1:0));
	if(stopBits==0){
		time+=T;
	}else if(stopBits==1){
		time+=T*1.5f;
	}else if(stopBits==2){
		time+=T*2;
	}
	pModbusRTU->frameIntervalTime=time;/*This parameter needs to be set according to the baud rate*/
	
	pModbusRTU->CRC16Update=0xFFFF;
	
	if(mdRTUSerialInitFun!=NULL){
		mdRTUSerialInitFun(pModbusRTU,baud, dataBits,stopBits,parity);
	}
	return ;
}
/**
* @brief This function init queue.
* @param[in] recvQueueData ????
* @param[in] recvQueueSize ????
* @param[in] msgProcessQueueData	????
* @param[in] msgProcessQueueSize	????
* @result None
*/
void MDS_RTU_QueueInit(PModbusS_RTU pModbus_RTU,
	uint8* recvQueueData,
	uint16  recvQueueSize,
	uint8* msgProcessQueueData,
	uint16 msgProcessQueueSize
){
	if(pModbus_RTU==NULL){
		return ;
	}
	MDInitQueue(&(pModbus_RTU->mdSqQueue),recvQueueData,recvQueueSize);
	MDInitQueue(&(pModbus_RTU->mdMsgSqQueue),msgProcessQueueData,msgProcessQueueSize);
}
/*******************************************************
*
* Function name :MDS_RTU_SetWriteListenFun
* Description        :This function can set a callback function, when the master writes the slave address, the set function will be called.
* Parameter         :
*        @pModbus_RTU   Object pointer of slave
*        @wFun    Set callback function
* Return          : нч
**********************************************************/
void MDS_RTU_SetWriteListenFun(PModbusS_RTU pModbus_RTU,MDSWriteFunciton wFun){
	if(pModbus_RTU==NULL){return ;}
	pModbus_RTU->mdsWriteFun=wFun;
}
/*******************************************************
*
* Function name :MDS_RTU_TimeHandler
* Description        :This function needs to be called in the timer interrupt, the interrupt interval time is 100US.
* Parameter         :
*        @obj            Object pointer of slave
* Return          : None
**********************************************************/
void MDS_RTU_TimeHandler(void *obj,uint32 times){
	uint32 tempTick=0;
	uint8 overFlag=0;
	PModbusS_RTU pModbusRTU=obj;
	if(!pModbusRTU){ return; }
	
	//pModbusRTU->timesTick++;
	pModbusRTU->timesTick=times;
	
	if(pModbusRTU->timesTick==0xFFFFFFFF){/*Overflowed pModbusRTU->lastTimesTick==0xFFFFFFFF*/
		tempTick=0xFFFFFFFF-pModbusRTU->lastSendTimes;
		pModbusRTU->timesTick=tempTick; /*System time offset*/
		pModbusRTU->lastSendTimes=0;/*Clear the last sending time*/
		overFlag=1;
	}
	
	if(pModbusRTU->lastTimesTick==0xFFFFFFFF){return ;}/*Has started receiving packets*/
	if(overFlag){
		pModbusRTU->timesTick += 0xFFFFFFFF-pModbusRTU->lastTimesTick;/*Time offset when sending*/
		pModbusRTU->lastTimesTick = tempTick; 
	}
	if((pModbusRTU->timesTick - pModbusRTU->lastTimesTick >= pModbusRTU->frameIntervalTime)){
		uint16 msgLen;
		uint16 i;
		uint8	 byte;
		if(pModbusRTU->CRC16Update!=0x0000){
			/*CRC error*/
			MDResetQueue(&(pModbusRTU->mdSqQueue));
			pModbusRTU->lastTimesTick=0xFFFFFFFF;
			return ;
		}
		/*End of one frame*/
		/*The messages of the data processing queue are moved to the message processing queue*/
		msgLen=MDQueueLength(&(pModbusRTU->mdSqQueue));
		MDenQueue(&(pModbusRTU->mdMsgSqQueue),&msgLen,UINT8_TYPE);/*Save frame length*/
		for(i=0;i<msgLen;i++){
			/*take out*/
			if(MDdeQueue(&(pModbusRTU->mdSqQueue),&byte,UINT8_TYPE)==FALSE){return ;}
			/*put in*/
			if(MDenQueue(&(pModbusRTU->mdMsgSqQueue),&byte,UINT8_TYPE)==FALSE){return ;}
		}
		pModbusRTU->lastTimesTick=0xFFFFFFFF;
	}
}

/*******************************************************
*
* Function name :MDS_RTU_RecvByte
* Description        :This function is called in the serial port interrupt, when a byte is received, this function is called
* Parameter         :
*        @obj            Object pointer of slave
*        @byte    Single byte received
* Return          : None
**********************************************************/
void MDS_RTU_RecvByte(void *obj,uint8 byte){
	PModbusS_RTU pModbusRTU=obj;
	if(!pModbusRTU){ return; }
	/*Put in the queue here*/
	if(MDenQueue(&(pModbusRTU->mdSqQueue),&byte,UINT8_TYPE)==FALSE){
		return ;
	}
	if(pModbusRTU->lastTimesTick==0xFFFFFFFF){
		pModbusRTU->CRC16Update=0xFFFF;
	}
	pModbusRTU->CRC16Update=MD_CRC16Update(pModbusRTU->CRC16Update,byte);
	/*Save the timestamp of the last character received*/
	pModbusRTU->lastTimesTick=pModbusRTU->timesTick;
}
/*******************************************************
*
* Function name :MDS_RTU_AddMapItem
* Description        :This function adds a mapping record to the discrete mapping table.
* Parameter         :
*        @obj            Object pointer of slave
*        @byte    Mapping item added
* Return          : None
**********************************************************/
BOOL MDS_RTU_AddMapItem(PModbusS_RTU pModbusRTU,PMapTableItem pMapTableItem){
	if(pModbusRTU==NULL ||pMapTableItem==NULL){
			return FALSE;
	}
	return MapTableAdd(pModbusRTU->pMapTableList, pMapTableItem,MDS_REG_COIL_ITEM_NUM);
}
#if	!MDS_USE_SEND_CACHE 
/*******************************************************
*
* Function name :MDS_RTU_SendByte
* Description        :Slave sends a byte
* Parameter         :
*        @pModbusRTU  Object pointer of slave
*        @byte    Bytes to be sent
* Return          : TRUE success , FALSE fail
**********************************************************/
static void MDS_RTU_SendByte(PModbusS_RTU pModbusRTU,uint8 byte){
	if(!pModbusRTU){ return; }
	TO_MDBase(pModbusRTU)->mdRTUSendBytesFunction(&byte,1);
}
#endif
/*******************************************************
*
* Function name :MDS_RTU_SerialProcess
* Description        :This function is called internally to get a packet data received.
* Parameter         :
*        @pModbus_RTU  Object pointer of slave
* Return          : None
**********************************************************/
static BOOL MDS_RTU_SerialProcess(PModbusS_RTU pModbus_RTU){
	uint8 byte;
	uint8 recvLen;
	uint16 i;
	if(!pModbus_RTU){return FALSE;}
	
	if(MDdeQueue(&(pModbus_RTU->mdMsgSqQueue),&recvLen,UINT8_TYPE)==FALSE){
		return FALSE;
	}
	for(i=0;i<recvLen;i++){
		/*Read a byte from the queue*/
		if(MDdeQueue(&(pModbus_RTU->mdMsgSqQueue),&byte,UINT8_TYPE)==FALSE){
			return FALSE;
		}
		if(pModbus_RTU->serialReadCount>MDS_RTU_CMD_SIZE-1){/**/
			continue;
		}
		pModbus_RTU->serialReadCache[pModbus_RTU->serialReadCount]=byte;/*save a byte*/
		pModbus_RTU->serialReadCount++;
	}
	
	if(pModbus_RTU->serialReadCount>=1){/*Function code has been read*/
		if(pModbus_RTU->serialReadCache[1]==1
			||pModbus_RTU->serialReadCache[1]==2
			||pModbus_RTU->serialReadCache[1]==3
			||pModbus_RTU->serialReadCache[1]==4
		||pModbus_RTU->serialReadCache[1]==5
		||pModbus_RTU->serialReadCache[1]==6
		){
			if(pModbus_RTU->serialReadCount==8){
				return TRUE;
			}
		}else if(	pModbus_RTU->serialReadCache[1]==15||pModbus_RTU->serialReadCache[1]==16){
			if(pModbus_RTU->serialReadCount>=9){
				uint8	 bytesNum=MDS_RTU_BYTES_NUM(pModbus_RTU);
				if(bytesNum+9==pModbus_RTU->serialReadCount){
					return TRUE;
				}
			}
		}else{
			return FALSE;
		}
	}
	
	return TRUE;
}
/*******************************************************
*
* Function name :MDS_RTU_Process
* Description        :This function processes the received packet data
* Parameter         :
*        @pModbus_RTU   Object pointer of slave
* Return          : None
**********************************************************/
void MDS_RTU_Process(PModbusS_RTU pModbus_RTU){
	BOOL res;
	if(!pModbus_RTU){return ;}
	
	if(pModbus_RTU->timesTick - pModbus_RTU->lastSendTimes < pModbus_RTU->frameIntervalTime){
		/*Frame interval time, 3.5T processing one frame*/
		return;
	}
	
	res = MDS_RTU_SerialProcess(pModbus_RTU);/*Read an instruction from the queue*/
	if(!res){
		goto __exit;
	}
	
	if(
		!(pModbus_RTU->serialReadCache[0]==0x00	/*Broadcast address*/
	||
		pModbus_RTU->serialReadCache[0]==pModbus_RTU->salveAddr
	)){
		/*Not belong to this slave, discard*/
		goto __exit;
	}

/*Function code 01-04 has a basically similar transmission structure*/
/*01 Read the coil status to obtain the current status of a group of logic coils (ON/OFF)*/
/*02 Read input status to obtain the current status of a group of switch inputs (ON/OFF)*/
/*03 Read the holding register Get the current binary value in one or more holding registers*/
/*04 Read input register Get the current binary value in one or more input registers*/
	if(MDS_RTU_FUN_CODE(pModbus_RTU)>=0x1
		&&MDS_RTU_FUN_CODE(pModbus_RTU)<=0x4
	){
		uint16 startReg=MDS_RTU_START_REG(pModbus_RTU);
		uint16 regNum=MDS_RTU_REGS_NUM(pModbus_RTU);
		MDS_RTU_ReadDataProcess(pModbus_RTU,startReg,regNum,MDS_RTU_FUN_CODE(pModbus_RTU));
	}else if(
		MDS_RTU_FUN_CODE(pModbus_RTU)==0x5|| 
		MDS_RTU_FUN_CODE(pModbus_RTU)==0x6
	){
/*05 Force a single coil to force the on-off status of a logic coil*/
/*06 Preset single register Load specific binary value into a holding register*/
		uint16 startReg=MDS_RTU_START_REG(pModbus_RTU);
		uint16 val= ((pModbus_RTU->serialReadCache[4])<<8)|pModbus_RTU->serialReadCache[5];

		MDS_RTU_WriteDataProcess(pModbus_RTU,startReg,1,MDS_RTU_FUN_CODE(pModbus_RTU),&val,2);
	}else if(	
			MDS_RTU_FUN_CODE(pModbus_RTU)==15||
			MDS_RTU_FUN_CODE(pModbus_RTU)==16
	){
/*15 Force multiple coils to force a series of continuous logic coils on and off*/
/*16 Preset multiple registers to load specific binary values into a series of continuous holding registers*/
		uint16 startReg=MDS_RTU_START_REG(pModbus_RTU);
		uint16 regNum=MDS_RTU_REGS_NUM(pModbus_RTU);
		uint8	 bytesNum=MDS_RTU_BYTES_NUM(pModbus_RTU);
		if(MDS_RTU_FUN_CODE(pModbus_RTU)==15){
			if(((regNum>>3)+((regNum%8)?1:0))!=bytesNum){
				MDS_RTU_SendErrorCode(pModbus_RTU,WRITE_COILS,ILLEGAL_DAT_VAL);
				/*A length mismatch error occurred, discard it*/
				goto __exit0;
			}
		}else {//16
			if((regNum<<1)!=bytesNum){
				MDS_RTU_SendErrorCode(pModbus_RTU,WRITE_REGS,ILLEGAL_DAT_VAL);
				/*A length mismatch error occurred, discard*/
				goto __exit0;
			}
		}
		
		MDS_RTU_WriteDataProcess(pModbus_RTU,startReg,regNum,MDS_RTU_FUN_CODE(pModbus_RTU),
		(uint16*)(&(pModbus_RTU->serialReadCache[7])),pModbus_RTU->serialReadCache[6]
		);
	}

__exit0:
	pModbus_RTU->lastSendTimes=pModbus_RTU->timesTick;
__exit:
	pModbus_RTU->serialReadCount = 0;

	return ;
}

/*******************************************************
*
* Function name :MDS_RTU_SendErrorCode
* Description        :This function returns the error code information received and processed to the host
* Parameter         :
*        @pModbus_RTU        Object pointer of slave
*        @anlCode            Exception code, see [ANLCode]
*        @errCode            Error code, see [ErrorCode]
* Return          : None
**********************************************************/
static void MDS_RTU_SendErrorCode(PModbusS_RTU pModbus_RTU,ANLCode anlCode,ErrorCode errCode){
	MD_RTU_SEND_MODE(pModbus_RTU);
	MDS_START_SEND(pModbus_RTU);
	MDS_SEND_BYTE(pModbus_RTU,pModbus_RTU->salveAddr);
	MDS_SEND_BYTE(pModbus_RTU,anlCode);
	MDS_SEND_BYTE(pModbus_RTU,errCode);
	MDS_SEND_END(pModbus_RTU);	
	MD_RTU_RECV_MODE(pModbus_RTU);
}
/*******************************************************
*
* Function name :MDS_RTU_ReadDataProcess
* Description        :This function handles the host's read data request
* Parameter         :
*        @pModbus_RTU    Object pointer of slave
*        @reg            Start of read address
*        @regNum            Number of data read
*        @funCode            Function code of operation
* Return          : TRUE success , FALSE fail
**********************************************************/
uint8 MDS_RTU_ReadDataProcess(PModbusS_RTU pModbus_RTU,uint16 reg,uint16 regNum,uint8 funCode){
	uint16 i=0;
	if(pModbus_RTU==NULL){return FALSE;}
	
	for(i=0;i<MDS_REG_COIL_ITEM_NUM;i++){
		if(pModbus_RTU->pMapTableList[i]==NULL){
			continue;
		}
		if(pModbus_RTU->pMapTableList[i]->modbusAddr<=reg&&
		(pModbus_RTU->pMapTableList[i]->modbusAddr+pModbus_RTU->pMapTableList[i]->modbusDataSize)>=(reg+regNum)
		)
		{
			/*Ensure that the read range is within the memory range*/
			if((funCode==1&&pModbus_RTU->pMapTableList[i]->addrType==COILS_TYPE)||
				(funCode==2&&pModbus_RTU->pMapTableList[i]->addrType==INPUT_TYPE)){
				/*Make sure it is the read BIT*/
				/*Get bit offset*/
				uint16 offsetAddr=reg-MDS_RTU_REG_COIL_ITEM_ADDR(pModbus_RTU->pMapTableList[i]);
				uint16 	j;
				uint16	lastIndex=0;
				uint8	 	tempByte=0;
				MD_RTU_SEND_MODE(pModbus_RTU);
				MDS_START_SEND(pModbus_RTU);
				MDS_SEND_BYTE(pModbus_RTU,pModbus_RTU->salveAddr);		
				MDS_SEND_BYTE(pModbus_RTU,funCode);
				MDS_SEND_BYTE(pModbus_RTU,(regNum>>3)+((regNum%8)>0?1:0));
				for(j=offsetAddr; j<offsetAddr+regNum; j++){		
					if(((j-offsetAddr)>>3)!=lastIndex){
						MDS_SEND_BYTE(pModbus_RTU,tempByte);
						tempByte=0;
						/*The current write byte position is different from the last time, it means that a byte needs to be sent*/
						lastIndex=(j-offsetAddr)>>3;
					}
					if(
						MD_GET_BIT(
						MDS_RTU_REG_COIL_ITEM_DATA(pModbus_RTU->pMapTableList[i])[j>>4],j%16)
					){
							MD_SET_BIT(tempByte,j%8);
					}else{
							MD_CLR_BIT(tempByte,j%8);
					}
				}
				MDS_SEND_BYTE(pModbus_RTU,tempByte);
				MDS_SEND_END(pModbus_RTU);
				MD_RTU_RECV_MODE(pModbus_RTU);
			}else if((funCode==3&&pModbus_RTU->pMapTableList[i]->addrType==HOLD_REGS_TYPE)||
				(funCode==4&&pModbus_RTU->pMapTableList[i]->addrType==INPUT_REGS_TYPE)
			){
				/*Make sure to read REG.*/
				/*Get two bytes offset*/
				uint16 j=0;
				uint16 offsetAddr=reg-MDS_RTU_REG_COIL_ITEM_ADDR(pModbus_RTU->pMapTableList[i]) ;
				MD_RTU_SEND_MODE(pModbus_RTU);
				MDS_START_SEND(pModbus_RTU);
				MDS_SEND_BYTE(pModbus_RTU,pModbus_RTU->salveAddr);
				MDS_SEND_BYTE(pModbus_RTU,funCode);
				MDS_SEND_BYTE(pModbus_RTU,regNum<<1);
				for(j=0;j<regNum<<1;j+=2){
					uint16 temp=MDS_RTU_REG_COIL_ITEM_DATA(pModbus_RTU->pMapTableList[i])[offsetAddr+(j>>1)];
					MDS_SEND_BYTE(pModbus_RTU,(temp>>8)&0xff);
					MDS_SEND_BYTE(pModbus_RTU,(temp)&0xff);
				}
				MDS_SEND_END(pModbus_RTU);
				MD_RTU_RECV_MODE(pModbus_RTU);
			}else { 
				/*Address cannot be read*/
				continue;
			}
			return TRUE;
		}
		
	}
	/*Address is abnormal*/
	MDS_RTU_SendErrorCode(pModbus_RTU,(ANLCode)(0x80+funCode),ILLEGAL_DAT_ADDR);
	return FALSE;
}
/*******************************************************
*
* Function name :MDS_RTU_WriteDataProcess
* Description        :This function handles the host's write data request
* Parameter         :
*        @pModbus_RTU     Object pointer of slave
*        @reg             Start of read address
*        @regNum          Number of data read
*        @funCode         Function code of operation
*        @data            Data written
*        @byteCount       How many bytes are the data written
* Return          : TRUE success , FALSE fail
**********************************************************/
uint8 MDS_RTU_WriteDataProcess(PModbusS_RTU pModbus_RTU,uint16 reg,
uint16 regNum,uint8 funCode,uint16* data,uint8 byteCount){
	uint8 res=FALSE;
	switch(funCode){
		case 5:/*Write single coil*/
		{
			if(data[0]==0xFF00 || data[0]==0x0000){
				if(data[0]==0xFF00){
					res=MDS_RTU_WriteBit(pModbus_RTU,reg,1,COILS_TYPE);
				}else {
					res=MDS_RTU_WriteBit(pModbus_RTU,reg,0,COILS_TYPE);
				}
				if(res){
					MD_RTU_SEND_MODE(pModbus_RTU);
					MDS_START_SEND(pModbus_RTU);
					MDS_SEND_BYTE(pModbus_RTU,pModbus_RTU->salveAddr);
					MDS_SEND_BYTE(pModbus_RTU,funCode);
					MDS_SEND_BYTE(pModbus_RTU,(reg>>8)&0xff);
					MDS_SEND_BYTE(pModbus_RTU,(reg)&0xff);
					MDS_SEND_BYTE(pModbus_RTU,((*data)>>8)&0xff);
					MDS_SEND_BYTE(pModbus_RTU,(*data)&0xff);
					MDS_SEND_END(pModbus_RTU);
					MD_RTU_RECV_MODE(pModbus_RTU);
					if(pModbus_RTU->mdsWriteFun){
						pModbus_RTU->mdsWriteFun(pModbus_RTU,reg,1,COILS_TYPE);
					}
				}else{
					MDS_RTU_SendErrorCode(pModbus_RTU,WRITE_SIN_COIL,ILLEGAL_DAT_ADDR);
				}
			}else{
				MDS_RTU_SendErrorCode(pModbus_RTU,WRITE_SIN_COIL,ILLEGAL_DAT_VAL);
			}
		}
			break;
		case 15:/*Write multiple coils*/
			res=MDS_RTU_WriteBits(pModbus_RTU, reg, regNum, data,COILS_TYPE);
			if(res){
				MD_RTU_SEND_MODE(pModbus_RTU);
				MDS_START_SEND(pModbus_RTU);
				MDS_SEND_BYTE(pModbus_RTU,pModbus_RTU->salveAddr);
				MDS_SEND_BYTE(pModbus_RTU,funCode);
				MDS_SEND_BYTE(pModbus_RTU,(reg>>8)&0xff);
				MDS_SEND_BYTE(pModbus_RTU,(reg)&0xff);
				MDS_SEND_BYTE(pModbus_RTU,((regNum)>>8)&0xff);
				MDS_SEND_BYTE(pModbus_RTU,(regNum)&0xff);
				MDS_SEND_END(pModbus_RTU);
				MD_RTU_RECV_MODE(pModbus_RTU);
				if(pModbus_RTU->mdsWriteFun){
					pModbus_RTU->mdsWriteFun(pModbus_RTU,reg,regNum,COILS_TYPE);
				}
			}else{
				MDS_RTU_SendErrorCode(pModbus_RTU,WRITE_COILS,ILLEGAL_DAT_ADDR);
			}
			break;
		case 6:/*Write single register*/
			res=MDS_RTU_WriteReg(pModbus_RTU,reg,data[0],HOLD_REGS_TYPE);
			if(res){
				MD_RTU_SEND_MODE(pModbus_RTU);
				MDS_START_SEND(pModbus_RTU);
				MDS_SEND_BYTE(pModbus_RTU,pModbus_RTU->salveAddr);
				MDS_SEND_BYTE(pModbus_RTU,funCode);
				MDS_SEND_BYTE(pModbus_RTU,(reg>>8)&0xff);
				MDS_SEND_BYTE(pModbus_RTU,(reg)&0xff);
				MDS_SEND_BYTE(pModbus_RTU,((*data)>>8)&0xff);
				MDS_SEND_BYTE(pModbus_RTU,(*data)&0xff);
				MDS_SEND_END(pModbus_RTU);
				MD_RTU_RECV_MODE(pModbus_RTU);
				if(pModbus_RTU->mdsWriteFun){
					pModbus_RTU->mdsWriteFun(pModbus_RTU,reg,1,HOLD_REGS_TYPE);
				}
			}else{
				MDS_RTU_SendErrorCode(pModbus_RTU,WRITE_SIN_REG,ILLEGAL_DAT_ADDR);
			}
			break;
		case 16:/*Write multiple registers*/
			res=MDS_RTU_WriteRegs(pModbus_RTU,reg,regNum,data,1,HOLD_REGS_TYPE);
			if(res){
				MD_RTU_SEND_MODE(pModbus_RTU);
				MDS_START_SEND(pModbus_RTU);
				MDS_SEND_BYTE(pModbus_RTU,pModbus_RTU->salveAddr);
				MDS_SEND_BYTE(pModbus_RTU,funCode);
				MDS_SEND_BYTE(pModbus_RTU,(reg>>8)&0xff);
				MDS_SEND_BYTE(pModbus_RTU,(reg)&0xff);
				MDS_SEND_BYTE(pModbus_RTU,((regNum)>>8)&0xff);
				MDS_SEND_BYTE(pModbus_RTU,(regNum)&0xff);
				MDS_SEND_END(pModbus_RTU);
				MD_RTU_RECV_MODE(pModbus_RTU);
				if(pModbus_RTU->mdsWriteFun){
					pModbus_RTU->mdsWriteFun(pModbus_RTU,reg,regNum,HOLD_REGS_TYPE);
				}
			}else{
				MDS_RTU_SendErrorCode(pModbus_RTU,WRITE_REGS,ILLEGAL_DAT_ADDR);
			}
			break;
	}
	if(!res){return FALSE;}
	return TRUE;
}



