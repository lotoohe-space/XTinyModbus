#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h"    
#define RS485_RW_CONV1 PAout(12)
void RS485RWConvInit1(void);
void uart_init(u32 bound);
void uart_send_bytes(u8* bytes,u16 len);
void uart_send_bytes_by_isr(u8* bytes,u16 len);
#endif


