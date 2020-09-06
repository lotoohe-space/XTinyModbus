/********************************************************************************
* @File name: MDM_RTU_Fun.c
* @Author: zspace
* @Emial: 1358745329@qq.com
* @Version: 1.0
* @Date: 2020-4-10
* @Description: Modbus RTU 主机功能模块
* 开源地址: https://github.com/lotoohe-space/XTinyModbus
********************************************************************************/

/*********************************头文件包含************************************/
#include "MDM_RTU_Fun.h"
#include "MDM_RTU_Serial.h"
#include "MD_RTU_Tool.h"
#include "MD_RTU_CRC16.h"
/*********************************结束******************************************/

/*********************************函数申明************************************/
#if !MDM_USE_SEND_CACHE
static void MDM_RTU_SendByte(PModbus_RTU pModbus_RTU,uint8 byte);
#endif
void MDM_RTU_RecvByte(void *obj,uint8 byte);
/*********************************结束******************************************/

/*******************************************************
*
* Function name :MDM_RTU_Init
* Description        :Modbus RTU 主机初始化
* Parameter         :
*        @pModbusRTU        主机对象指针    
*        @mdRTUSerialInitFun    串口初始化函数
*        @baud    波特率
*        @dataBits    数据位
*        @stopBits    停止位
*        @parity    奇偶校验位
* Return          : 参见[MDError]
**********************************************************/
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
		pModbusRTU->pMapTableList[i] = NULL;
	}
	TO_MDBase(pModbusRTU)->mdRTUTimeHandlerFunction=MDM_RTU_TimeHandler;
	/*数据发送接收有关的函数*/
	TO_MDBase(pModbusRTU)->mdRTUSendBytesFunction=NULL;
	TO_MDBase(pModbusRTU)->mdRTURecByteFunction=MDM_RTU_RecvByte;
	TO_MDBase(pModbusRTU)->mdRTURecSendConv=NULL;
#if MDM_USE_SEND_CACHE
	pModbusRTU->serialSendCount=0;
