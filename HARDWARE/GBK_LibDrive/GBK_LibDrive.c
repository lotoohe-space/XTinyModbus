 

#include "GBK_LibDrive.h"	
#include "stm32f10x.h"	



//////////////////////////////////////////////////////////////////////////////////	



//汉字字库--驱动代码	   

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

//********************************************************************************

////////////////////////////////////////////////////////////////////////////////// 	 


void GBK_Lib_Init(void)
{
	 	
	 #if    Used_FontIO
	  
	
	  GPIO_InitTypeDef  GPIO_InitStructure;	
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOF, ENABLE);//使能GPIOB,F时钟

		//PB1--SCLK 初始化设置
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;            //PB1设置为推挽输出
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	  GPIO_Init(GPIOB, &GPIO_InitStructure);               //初始化
	
    //PB2--MISO 初始化设置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;            //PB2 设置为上拉输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		   //上拉输入
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);               //初始化

		
		//PF11--CS 初始化设置  //PF9--MOSI 初始化设置
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_11;     //PF11设置推挽输出
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		        //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		         //IO口速度为50MHz
	  GPIO_Init(GPIOF, &GPIO_InitStructure);                      //初始化	
				
		FCLK_SET;//空闲状态为高电平
		
		#endif
		
		GBK_ReadID();	//读取加密关键字节. --使用字库前，必须先读取一次字库关键字符串，
                  //确认字库已经解锁，否则会出现缺失字符的现象
}


//////////////////////////////////////////////////////////////////////////////////	

//延时函数--驱动代码	   

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

//********************************************************************************

////////////////////////////////////////////////////////////////////////////////// 	

void GBK_delay(unsigned char Time)
{
     unsigned char m,n;
	
     for(n=0;n<Time;n++)
	   {
      for(m=0;m<100;m++);
		 }
}

//////////////////////////////////////////////////////////////////////////////////	

//IO接口读写数据--驱动代码	   

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

//********************************************************************************

////////////////////////////////////////////////////////////////////////////////// 


#if    Used_FontIO

unsigned char Font_SPI_WR(unsigned char byte)
{
	unsigned char bit_ctr;
	
	for(bit_ctr=0;bit_ctr<8;bit_ctr++)  // 输出8位
	{
		
		
		if((byte&0x80)==0x80)FDI_SET; 			// MSB TO MOSI
			else FDI_CLR; 

		FCLK_CLR;
		
		byte=(byte<<1);					// shift next bit to MSB
				
		byte|=FDO_IN;	        		// capture current MISO bit
		
		FCLK_SET;
		
	}
	
	FCLK_SET;//空闲状态为高电平
	
	return byte;
	
}

#endif




//********************************************************************************
//函数：void FontLib_Read(u8* pBuffer,u32 ReadAddr,u16 Num_Read) 
//读取SPI FLASH  
//在指定地址开始读取指定长度的数据
//pBuffer:数据存储区
//ReadAddr:开始读取的地址(24bit)
//Num_Read:要读取的字节数(最大65535)

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	
//********************************************************************************

void FontLib_Read(u8* pBuffer,u32 ReadAddr,u16 Num_Read)   
{ 
 	  u16 i;  
	
	  Font_CS_EN;                           //使能器件  
	
    Font_SPI_WR(FontRead_CMD);            //发送读取命令   
    Font_SPI_WR((u8)((ReadAddr)>>16));    //发送24bit地址    
    Font_SPI_WR((u8)((ReadAddr)>>8));   
    Font_SPI_WR((u8)ReadAddr);   
    for(i=0;i<Num_Read;i++)
	  { 
        pBuffer[i]=Font_SPI_WR(0xFF);   //循环读数  
    }
		
 	  Font_CS_SN;  		                      //失能器件  
		
}


//********************************************************************************
//函数：void GBK_ReadID(void)

//函数功能：读取字库信息，解锁字库可用（必须读）

//读字库型号ID--使用字库前，必须先读取一次字库，
//确认字库已经解锁，否则会出现缺失字符的现象

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	
//********************************************************************************
void GBK_ReadID(void)
{
	u8 Temp[24];
	u8 i;
	
	LCD_Clear(WHITE);//清除屏幕
	
	for(i=0;i<24;i++)Temp[i]=0;//初始化缓冲区

  FontLib_Read(Temp,0,17);  
  DrawFont_GBK16B(16,16,RED,Temp);
	
	for(i=0;i<24;i++)Temp[i]=0;//初始化缓冲区
  FontLib_Read(Temp,20,8);   	
	DrawFont_GBK16B(16,32,RED,Temp);
	
	for(i=0;i<24;i++)Temp[i]=0;//初始化缓冲区
  FontLib_Read(Temp,30,8);   	
	DrawFont_GBK16B(16,48,RED,Temp);

  delay_ms(200);	//延时显示	--显示部分可以不显示，读取是必须的
	
	
} 

