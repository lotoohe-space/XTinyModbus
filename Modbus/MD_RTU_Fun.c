#include "MD_RTU_Fun.h"
#include "MD_RTU_CRC16.h"
#include "MD_RTU_Serial.h"
#include "MD_RTU_User_Fun.h"
/*处理相关*/
void MDS_RTU_RecvByte(void *obj,uint8 byte);
void MDS_RTU_TimeHandler(void *obj,uint32 times);

/**/
uint8 MDS_RTU_ReadDataProcess(PModbusS_RTU pModbus_RTU,uint16 reg,uint16 regNum,uint8 funCode);
uint8 MDS_RTU_WriteDataProcess(PModbusS_RTU pModbus_RTU,uint16 reg,uint16 regNum,uint8 funCode,uint16* data,uint8 byteCount);

void MDS_RTU_Init(PModbusS_RTU pModbusRTU,uint8 salveAddr,uint32 baud,uint8 dataBits,uint8 stopBits,uint8 parity){
	float T;
	if(pModbusRTU==NULL){
		return ;
	}
	MDInitQueue(&(pModbusRTU->mdSqQueue));
	MDInitQueue(&(pModbusRTU->mdMsgSqQueue));
	pModbusRTU->salveAddr=salveAddr;
	pModbusRTU->serialReadCount=0;
	pModbusRTU->mdRTUReadDataProcessFunction=NULL;
	pModbusRTU->mdRUTWriteDataProcessFunction=NULL;
	pModbusRTU->mdRTUTimeHandlerFunction=MDS_RTU_TimeHandler;
	
	pModbusRTU->mdsRTURecByteFunction=MDS_RTU_RecvByte;
	pModbusRTU->mdsRTUSendBytesFunction=NULL;
	pModbusRTU->mdsRTURecSendConv=NULL;
	
	pModbusRTU->lastTimesTick=0xFFFFFFFF;
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
	
	pModbusRTU->CRC16Update=0xFFFF;
	/*初始化串行口相关*/
	MDSInitSerial(pModbusRTU,baud, dataBits,stopBits,parity);

	return ;
}
/*定时处理函数*/
void MDS_RTU_TimeHandler(void *obj,uint32 times){
	PModbusS_RTU pModbusRTU=obj;
	if(!pModbusRTU){ return; }
	pModbusRTU->timesTick=times;
	/*不需要处理*/
	if(pModbusRTU->lastTimesTick==0xFFFFFFFF){return ;}
	if(pModbusRTU->timesTick-pModbusRTU->lastTimesTick>=pModbusRTU->frameIntervalTime){
		uint16 msgLen;
		uint16 i;
		uint8	 byte;
		if(pModbusRTU->CRC16Update!=0x0000){
			/*CRC错误*/
			MDResetQueue(&(pModbusRTU->mdSqQueue));
			pModbusRTU->lastTimesTick=0xFFFFFFFF;
			return ;
		}
		/*一帧结束*/
		/*数据处理队列的消息搬到消息处理队列中*/
		msgLen=MDQueueLength(&(pModbusRTU->mdSqQueue));
		for(i=0;i<msgLen;i++){
			/*取出来*/
			if(MDdeQueue(&(pModbusRTU->mdSqQueue),&byte)==FALSE){return ;}
			/*放进去*/
			if(MDenQueue(&(pModbusRTU->mdMsgSqQueue),byte)==FALSE){return ;}
		}
		pModbusRTU->lastTimesTick=0xFFFFFFFF;
	}
}
/*该函数接收数据并且放入队列中*/
void MDS_RTU_RecvByte(void *obj,uint8 byte){
	PModbusS_RTU pModbusRTU=obj;
	if(!pModbusRTU){ return; }
	/*在这里放入队列中*/
	MDenQueue(&(pModbusRTU->mdSqQueue),byte);
	if(pModbusRTU->lastTimesTick==0xFFFFFFFF){
		pModbusRTU->CRC16Update=0xFFFF;
	}
	pModbusRTU->CRC16Update=crc16_update(pModbusRTU->CRC16Update,byte);
	/*保存上次接收的字符的时间戳*/
	pModbusRTU->lastTimesTick=pModbusRTU->timesTick;
}
/*发送一个字节*/
static void MDS_RTU_SendByte(PModbusS_RTU pModbusRTU,uint8 byte){
	if(!pModbusRTU){ return; }
	pModbusRTU->mdsRTUSendBytesFunction(&byte,1);
}
/*内部调用的函数*/
static BOOL MDS_RTU_SerialProcess(PModbusS_RTU pModbus_RTU){
	uint8 byte;
	if(!pModbus_RTU){return FALSE;}
	
	/*从队列读取一个字节*/
	if(MDdeQueue(&(pModbus_RTU->mdMsgSqQueue),&byte)==FALSE){
		return FALSE;
	}
	pModbus_RTU->serialReadCache[pModbus_RTU->serialReadCount]=byte;/*save a byte*/
	
	if(pModbus_RTU->serialReadCount>=1){/*已经读取功能码*/
		if(
			pModbus_RTU->serialReadCache[1]>=0x01&&
			pModbus_RTU->serialReadCache[1]<=0x04
		){
			/*以上功能码具有一致性*/
			if(pModbus_RTU->serialReadCount>5){
				/*读取了REG与REG_NUM*/
				//uint16 regNum;/*寄存器个数，可计算得到剩余需要读取个数=regNum*2+2*/
				//regNum = MDS_RTU_REGS_NUM(pModbus_RTU);
				//pModbus_RTU->needReadLen = (regNum<<1) + 2;
				if(pModbus_RTU->serialReadCount>=7){
					pModbus_RTU->serialReadCount++;
					/*数据读取够了*/
					return TRUE;
				}
			}
		}else if(	
			pModbus_RTU->serialReadCache[1]==5||
			pModbus_RTU->serialReadCache[1]==6
		){
			/*以上功能码具有一致性*/
			/*写单线圈，写单寄存器*/
			if(pModbus_RTU->serialReadCount>=7){
				pModbus_RTU->serialReadCount++;
				/*数据读取够了*/
					return TRUE;
			}	
		}else if(	
			pModbus_RTU->serialReadCache[1]==15||
			pModbus_RTU->serialReadCache[1]==16
		){
			/*以上功能码具有一致性*/
			if(pModbus_RTU->serialReadCount>6){
				if(pModbus_RTU->serialReadCount>=
					(6+pModbus_RTU->serialReadCache[6]+2)
				){
					pModbus_RTU->serialReadCount++;
					/*数据读取够了*/
					return TRUE;
				}
			}
		}
	}
	
	pModbus_RTU->serialReadCount++;

	/*还没有考虑单条指令超过缓存的情况*/
	
	return FALSE;
}
void MDS_RTU_Loop(PModbusS_RTU pModbus_RTU){
	BOOL res;
	if(!pModbus_RTU){return ;}
	res = MDS_RTU_SerialProcess(pModbus_RTU);/*从队列中读取一个指令*/
	if(!res){
		return ;
	}
	if(pModbus_RTU->serialReadCache[0]!=pModbus_RTU->salveAddr){
		/*不属于本从机，丢弃*/
		goto __exit;
	}
	
	/*功能码01-04具有基本类似的传输结构*/
	//01 读取线圈状态 	取得一组逻辑线圈的当前状态（ON/OFF)
	//02 读取输入状态 	取得一组开关输入的当前状态(ON/OFF)
	//03 读取保持寄存器 在一个或多个保持寄存器中取得当前的二进制值
	//04 读取输入寄存器 在一个或多个输入寄存器中取得当前的二进制值
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
	//05 强置单线圈 强置一个逻辑线圈的通断状态
	//06 预置单寄存器 把具体二进值装入一个保持寄存器
		uint16 startReg=MDS_RTU_START_REG(pModbus_RTU);
		uint16 val= ((pModbus_RTU->serialReadCache[4])<<8)|pModbus_RTU->serialReadCache[5];

		MDS_RTU_WriteDataProcess(pModbus_RTU,startReg,1,MDS_RTU_FUN_CODE(pModbus_RTU),&val,2);
	}else if(	
			MDS_RTU_FUN_CODE(pModbus_RTU)==15||
			MDS_RTU_FUN_CODE(pModbus_RTU)==16
	){
	//15 强置多线圈 强置一串连续逻辑线圈的通断
	//16 预置多寄存器 把具体的二进制值装入一串连续的保持寄存器
		uint16 startReg=MDS_RTU_START_REG(pModbus_RTU);
		uint16 regNum=MDS_RTU_REGS_NUM(pModbus_RTU);
		MDS_RTU_WriteDataProcess(pModbus_RTU,startReg,regNum,MDS_RTU_FUN_CODE(pModbus_RTU),
		(uint16*)(&(pModbus_RTU->serialReadCache[7])),pModbus_RTU->serialReadCache[6]
		);
	}
