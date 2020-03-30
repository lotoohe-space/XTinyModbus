#include "MDM_RTU_Fun.h"
#include "MD_RTU_Tool.h"
#include "MD_RTU_CRC16.h"


#define MEM_RTU_START_EN()	{uint16 CRCUpdate=0xFFFF;
#define MEM_RTU_EN_QUEUE(a,b) MDM_RTU_SendByte((a),(b));\
CRCUpdate=crc16_update(CRCUpdate,(b))
#define MEM_RTU_END_EN(a)		(TO_MDBase(a))->mdRTUSendBytesFunction((uint8*)(&CRCUpdate),2);}

static void MDM_RTU_SendByte(PModbus_RTU pModbus_RTU,uint8 byte);
void MDM_RTU_TimeHandler(void *obj,uint32 times);
void MDM_RTU_RecvByte(void *obj,uint8 byte);

MDError MDM_RTU_Init(
	PModbus_RTU pModbusRTU,
	MD_RTU_SerialInit mdRTUSerialInitFun,
	uint32 baud,
	uint8 dataBits,
	uint8 stopBits,
	uint8 parity
){
	float T;
	uint8 i;
	if(pModbusRTU==NULL){return ERR_VOID;}
	
	MDInitQueue(&(pModbusRTU->mdSqQueue));
	for(i=0;i<MDM_REG_COIL_ITEM_NUM;i++){
		pModbusRTU->pRegCoilList[i] = NULL;
	}
	TO_MDBase(pModbusRTU)->mdRTUTimeHandlerFunction=MDM_RTU_TimeHandler;
	/*数据发送接收有关的函数*/
	TO_MDBase(pModbusRTU)->mdRTUSendBytesFunction=NULL;
	TO_MDBase(pModbusRTU)->mdRTURecByteFunction=MDM_RTU_RecvByte;
	TO_MDBase(pModbusRTU)->mdRTURecSendConv=NULL;

	/*上次接收的时间,0xFFFFFFF表示未起开始检测帧*/
	pModbusRTU->lastTimesTick=0xFFFFFFFF;
	/*当前的实时时间单位100US*/
	pModbusRTU->timesTick=0;
	
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
	pModbusRTU->frameIntervalTime=time;/*该参数需要根据波特率设置*/
	
	pModbusRTU->recvFlag=0;/*接收标志*/
	if(mdRTUSerialInitFun!=NULL){
		mdRTUSerialInitFun(pModbusRTU,baud, dataBits,stopBits,parity);
	}
	
	return ERR_NONE;
}
/*控制块初始化*/
void MDM_RTU_CB_Init(
	PModbus_RTU_CB 	pModbusRTUCB
	,PModbus_RTU 		pModbusRTU
	,uint16 				sendIntervalTime
	,uint32					sendOverTime/*发送超时时间*/
	,uint8 					RTTimes/*重传次数 当其为255时表示一直进行重传*/
){
	if(pModbusRTUCB==NULL){return ;}
	pModbusRTUCB->sendIntervalTime=sendIntervalTime;
	pModbusRTUCB->pModbus_RTU=pModbusRTU;
	pModbusRTUCB->sendTimeTick=0;
	pModbusRTUCB->sendOverTime=sendOverTime;
	pModbusRTUCB->RTTimes=RTTimes;
	pModbusRTUCB->sendFlag=0;
	pModbusRTU->parentObj=NULL;
}
/*控制块超时复位*/
void MDM_RTU_CB_OverTimeReset(PModbus_RTU_CB 	pModbusRTUCB){
	if(pModbusRTUCB==NULL){return ;}
	pModbusRTUCB->RTCount=0;
	pModbusRTUCB->sendFlag=0;
}
/*定时处理函数*/
void MDM_RTU_TimeHandler(void *obj,uint32 times){
	PModbus_RTU pModbusRTU=obj;
	if(!pModbusRTU){ return; }
	pModbusRTU->timesTick=times;
	/*不需要处理*/
	if(pModbusRTU->lastTimesTick==0xFFFFFFFF){return ;}
	if(pModbusRTU->timesTick-pModbusRTU->lastTimesTick>=pModbusRTU->frameIntervalTime){
		if(pModbusRTU->CRC16Update!=0x0000){
			/*CRC错误*/
			MDResetQueue(&(pModbusRTU->mdSqQueue));
			pModbusRTU->lastTimesTick=0xFFFFFFFF;
			return ;
		}
		/*一帧结束*/
		pModbusRTU->recvFlag=1;
		pModbusRTU->lastTimesTick=0xFFFFFFFF;
	}
}
/*该函数接收数据并且放入队列中*/
void MDM_RTU_RecvByte(void *obj,uint8 byte){
	PModbus_RTU pModbusRTU=obj;
	if(!pModbusRTU){ return; }
	if(MDenQueue(&(pModbusRTU->mdSqQueue),byte)==FALSE){
		return ;
	}
	if(pModbusRTU->lastTimesTick==0xFFFFFFFF){
		pModbusRTU->CRC16Update=0xFFFF;
	}
	pModbusRTU->CRC16Update=crc16_update(pModbusRTU->CRC16Update,byte);
	/*保存上次接收的字符的时间戳*/
	pModbusRTU->lastTimesTick=pModbusRTU->timesTick;
}
/*发送一个字节*/
static void MDM_RTU_SendByte(PModbus_RTU pModbus_RTU,uint8 byte){
	if(!pModbus_RTU){ return; }
	TO_MDBase(pModbus_RTU)->mdRTUSendBytesFunction(&byte,1);
}
/*获取返回的数据的数据*/
MDError MDM_RTU_ReadByte(PModbus_RTU pModbusRTU,uint8 *res,uint8 len){
	uint8 i;
	if(res==NULL){return ERR_VOID;}
	for(i=0;i<len;i++){
		if(!MDdeQueue(&(pModbusRTU->mdSqQueue),(res+i))){
			return ERR_QUEUE;
		}
	}
	return ERR_NONE;
}
/*获取返回的数据的数据*/
MDError MDM_RTU_ReadUint16(PModbus_RTU pModbusRTU,uint16 *res,uint8 len){
	uint8 i;
	uint8 byte;
	if(res==NULL){return ERR_VOID;}
	for(i=0;i<len;i++){
		if(!MDdeQueue(&(pModbusRTU->mdSqQueue),&byte)){
			return ERR_QUEUE;
		}
		res[i]=byte<<8;
		if(!MDdeQueue(&(pModbusRTU->mdSqQueue),&byte)){
			return ERR_QUEUE;
		}
		res[i]|=byte;
	}
	return ERR_NONE;
}
/*读功能函数*/
static void MDM_RTU_ReadFun(PModbus_RTU pModbus_RTU,uint8 funCode,uint8 slaveAddr,uint16 startAddr,uint16 numOf){
	MEM_RTU_START_EN();
	MEM_RTU_EN_QUEUE(pModbus_RTU,slaveAddr);
	MEM_RTU_EN_QUEUE(pModbus_RTU,funCode);
	MEM_RTU_EN_QUEUE(pModbus_RTU,MD_H_BYTE(startAddr));
	MEM_RTU_EN_QUEUE(pModbus_RTU,MD_L_BYTE(startAddr));
	MEM_RTU_EN_QUEUE(pModbus_RTU,MD_H_BYTE(numOf));
	MEM_RTU_EN_QUEUE(pModbus_RTU,MD_L_BYTE(numOf));
	MEM_RTU_END_EN(pModbus_RTU);
}
/*写单个线圈与寄存器函数*/
static void MDM_RTU_WriteSingleFun(PModbus_RTU pModbus_RTU,uint8 funCode,uint8 slaveAddr,uint16 startAddr,uint16 value){
	MEM_RTU_START_EN();
	MEM_RTU_EN_QUEUE(pModbus_RTU,slaveAddr);
	MEM_RTU_EN_QUEUE(pModbus_RTU,funCode);
	MEM_RTU_EN_QUEUE(pModbus_RTU,MD_H_BYTE(startAddr));
	MEM_RTU_EN_QUEUE(pModbus_RTU,MD_L_BYTE(startAddr));
	MEM_RTU_EN_QUEUE(pModbus_RTU,MD_H_BYTE(value));
	MEM_RTU_EN_QUEUE(pModbus_RTU,MD_L_BYTE(value));
	MEM_RTU_END_EN(pModbus_RTU);
}
/*写多个线圈与多个寄存器*/
static void MDM_RTU_WriteFun(PModbus_RTU pModbus_RTU,
	uint8 funCode,uint8 slaveAddr,uint16 startAddr,uint8 numOf,uint8 *data){
	uint16 i;
	uint8 wLen;
	MEM_RTU_START_EN();
	MEM_RTU_EN_QUEUE(pModbus_RTU,slaveAddr);
	MEM_RTU_EN_QUEUE(pModbus_RTU,funCode);
	MEM_RTU_EN_QUEUE(pModbus_RTU,MD_H_BYTE(startAddr));
	MEM_RTU_EN_QUEUE(pModbus_RTU,MD_L_BYTE(startAddr));
	MEM_RTU_EN_QUEUE(pModbus_RTU,MD_H_BYTE(numOf));
	MEM_RTU_EN_QUEUE(pModbus_RTU,MD_L_BYTE(numOf));
	if(funCode==15){
		wLen=(numOf>>3) + ((numOf%8)?1:0);
	}else if(funCode==16){
		wLen=numOf<<1;
	}
	MEM_RTU_EN_QUEUE(pModbus_RTU,wLen);
	if(funCode == 15){
		for(i=0;i<wLen;i++){
			MEM_RTU_EN_QUEUE(pModbus_RTU,data[i]);
		}
	}else if(funCode == 16){
		uint16 *tempData= (uint16*)data;
		for(i=0;i<numOf;i++){
			MEM_RTU_EN_QUEUE(pModbus_RTU,(tempData[i]>>8));
			MEM_RTU_EN_QUEUE(pModbus_RTU,(tempData[i]&0xff));
		}
	} 
	MEM_RTU_END_EN(pModbus_RTU);
}
/*向映射中写入bit*/
BOOL MDM_RTU_InsideWriteBits(void* obj,uint16 modbusAddr,uint16 numOf, uint8 *bit){
	uint16 i;
	PModbus_RTU pModbus_RTU = obj;
	if(pModbus_RTU==NULL){return FALSE;}
	for(i=0;i<MDM_REG_COIL_ITEM_NUM;i++){
		if(pModbus_RTU->pRegCoilList[i]==NULL){
			continue;
		}
		if(pModbus_RTU->pRegCoilList[i]->modbusAddr<=modbusAddr&&
		(pModbus_RTU->pRegCoilList[i]->modbusAddr+pModbus_RTU->pRegCoilList[i]->modbusDataSize)>=(modbusAddr+numOf)
		){
			if(pModbus_RTU->pRegCoilList[i]->addrType==BIT_TYPE){/*必须是BIT类型*/
				uint16 offsetAddr=modbusAddr-MDS_RTU_REG_COIL_ITEM_ADDR(pModbus_RTU->pRegCoilList[i]);
				uint16 j;
				for(j=0;j<numOf;j++){
					if(
						MD_GET_BIT( bit[j>>3] ,j%8)
					){
						MD_SET_BIT(
							MDS_RTU_REG_COIL_ITEM_DATA(
							pModbus_RTU->pRegCoilList[i])[(offsetAddr+j)>>4]
						,(j+offsetAddr)%16);
					}else{
						MD_CLR_BIT(
							MDS_RTU_REG_COIL_ITEM_DATA(
							pModbus_RTU->pRegCoilList[i])[(offsetAddr+j)>>4]
						,(j+offsetAddr)%16);
					}
				}
				return TRUE;
			}
		}
	}
	return FALSE;
}
/*写离散寄存器*/
BOOL MDM_RTU_InsideWriteRegs(void* obj,uint16 modbusAddr,uint16 numOf, uint16 *reg,uint8 isBigE){
	uint16 i;
	PModbus_RTU pModbusS_RTU = obj;
	if(pModbusS_RTU==NULL){return FALSE;}
	for(i=0;i<MDM_REG_COIL_ITEM_NUM;i++){
		if(pModbusS_RTU->pRegCoilList[i]==NULL){
			continue;
		}
		if(pModbusS_RTU->pRegCoilList[i]->modbusAddr<=modbusAddr&&
		(pModbusS_RTU->pRegCoilList[i]->modbusAddr+pModbusS_RTU->pRegCoilList[i]->modbusDataSize)>=(modbusAddr+numOf)
		){
			if(pModbusS_RTU->pRegCoilList[i]->addrType==REG_TYPE){/*必须是REG类型*/
				uint16 offsetAddr=modbusAddr-MDS_RTU_REG_COIL_ITEM_ADDR(pModbusS_RTU->pRegCoilList[i]);
				uint16 j=0;
				for(j=0;j<numOf;j++){
					MDS_RTU_REG_COIL_ITEM_DATA(pModbusS_RTU->pRegCoilList[i])[offsetAddr+j]=
					isBigE?MD_SWAP_HL(reg[j]):reg[j];
				}		
				return TRUE;
			}
		}
	}
	return FALSE;
}

