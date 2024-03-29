
XTinyModbus
===

A small embedded Modbus protocol stack, written in C language, support master and slave, support discrete mapping, support non-blocking and blocking read and write mode, and provide a complete example, can be transplanted to different processors, simple to use, portable.<br>

0.Log
----

2022/10/7 <br>
Add a set of APIs to allow mixed non-blocking and blocking mode calls in host mode in bare metal state.<br>
 Check out the functions related to xx_MixXXX in MDM_RTU_Fun.c.<br>
Fix the bug of time frame calculation error.<br>

2020/11/23 <br>
Optimization of slave protocol parsing strategy, more stable <br>
Optimize the loop queue to make it more adaptable <br>
The loop queue memory is modified to the user specified <br>
Host added RTOS support, can be opened through MD_RTU_USED_OS under md_rtu_config. h, the default is closed, has been transplanted to XTinyRTOS, can be modified by md_rtu_sysinteraction. c transplant <br>
Other modifications and optimizations <br>

2020/9/24 <br>
Added the function of not polling when the slave is disconnected, the master can use the MDM_RTU_CB_SetDisPollEnFlag function to set it,<br>
You can use the MDM_RTU_CB_ClrDisFlag function to reset. <br>
Add a simple host sending controller. <br>
Fix the BUG of mixed call of host blocking mode and non-blocking mode. <br>
All comments in the code are modified to English. <br>

2020/9/6
Add a multi slave example. <br>
Add some macro for testing. <br>

2020/8/31
Add MD_ RTU_ Configure the protocol stack with config. H. <br>
Test macro configuration added. <br>
Modification of file name. <br>
Some other changes. <br>

2020/6/6
Fixed a size end error.<br>
Slaver 	corrected errors in reading discrete mapping functions.<br>
		Slave supports broadcast addresses.<br>
Master host adds send cache.<br>

2020/4/28
Discrete mappings for Master hosts will have to specify slave Numbers instead of sharing.<br>

2020/4/23
Slave 	Fixes a BUG in the CRC macro.<br>
		Fixed discrete mapping table master and slave size inconsistency BUG.<br>
Master 	Fixes bugs that cause errors in read commands and dependent variables.<br>

The serial port initialization location is changed to md_rtu_serial.c and mdm_rtu_serial.c.<br>
Fixed some naming conventions.<br>

2020/4/13
Slave adds the send cache, which is optional and enabled by default through the macro configuration in md_rtu_fun. h.<br>

2020/4/12
Added transceiver conversion control.<br>

2020/4/10
Code to add comments, specification of comments, optimize the structure of the file storage.<br>

2020/4/4
Slave fixes a bug with a minimal probability that the timer overflow can cause packet loss.<br>

2020/4/3
Salve changes the discrete mapping, and the input is mapped separately <br>

2020/3/31
Salve adds host write callback function listen.<br>
Salve adds partial exception and error code support.<br>
Salve fixed an error sending unspaced frames.<br>
Optimize CRC16 efficiency.<br>

1.Features
----

ModBus RTU salve feature: <br>
1) realize Modbus RTU salve<br>
2) join the double-queue mechanism.<br>
3) implement function code 1,2,3,4,5,6,15,16.<br>
4) support discrete mapping between modbus address and local address.<br>
5) provide user to read and write modbus address interface.<br>
6) support multiple slaves.<br>

ModBus RTU master feature: <br>
1) realize Modbus RTU master<br>
2) support non-blocking type read and write <br>
3) support blocking type read and write <br>
4) support regular retransmission <br>
5) support timeout retransmission <br>
6) support read data discrete mapping <br>

