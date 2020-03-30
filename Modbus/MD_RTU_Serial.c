/**
* @file 		MD_RTU_Serial.c
* @brief		无
* @details	无
* @author		zspace
* @date		2020/3/23
* @version	A001
* @par History:  无       
*/
#include "MD_RTU_Serial.h"

#include "MD_RTU_Fun.h"
#include "usart.h"

/*当前串口的Modbus*/
PModbusBase pModbusBase=NULL;

void MDSSerialSendBytes(uint8 *bytes,uint16 num);
void MDSSerialSWRecv_Send(uint8 mode);
void MDSTimeHandler100US(uint32 times);

/*硬件初始化函数，可在此初始化串口*/
void MDSInitSerial(void* obj,uint32 baud,uint8 dataBits,uint8 stopBit,uint8 parity){
	pModbusBase=obj;
	if(obj==NULL){return ;}
	
	pModbusBase->mdRTUSendBytesFunction=MDSSerialSendBytes;
	pModbusBase->mdRTURecSendConv=MDSSerialSWRecv_Send;
}

/*bsp层中断接收调用这个函数*/
void MDSSerialRecvByte(uint8 byte){
	if(pModbusBase==NULL){return;}
	pModbusBase->mdRTURecByteFunction(pModbusBase , byte);
}

/*切换接收，或者发送*/
void MDSSerialSWRecv_Send(uint8 mode){
	/*暂时没有实现该功能*/
}
/*发送函数*/
void MDSSerialSendBytes(uint8 *bytes,uint16 num){
	/*在下面调用bsp的发送函数*/
	uart_send_bytes(bytes,num);
}
/*定时器中调用该函数*/
void MDSTimeHandler100US(uint32 times){
	if(pModbusBase==NULL){return;}
	pModbusBase->mdRTUTimeHandlerFunction(pModbusBase ,times);
}