/*非阻塞式读写*/
MDError MDM_RTU_NB_RW(
	PModbus_RTU_CB pModbus_RTU_CB,
	ModbusFunCode funCode,
	uint8 slaveAddr,
	uint16 startAddr,
	uint16 numOf,
	void *wData
){
	MDError errRes;
	uint8 index;
	uint16 wAddr;
	errRes = ERR_NONE;
	if(pModbus_RTU_CB==NULL){return ERR_VOID;}
	if(pModbus_RTU_CB->pModbus_RTU==NULL){return ERR_VOID;}
	
	if(	pModbus_RTU_CB->pModbus_RTU->parentObj!=NULL &&
			pModbus_RTU_CB!=pModbus_RTU_CB->pModbus_RTU->parentObj){
			return ERR_IDLE;
	}
	
	if(pModbus_RTU_CB->sendFlag==0){/*还没发送过，或者已经发送成功*/
		/*清除接收队列*/
		MDResetQueue(&(pModbus_RTU_CB->pModbus_RTU->mdSqQueue));
		if(funCode>=1 && funCode<=4){
			/*还没有发送过，则发送*/
			MDM_RTU_ReadFun(pModbus_RTU_CB->pModbus_RTU,funCode,slaveAddr,startAddr,numOf);
		}
		else if(funCode==5||funCode==6){
			if(numOf>=1){/*长度必须大于等于1*/
				MDM_RTU_WriteSingleFun(pModbus_RTU_CB->pModbus_RTU,funCode,slaveAddr,startAddr,((uint16*)(wData))[0]);
			}
		}else if(funCode==15||funCode==16){
			MDM_RTU_WriteFun(pModbus_RTU_CB->pModbus_RTU,funCode,slaveAddr,startAddr,numOf,(uint8*)(wData));
		}
		/*设置超时时间起点*/
		pModbus_RTU_CB->sendTimeTick=pModbus_RTU_CB->pModbus_RTU->timesTick;
		
		/*已经发送数据，等待超时检测*/
		//MD_SET_SENDED_FLAG(pModbus_RTU_CB);
		pModbus_RTU_CB->sendFlag=1;
		/*设置该块正在工作*/
		pModbus_RTU_CB->pModbus_RTU->parentObj=pModbus_RTU_CB;
		return ERR_SEND_FIN;
	}else if(pModbus_RTU_CB->RTCount<pModbus_RTU_CB->RTTimes){/*已经发送过，并且没有出现发送错误的情况*/
		
		if(pModbus_RTU_CB->pModbus_RTU->recvFlag){/*收到了数据*/
			uint8 byte;
			/*清楚标志*/
			pModbus_RTU_CB->pModbus_RTU->recvFlag=0;
			
			if(!MDdeQueue(&(pModbus_RTU_CB->pModbus_RTU->mdSqQueue),&byte)){
				errRes =  ERR_DATA_LEN;
				goto _exit;
			}
			/*如果收到数据，则对数据进行处理*/
			if(slaveAddr!=byte){
				errRes =  ERR_SLAVE_ADDR;
				goto _exit;
			}
			if(!MDdeQueue(&(pModbus_RTU_CB->pModbus_RTU->mdSqQueue),&byte)){
				errRes =  ERR_DATA_LEN;
				goto _exit;
			}
			
			switch(byte){
				case 0x1:/*读线圈成功*/
				case 0x2:/*读输入离散量*/
				{
					uint16 i;
					MDdeQueue(&(pModbus_RTU_CB->pModbus_RTU->mdSqQueue),&byte);
					if((byte+2)!=MDQueueLength(&(pModbus_RTU_CB->pModbus_RTU->mdSqQueue))){
						/*长度错误*/
						errRes =  ERR_DATA_LEN;
						goto _exit;
					}
					index = numOf;
					wAddr=startAddr;
					for(i=0;i<byte;i++){
						uint8 rByte;
						if(!MDdeQueue(&(pModbus_RTU_CB->pModbus_RTU->mdSqQueue),&rByte)){
								/*长度错误*/
								errRes =  ERR_DATA_LEN;
								goto _exit;
						}
						/*单次存小于等于8bit*/
						if(!MDM_RTU_InsideWriteBits(pModbus_RTU_CB->pModbus_RTU,wAddr,((index<8)?index:8), &rByte)){
							errRes= ERR_DATA_SAVE;
							goto _exit;
						}
						wAddr += ((index<8)?index:8);
						index-=8;
					}
//					/*释放通道*/
//					pModbus_RTU_CB->pModbus_RTU->parentObj=NULL;
					/*从机收到*/
						pModbus_RTU_CB->sendFlag=2;
					errRes= ERR_RW_FIN;
					goto _exit;
				}
				case 0x3:/*读保持寄存器*/
				case 0x4:/*读输入寄存器*/
				{
					uint16 i;
					uint16 len;
					MDdeQueue(&(pModbus_RTU_CB->pModbus_RTU->mdSqQueue),&byte);
					if((byte+2)!=MDQueueLength(&(pModbus_RTU_CB->pModbus_RTU->mdSqQueue))){
						/*长度错误*/
						errRes= ERR_DATA_LEN;
						goto _exit;
					}
					len=byte>>1;
					for(i=0;i<len ;i++){
						uint16 	wTemp;
						uint8		rByte;
						MDdeQueue(&(pModbus_RTU_CB->pModbus_RTU->mdSqQueue),&rByte);
						wTemp=(rByte<<8);
						MDdeQueue(&(pModbus_RTU_CB->pModbus_RTU->mdSqQueue),&rByte);
						wTemp|=rByte;
						if(!MDM_RTU_InsideWriteRegs(pModbus_RTU_CB->pModbus_RTU,startAddr+i,1,&wTemp,0)){
							errRes= ERR_DATA_SAVE;
							goto _exit;
						}
					}
						pModbus_RTU_CB->sendFlag=2;
					errRes= ERR_RW_FIN;
					goto _exit;
				}	
				
				case 0x5:/*写单线圈*/
				case 0x6:	/*写单个寄存器*/
				{
					uint16 res;
					if(MDM_RTU_ReadUint16(pModbus_RTU_CB->pModbus_RTU,&res,1)!=ERR_NONE){
						errRes= ERR_DATA_LEN;
						goto _exit;
					}
					/*返回地址不匹配错误*/
					if(res!=startAddr){ 
						errRes= ERR_WRITE_COIL;
						goto _exit;
					}
					if(MDM_RTU_ReadUint16(pModbus_RTU_CB->pModbus_RTU,&res,1)!=ERR_NONE){
						errRes= ERR_DATA_LEN;
						goto _exit;
					}
					/*返回数据不匹配*/
					if(res!=*((uint16*)wData)){
						errRes= ERR_WRITE_COIL;
						goto _exit;
					}
						pModbus_RTU_CB->sendFlag=2;
					errRes= ERR_RW_FIN;
					goto _exit;
				}
				case 0x0F:	/*写多线圈*/
				case 0x10:{	/*写多寄存器*/
					uint16 res;
					if(MDM_RTU_ReadUint16(pModbus_RTU_CB->pModbus_RTU,&res,1)!=ERR_NONE){
						errRes= ERR_DATA_LEN;
						goto _exit;
					}
					/*返回地址不匹配错误*/
					if(res!=startAddr){
						errRes= ERR_WRITE_COIL; 
						goto _exit;
					}
					if(MDM_RTU_ReadUint16(pModbus_RTU_CB->pModbus_RTU,&res,1)!=ERR_NONE){
						errRes= ERR_DATA_LEN;
						goto _exit;
					}
					/*返回数据不匹配*/
					if(res!=numOf){
						errRes= ERR_WRITE_COIL;
						goto _exit;
					}
					pModbus_RTU_CB->sendFlag=2;
					errRes= ERR_RW_FIN;
					goto _exit;
				}
				case 0x81: pModbus_RTU_CB->sendFlag=3; errRes= ERR_READ_COIL; goto _exit;			/*读线圈异常*/
				case 0x82: pModbus_RTU_CB->sendFlag=3; errRes= ERR_READ_INPUT; goto _exit;			/*读输入离散量异常*/
				case 0x83: pModbus_RTU_CB->sendFlag=3; errRes= ERR_READ_HOLD_REG; goto _exit;	/*读保持寄存器错误*/
				case 0x84: pModbus_RTU_CB->sendFlag=3; errRes= ERR_READ_INPUT_REG; goto _exit;	/*读输入寄存器错误*/
				case 0x85: pModbus_RTU_CB->sendFlag=3; errRes= ERR_WRITE_COIL; goto _exit;			/*写单线圈错误*/
				case 0x86: pModbus_RTU_CB->sendFlag=3; errRes= ERR_WRITE_REG; goto _exit;			/*写单寄错误*/
				case 0x8F: pModbus_RTU_CB->sendFlag=3; errRes= ERR_WRITE_COIL; goto _exit;			/*写多线圈错误*/	
				case 0x90: pModbus_RTU_CB->sendFlag=3; errRes= ERR_WRITE_REG; goto _exit;			/*写多寄存器错误*/	
			}	

		}
		if(pModbus_RTU_CB->sendFlag==2){/*如果发送成功则进行从新发送*/
			/*定时重发*/
			if(pModbus_RTU_CB->pModbus_RTU->timesTick-pModbus_RTU_CB->sendTimeTick
				>=pModbus_RTU_CB->sendIntervalTime){
				/*重新发送*/
				//MD_CLR_SENDED_FLAG(pModbus_RTU_CB);
				pModbus_RTU_CB->sendFlag=0;
	//			/*释放通道*/
	//			pModbus_RTU_CB->pModbus_RTU->parentObj=NULL;
				errRes= ERR_NONE;
				goto _exit;
			}
		}else if(pModbus_RTU_CB->sendFlag==1 || pModbus_RTU_CB->sendFlag==3){/*已经发送了，但是还没收到反馈，则执行超时检测*/		
			/*接收超时检测*/
			if(pModbus_RTU_CB->pModbus_RTU->timesTick-pModbus_RTU_CB->sendTimeTick
				>=pModbus_RTU_CB->sendOverTime){
					/*设置超时时间起点*/
					pModbus_RTU_CB->sendTimeTick=pModbus_RTU_CB->pModbus_RTU->timesTick;
					/*重新发送*/
					pModbus_RTU_CB->sendFlag=0;
					/*重传次数+1*/
					pModbus_RTU_CB->RTCount++;
					
					/*释放通道*/
					pModbus_RTU_CB->pModbus_RTU->parentObj=NULL;
					
					if(pModbus_RTU_CB->RTCount>=pModbus_RTU_CB->RTTimes){
						/*重传次数超了*/
						errRes= ERR_RW_OV_TIME_ERR;
						goto _exit;
					}
					/*超时了*/
					errRes= ERR_OVER_TIME;
					goto _exit;
			}
		}
	}else {
		/*重传次数超了*/
		errRes= ERR_RW_OV_TIME_ERR;
		goto _exit;
	}

	if(pModbus_RTU_CB->sendFlag==1){/*已经发送过则返回发送完成标志*/
		return ERR_SEND_FIN; 
	}
_exit:
//	MD_CLR_SENDED_FLAG(pModbus_RTU_CB);
//	if(errRes == ERR_RW_FIN){
//		pModbus_RTU_CB->RTCount=0;
//	}
	/*释放通道*/
	pModbus_RTU_CB->pModbus_RTU->parentObj=NULL;
	return errRes;
}
/*阻塞式读写*/
MDError MDM_RTU_RW(
	PModbus_RTU_CB pModbus_RTU_CB,
	ModbusFunCode funCode,
	uint8 slaveAddr,
	uint16 startAddr,
	uint16 numOf,
	void *wData
){
	MDError res;
	do{
		res = MDM_RTU_NB_RW(pModbus_RTU_CB,funCode,slaveAddr,startAddr,numOf,wData);
		if(res != ERR_RW_FIN){/*出现错误*/
			if(res == ERR_RW_OV_TIME_ERR){/*重传超时了*/
				/*使能重传*/
				MDM_RTU_CB_OverTimeReset(pModbus_RTU_CB);
				return res;
			}
		}
	}while(res!=ERR_RW_FIN);
	//pModbus_RTU_CB->sendFlag=0;
	return res;
}

