#include "sys.h"
#include "usart.h"	 
#include "Sys_Config.h"
#if MD_USD_SALVE
#include "MDS_RTU_Serial.h"
#else
#include "MDM_RTU_Serial.h"
#endif
////////////////////////////////////////////////////////////////////////////////// 	 

#if SYSTEM_SUPPORT_OS
#include "includes.h"		
#endif

//////////////////////////////////////////////////////////////////
#if 1
#pragma import(__use_no_semihosting)             
              
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       

void _sys_exit(int x) 
{ 
	x = x; 
} 

int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*使用microLib的方法*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
void uart_send_bytes(u8* bytes,u16 len){
	u16 i=0;
	for(i=0;i<len;i++){
			while((USART1->SR&0X40)==0);
    USART1->DR = (u8) (bytes[i]);  
	}
}
u8 *sendBytes=NULL;
u16 sendLen=0;
u16 sendI=0;
u8 sendFlag=0;
void uart_send_bytes_by_isr(u8* bytes,u16 len){
	sendBytes=bytes;
	sendLen=len;
	sendI=0;
	sendFlag=0;
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	while(sendFlag==0);
}

  
void uart_init(u32 bound){

  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);	//使能USART1，GPIOA时钟

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);


  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);	
  


	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	

  USART_Init(USART1, &USART_InitStructure); 
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  USART_Cmd(USART1, ENABLE);                   

}

void USART1_IRQHandler(void)                
	{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  
	{
		Res =USART_ReceiveData(USART1);	
		#if MD_USD_SALVE
		MDSSerialRecvByte(Res);
		#else
		MDMSerialRecvByte(Res);
		#endif

	} 

  if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET){  
    USART_SendData(USART1, sendBytes[sendI++]);
    if(sendI == sendLen)
    {   
			sendFlag=1;
      USART_ITConfig(USART1, USART_IT_TXE, DISABLE); 
    }   
  }   
#if SYSTEM_SUPPORT_OS 
	OSIntExit();  											 
#endif
} 


