/********************************************************************************
* @File name: MD_RTU_Fun.c
* @Author: zspace
* @Version: 1.0
* @Date: 2020-4-10
* @Description: Modbus RTU Slave从机接收功能函数。
********************************************************************************/

/*********************************头文件包含************************************/
#include "MDS_RTU_Fun.h"
#include "MD_RTU_CRC16.h"
#include "MDS_RTU_Serial.h"
#include "MDS_RTU_User_Fun.h"
/*********************************结束******************************************/

/**********************************函数声明*************************************/
void MDS_RTU_RecvByte(void *obj,uint8 byte);
void MDS_RTU_TimeHandler(void *obj);

static void MDS_RTU_SendErrorCode(PModbusS_RTU pModbus_RTU,ANLCode anlCode,ErrorCode errCode);
uint8 MDS_RTU_ReadDataProcess(PModbusS_RTU pModbus_RTU,uint16 reg,uint16 regNum,uint8 funCode);
uint8 MDS_RTU_WriteDataProcess(PModbusS_RTU pModbus_RTU,uint16 reg,uint16 regNum,uint8 funCode,uint16* data,uint8 byteCount);
/*********************************结束******************************************/

/*******************************************************
*
* Function name :MDS_RTU_Init
* Description        :初始化一个从机
* Parameter         :
*        @pModbusRTU            从机结构体指针
*        @mdRTUSerialInitFun    硬件串口初始化函数  
*        @salveAddr            从机地址
*        @baud            波特率
*        @dataBits            数据位
*        @stopBits           停止位 
*        @parity       奇偶校验位
* Return          : 无
**********************************************************/
void MDS_RTU_Init(PModbusS_RTU pModbusRTU,MD_RTU_SerialInit mdRTUSerialInitFun,uint8 salveAddr,uint32 baud,uint8 dataBits,uint8 stopBits,uint8 parity){
	uint8 i;
	float T;
	if(pModbusRTU==NULL){
		return ;
	}
	MDInitQueue(&(pModbusRTU->mdSqQueue));
	MDInitQueue(&(pModbusRTU->mdMsgSqQueue));
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
	TO_MDBase(pModbusRTU)->mdRTUSendBytesFunction=MDSSerialSendBytes;
	TO_MDBase(pModbusRTU)->mdRTURecSendConv=MDSSerialSWRecv_Send;
	
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
	pModbusRTU->frameIntervalTime=time;/*该参数需要根据波特率设置*/
	
	pModbusRTU->CRC16Update=0xFFFF;
	
	if(mdRTUSerialInitFun!=NULL){
		mdRTUSerialInitFun(pModbusRTU,baud, dataBits,stopBits,parity);
	}
	return ;
}
/*******************************************************
*
* Function name :MDS_RTU_SetWriteListenFun
* Description        :该函数可以设置一个回调函数，当主机写从机地址时，设置的函数将被调用
* Parameter         :
*        @pModbus_RTU            从机结构体指针
*        @wFun    设置的回调函数
* Return          : 无
**********************************************************/
void MDS_RTU_SetWriteListenFun(PModbusS_RTU pModbus_RTU,MDSWriteFunciton wFun){
	if(pModbus_RTU==NULL){return ;}
	pModbus_RTU->mdsWriteFun=wFun;
}
/*******************************************************
*
* Function name :MDS_RTU_TimeHandler
* Description        :该函数需要在定时器中断中调用，中断间隔时间100US
* Parameter         :
*        @obj            从机结构体指针
* Return          : 无
**********************************************************/
void MDS_RTU_TimeHandler(void *obj){
	uint32 tempTick=0;
	uint8 overFlag=0;
	PModbusS_RTU pModbusRTU=obj;
	if(!pModbusRTU){ return; }
	
	pModbusRTU->timesTick++;
	
	if(pModbusRTU->timesTick==0xFFFFFFFF){/*溢出了 pModbusRTU->lastTimesTick==0xFFFFFFFF*/
		tempTick=0xFFFFFFFF-pModbusRTU->lastSendTimes;
		pModbusRTU->timesTick=tempTick; /*系统时间偏移*/
		pModbusRTU->lastSendTimes=0;/*上次发送时间清零*/
		overFlag=1;
	}
	
	if(pModbusRTU->lastTimesTick==0xFFFFFFFF){return ;}/*已经开始接收包*/
	if(overFlag){ /*时间溢出*/
		pModbusRTU->timesTick += 0xFFFFFFFF-pModbusRTU->lastTimesTick;/*发送时时间偏移*/
		pModbusRTU->lastTimesTick = tempTick; 
	}
	if((pModbusRTU->timesTick - pModbusRTU->lastTimesTick >= pModbusRTU->frameIntervalTime)){
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

/*******************************************************
*
* Function name :MDS_RTU_RecvByte
* Description        :该函数在串口中断中调用，当接收到一个字节后调用该函数
* Parameter         :
*        @obj            从机结构体指针
*        @byte    接收到的单字节
* Return          : 无
**********************************************************/
void MDS_RTU_RecvByte(void *obj,uint8 byte){
	PModbusS_RTU pModbusRTU=obj;
	if(!pModbusRTU){ return; }
	/*在这里放入队列中*/
	if(MDenQueue(&(pModbusRTU->mdSqQueue),byte)==FALSE){
		return ;
	}
	if(pModbusRTU->lastTimesTick==0xFFFFFFFF){
		pModbusRTU->CRC16Update=0xFFFF;
	}
	pModbusRTU->CRC16Update=MD_CRC16Update(pModbusRTU->CRC16Update,byte);
	/*保存上次接收的字符的时间戳*/
	pModbusRTU->lastTimesTick=pModbusRTU->timesTick;
}
/*******************************************************
*
* Function name :MDS_RTU_AddMapItem
* Description        :该函数向离散映射表中添加一条映射记录
* Parameter         :
*        @obj            从机结构体指针
*        @byte    接收到的单字节
* Return          : 无
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
* Description        :从机发送一个字节
* Parameter         :
*        @pModbusRTU            从机结构体指针
*        @byte    需要发送的字节
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
* Description        :该函数内部调用，获取接收到的一个包数据
* Parameter         :
*        @pModbus_RTU            从机结构体指针
* Return          : 无
**********************************************************/
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
		}else {/*不支持的功能码*/
			
		}
	}
	
	if(pModbus_RTU->serialReadCount<=MDS_RTU_CMD_SIZE){
		pModbus_RTU->serialReadCount++;
	}
	
	return FALSE;
}
/*******************************************************
*
* Function name :MDS_RTU_Process
* Description        :该函数处理接收到的包数据
* Parameter         :
*        @pModbus_RTU            从机结构体指针
* Return          : 无
**********************************************************/
void MDS_RTU_Process(PModbusS_RTU pModbus_RTU){
	BOOL res;
	if(!pModbus_RTU){return ;}
	
	if(pModbus_RTU->timesTick - pModbus_RTU->lastSendTimes < pModbus_RTU->frameIntervalTime){
		/*帧间隔时间，3.5T处理一帧*/
		return;
	}
	
	res = MDS_RTU_SerialProcess(pModbus_RTU);/*从队列中读取一个指令*/
	if(!res){
		return ;
	}
	
	if(
		pModbus_RTU->serialReadCache[0]!=0x00	/*广播地址*/
	&&
		pModbus_RTU->serialReadCache[0]!=pModbus_RTU->salveAddr
	){
		/*不属于本从机，丢弃*/
		goto __exit;
	}

	/*功能码01-04具有基本类似的传输结构*/
	/*01 读取线圈状态 	取得一组逻辑线圈的当前状态（ON/OFF)*/
	/*02 读取输入状态 	取得一组开关输入的当前状态(ON/OFF)*/
	/*03 读取保持寄存器 在一个或多个保持寄存器中取得当前的二进制值*/
	/*04 读取输入寄存器 在一个或多个输入寄存器中取得当前的二进制值*/
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
	/*05 强置单线圈 强置一个逻辑线圈的通断状态*/
	/*06 预置单寄存器 把具体二进值装入一个保持寄存器*/
		uint16 startReg=MDS_RTU_START_REG(pModbus_RTU);
		uint16 val= ((pModbus_RTU->serialReadCache[4])<<8)|pModbus_RTU->serialReadCache[5];

		MDS_RTU_WriteDataProcess(pModbus_RTU,startReg,1,MDS_RTU_FUN_CODE(pModbus_RTU),&val,2);
	}else if(	
			MDS_RTU_FUN_CODE(pModbus_RTU)==15||
			MDS_RTU_FUN_CODE(pModbus_RTU)==16
	){
	/*15 强置多线圈 强置一串连续逻辑线圈的通断*/
	/*16 预置多寄存器 把具体的二进制值装入一串连续的保持寄存器*/
		uint16 startReg=MDS_RTU_START_REG(pModbus_RTU);
		uint16 regNum=MDS_RTU_REGS_NUM(pModbus_RTU);
		uint8	 bytesNum=MDS_RTU_BYTES_NUM(pModbus_RTU);
		if(MDS_RTU_FUN_CODE(pModbus_RTU)==15){
			if(((regNum>>3)+((regNum%8)?1:0))!=bytesNum){
				MDS_RTU_SendErrorCode(pModbus_RTU,WRITE_COILS,ILLEGAL_DAT_VAL);
				/*出现了长度不匹配错误，丢弃*/
				goto __exit0;
			}
		}else {//16
			if((regNum<<1)!=bytesNum){
				MDS_RTU_SendErrorCode(pModbus_RTU,WRITE_REGS,ILLEGAL_DAT_VAL);
				/*出现了长度不匹配错误，丢弃*/
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
* Description        :该函数返回接收与处理中的错误码信息给主机
* Parameter         :
*        @pModbus_RTU            从机结构体指针
*        @anlCode            异常码，参见[ANLCode]
*        @errCode            错误码,参见[ErrorCode]
* Return          : 无
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
* Description        :该函数处理主机的读数据请求
* Parameter         :
*        @pModbus_RTU            从机结构体指针
*        @reg            读取的地址起始
*        @regNum            读取的数据个数
*        @funCode            操作的功能码
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
			/*确保读取的范围在内存的范围之内*/
			if((funCode==1&&pModbus_RTU->pMapTableList[i]->addrType==COILS_TYPE)||
				(funCode==2&&pModbus_RTU->pMapTableList[i]->addrType==INPUT_TYPE)){
				/*确保是读取的bit*/
				/*得到位偏移*/
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
						/*当前写入字节位置与上次不同则说明需要发送一个字节*/
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
				/*确保读取的是reg*/
					/*得到uint16偏移*/
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
				/*地址不能读取*/
				continue;
			}
			return TRUE;
		}
	}
	/*地址异常*/
	MDS_RTU_SendErrorCode(pModbus_RTU,(ANLCode)(0x80+funCode),ILLEGAL_DAT_ADDR);
	return FALSE;
}
/*******************************************************
*
* Function name :MDS_RTU_WriteDataProcess
* Description        :该函数处理主机的写数据请求
* Parameter         :
*        @pModbus_RTU            从机结构体指针
*        @reg            读取的地址起始
*        @regNum            读取的数据个数
*        @funCode            操作的功能码
*        @data            写入的数据
*        @byteCount            写入的数据占多少字节
* Return          : TRUE success , FALSE fail
**********************************************************/
uint8 MDS_RTU_WriteDataProcess(PModbusS_RTU pModbus_RTU,uint16 reg,
uint16 regNum,uint8 funCode,uint16* data,uint8 byteCount){
	uint8 res=FALSE;
	switch(funCode){
		case 5:/*写单线圈*/
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
		case 15:/*写多线圈*/
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
		case 6:/*写单寄存器*/
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
		case 16:/*写多寄存器*/
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