MDError MDM_RTU_ReadCoil(PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf){
	return MDM_RTU_RW(pModbus_RTU_CB,READ_COIL,slaveAddr,startAddr,numOf,NULL);
};
MDError MDM_RTU_ReadInput(PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf){
	return MDM_RTU_RW(pModbus_RTU_CB,READ_INPUT,slaveAddr,startAddr,numOf,NULL);
};
MDError MDM_RTU_ReadHoldReg(PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf){
	return MDM_RTU_RW(pModbus_RTU_CB,READ_HOLD_REG,slaveAddr,startAddr,numOf,NULL);
};
MDError MDM_RTU_ReadInputReg(PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf){
	return MDM_RTU_RW(pModbus_RTU_CB,READ_INPUT_REG,slaveAddr,startAddr,numOf,NULL);
};
MDError MDM_RTU_WriteSingleCoil(
	PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf,BOOL boolVal){
	uint16 temp;
	temp=boolVal?0xFF00:0x0000;
	return MDM_RTU_RW(pModbus_RTU_CB,WRITE_SIN_COIL,slaveAddr,startAddr,1,(void*)(&temp));
};
MDError MDM_RTU_WriteSingleReg(
	PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf,uint16 val){
	return MDM_RTU_RW(pModbus_RTU_CB,WRITE_SIN_REG,slaveAddr,startAddr,1,(void*)(&val));
};
MDError MDM_RTU_WriteCoils(
	PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf,uint8* val){
	return MDM_RTU_RW(pModbus_RTU_CB,WRITE_COILS,slaveAddr,startAddr,numOf,(void*)(val));
};
MDError MDM_RTU_WriteRegs(
	PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf,uint16* val){
	return MDM_RTU_RW(pModbus_RTU_CB,WRITE_REGS,slaveAddr,startAddr,numOf,(void*)(val));
};
/////
MDError MDM_RTU_NB_ReadCoil(PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf){
	return MDM_RTU_NB_RW(pModbus_RTU_CB,READ_COIL,slaveAddr,startAddr,numOf,NULL);
};
MDError MDM_RTU_NB_ReadInput(PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf){
	return MDM_RTU_NB_RW(pModbus_RTU_CB,READ_INPUT,slaveAddr,startAddr,numOf,NULL);
};
MDError MDM_RTU_NB_ReadHoldReg(PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf){
	return MDM_RTU_NB_RW(pModbus_RTU_CB,READ_HOLD_REG,slaveAddr,startAddr,numOf,NULL);
};
MDError MDM_RTU_NB_ReadInputReg(PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf){
	return MDM_RTU_NB_RW(pModbus_RTU_CB,READ_INPUT_REG,slaveAddr,startAddr,numOf,NULL);
};
MDError MDM_RTU_NB_WriteSingleCoil(
	PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf,BOOL boolVal){
	uint16 temp;
	temp=boolVal?0xFF00:0x0000;
	return MDM_RTU_NB_RW(pModbus_RTU_CB,WRITE_SIN_COIL,slaveAddr,startAddr,1,(void*)(&temp));
};
MDError MDM_RTU_NB_WriteSingleReg(
	PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf,uint16 val){
	return MDM_RTU_NB_RW(pModbus_RTU_CB,WRITE_SIN_REG,slaveAddr,startAddr,1,(void*)(&val));
};
MDError MDM_RTU_NB_WriteCoils(
	PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf,uint8* val){
	return MDM_RTU_NB_RW(pModbus_RTU_CB,WRITE_COILS,slaveAddr,startAddr,numOf,(void*)(val));
};
MDError MDM_RTU_NB_WriteRegs(
	PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf,uint16* val){
	return MDM_RTU_NB_RW(pModbus_RTU_CB,WRITE_REGS,slaveAddr,startAddr,numOf,(void*)(val));
};
	