2.How to transplant
----
The example gives an example of a port in the STM32F1 series of chips that need to be ported to other chips, need to modify md_rtu_serial.c, implement port port port function migration, and need to call serial port and timer functions in the interrupt function. < br >
Serial port transplantation function:
```c
/*Send function*/
void MDSSerialSendBytes(uint8 *bytes,uint16 num){
	/*The BSP sending function is called below*/
	uart_send_bytes(bytes,num);
}
```
Call the following function in the timer interrupt function, the timer interval is 100US:
```c
/*This function is called in the timer*/
void MDSTimeHandler100US(uint32 times){
	if(_pModbus_RTU==NULL){return;}
	_pModbus_RTU->mdRTUTimeHandlerFunction(_pModbus_RTU ,times);
}
```
The following function is called in the serial port receive interrupt
```c
/*The BSP layer interrupts the receiving call to this function*/
void MDSSerialRecvByte(uint8 byte){
	if(_pModbus_RTU==NULL){return;}
	_pModbus_RTU->mdsRTURecByteFunction(_pModbus_RTU , byte);
}
```
3.How to use
----
You can see an example of using the md_rtu_app.c file.
The following code adds two address mapping items, and the modbus address corresponds to the local address:
```c
uint16 regCoilData0[32]={1,2,3,4,5,6,7,8,9,10,11,12};
RegCoilItem regCoilItem0={
. ModbusAddr =0x0000, /* address in MODBUS */
. ModbusData =regCoilData0, /* mapped memory unit */
. ModbusDataSize =32, /* the size of the map */
.addrtype =REG_TYPE /* the type of the map */
};
uint16 regCoilData1[4]={0};
RegCoilItem regCoilItem1={
. ModbusAddr =0x0000, /* address in MODBUS */
. ModbusData =regCoilData1, /* mapped memory unit */
. ModbusDataSize =64, /* the size of the map */
.addrtype =BIT_TYPE /* type of mapping */
};
	/*Add an address map*/
	if(RegCoilListAdd(&modbusS_RTU, &regCoilItem0)==FALSE){
		return FALSE;
	}
	if(RegCoilListAdd(&modbusS_RTU, &regCoilItem1)==FALSE){
		return FALSE;
	}
```
The following code can be used to read and write modbus registers and coil values:
```c
Uint16 temp = 0 XFF;
Uint16 temp1 [] = {1, 2, 3, 4, 5};
MDS_RTU_WriteBits (& modbusS_RTU, 1, 5, & temp);
MDS_RTU_WriteReg (& modbusS_RTU, 11, temp);
MDS_RTU_WriteRegs (& modbusS_RTU, 5, 5, temp1, 0).
```
4.Future features
----

Modbus RTU host, Modbus ASCII host and slave.


XTinyModbus
===

一个应用于嵌入式方面的小型Modbus协议栈，采用C语言编写，支持master与slave，支持离散映射，支持非阻塞与阻塞读写模式，并提供完整示例，可以移植到不同的处理器，使用简单，移植方便。<br>

0.日志
----

2020/11/23 <br>
优化从机协议解析策略，更加稳定<br>
优化循环队列，使得其适应性更强<br>
循环队列内存修改为用户指定<br>
主机增加RTOS支持，可以通过MD_RTU_Config.h下的MD_RTU_USED_OS开启，默认关闭，已经移植好XTinyRTOS，可以通过修改MD_RTU_Sysinterface.c进行移植<br>
其它修改及优化<br>

2020/9/24 <br>
增加从机掉线不轮询功能，主机可以使用MDM_RTU_CB_SetDisPollEnFlag函数进行设置，<br>
可以使用MDM_RTU_CB_ClrDisFlag函数进行复位。<br>
增加简单的主机发送控制器。<br>
修复主机阻塞模式和非阻塞模式混合调用的BUG。<br>
代码中所有注释修改为英文。<br>

2020/9/6
增加多从机示例。<br>
增加一些测试用的宏。<br>

2020/8/31 
增加MD_RTU_Config.h进行协议栈配置。<br>
增加测试配置宏。<br>
文件命名的修改。<br>
一些其它的修改。<br>

2020/6/6
修正一个大小端错误。<br>
Slaver	修正离散映射函数读取的错误。<br>
		从机支持广播地址。<br>
Master	主机添加发送缓存。<br>

2020/4/28
Master	主机的离散映射将必须指定从机号，不在采用共用方式。<br>

2020/4/23
Slave 	修复CRC宏的一个BUG。<br>
		修复离散映射表主从机大小不一致的BUG。<br>
Master	修复读命令，因变量从用导致错误的BUG。<br>
串口初始化位置修改到MD_RTU_Serial.c 与 MDM_RTU_Serial.c内。<br>
修正一些命名规范。<br>

