#include "MDM_RTU_Serial.h"
#include "MD_RTU_Tool.h"
#include "usart.h"

/*当前串口的Modbus*/
PModbusBase pModbusMBase=NULL;

static void MDMSerialSendBytes(uint8 *bytes,uint16 num);
static void MDMSerialSWRecv_Send(uint8 mode);
void MDMTimeHandler100US(uint32 times);

/*硬件初始化函数，可在此初始化串口*/
void MDMInitSerial(void* obj,uint32 baud,uint8 dataBits,uint8 stopBit,uint8 parity){
	pModbusMBase=obj;
	if(obj==NULL){return ;}
	
	pModbusMBase->mdRTUSendBytesFunction=MDMSerialSendBytes;
	pModbusMBase->mdRTURecSendConv=MDMSerialSWRecv_Send;
}
/*定时器中调用该函数*/
void MDMTimeHandler100US(uint32 times){
	if(pModbusMBase==NULL){return;}
	pModbusMBase->mdRTUTimeHandlerFunction(pModbusMBase ,times);
}
/*bsp层中断接收调用这个函数*/
void MDMSerialRecvByte(uint8 byte){
	if(pModbusMBase==NULL){return;}
	pModbusMBase->mdRTURecByteFunction(pModbusMBase , byte);
}

/*切换接收，或者发送*/
static void MDMSerialSWRecv_Send(uint8 mode){
	/*暂时没有实现该功能*/
}
/*发送函数*/
static void MDMSerialSendBytes(uint8 *bytes,uint16 num){
	/*在下面调用bsp的发送函数*/
	uart_send_bytes(bytes,num);
}


