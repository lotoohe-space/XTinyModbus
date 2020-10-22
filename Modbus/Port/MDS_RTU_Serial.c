/********************************************************************************
* @File name: MD_RTU_Serial.c
* @Author: zspace
* @Emial: 1358745329@qq.com
* @Version: 1.0
* @Date: 2020-4-10
* @Description: Modbus RTU serial port related modules
* Open source address: https://github.com/lotoohe-space/XTinyModbus
********************************************************************************/
/*********************************HEAD FILE************************************/
#include "MDS_RTU_Serial.h"
#include "MDS_RTU_Fun.h"

#include "usart.h"
/*********************************END******************************************/

/*********************************GLOBAL VARIABLE************************************/
PModbusBase pModbusBase=NULL;			/*Modbus of the current serial port*/
/*********************************END******************************************/

/*********************************FUNCTION DECLARATION************************************/
static void MDSSerialSendBytes(uint8 *bytes,uint16 num);
static void MDSSerialSWRecv_Send(uint8 mode);
/*********************************END******************************************/

/*******************************************************
*
* Function name :MDSInitSerial
* Description        	:Hardware initialization function, you can initialize the serial port here
* Parameter         	:
*        @obj        	Slave object pointer    
*        @baud    		Baud rate
*        @dataBits    Data bit
*        @stopBit  		Stop bit
*        @parity    	Parity bit
* Return          : 	None
**********************************************************/
void MDSInitSerial(void* obj,uint32 baud,uint8 dataBits,uint8 stopBit,uint8 parity){
	pModbusBase=obj;
	if(obj==NULL){return ;}
	
	pModbusBase->mdRTUSendBytesFunction=MDSSerialSendBytes;
	pModbusBase->mdRTURecSendConv=MDSSerialSWRecv_Send;
	
	
	/*Hardware initialization*/
	uart_init(baud);
}
/*******************************************************
*
* Function name :MDSSerialRecvByte
* Description        :Bsp layer interrupt receiving call this function
* Parameter         :
*        @byte        A byte received    
* Return          : None
**********************************************************/
void MDSSerialRecvByte(uint8 byte){
	if(pModbusBase==NULL){return;}
	pModbusBase->mdRTURecByteFunction(pModbusBase , byte);
}
/*******************************************************
*
* Function name :MDSSerialSWRecv_Send
* Description        :Switch to receive or send
* Parameter         :
*        @mode      TRUE send, FALSE receive
* Return          : None
**********************************************************/
void MDSSerialSWRecv_Send(uint8 mode){
	/*Send and receive conversion*/
	/*Fill in the converted code below*/
	
	/*Different hardware may require a little delay after setting conversion*/
}
/*******************************************************
*
* Function name :MDSSerialSendBytes
* Description        :Send function
* Parameter         :
*        @bytes        Data sent
*        @num        How many bytes to send
* Return          : None
**********************************************************/
void MDSSerialSendBytes(uint8 *bytes,uint16 num){
	/*Call the send function of bsp below*/
	uart_send_bytes_by_isr(bytes,num);
}
/*******************************************************
*
* Function name :MDSTimeHandler100US
* Description        :Call this function in the timer, the timing unit is 100US
* Parameter         :None
* Return          :NOne
**********************************************************/
void MDSTimeHandler100US(void){
	if(pModbusBase==NULL){return;}
	pModbusBase->mdRTUTimeHandlerFunction(pModbusBase);
}