//////////////////////////   获取点阵ASCII点阵字符 //////////////////////////////////////////////////////
//函数：void GBK_GetASC_Point(uint8_t *code, uint8_t *Pdot, uint8_t Font, uint16_t Num)
//*code：字符指针开始-字符串的开始地址,GBK码-从字库中查找出字模
//*Pdot：数据存放地址 (size/8+((size%8)?1:0))*(size) bytes大小	
// Font: 字体型号
// Num : 字体大小

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	
//********************************************************************************

////////////////////////////////////////////////////////////////////////////////// 	 

void GBK_GetASC_Point(uint8_t *code, uint8_t *Pdot, uint8_t Font, uint16_t Num)
{		    
	 uint8_t QW;    //ASC码序号
	 uint32_t  ADDRESS;
	
	 QW=*code;//区位码--获取序号
	
	 //csize=(size/8+((size%8)?1:0))*(size/2);//得到字体一个字符对应点阵集所占的字节数	 
	
	switch(Font)
	{

		case 12:
			ADDRESS=((unsigned long)QW*Num)+ASCII6x12_ST;	  //得到字库中的字节偏移量  	
			break;
		case 16:
			ADDRESS=((unsigned long)QW*Num)+ASCII8x16_ST;	  //得到字库中的字节偏移量  	
			break;
		case 24:
			ADDRESS=((unsigned long)QW*Num)+ASCII12x24_ST;	//得到字库中的字节偏移量  
			break;
		case 32:
			ADDRESS=((unsigned long)QW*Num)+ASCII16x32_ST;	//得到字库中的字节偏移量  
			break;
		case 48:
			ADDRESS=((unsigned long)QW*Num)+ASCII24x48_ST;	//得到字库中的字节偏移量  
			break;
		case 64:
			ADDRESS=((unsigned long)QW*Num)+ASCII32x64_ST;	//得到字库中的字节偏移量  
			break;
		
    default: return;
		
			
	} 

	FontLib_Read(Pdot,ADDRESS,Num);//读取点阵字符
	
}  


//////////////////////////   在指定位置显示ASCII码 字符--点阵字符 //////////////////////////////////////////////////////
//函数：void GBK_ShowASCII(uint16_t x, uint16_t y, uint8_t *N_Word, uint8_t size, uint16_t D_Color, uint16_t B_Color, uint8_t mode)
//显示一个指定大小的ASC字符

//x,y    : ASCII的坐标--16位
//N_Word : ASCII--序号--8位
//size   : 字体大小-字号-8位
//D_Color：点阵字颜色--16位
//B_Color：背景颜色--16位
//mode:  0,正常显示,1,叠加显示	--8位

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	
//********************************************************************************

////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 	

