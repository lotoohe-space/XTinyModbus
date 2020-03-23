#include "sys.h" 
#include "lcd.h"
#include "Text.h"	
#include "string.h"												    
#include "usart.h"		

 

//////////////////////////////////////////////////////////////////////////////////	 
 
//汉字显示 驱动代码	    

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	
/******************************************************************************/								  


/**************************************************************************************/

//函数： void Copy_Mem (unsigned char *P1, const unsigned char *P2,unsigned int Count)

//函数功能：内部存储-拷贝函数

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com
/**************************************************************************************/

void Copy_Mem (unsigned char *P1, const unsigned char *P2,unsigned int Count)
{
   
   unsigned int i ;
   for(i=0;i<Count;i++)*P1++=*P2++;

  
}


/**************************************************************************************/

//函数：void Copy_HZK16(u8 *Dot, u8 num, u8 Q, u8 W)

//函数功能：根据区位码和字号，获取对应的点阵

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com
/**************************************************************************************/

void Copy_HZK16(u8 *Dot, u8 num, u8 Q, u8 W)
{
	 unsigned short k;
	
	 for (k=0;k<hz16_num;k++) //限制在字库里查找对应字符的次数
			{
			  if ((hz16[k].Index[0]==Q)&&(hz16[k].Index[1]== W))
			  {
					Copy_Mem(Dot,&hz16[k].Msk[0],num);
				}
			}
}

/**************************************************************************************/

//函数：void Copy_HZK16(u8 *Dot, u8 num, u8 Q, u8 W)

//函数功能：根据区位码和字号，获取对应的点阵

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com
/**************************************************************************************/

void Copy_HZK24(u8 *Dot, u8 num, u8 Q, u8 W)
{
	 unsigned short k;
	
	 for (k=0;k<hz24_num;k++) //限制在字库里查找对应字符的次数
			{
			  if ((hz24[k].Index[0]==Q)&&(hz24[k].Index[1]== W))
			  {
					Copy_Mem(Dot,&hz24[k].Msk[0],num);
				}
			}
}

/**************************************************************************************/
//函数：void Get_HzMat(u8 *code,u8 *mat,u8 size,u8 Font)
//函数功能：根据区位码和字号，获取对应的点阵
//参数： 
//code 字符指针开始 从字库中查找出字模
//mat  数据存放地址 (size/8+((size%8)?1:0))*(size) bytes大小	
//size:字体点阵数据的个数
//Font:字体类型

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com
/**************************************************************************************/

void Get_HzMat(u8 *code, u8 *mat, u8 size, u8 Font)
{		    
	unsigned char qh,ql;
	unsigned char i;					  
	
		
	qh=*code;
	
	ql=*(++code);
	
	if(qh<0x81||ql<0x40||ql==0xff||qh==0xff)//非 常用汉字
	{   		    
	    for(i=0;i<size;i++)*mat++=0x00;//填充满格
	    return; //结束访问
	} 
	
	switch(Font)
	{
		case 12:
			//
			break;
		case 16:
			Copy_HZK16(mat,size, qh, ql);
			break;
		case 24:
			Copy_HZK24(mat,size, qh, ql);
			break;
			
	}     												    
}

/**************************************************************************************/
//函数：void Show_Font(u16 x,u16 y,u8 *font,u8 size,u16 color,u8 mode)
//函数功能：显示一个指定大小的汉字
//参数：
//x,y :汉字的坐标
//font:汉字区位码
//size:字体大小
//color,字符颜色

//mode:0,正常显示,1,叠加显示	  

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com
/**************************************************************************************/

