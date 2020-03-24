XTinyModbus
===
1.目前项目状态
----
1)实现ModBus RTU salve。<br>
2)加入双队列机制。<br>
3)实现功能码1,2,3,4,5,6,15,16。<br>
4)支持modbus地址与本机地址离散映射。<br>
5)提供用户读写modbus地址接口<br>
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
实现Modbus RTU主机，Modbus ASCII主机与从机。




