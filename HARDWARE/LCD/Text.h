#ifndef __Text_H__
#define __Text_H__	 

#include "sys.h"

#include "font.h"

//////////////////////////////////////////////////////////////////////////////////	 
 
//汉字显示 驱动代码	

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	
/******************************************************************************/		 
 					     
void Get_HzMat(u8 *code,u8 *mat,u8 size,u8 Font);			//得到汉字的点阵码

void Show_Font(u16 x,u16 y,u8 *font,u8 size,u16 color,u8 mode);					//在指定位置显示一个汉字

void Show_Str(u16 x,u16 y,u16 width,u16 height,u8*str,u8 size, u16 color, u8 mode);	//在指定位置显示一个字符串 

void Draw_Font16B(u16 x,u16 y, u16 color, u8*str);//在指定位置--显示 16x16 大小的点阵字符串

void Draw_Font24B(u16 x,u16 y, u16 color, u8*str);//在指定位置--显示 24x24 大小的点阵字符串


#endif