void Show_Font(u16 x,u16 y,u8 *font,u8 size,u16 color,u8 mode)
{
	u8 temp,t,t1;
	u16 x0=x;
	u8 dzk[72];
	u8 Num;
	
	Num=(size/8+((size%8)?1:0))*(size);//得到字体一个字符对应点阵集所占的字节数	 
	
	if(size!=12&&size!=16&&size!=24)return;	//不支持的size
	
	Get_HzMat(font,dzk,Num,size);	//得到相应大小的点阵数据 
	
	for(t=0;t<Num;t++)
	{   												   
		
		temp=dzk[t];			//得到点阵数据     
		
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,color);
			
			else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR);
			
			temp<<=1;
			
			x++;
			
			if((x-x0)==size)
			{
				x=x0;
				y++;
//				break;
			}
		}  	 
	}  
}

/**************************************************************************************/
//函数：Show_Str(u16 x,u16 y,u16 width,u16 height,u8*str,u8 size, u16 color, u8 mode)
//函数功能：在指定位置开始显示一个字符串，支持自动换行	
//参数：
//(x,y):起始坐标
//width,height:区域
//str  :字符串
//size :字体大小
//color,字符颜色
//mode:0,非叠加方式;1,叠加方式    	

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com
/**************************************************************************************/
void Show_Str(u16 x,u16 y,u16 width,u16 height, u8*str, u8 size, u16 color, u8 mode)
{					
	u16 x0=x;
	u16 y0=y;							  	  
  u8 bHz=0;     //字符或者中文  
		
  while(*str!=0)//数据未结束
    { 
        if(!bHz)
        {
	        if(*str>0x80)bHz=1;//中文 
					
	        else              //字符
	        {      
            if(x>(x0+width-size/2))//换行
						{				   
							y+=size;
							x=x0;	   
						}	
						
		        if(y>(y0+height-size))break;//越界返回
						
		        if(*str==13)//换行符号
		        {         
		            y+=size;
					      x=x0;
		            str++; 
		        }  
		        else LCD_ShowChar(x,y,*str,size,color,mode);//有效部分写入 
				    str++; 
		        x+=size/2; //字符,为全字的一半 
	        }
        }
				else//中文 
        {     
            bHz=0;//有汉字库 
					
            if(x>(x0+width-size))//换行
						{	    
							y+=size;
							x=x0;		  
						}
						
	        if(y>(y0+height-size))break;//越界返回  
						
	        Show_Font(x,y,str,size,color,mode); //显示这个汉字,空心显示 
						
	        str+=2; 
	        x+=size;//下一个汉字偏移	    
        }						 
    }   
}

//**************************************************************************************/
//函数：Draw_Font16B(u16 x,u16 y, u16 color, u8*str)
//函数功能：在指定位置开始显示一个16x16点阵的--字符串，支持自动换行(至屏幕一行的终点后，自动换到下一个x起点位置)	
//参数：

//(x,y):起始坐标
//color,字符颜色
//str  :字符串


//非叠加方式;非点阵字符的部分，填充背景颜色

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com
//**************************************************************************************/


void Draw_Font16B(u16 x,u16 y, u16 color, u8*str)
{
	u16 width;
	u16 height;
	
	width=lcddev.width-x;
	height=lcddev.height-y;
	
	Show_Str(x,y,width,height, str, 16, color, 0);
	
	
}

//**************************************************************************************/
//函数：void Draw_Font24B(u16 x,u16 y, u16 color, u8*str)
//函数功能：在指定位置开始显示一个24x24点阵的--字符串，支持自动换行(至屏幕一行的终点后，自动换到下一个x起点位置)	
//参数：

//(x,y):起始坐标
//color,字符颜色
//str  :字符串


//非叠加方式;非点阵字符的部分，填充背景颜色

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com
//**************************************************************************************/

void Draw_Font24B(u16 x,u16 y, u16 color, u8*str)
{
	u16 width;
	u16 height;
	
	width=lcddev.width-x;
	height=lcddev.height-y;
	
	Show_Str(x,y,width,height, str, 24, color, 0);
	
	
}










/*******************************************************************************/

//DevEBox  大越创新

//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com
/*******************************************************************************/







		  






