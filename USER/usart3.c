#include "usart3.h"
#include "Sys_Config.h"
#if MD_USD_SALVE
#include "MDS_RTU_Serial_1.h"
#else 
#include "MDM_RTU_Serial.h"
#endif

void RS485RWConvInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC| RCC_APB2Periph_AFIO, ENABLE);//使能PORTC口时钟 
	BKP_TamperPinCmd(DISABLE);//关闭入侵检测功能，也就是PC13，可以当普通IO使用
	BKP_ITConfig(DISABLE);    //这样就可以当输出用。

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;  //最低速率2MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOC,GPIO_Pin_13);    //默认输出

}


void init_usart3(u32 baudRate){
	USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStruct;
	/* Enable GPIO clock */
  RCC_APB2PeriphClockCmd(
  RCC_APB2Periph_GPIOB| RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); 
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  USART_InitStructure.USART_BaudRate = baudRate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
 /* Configure USARTz */
  USART_Init(USART3, &USART_InitStructure);	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);	
	USART_Cmd(USART3, ENABLE);

	//设置中断优先级
	NVIC_InitStruct.NVIC_IRQChannel=USART3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
	NVIC_Init(&NVIC_InitStruct);	
	
	RS485RWConvInit();
}
void usart3_send_byte(u8 byte){
	while(USART_GetFlagStatus(USART3,USART_FLAG_TC )==RESET);	
	USART_SendData(USART3,byte);
	while(USART_GetFlagStatus(USART3,USART_FLAG_TC )==RESET);	
}
void usart3_send_bytes(u8 *bytes,int len){
	int i;
	for(i=0;i<len;i++){
		usart3_send_byte(bytes[i]);
	}
}
void usart3_send_string(char *string){
	while(*string){
		usart3_send_byte(*string++);
	}
}

void USART3_IRQHandler(void){
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET){
			uint8_t data = USART_ReceiveData(USART3);
			#if MD_USD_SALVE
				MDSSerialRecvByte_1(data);
			#else 
				#if MDM_USD_USART3
					MDMSerialRecvByte(data);
				#endif
			#endif
    }
}