2020/4/13
Slave增加发送缓存，该项为可选项，可以通过MD_RTU_Fun.h内的宏配置，默认开启。<br>

2020/4/12
增加收发转换控制。<br>

2020/4/10
代码添加注释，规范注释方式，优化文件存放结构。<br>

2020/4/4
Slave修复定时器溢出可能导致丢包的极小概率bug。<br>

2020/4/3
Slave更改离散映射方式，输入分开映射<br>

2020/3/31
Slave添加主机写回调函数监听。<br>
Slave添加部分异常码与错误码支持。<br>
Slave修复发送未进行帧间隔的错误。<br>
优化CRC16效率。<br>

1.特性
----
ModBus RTU Slave 特性：<br>
1)实现Modbus RTU Slave<br>
2)加入双队列机制。<br>
3)实现功能码1,2,3,4,5,6,15,16。<br>
4)支持modbus地址与本机地址离散映射。<br>
5)提供用户读写modbus地址接口。<br>
6)支持多个从机。<br>

ModBus RTU master 特性：<br>
1)实现Modbus RTU master<br>
2)支持非阻塞式读写<br>
3)支持阻塞式读写<br>
4)支持定时重发<br>
5)支持超时重传<br>
6)支持读数据离散映射<br>

2.如何移植
----
示例中给出了在STM32F1系列芯片的一个移植示例，去过需要移植到其它芯片，需要对MD_RTU_Serial.c进行修改，实现串口函数的移植，以及需要在中断函数中调用串口与定时器的 函数。<br>
串口移植函数：
```c
/*发送函数*/
void MDSSerialSendBytes(uint8 *bytes,uint16 num){
	/*在下面调用bsp的发送函数*/
	uart_send_bytes(bytes,num);
}
```
在定时器中断函数中调用下面这个函数，定时器间隔100US：
```c
/*定时器中调用该函数*/
void MDSTimeHandler100US(uint32 times){
	if(_pModbus_RTU==NULL){return;}
	_pModbus_RTU->mdRTUTimeHandlerFunction(_pModbus_RTU ,times);
}
```
在串口接收中断中调用下面这个函数
```c
/*bsp层中断接收调用这个函数*/
void MDSSerialRecvByte(uint8 byte){
	if(_pModbus_RTU==NULL){return;}
	_pModbus_RTU->mdsRTURecByteFunction(_pModbus_RTU , byte);
}
```
3.如何使用
----
使用方式可以查看MD_RTU_APP.c文件的使用示例。
下面代码添加两个地址映射项，吧modbus地址与本地地址对应起来：
```c
uint16 regCoilData0[32]={1,2,3,4,5,6,7,8,9,10,11,12};
RegCoilItem regCoilItem0={
	.modbusAddr=0x0000,				/*MODBUS中的地址*/
	.modbusData=regCoilData0,	/*映射的内存单元*/
	.modbusDataSize=32,				/*映射的大小*/
	.addrType=REG_TYPE				/*映射的类型*/
};
uint16 regCoilData1[4]={0};
RegCoilItem regCoilItem1={
	.modbusAddr=0x0000,				/*MODBUS中的地址*/
	.modbusData=regCoilData1,	/*映射的内存单元*/
	.modbusDataSize=64,				/*映射的大小*/
	.addrType=BIT_TYPE				/*映射的类型*/
};
	/*添加一个地址映射*/
	if(RegCoilListAdd(&modbusS_RTU, &regCoilItem0)==FALSE){
		return FALSE;
	}
	if(RegCoilListAdd(&modbusS_RTU, &regCoilItem1)==FALSE){
		return FALSE;
	}
```
下面的代码可以实现用户端读写modbus 寄存器与线圈值：
```c
	uint16 temp=0xff;
	uint16 temp1[]={1,2,3,4,5};
	MDS_RTU_WriteBits(&modbusS_RTU,1,5,&temp);
	MDS_RTU_WriteReg(&modbusS_RTU,11, temp);
	MDS_RTU_WriteRegs(&modbusS_RTU,5,5, temp1,0);
```
4.未来功能
Modbus ASCII主机与从机。

5.谢鸣
紫§尘、依叶知秋


















