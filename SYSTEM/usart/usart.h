#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h"    

void uart_init(u32 bound);
void uart_send_bytes(u8* bytes,u16 len);
void uart_send_bytes_by_isr(u8* bytes,u16 len);
#endif


