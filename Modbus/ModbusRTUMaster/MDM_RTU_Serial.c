/********************************************************************************
* @File name: MD_RTU_Serial.c
* @Author: zspace
* @Version: 1.0
* @Date: 2020-4-10
* @Description: Modbus RTU 串口相关模块
* 开源地址: https://github.com/lotoohe-space/XTinyModbus
********************************************************************************/

/*********************************头文件包含************************************/
#include "MDM_RTU_Serial.h"
#include "MD_RTU_Tool.h"
#include "usart.h"
/*********************************结束******************************************/

/*********************************全局变量************************************/
PModbusBase pModbusMBase=NULL;		/*当前串口的Modbus*/
/*********************************结束******************************************/

/*********************************函数申明************************************/

//void MDMTimeHandler100US(void);
/*********************************结束******************************************/

/*******************************************************
*
* Function name :MDMInitSerial
* Description        	:硬件初始化函数，可在此初始化串口
* Parameter         	:
*        @obj        	主机对象指针    
*        @baud    		波特率
*        @dataBits    数据位
*        @stopBit    	停止位
*        @parity    	奇偶校验位
* Return          		:无
**********************************************************/
void MDMInitSerial(void* obj,uint32 baud,uint8 dataBits,uint8 stopBit,uint8 parity){
	pModbusMBase=obj;
	if(obj==NULL){return ;}
	
//	pModbusMBase->mdRTUSendBytesFunction=MDMSerialSendBytes;
//	pModbusMBase->mdRTURecSendConv=MDMSerialSWRecv_Send;
	
	/*硬件初始化*/
	uart_init(baud);
}
/*******************************************************
*
* Function name :MDMTimeHandler100US
* Description        :定时器中调用该函数
* Parameter         :无
* Return          : 无
**********************************************************/
void MDMTimeHandler100US(void){
	if(pModbusMBase==NULL){return;}
	pModbusMBase->mdRTUTimeHandlerFunction(pModbusMBase );
}
/*******************************************************
*
* Function name :MDMSerialRecvByte
* Description        :bsp层串口中断接收调用这个函数
* Parameter         :
*        @byte    接收的一字节
* Return          : 无
**********************************************************/
void MDMSerialRecvByte(uint8 byte){
	if(pModbusMBase==NULL){return;}
	pModbusMBase->mdRTURecByteFunction(pModbusMBase , byte);
}
/*******************************************************
*
* Function name :MDMSerialRecvByte
* Description        :切换接收，或者发送
* Parameter         :
*        @mode    TRUE 发送， FALSE接收
* Return          : 无
**********************************************************/
void MDMSerialSWRecv_Send(uint8 mode){
	/*收发转换*/
	/*下面填写转换的代码*/
	
	/*不同的硬件可能在设置转换后需要一点延时*/
}
/*******************************************************
*
* Function name :MDMSerialSendBytes
* Description        :发送函数
* Parameter         :
*        @bytes    发送的数据
*        @num    发送多少个字节
* Return          : 无
**********************************************************/
void MDMSerialSendBytes(uint8 *bytes,uint16 num){
	/*在下面调用bsp的发送函数*/
	uart_send_bytes(bytes,num);
}