__exit:
	pModbus_RTU->serialReadCount = 0;

	return ;
}

/*读数据处理*/
uint8 MDS_RTU_ReadDataProcess(PModbusS_RTU pModbus_RTU,uint16 reg,uint16 regNum,uint8 funCode){
	uint16 i=0;
	if(pModbus_RTU==NULL){return FALSE;}
	
	for(i=0;i<REG_COIL_ITEM_NUM;i++){
		if(pModbus_RTU->pRegCoilList[i]==NULL){
			continue;
		}
		if(pModbus_RTU->pRegCoilList[i]->modbusAddr<=reg&&
		(pModbus_RTU->pRegCoilList[i]->modbusAddr+pModbus_RTU->pRegCoilList[i]->modbusDataSize)>=(reg+regNum)
		)
		{
			/*确保读取的范围在内存的范围之内*/
			if((funCode==1||funCode==2) && pModbus_RTU->pRegCoilList[i]->addrType==BIT_TYPE){
				/*确保是读取的bit*/
				/*得到位偏移*/
				uint16 offsetAddr=reg-MDS_RTU_REG_COIL_ITEM_ADDR(pModbus_RTU->pRegCoilList[i]);
				uint16 	j;
				uint16	lastIndex=0;
				uint8	 	tempByte=0;
				MSD_START_SEND(pModbus_RTU);
				MSD_SEND_BYTE(pModbus_RTU,pModbus_RTU->salveAddr);		
				MSD_SEND_BYTE(pModbus_RTU,funCode);
				MSD_SEND_BYTE(pModbus_RTU,(regNum>>3)+((regNum%8)>0?1:0));
				for(j=offsetAddr; j<offsetAddr+regNum; j++){		
					if(((j-offsetAddr)>>3)!=lastIndex){
						MSD_SEND_BYTE(pModbus_RTU,tempByte);
						tempByte=0;
						/*当前写入字节位置与上次不同则说明需要发送一个字节*/
						lastIndex=(j-offsetAddr)>>3;
					}
					if(
						MDS_GET_BIT(
						MDS_RTU_REG_COIL_ITEM_DATA(pModbus_RTU->pRegCoilList[i])[j>>4],j%16)
					){
							MDS_SET_BIT(tempByte,j%8);
					}else{
							MDS_CLR_BIT(tempByte,j%8);
					}
				}
				MSD_SEND_BYTE(pModbus_RTU,tempByte);
				MSD_SEND_END(pModbus_RTU);
			}else if((funCode==3||funCode==4) && pModbus_RTU->pRegCoilList[i]->addrType==REG_TYPE){
				/*确保读取的是reg*/
					/*得到uint16偏移*/
				uint16 j=0;
				uint16 offsetAddr=reg-MDS_RTU_REG_COIL_ITEM_ADDR(pModbus_RTU->pRegCoilList[i]) ;
				MSD_START_SEND(pModbus_RTU);
				MSD_SEND_BYTE(pModbus_RTU,pModbus_RTU->salveAddr);
				MSD_SEND_BYTE(pModbus_RTU,funCode);
				MSD_SEND_BYTE(pModbus_RTU,regNum<<1);
				for(j=0;j<regNum<<1;j+=2){
					uint16 temp=MDS_RTU_REG_COIL_ITEM_DATA(pModbus_RTU->pRegCoilList[i])[offsetAddr+(j>>1)];
					MSD_SEND_BYTE(pModbus_RTU,(temp>>8)&0xff);
					MSD_SEND_BYTE(pModbus_RTU,(temp)&0xff);
				}
				MSD_SEND_END(pModbus_RTU);

			}else { 
				/*地址不能读取*/
				continue;
			}
			return TRUE;
		}
	}
	
	return TRUE;
}
uint8 MDS_RTU_WriteDataProcess(PModbusS_RTU pModbus_RTU,uint16 reg,
uint16 regNum,uint8 funCode,uint16* data,uint8 byteCount){
	uint8 res=FALSE;
	switch(funCode){
		case 5:
		{
			if(data[0]==0xFF00){
				res=MDS_RTU_WriteBit(pModbus_RTU,reg,1);
			}else if(data[0]==0x0000){
				res=MDS_RTU_WriteBit(pModbus_RTU,reg,0);
			}
			if(res){
				MSD_START_SEND(pModbus_RTU);
				MSD_SEND_BYTE(pModbus_RTU,pModbus_RTU->salveAddr);
				MSD_SEND_BYTE(pModbus_RTU,funCode);
				MSD_SEND_BYTE(pModbus_RTU,(reg>>8)&0xff);
				MSD_SEND_BYTE(pModbus_RTU,(reg)&0xff);
				MSD_SEND_BYTE(pModbus_RTU,((*data)>>8)&0xff);
				MSD_SEND_BYTE(pModbus_RTU,(*data)&0xff);
				MSD_SEND_END(pModbus_RTU);
			}
		}
			break;
		case 15:
			res=MDS_RTU_WriteBits(pModbus_RTU, reg, regNum, data);
			if(res){
				MSD_START_SEND(pModbus_RTU);
				MSD_SEND_BYTE(pModbus_RTU,pModbus_RTU->salveAddr);
				MSD_SEND_BYTE(pModbus_RTU,funCode);
				MSD_SEND_BYTE(pModbus_RTU,(reg>>8)&0xff);
				MSD_SEND_BYTE(pModbus_RTU,(reg)&0xff);
				MSD_SEND_BYTE(pModbus_RTU,((regNum)>>8)&0xff);
				MSD_SEND_BYTE(pModbus_RTU,(regNum)&0xff);
				MSD_SEND_END(pModbus_RTU);
			}
			break;
		case 6:
			res=MDS_RTU_WriteReg(pModbus_RTU,reg,data[0]);
			if(res){
				MSD_START_SEND(pModbus_RTU);
				MSD_SEND_BYTE(pModbus_RTU,pModbus_RTU->salveAddr);
				MSD_SEND_BYTE(pModbus_RTU,funCode);
				MSD_SEND_BYTE(pModbus_RTU,(reg>>8)&0xff);
				MSD_SEND_BYTE(pModbus_RTU,(reg)&0xff);
				MSD_SEND_BYTE(pModbus_RTU,((*data)>>8)&0xff);
				MSD_SEND_BYTE(pModbus_RTU,(*data)&0xff);
				MSD_SEND_END(pModbus_RTU);
			}
			break;
		case 16:
			res=MDS_RTU_WriteRegs(pModbus_RTU,reg,regNum,data,1);
			if(res){
				MSD_START_SEND(pModbus_RTU);
				MSD_SEND_BYTE(pModbus_RTU,pModbus_RTU->salveAddr);
				MSD_SEND_BYTE(pModbus_RTU,funCode);
				MSD_SEND_BYTE(pModbus_RTU,(reg>>8)&0xff);
				MSD_SEND_BYTE(pModbus_RTU,(reg)&0xff);
				MSD_SEND_BYTE(pModbus_RTU,((regNum)>>8)&0xff);
				MSD_SEND_BYTE(pModbus_RTU,(regNum)&0xff);
				MSD_SEND_END(pModbus_RTU);
			}
			break;
	}
	if(!res){return FALSE;}
	return TRUE;
	//	uint16 i=0;
//	if(pModbus_RTU==NULL){return FALSE;}
//	
//	for(i=0;i<REG_COIL_ITEM_NUM;i++){
//		if(pModbus_RTU->pRegCoilList[i]==NULL){
//			continue;
//		}
//		if(pModbus_RTU->pRegCoilList[i]->modbusAddr<=reg&&
//		(pModbus_RTU->pRegCoilList[i]->modbusAddr+pModbus_RTU->pRegCoilList[i]->modbusDataSize)>=(reg+regNum)
//		)
//		{
//			/*确保读取的范围在内存的范围之内*/
//			if((funCode==5 &&pModbus_RTU->pRegCoilList[i]->addrType==BIT_TYPE)
//			||(funCode==6 &&pModbus_RTU->pRegCoilList[i]->addrType==REG_TYPE)
//				){
//				
//				uint16 offsetAddr=reg-MDS_RTU_REG_COIL_ITEM_ADDR(pModbus_RTU->pRegCoilList[i]);
//				if(funCode==5){
//					if(data[0]==0xff00){
//						MDS_SET_BIT(
//							MDS_RTU_REG_COIL_ITEM_DATA(pModbus_RTU->pRegCoilList[i])[offsetAddr>>4],offsetAddr%16);
//					}else if(data[0]==0x0000){
//						MDS_CLR_BIT(
//							MDS_RTU_REG_COIL_ITEM_DATA(pModbus_RTU->pRegCoilList[i])[offsetAddr>>4],offsetAddr%16);
//					}
//				}else {
//					MDS_RTU_REG_COIL_ITEM_DATA(pModbus_RTU->pRegCoilList[i])[offsetAddr]=data[0];
//				}
//				MSD_START_SEND(pModbus_RTU);
//				MSD_SEND_BYTE(pModbus_RTU,pModbus_RTU->salveAddr);
//				MSD_SEND_BYTE(pModbus_RTU,funCode);
//				MSD_SEND_BYTE(pModbus_RTU,(reg>>8)&0xff);
//				MSD_SEND_BYTE(pModbus_RTU,(reg)&0xff);
//				MSD_SEND_BYTE(pModbus_RTU,((*data)>>8)&0xff);
//				MSD_SEND_BYTE(pModbus_RTU,(*data)&0xff);
//				MSD_SEND_END(pModbus_RTU);
//			}else if(
//				(funCode==15 &&pModbus_RTU->pRegCoilList[i]->addrType==BIT_TYPE)
//			||(funCode==16 &&pModbus_RTU->pRegCoilList[i]->addrType==REG_TYPE)
//				){
//				uint16 offsetAddr=reg-MDS_RTU_REG_COIL_ITEM_ADDR(pModbus_RTU->pRegCoilList[i]);
//				if(funCode==15){/*强置多线圈*/
//					uint16 j;
//					for(j=0;j<regNum;j++){
//						uint8 *byteData=(uint8*)data;
//						if(
//							MDS_GET_BIT( byteData[j>>3] ,j%8)
//						){
//							MDS_SET_BIT(
//								MDS_RTU_REG_COIL_ITEM_DATA(
//								pModbus_RTU->pRegCoilList[i])[(offsetAddr+j)>>4]
//							,(j+offsetAddr)%16);
//						}else{
//							MDS_CLR_BIT(
//								MDS_RTU_REG_COIL_ITEM_DATA(
//								pModbus_RTU->pRegCoilList[i])[(offsetAddr+j)>>4]
//							,(j+offsetAddr)%16);
//						}
//					}
//				}else{/*预置多寄存器*/
//					uint16 j=0;
//					for(j=0;j<regNum;j++){
//						MDS_RTU_REG_COIL_ITEM_DATA(pModbus_RTU->pRegCoilList[i])[offsetAddr+j]=MDS_SWAP_HL(data[j]);
//					}					
//				}
//				MSD_START_SEND(pModbus_RTU);
//				MSD_SEND_BYTE(pModbus_RTU,pModbus_RTU->salveAddr);
//				MSD_SEND_BYTE(pModbus_RTU,funCode);
//				MSD_SEND_BYTE(pModbus_RTU,(reg>>8)&0xff);
//				MSD_SEND_BYTE(pModbus_RTU,(reg)&0xff);
//				MSD_SEND_BYTE(pModbus_RTU,((regNum)>>8)&0xff);
//				MSD_SEND_BYTE(pModbus_RTU,(regNum)&0xff);
//				MSD_SEND_END(pModbus_RTU);
//			}
//		}
//	}
//	return TRUE;
}



