#ifndef _USART3_H__
#define _USART3_H__
#include "sys.h"

#define RS485_RW_CONV PCout(13)

void init_usart3(u32 baudRate);
void usart3_send_byte(u8 byte);
void usart3_send_bytes(u8 *bytes,int len);
void usart3_send_string(char *string);
#endif