#endif
	/*上次接收的时间,0xFFFFFFF表示未起开始检测帧*/
	pModbusRTU->lastTimesTick=0xFFFFFFFF;
	/*当前的实时时间单位100US*/
	pModbusRTU->timesTick=0;
	
	T=(1.0/(float)baud)*100000;
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
/*******************************************************
*
* Function name :MDM_RTU_CB_Init
* Description        :Modbus RTU 主机发送控制块初始化，控制块主要包含从发时间，次数等信息维护
* Parameter         :
*        @pModbusRTUCB        发送控制块对象指针    
*        @pModbusRTU    主机对象指针
*        @sendIntervalTime    发送间隔时间
*        @sendOverTime    发送超时时间
*        @RTTimes    重传次数
* Return          : 无
**********************************************************/
void MDM_RTU_CB_Init(
	PModbus_RTU_CB 	pModbusRTUCB
	,PModbus_RTU 		pModbusRTU
	,uint32 				sendIntervalTime
	,uint32					sendOverTime
	,uint8 					RTTimes
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
/*******************************************************
*
* Function name :MDM_RTU_CB_OverTimeReset
* Description        :发送控制块超时复位
* Parameter         :
*        @pModbusRTUCB        发送控制块对象指针    
* Return          : 无
**********************************************************/
void MDM_RTU_CB_OverTimeReset(PModbus_RTU_CB 	pModbusRTUCB){
	if(pModbusRTUCB==NULL){return ;}
	pModbusRTUCB->RTCount=0;
	pModbusRTUCB->sendFlag=0;
}
/*******************************************************
*
* Function name :MDM_RTU_TimeHandler
* Description        :定时处理函数，定时单位100US
* Parameter         :
*        @obj        主机对象指针    
* Return          : 无
**********************************************************/
void MDM_RTU_TimeHandler(void *obj){
	PModbus_RTU pModbusRTU=obj;
	if(!pModbusRTU){ return; }
	pModbusRTU->timesTick++;
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
/*******************************************************
*
* Function name :MDM_RTU_RecvByte
* Description        :该函数接收数据并且放入队列中
* Parameter         :
*        @obj        主机对象指针    
*        @byte       接收的一个字节    
* Return          : 无
**********************************************************/
void MDM_RTU_RecvByte(void *obj,uint8 byte){
	PModbus_RTU pModbusRTU=obj;
	if(!pModbusRTU){ return; }
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
#if !MDM_USE_SEND_CACHE
/*******************************************************
*
* Function name :MDM_RTU_SendByte
* Description        :发送一个字节
* Parameter         :
*        @pModbus_RTU        主机对象指针    
*        @byte       发送的一个字节    
* Return          : 无
**********************************************************/
static void MDM_RTU_SendByte(PModbus_RTU pModbus_RTU,uint8 byte){
	if(!pModbus_RTU){ return; }
	TO_MDBase(pModbus_RTU)->mdRTUSendBytesFunction(&byte,1);
}
#endif
/*******************************************************
*
* Function name :MDM_RTU_AddMapItem
* Description        :该函数向离散映射表中添加一条映射记录
* Parameter         :
*        @pModbusRTU      主机结构体指针
*        @pRegCoilItem    需要添加的项
* Return          : 无
**********************************************************/
BOOL MDM_RTU_AddMapItem(PModbus_RTU pModbusRTU,PMapTableItem pMapTableItem){
	if(pModbusRTU==NULL || pMapTableItem==NULL){
			return FALSE;
	}
	return MapTableAdd(pModbusRTU->pMapTableList, pMapTableItem,MDM_REG_COIL_ITEM_NUM);
}

/*******************************************************
*
* Function name :MDM_RTU_ReadByte
* Description        :从接收队列中获取数据
* Parameter         :
*        @pModbus_RTU        主机对象指针    
*        @res       获取数据缓存
*        @len       获取的数据长度    
* Return          : 无
**********************************************************/
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
/*******************************************************
*
* Function name :MDM_RTU_ReadUint16
* Description        :从接收队列中获取数据
* Parameter         :
*        @pModbus_RTU        主机对象指针    
*        @res       获取数据缓存
*        @len       获取的数据长度    
* Return          : 无
**********************************************************/
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
/*******************************************************
*
* Function name :MDM_RTU_ReadFun
* Description        :该函数发送读功能相关指令
* Parameter         :
*        @pModbus_RTU        主机对象指针    
*        @funCode       功能码
*        @slaveAddr       从机地址    
*        @startAddr       读取开始地址    
*        @numOf       读取的个数    
* Return          : 无
**********************************************************/
static void MDM_RTU_ReadFun(PModbus_RTU pModbus_RTU,uint8 funCode,uint8 slaveAddr,uint16 startAddr,uint16 numOf){
	MD_RTU_SEND_MODE(pModbus_RTU);
	MEM_RTU_START_EN(pModbus_RTU);
	MEM_RTU_EN_QUEUE(pModbus_RTU,slaveAddr);
	MEM_RTU_EN_QUEUE(pModbus_RTU,funCode);
	MEM_RTU_EN_QUEUE(pModbus_RTU,MD_H_BYTE(startAddr));
	MEM_RTU_EN_QUEUE(pModbus_RTU,MD_L_BYTE(startAddr));
	MEM_RTU_EN_QUEUE(pModbus_RTU,MD_H_BYTE(numOf));
	MEM_RTU_EN_QUEUE(pModbus_RTU,MD_L_BYTE(numOf));
	MEM_RTU_END_EN(pModbus_RTU);
	MD_RTU_RECV_MODE(pModbus_RTU);
}
/*******************************************************
*
* Function name :MDM_RTU_WriteSingleFun
* Description        :写单个线圈与寄存器函数
* Parameter         :
*        @pModbus_RTU        主机对象指针    
*        @funCode       功能码
*        @slaveAddr       从机地址    
*        @startAddr       写开始地址    
*        @value       写入的值  
* Return          : 无
**********************************************************/
static void MDM_RTU_WriteSingleFun(PModbus_RTU pModbus_RTU,uint8 funCode,uint8 slaveAddr,uint16 startAddr,uint16 value){
	MD_RTU_SEND_MODE(pModbus_RTU);
	MEM_RTU_START_EN(pModbus_RTU);
	MEM_RTU_EN_QUEUE(pModbus_RTU,slaveAddr);
	MEM_RTU_EN_QUEUE(pModbus_RTU,funCode);
	MEM_RTU_EN_QUEUE(pModbus_RTU,MD_H_BYTE(startAddr));
	MEM_RTU_EN_QUEUE(pModbus_RTU,MD_L_BYTE(startAddr));
	MEM_RTU_EN_QUEUE(pModbus_RTU,MD_H_BYTE(value));
	MEM_RTU_EN_QUEUE(pModbus_RTU,MD_L_BYTE(value));
	MEM_RTU_END_EN(pModbus_RTU);
	MD_RTU_RECV_MODE(pModbus_RTU);
}
/*******************************************************
*
* Function name :MDM_RTU_WriteFun
* Description        :写多个线圈与多个寄存器
* Parameter         :
*        @pModbus_RTU        主机对象指针    
*        @funCode       功能码
*        @slaveAddr       从机地址    
*        @startAddr       写开始地址    
*        @numOf       写入的数据个数
*        @data       写入的数据 
* Return          : 无
**********************************************************/
static void MDM_RTU_WriteFun(PModbus_RTU pModbus_RTU,
	uint8 funCode,uint8 slaveAddr,uint16 startAddr,uint8 numOf,uint8 *data){
	uint16 i;
	uint8 wLen;
	MD_RTU_SEND_MODE(pModbus_RTU);
	MEM_RTU_START_EN(pModbus_RTU);
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
	MD_RTU_RECV_MODE(pModbus_RTU);
}
/*******************************************************
*
* Function name :MDM_RTU_InsideWriteBits
* Description        :向离散映射中写入bit
* Parameter         :
*        @obj        主机对象指针    
*        @modbusAddr   离散映射的modbus地址
*        @numOf       写入个数    
*        @bit       写入的数据    
*        @opAddrType       写入的地址类型(COILS_TYPE,INPUT_TYPE)，参见[AddrType]
* Return          : TRUE , FALSE
**********************************************************/
BOOL MDM_RTU_InsideWriteBits(
	void* obj,
	uint16 modbusAddr,
	uint16 numOf, 
	uint8 *bit, 
	AddrType opAddrType,
	uint8 devAddr
){
	uint16 i;
	PModbus_RTU pModbus_RTU = obj;
	if(pModbus_RTU==NULL){return FALSE;}
	if(opAddrType != COILS_TYPE && opAddrType != INPUT_TYPE){return FALSE;}
	
	for(i=0;i<MDM_REG_COIL_ITEM_NUM;i++){
		if(pModbus_RTU->pMapTableList[i]==NULL){
			continue;
		}
		/*检查设备号*/
		if(devAddr!=pModbus_RTU->pMapTableList[i]->devAddr){continue;}
		
		if(pModbus_RTU->pMapTableList[i]->modbusAddr<=modbusAddr&&
		(pModbus_RTU->pMapTableList[i]->modbusAddr+pModbus_RTU->pMapTableList[i]->modbusDataSize)>=(modbusAddr+numOf)
		){
			if(pModbus_RTU->pMapTableList[i]->addrType==opAddrType){/*必须是BIT类型*/
				uint16 offsetAddr=modbusAddr-MDS_RTU_REG_COIL_ITEM_ADDR(pModbus_RTU->pMapTableList[i]);
				uint16 j;
				for(j=0;j<numOf;j++){
					if(
						MD_GET_BIT( bit[j>>3] ,j%8)
					){
						MD_SET_BIT(
							MDS_RTU_REG_COIL_ITEM_DATA(
							pModbus_RTU->pMapTableList[i])[(offsetAddr+j)>>4]
						,(j+offsetAddr)%16);
					}else{
						MD_CLR_BIT(
							MDS_RTU_REG_COIL_ITEM_DATA(
							pModbus_RTU->pMapTableList[i])[(offsetAddr+j)>>4]
						,(j+offsetAddr)%16);
					}
				}
				return TRUE;
			}
		}
	}
	return FALSE;
}
/*******************************************************
*
* Function name :MDM_RTU_InsideWriteRegs
* Description        :写离散寄存器
* Parameter         :
*        @obj        主机对象指针    
*        @modbusAddr   离散映射的modbus地址
*        @numOf       写入个数    
*        @reg       写入的数据    
*        @isBigE       大端还是小端方式  
*        @opAddrType       写入的地址类型(HOLD_REGS_TYPE,INPUT_REGS_TYPE)，参见[AddrType]
* Return          : TRUE , FALSE
**********************************************************/
BOOL MDM_RTU_InsideWriteRegs(
void* obj,
uint16 modbusAddr,
uint16 numOf, 
uint16 *reg,
uint8 isBigE, 
AddrType opAddrType,
uint8 devAddr
){
	uint16 i;
	PModbus_RTU pModbus_RTU = obj;
	if(pModbus_RTU==NULL){return FALSE;}
	if(opAddrType != HOLD_REGS_TYPE && opAddrType != INPUT_REGS_TYPE){return FALSE;}
	
	for(i=0;i<MDM_REG_COIL_ITEM_NUM;i++){
		if(pModbus_RTU->pMapTableList[i]==NULL){
			continue;
		}
		/*检查设备号*/
		if(devAddr!=pModbus_RTU->pMapTableList[i]->devAddr){continue;}
		
		if(pModbus_RTU->pMapTableList[i]->modbusAddr<=modbusAddr&&
		(pModbus_RTU->pMapTableList[i]->modbusAddr+pModbus_RTU->pMapTableList[i]->modbusDataSize)>=(modbusAddr+numOf)
		){
			if(pModbus_RTU->pMapTableList[i]->addrType==opAddrType){/*必须是REG类型*/
				uint16 offsetAddr=modbusAddr-MDS_RTU_REG_COIL_ITEM_ADDR(pModbus_RTU->pMapTableList[i]);
				uint16 j=0;
				for(j=0;j<numOf;j++){
					MDS_RTU_REG_COIL_ITEM_DATA(pModbus_RTU->pMapTableList[i])[offsetAddr+j]=
					isBigE?MD_SWAP_HL(reg[j]):reg[j];
				}		
				return TRUE;
			}
		}
	}
	return FALSE;
}
/*******************************************************
*
* Function name :MDM_RTU_NB_RW
* Description        :非阻塞式读写
* Parameter         :
*        @pModbus_RTU_CB    写入控制块对象指针  
*        @funCode   功能码，参见[ModbusFunCode]
*        @slaveAddr      	从机地址    
*        @startAddr       读写开始地址    
*        @numOf       读写数据个数  
*        @wData       如果是写入功能码，那它就是写入的数据
* Return          : 参见[MDError]
**********************************************************/
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
			uint8 funCodeByte=0;
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
			if(!MDdeQueue(&(pModbus_RTU_CB->pModbus_RTU->mdSqQueue),&funCodeByte)){
				errRes =  ERR_DATA_LEN;
				goto _exit;
			}
			
			switch(funCodeByte){
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
						if(!MDM_RTU_InsideWriteBits(pModbus_RTU_CB->pModbus_RTU,wAddr,((index<8)?index:8), &rByte,(AddrType)funCodeByte,slaveAddr)){
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
						if(!MDM_RTU_InsideWriteRegs(pModbus_RTU_CB->pModbus_RTU,startAddr+i,1,&wTemp,0,(AddrType)funCodeByte,slaveAddr)){
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
/*******************************************************
*
* Function name :MDM_RTU_RW
* Description        :阻塞式读写
* Parameter         :
*        @pModbus_RTU_CB    写入控制块对象指针  
*        @funCode   功能码，参见[ModbusFunCode]
*        @slaveAddr      	从机地址    
*        @startAddr       读写开始地址    
*        @numOf       读写数据个数  
*        @wData       如果是写入功能码，那它就是写入的数据
* Return          : 参见[MDError]
**********************************************************/
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
/*******************************************************
*
* Function name :MDM_RTU_ReadCoil
* Description        :读线圈
* Parameter         :
*        @pModbus_RTU_CB    写入控制块对象指针  
*        @slaveAddr      	从机地址    
*        @startAddr       读开始地址    
*        @numOf       读数据个数  
* Return          : 参见[MDError]
**********************************************************/
MDError MDM_RTU_ReadCoil(PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf){
	return MDM_RTU_RW(pModbus_RTU_CB,READ_COIL,slaveAddr,startAddr,numOf,NULL);
};
/*******************************************************
*
* Function name :MDM_RTU_ReadInput
* Description        :读输入
* Parameter         :
*        @pModbus_RTU_CB    写入控制块对象指针  
*        @slaveAddr      	从机地址    
*        @startAddr       读开始地址    
*        @numOf       读数据个数  
* Return          : 参见[MDError]
**********************************************************/
MDError MDM_RTU_ReadInput(PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf){
	return MDM_RTU_RW(pModbus_RTU_CB,READ_INPUT,slaveAddr,startAddr,numOf,NULL);
};
/*******************************************************
*
* Function name :MDM_RTU_ReadHoldReg
* Description        :读保持寄存器
* Parameter         :
*        @pModbus_RTU_CB    写入控制块对象指针  
*        @slaveAddr      	从机地址    
*        @startAddr       读开始地址    
*        @numOf       读数据个数  
* Return          : 参见[MDError]
**********************************************************/
MDError MDM_RTU_ReadHoldReg(PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf){
	return MDM_RTU_RW(pModbus_RTU_CB,READ_HOLD_REG,slaveAddr,startAddr,numOf,NULL);
};
/*******************************************************
*
* Function name :MDM_RTU_ReadInputReg
* Description        :读输入寄存器
* Parameter         :
*        @pModbus_RTU_CB    写入控制块对象指针  
*        @slaveAddr      	从机地址    
*        @startAddr       读开始地址    
*        @numOf       读数据个数  
* Return          : 参见[MDError]
**********************************************************/
MDError MDM_RTU_ReadInputReg(PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf){
	return MDM_RTU_RW(pModbus_RTU_CB,READ_INPUT_REG,slaveAddr,startAddr,numOf,NULL);
};
/*******************************************************
*
* Function name :MDM_RTU_WriteSingleCoil
* Description        :写单个线圈
* Parameter         :
*        @pModbus_RTU_CB    写入控制块对象指针  
*        @slaveAddr      	从机地址    
*        @startAddr       读开始地址    
*        @boolVal      TRUE , FALSE  
* Return          : 参见[MDError]
**********************************************************/
MDError MDM_RTU_WriteSingleCoil(
	PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,BOOL boolVal){
	uint16 temp;
	temp=boolVal?0xFF00:0x0000;
	return MDM_RTU_RW(pModbus_RTU_CB,WRITE_SIN_COIL,slaveAddr,startAddr,1,(void*)(&temp));
};
/*******************************************************
*
* Function name :MDM_RTU_WriteSingleReg
* Description        :写单个寄存器
* Parameter         :
*        @pModbus_RTU_CB    写入控制块对象指针  
*        @slaveAddr      	从机地址    
*        @startAddr       读开始地址    
*        @boolVal      TRUE , FALSE  
* Return          : 参见[MDError]
**********************************************************/
MDError MDM_RTU_WriteSingleReg(
	PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 val){
	return MDM_RTU_RW(pModbus_RTU_CB,WRITE_SIN_REG,slaveAddr,startAddr,1,(void*)(&val));
};
/*******************************************************
*
* Function name :MDM_RTU_WriteCoils
* Description        :写线圈
* Parameter         :
*        @pModbus_RTU_CB    写入控制块对象指针  
*        @slaveAddr      	从机地址    
*        @startAddr       读开始地址    
*        @numOf       写数据个数  
*        @boolVal      TRUE , FALSE  
* Return          : 参见[MDError]
**********************************************************/
MDError MDM_RTU_WriteCoils(
	PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf,uint8* val){
	return MDM_RTU_RW(pModbus_RTU_CB,WRITE_COILS,slaveAddr,startAddr,numOf,(void*)(val));
};
/*******************************************************
*
* Function name :MDM_RTU_WriteRegs
* Description        :写寄存器
* Parameter         :
*        @pModbus_RTU_CB    写入控制块对象指针  
*        @slaveAddr      	从机地址    
*        @startAddr       读开始地址    
*        @numOf       写数据个数  
*        @boolVal      TRUE , FALSE  
* Return          : 参见[MDError]
**********************************************************/
MDError MDM_RTU_WriteRegs(
	PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf,uint16* val){
	return MDM_RTU_RW(pModbus_RTU_CB,WRITE_REGS,slaveAddr,startAddr,numOf,(void*)(val));
};

/*******************************************************
*
* Function name :MDM_RTU_ReadCoil
* Description        :非阻塞读线圈
* Parameter         :
*        @pModbus_RTU_CB    写入控制块对象指针  
*        @slaveAddr      	从机地址    
*        @startAddr       读开始地址    
*        @numOf       读数据个数  
* Return          : 参见[MDError]
**********************************************************/
MDError MDM_RTU_NB_ReadCoil(PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf){
	return MDM_RTU_NB_RW(pModbus_RTU_CB,READ_COIL,slaveAddr,startAddr,numOf,NULL);
};
/*******************************************************
*
* Function name :MDM_RTU_ReadInput
* Description        :非阻塞读输入
* Parameter         :
*        @pModbus_RTU_CB    写入控制块对象指针  
*        @slaveAddr      	从机地址    
*        @startAddr       读开始地址    
*        @numOf       读数据个数  
* Return          : 参见[MDError]
**********************************************************/
MDError MDM_RTU_NB_ReadInput(PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf){
	return MDM_RTU_NB_RW(pModbus_RTU_CB,READ_INPUT,slaveAddr,startAddr,numOf,NULL);
};
/*******************************************************
*
* Function name :MDM_RTU_ReadHoldReg
* Description        :非阻塞读保持寄存器
* Parameter         :
*        @pModbus_RTU_CB    写入控制块对象指针  
*        @slaveAddr      	从机地址    
*        @startAddr       读开始地址    
*        @numOf       读数据个数  
* Return          : 参见[MDError]
**********************************************************/
MDError MDM_RTU_NB_ReadHoldReg(PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf){
	return MDM_RTU_NB_RW(pModbus_RTU_CB,READ_HOLD_REG,slaveAddr,startAddr,numOf,NULL);
};
/*******************************************************
*
* Function name :MDM_RTU_ReadHoldReg
* Description        :非阻塞读输入寄存器
* Parameter         :
*        @pModbus_RTU_CB    写入控制块对象指针  
*        @slaveAddr      	从机地址    
*        @startAddr       读开始地址    
*        @numOf       读数据个数  
* Return          : 参见[MDError]
**********************************************************/
MDError MDM_RTU_NB_ReadInputReg(PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf){
	return MDM_RTU_NB_RW(pModbus_RTU_CB,READ_INPUT_REG,slaveAddr,startAddr,numOf,NULL);
};
/*******************************************************
*
* Function name :MDM_RTU_WriteSingleCoil
* Description        :非阻塞写单个线圈
* Parameter         :
*        @pModbus_RTU_CB    写入控制块对象指针  
*        @slaveAddr      	从机地址    
*        @startAddr       读开始地址    
*        @boolVal      TRUE , FALSE  
* Return          : 参见[MDError]
**********************************************************/
MDError MDM_RTU_NB_WriteSingleCoil(
	PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,BOOL boolVal){
	uint16 temp;
	temp=boolVal?0xFF00:0x0000;
	return MDM_RTU_NB_RW(pModbus_RTU_CB,WRITE_SIN_COIL,slaveAddr,startAddr,1,(void*)(&temp));
};
/*******************************************************
*
* Function name :MDM_RTU_WriteSingleReg
* Description        :非阻塞写单个寄存器
* Parameter         :
*        @pModbus_RTU_CB    写入控制块对象指针  
*        @slaveAddr      	从机地址    
*        @startAddr       读开始地址    
*        @boolVal      TRUE , FALSE  
* Return          : 参见[MDError]
**********************************************************/
MDError MDM_RTU_NB_WriteSingleReg(
	PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 val){
	return MDM_RTU_NB_RW(pModbus_RTU_CB,WRITE_SIN_REG,slaveAddr,startAddr,1,(void*)(&val));
};
/*******************************************************
*
* Function name :MDM_RTU_WriteCoils
* Description        :非阻塞写线圈
* Parameter         :
*        @pModbus_RTU_CB    写入控制块对象指针  
*        @slaveAddr      	从机地址    
*        @startAddr       读开始地址    
*        @numOf       写数据个数  
*        @boolVal      TRUE , FALSE  
* Return          : 参见[MDError]
**********************************************************/
MDError MDM_RTU_NB_WriteCoils(
	PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf,uint8* val){
	return MDM_RTU_NB_RW(pModbus_RTU_CB,WRITE_COILS,slaveAddr,startAddr,numOf,(void*)(val));
};
/*******************************************************
*
* Function name :MDM_RTU_WriteRegs
* Description        :非阻塞写寄存器
* Parameter         :
*        @pModbus_RTU_CB    写入控制块对象指针  
*        @slaveAddr      	从机地址    
*        @startAddr       读开始地址    
*        @numOf       写数据个数  
*        @boolVal      TRUE , FALSE  
* Return          : 参见[MDError]
**********************************************************/
MDError MDM_RTU_NB_WriteRegs(
	PModbus_RTU_CB pModbus_RTU_CB,uint8 slaveAddr,uint16 startAddr,uint16 numOf,uint16* val){
	return MDM_RTU_NB_RW(pModbus_RTU_CB,WRITE_REGS,slaveAddr,startAddr,numOf,(void*)(val));
};
	
