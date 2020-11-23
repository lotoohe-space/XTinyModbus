/********************************************************************************
* @File name: MD_RTU_Serial.c
* @Author: zspace
* @Emial: 1358745329@qq.com
* @Version: 1.0
* @Date: 2020-4-10
* @Description: Modbus RTU Serial related modules
* Open source address: https://github.com/lotoohe-space/XTinyModbus
********************************************************************************/

/*********************************HEAD FILE************************************/
#include "MDM_RTU_Serial.h"
#include "MD_RTU_Tool.h"

/*User-related header files*/
#include "Sys_config.h"
#if	!MDM_USD_USART3
#include "usart.h"
#else 
#include "usart3.h"
#endif

/*********************************END******************************************/

/*********************************GLOBAL VARIABLE************************************/
PModbusBase pModbusMBase=NULL;		/*Modbus of the current serial port*/
/*********************************END******************************************/

/*********************************FUNCTION DECLARATION************************************/

//void MDMTimeHandler100US(void);
/*********************************END******************************************/

/*******************************************************
*
* Function name :MDMInitSerial
* Description        	:Hardware initialization function, you can initialize the serial port here
* Parameter         	:
*        @obj        	Host object pointer    
*        @baud    		Baud rate
*        @dataBits    Data bit
*        @stopBit    	Stop bit
*        @parity    	Parity bit
* Return          		:None
**********************************************************/
void MDMInitSerial(void* obj,uint32 baud,uint8 dataBits,uint8 stopBit,uint8 parity){
	pModbusMBase=obj;
	if(obj==NULL){return ;}
	
	
	pModbusMBase->mdRTUSendBytesFunction=MDMSerialSendBytes;
	pModbusMBase->mdRTURecSendConv=MDMSerialSWRecv_Send;
	
	/*Hardware initialization*/
	#if	!MDM_USD_USART3
		uart_init(baud);
	#else 
		init_usart3(baud);
	#endif
	
}
/*******************************************************
*
* Function name :MDMTimeHandler100US
* Description        :Call this function in the timer
* Parameter         :None
* Return          :None
**********************************************************/
void MDMTimeHandler100US(uint32 times){
	if(pModbusMBase==NULL){return;}
	pModbusMBase->mdRTUTimeHandlerFunction(pModbusMBase,times);
}
/*******************************************************
*
* Function name :MDMSerialRecvByte
* Description        :Bsp layer serial port interrupt receiving call this function
* Parameter         :
*        @byte    Byte received
* Return          : None
**********************************************************/
void MDMSerialRecvByte(uint8 byte){
	if(pModbusMBase==NULL){return;}
	pModbusMBase->mdRTURecByteFunction(pModbusMBase , byte);
}
/*******************************************************
*
* Function name :MDMSerialRecvByte
* Description        :Switch to receive or send
* Parameter         :
*        @mode    TRUE send, FALSE receive
* Return          : None
**********************************************************/
void MDMSerialSWRecv_Send(uint8 mode){
	/*Send and receive conversion*/
	/*Fill in the converted code below*/
	#if	!MDM_USD_USART3
		RS485_RW_CONV1=mode;
	#else
		RS485_RW_CONV=mode;
	#endif
	/*Different hardware may require a little delay after setting conversion*/
}
/*******************************************************
*
* Function name :MDMSerialSendBytes
* Description        :Send function
* Parameter         :
*        @bytes    Data sent
*        @num    How many bytes to send
* Return          : None
**********************************************************/
void MDMSerialSendBytes(uint8 *bytes,uint16 num){
	#if	!MDM_USD_USART3
	/*Call the send function of bsp below*/
	uart_send_bytes(bytes,num);
	#else
	usart3_send_bytes(bytes,num);
	#endif
}