void GBK_ShowASCII(uint16_t x, uint16_t y, uint8_t *N_Word, uint8_t size, uint16_t D_Color, uint16_t B_Color, uint8_t mode)
{
	
	uint16_t csize; //得到字体一个字符对应点阵集所占的字节数	 
	
	uint8_t temp;
	
	uint16_t t,t1;
	
	uint16_t y0=y;
	
	uint8_t dzk[256];  
	
	csize=(size/8+((size%8)?1:0))*(size/2);		//ASCII字符大小占半个汉字的宽度，得到字体一个字符对应点阵集所占的字节数		 
	
	if(size!=12&&size!=16&&size!=24&&size!=32&&size!=48&&size!=64)return;	//不支持的size
	
	GBK_GetASC_Point(N_Word,dzk,size,csize);	              //得到相应大小的点阵数据 
	
	for(t=0;t<csize;t++)
	{
		
		temp=dzk[t];			//得到点阵数据   
		
		for(t1=0;t1<8;t1++)//纵向输出点阵显示到屏幕
		{
			if(temp&0x80) LCD_Fast_DrawPoint(x,y,D_Color);   //显示字符点阵
			
			else if(mode==0)LCD_Fast_DrawPoint(x,y,B_Color); //显示背景颜色
			
			temp<<=1;
			
			y++;
			
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
	}  
}



 
//////////////////////////   获取点阵汉字GBK字库--点阵字符 //////////////////////////////////////////////////////
//函数：void GBK_Lib_GetHz(uint8_t *code, uint8_t *Pdot, uint8_t Font, uint16_t Num)
//*code：字符指针开始-字符串的开始地址,GBK码-从字库中查找出字模
//*Pdot：数据存放地址 (size/8+((size%8)?1:0))*(size) bytes大小	
// Font: 字体型号
// Num : 点阵字符数量

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	
//********************************************************************************

////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 	


void GBK_Lib_GetHz(uint8_t *code, uint8_t *Pdot, uint8_t Font, uint16_t Num)
{		    
	 
	 uint8_t   qh,ql;
	 uint16_t   i;	
	 uint32_t  foffset;
	 uint32_t  ADDRESS;
	
	 //csize=(size/8+((size%8)?1:0))*(size);//得到字体一个字符对应点阵集所占的字节数	 
	
	 qh=*code;       //获得字符的区位码
	 ql=*(++code);   //获得字符的区位码
	
	 if(qh<0x81||ql<0x40||ql==0xff||qh==0xff)//非常用汉字
		{   		    
				for(i=0;i<Num;i++)*Pdot++=0x00;   //填充满格--0不显示
				return;                            //结束访问
		} 

		
		if(ql<0x7f)ql-=0x40;  //注意!
		  else ql-=0x41;

		  qh-=0x81;

	
	foffset=((unsigned long)190*qh+ql)*Num;	//得到字库中的字节偏移量  	
	
	switch(Font)
	{
    case 12:
			ADDRESS=foffset+GBK12x12_ST;     //汉字  16x16 点阵的起始位置
			break;
		case 16:
			ADDRESS=foffset+GBK16x16_ST;     //汉字  16x16 点阵的起始位置
			break;
		case 24:
			ADDRESS=foffset+GBK24x24_ST;     //汉字  24x24 点阵的起始位置
			break;
		case 32:
			ADDRESS=foffset+GBK32x32_ST;     //汉字  32x32 点阵的起始位置
			break;
		 default: return;
			
	} 

	FontLib_Read(Pdot,ADDRESS,Num);//读取点阵字符
	
} 


//////////////////////////   在指定位置显示汉字字符--点阵字符 //////////////////////////////////////////////////////
//函数：void GBK_Show_Font(uint16_t x, uint16_t y, uint8_t *font, uint8_t size, uint16_t D_Color, uint16_t B_Color, uint8_t mode)

//显示一个指定大小的汉字
//x,y    : ASCII的坐标     --16位
//N_Word : ASCII--序号     --8位
//size   : 字体大小-字号   --8位
//D_Color：点阵字颜色      --16位
//B_Color：背景颜色        --16位
//mode:  0,正常显示,1,叠加显示	--8位
	
//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	
//********************************************************************************

////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 	

void GBK_Show_Font(uint16_t x, uint16_t y, uint8_t *font, uint8_t size, uint16_t D_Color, uint16_t B_Color, uint8_t mode)
{
	
	uint16_t  csize;                                    //得到字体一个字符对应点阵集所占的字节数	 
	uint8_t   temp,t,t1;
	uint16_t  y0=y;
	uint8_t   dzk[128];  
	
	csize=(size/8+((size%8)?1:0))*(size);              //得到字体一个字符对应点阵集所占的字节数	 
	
	if(size!=12&&size!=16&&size!=24&&size!=32)return;	 //不支持的size
	
	GBK_Lib_GetHz(font,dzk,size,csize);	                   //得到相应大小的点阵数据 
	
	for(t=0;t<csize;t++)
	 {   												   
		  temp=dzk[t];			                             //得到点阵数据   
		
			for(t1=0;t1<8;t1++)
			{
				if(temp&0x80)LCD_Fast_DrawPoint(x,y,D_Color);//显示字符点阵
				
				else if(mode==0)LCD_Fast_DrawPoint(x,y,B_Color); //显示背景颜色
				
				temp<<=1;
				
				y++;
				
				if((y-y0)==size)
				{
					y=y0;
					x++;
					break;
				}
			}  	 
	}  
}




/////////////////////////////////////在指定位置显示字符串///////////////////////////////////////////////////////////////////////////// 
//函数：void GBK_Show_Str(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *str, uint8_t size, uint16_t D_Color, uint16_t B_Color, uint8_t mode)
//在指定位置开始显示一个字符串	    
//支持自动换行
//(x,y):起始坐标
//width,height:可以显示字符的区域
//str  :字符串
//size :字体大小
//D_Color：点阵字颜色      --16位
//B_Color：背景颜色        --16位
//mode:0,非叠加方式;1,叠加方式 

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	
//********************************************************************************

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GBK_Show_Str(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *str, uint8_t size, uint16_t D_Color, uint16_t B_Color, uint8_t mode)
{					
	uint16_t x0=x;
	uint16_t y0=y;							  	  
  uint8_t bHz=0;     //字符或者中文  	    			

	
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
		        else GBK_ShowASCII(x, y, str, size, D_Color, B_Color, mode);  //LCD_ShowChar(x,y,*str,size,mode);//有效部分写入 
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
	        
					if(y>(y0+height-size))break;  //越界返回  	
					
	        GBK_Show_Font(x,y,str,size, D_Color, B_Color, mode); //显示这个汉字,空心显示 
					
	        str+=2; 
	        x+=size;//下一个汉字偏移	    
        }						 
    }   
}  

/////////////////////////////////////在指定宽度的中间显示字符串///////////////////////////////////////////////////////////////////////////// 

//函数：void GBK_Show_StrMid(uint16_t x,uint16_t y, uint8_t size, uint8_t len, uint16_t D_Color, uint16_t B_Color,uint8_t*str)

//在指定宽度的中间显示字符串
//如果字符长度超过了len,则用GBK_Show__Str显示
//len:指定要显示的宽度			  

//D_Color：点阵字颜色      --16位
//B_Color：背景颜色        --16位

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	
//********************************************************************************

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GBK_Show_StrMid(uint16_t x,uint16_t y, uint8_t size, uint8_t len, uint16_t D_Color, uint16_t B_Color,uint8_t*str)
{
	uint16_t strlenth=0;
		
  strlenth=strlen((const char*)str);
	strlenth*=size/2;
	if(strlenth>len)GBK_Show_Str(x,y,lcddev.width,lcddev.height,str,size,D_Color,B_Color,1);
	else
	{
		strlenth=(len-strlenth)/2;
	  GBK_Show_Str(strlenth+x,y,lcddev.width,lcddev.height,str,size,D_Color,B_Color,1);
	}
}   


//**************************************************************************************/
//函数：void DrawFont_GBK12B(u16 x,u16 y, u16 color, u8*str)
//函数功能：在指定位置开始显示一个12x12点阵的--字符串，支持自动换行(至屏幕一行的终点后，自动换到下一个x起点位置)	
//参数：

//(x,y):起始坐标
//color,字符颜色
//str  :字符串


//非叠加方式;非点阵字符的部分，填充背景颜色

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com
//**************************************************************************************/


void DrawFont_GBK12B(u16 x,u16 y, u16 color, u8*str)
{
	u16 width;
	u16 height;
	
	width=lcddev.width-x;
	height=lcddev.height-y;
	
	GBK_Show_Str(x,y,width,height, str, 12, color,BACK_COLOR, 0);
	
	
}


//**************************************************************************************/
//函数：void DrawFont_GBK16B(u16 x,u16 y, u16 color, u8*str)
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


void DrawFont_GBK16B(u16 x,u16 y, u16 color, u8*str)
{
	u16 width;
	u16 height;
	
	width=lcddev.width-x;
	height=lcddev.height-y;
	
	GBK_Show_Str(x,y,width,height, str, 16, color,BACK_COLOR, 0);
	
	
}

//**************************************************************************************/
//函数：void DrawFont_GBK24B(u16 x,u16 y, u16 color, u8*str)
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

void DrawFont_GBK24B(u16 x,u16 y, u16 color, u8*str)
{
	u16 width;
	u16 height;
	
	width=lcddev.width-x;
	height=lcddev.height-y;
	
	GBK_Show_Str(x,y,width,height, str, 24, color,BACK_COLOR, 0);
	
	
}

//**************************************************************************************/
//函数：void DrawFont_GBK32B(u16 x,u16 y, u16 color, u8*str)
//函数功能：在指定位置开始显示一个32x32点阵的--字符串，支持自动换行(至屏幕一行的终点后，自动换到下一个x起点位置)	
//参数：

//(x,y):起始坐标
//color,字符颜色
//str  :字符串


//非叠加方式;非点阵字符的部分，填充背景颜色

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com
//**************************************************************************************/

void DrawFont_GBK32B(u16 x,u16 y, u16 color, u8*str)
{
	u16 width;
	u16 height;
	
	width=lcddev.width-x;
	height=lcddev.height-y;
	
	GBK_Show_Str(x,y,width,height, str, 32, color,BACK_COLOR, 0);
	
	
}

//**************************************************************************************/
//函数：void DrawFontASC_GBK48B(u16 x,u16 y, u16 color, u8*str)
//函数功能：在指定位置开始显示一个24x48点阵的--ACSII码 （本字库只支持ASCII码），支持自动换行(至屏幕一行的终点后，自动换到下一个x起点位置)	
//参数：

//(x,y):起始坐标
//color,字符颜色
//str  :字符串


//非叠加方式;非点阵字符的部分，填充背景颜色

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com
//**************************************************************************************/

void DrawFontASC_GBK48B(u16 x,u16 y, u16 color, u8*str)
{
	u16 width;
	u16 height;
	
	width=lcddev.width-x;
	height=lcddev.height-y;
	
	GBK_Show_Str(x,y,width,height, str, 48, color,BACK_COLOR, 0);
	
	
}


//**************************************************************************************/
//函数：void DrawFontASC_GBK64B(u16 x,u16 y, u16 color, u8*str)
//函数功能：在指定位置开始显示一个32x64点阵的--ACSII码 （本字库只支持ASCII码），支持自动换行(至屏幕一行的终点后，自动换到下一个x起点位置)	
//参数：

//(x,y):起始坐标
//color,字符颜色
//str  :字符串


//非叠加方式;非点阵字符的部分，填充背景颜色

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com
//**************************************************************************************/

void DrawFontASC_GBK64B(u16 x,u16 y, u16 color, u8*str)
{
	u16 width;
	u16 height;
	
	width=lcddev.width-x;
	height=lcddev.height-y;
	
	GBK_Show_Str(x,y,width,height, str, 64, color,BACK_COLOR, 0);
	
	
}




//****************************************************************************************/
//函数：void GBK_LibFont_Test(void)
//功能描述: GBK字体字形显示测试


//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com
/****************************************************************************************/

void GBK_LibFont_Test(void)
{
	
//	LCD_Clear(WHITE);
//	
//	DrawFont_GBK16B(24,16,BLUE,"2: GBK字库--显示测试");
//	delay_ms(1000);
	
	LCD_Clear(WHITE);
	
	GBK_Show_Str(16,0,240,32,"十种字库测试",32,BLUE,WHITE,0);	
	GBK_Show_Str(16,40,240,16,"设计：大越电子",16,BLUE,WHITE,0);	
	GBK_Show_Str(16,56,240,16,"From: mcudev.taobao.com",16,BLUE,WHITE,0);
	
	DrawFont_GBK12B(16,72,BLUE,"Date: 2019/05/08");

	GBK_Show_StrMid(16,96,12,200,BLUE,WHITE,"居中显示函数: 2019/05/08");//居中显示	
								
	GBK_Show_Str(16,120,240,12,"汉字12x12-GBK字库：",12,BLUE,WHITE,0);	//显示内码低字节
				
				
	GBK_Show_Str(16,136,240,16,"汉字16x16字库：",16,BLUE,WHITE,0);	//显示内码低字节
				

	GBK_Show_Str(16,156,240,24,"24x24字库：",24,BLUE,WHITE,0);	//显示内码低字节
				
				
	DrawFont_GBK32B(16,184,BLUE,"32x32字库：");	//显示内码低字节
						 	
	//GBK_Show_Str(16,220,240,48,"48",48,D_Color,B_Color,0);
  DrawFontASC_GBK48B(16,220,BLUE,"48");

  //GBK_Show_Str(120,220,240,64,"64",64,D_Color,B_Color,0);	
	DrawFontASC_GBK64B(120,220,BLUE,"64");
	
	GBK_Show_Str(8,280,240,16,"触摸屏已经工作，可以画画",16,RED,WHITE,0);	//字库方式显示
	GBK_Show_Str(8,300,240,16,"触摸屏重新校准，请按KEY0按键",16,RED,WHITE,0);	//字库方式显示
	delay_ms(100);	
}







































//****************************************************************************************/
//函数：void GBK_LibFont_Test(void)
//功能描述: GBK字体字形显示测试


//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com
/****************************************************************************************/





		  






