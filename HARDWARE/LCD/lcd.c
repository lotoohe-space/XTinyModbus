#include "lcd.h"
#include "stdlib.h"
#include "font.h" 
#include "usart.h"	 
#include "delay.h"	

#include "Text.h"
#include "Picture.h"
#include "Dis_Picture.h" 

//////////////////////////////////////////////////////////////////////////////////	 

/******************************************************************/

//STM32F103ZE核心板
//2.4寸/2.8寸/3.2寸  TFT液晶驱动	  
//支持驱动IC型号包括:ILI9341等	

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/******************************************************************/


//////////////////////////////////////////////////////////////////////////////////	 

//LCD的画笔颜色和背景色	   
u16 POINT_COLOR=0x0000;	//画笔颜色
u16 BACK_COLOR=0xFFFF;  //背景色 
 



//管理LCD重要参数
//默认为竖屏
_lcd_dev lcddev;



/******************************************************************/

//写寄存器函数
//regval:寄存器值

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/******************************************************************/
	 

void LCD_WR_REG(u16 regval)
{   
	LCD->LCD_REG=regval;//写入要写的寄存器序号	 
}


/******************************************************************/


//写LCD数据
//data:要写入的值

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/******************************************************************/

void LCD_WR_DATA(u16 data)
{	 
	LCD->LCD_RAM=data;		 
}


/******************************************************************/

//读LCD数据
//返回值:读到的值

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/******************************************************************/


u16 LCD_RD_DATA(void)
{
	vu16 ram;			//防止被优化
	ram=LCD->LCD_RAM;	
	return ram;	 
}			


/******************************************************************/

//写寄存器
//LCD_Reg:寄存器地址
//LCD_RegValue:要写入的数据

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/******************************************************************/


void LCD_WriteReg(u16 LCD_Reg,u16 LCD_RegValue)
{	
	LCD->LCD_REG = LCD_Reg;		//写入要写的寄存器序号	 
	LCD->LCD_RAM = LCD_RegValue;//写入数据	    		 
}	  


/******************************************************************/

//读寄存器
//LCD_Reg:寄存器地址
//返回值:读到的数据

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/******************************************************************/


u16 LCD_ReadReg(u16 LCD_Reg)
{										   
	LCD_WR_REG(LCD_Reg);		//写入要读的寄存器序号
	delay_us(5);		  
	return LCD_RD_DATA();		//返回读到的值
}  



/******************************************************************/

//开始写GRAM

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/******************************************************************/



void LCD_WriteRAM_Prepare(void)
{
 	LCD->LCD_REG=lcddev.wramcmd;	  
}	 



/******************************************************************/

//LCD写GRAM
//RGB_Code:颜色值

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/******************************************************************/


void LCD_WriteRAM(u16 RGB_Code)
{							    
	LCD->LCD_RAM = RGB_Code;//写十六位GRAM
}



/******************************************************************/

//从ILI93xx读出的数据为GBR格式，而我们写入的时候为RGB格式。
//通过该函数转换
//c:GBR格式的颜色值
//返回值：RGB格式的颜色值

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/******************************************************************/


u16 LCD_BGR2RGB(u16 c)
{
	u16  r,g,b,rgb;   
	b=(c>>0)&0x1f;
	g=(c>>5)&0x3f;
	r=(c>>11)&0x1f;	 
	rgb=(b<<11)+(g<<5)+(r<<0);		 
	return(rgb);
} 



/******************************************************************/

//当mdk -O1时间优化时需要设置
//延时i

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/******************************************************************/



void opt_delay(u8 i)
{
	while(i--);
}



/******************************************************************/

//读取个某点的颜色值	 
//x,y:坐标
//返回值:此点的颜色

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/******************************************************************/


u16 LCD_ReadPoint(u16 x,u16 y)
{
 	u16 r=0,g=0,b=0;
	
	if(x>=lcddev.width||y>=lcddev.height)return 0;	//超过了范围,直接返回		 
  
	LCD_SetCursor(x,y);	 
	
	if(lcddev.id==0X9341)LCD_WR_REG(0X2E);//发送读GRAM指令
	  
 	r=LCD_RD_DATA();								//dummy Read	   

	opt_delay(2);	  
 	r=LCD_RD_DATA();  		  						//实际坐标颜色

 	if(lcddev.id==0X9341)		//9341要分2次读出
 	{
		opt_delay(2);	  
		b=LCD_RD_DATA(); 
		g=r&0XFF;		//对于9341,第一次读取的是RG的值,R在前,G在后,各占8位
		g<<=8;
	} 
	
  if(lcddev.id==0X9341)return (((r>>11)<<11)|((g>>10)<<5)|(b>>11));//ILI9341需要公式转换一下
	else return LCD_BGR2RGB(r);						//其他IC
}			



/******************************************************************/

//LCD开启显示

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/******************************************************************/



void LCD_DisplayOn(void)
{					   
	if(lcddev.id==0X9341)LCD_WR_REG(0X29);	//开启显示
	
}	 

/******************************************************************/

//LCD关闭显示

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/******************************************************************/


void LCD_DisplayOff(void)
{	   
	if(lcddev.id==0X9341)LCD_WR_REG(0X28);	//关闭显示
	
}  

/******************************************************************/

//设置光标位置
//Xpos:横坐标
//Ypos:纵坐标

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/******************************************************************/


void LCD_SetCursor(u16 Xpos, u16 Ypos)
{	 
 	if(lcddev.id==0X9341)
	{		    
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(Xpos>>8);
		LCD_WR_DATA(Xpos&0XFF); 			 
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(Ypos>>8);
		LCD_WR_DATA(Ypos&0XFF); 		
	}
 
} 


/******************************************************************/

//设置LCD的自动扫描方向
//所以,一般设置为L2R_U2D即可,如果设置为其他扫描方式,可能导致显示不正常.
//dir:0~7,代表8个方向(具体定义见lcd.h)

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/******************************************************************/


	   
void LCD_Scan_Dir(u8 dir)
{
	u16 regval=0;
	
//	u16 temp; 
	
	if(lcddev.id==0x9341)
	{
		switch(dir)
		{
			case L2R_U2D://从左到右,从上到下
				regval|=(0<<7)|(0<<6)|(0<<5); 
				break;
			case L2R_D2U://从左到右,从下到上
				regval|=(1<<7)|(0<<6)|(0<<5); 
				break;
			case R2L_U2D://从右到左,从上到下
				regval|=(0<<7)|(1<<6)|(0<<5); 
				break;
			case R2L_D2U://从右到左,从下到上
				regval|=(1<<7)|(1<<6)|(0<<5); 
				break;	 
			case U2D_L2R://从上到下,从左到右
				regval|=(0<<7)|(0<<6)|(1<<5); 
				break;
			case U2D_R2L://从上到下,从右到左
				regval|=(0<<7)|(1<<6)|(1<<5); 
				break;
			case D2U_L2R://从下到上,从左到右
				regval|=(1<<7)|(0<<6)|(1<<5); 
				break;
			case D2U_R2L://从下到上,从右到左
				regval|=(1<<7)|(1<<6)|(1<<5); 
				break;	 
		}
		
		  LCD_WriteReg(0x36,regval|0x08);//改变扫描方向命令  ---此处需要查看数据手册，确定RGB颜色交换位的配置

	//以下设置，为窗口参数设置，设置了全屏的显示范围			
		
			LCD_WR_REG(lcddev.setxcmd); 
			LCD_WR_DATA(0);
		  LCD_WR_DATA(0);
			LCD_WR_DATA((lcddev.width-1)>>8);
		  LCD_WR_DATA((lcddev.width-1)&0XFF);
		
			LCD_WR_REG(lcddev.setycmd); 
			LCD_WR_DATA(0);
		  LCD_WR_DATA(0);
			LCD_WR_DATA((lcddev.height-1)>>8);
		  LCD_WR_DATA((lcddev.height-1)&0XFF);  
		
  	}

}

/******************************************************************/

//画点
//x,y:坐标
//POINT_COLOR:此点的颜色

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/******************************************************************/


void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetCursor(x,y);		//设置光标位置 
	LCD_WriteRAM_Prepare();	//开始写入GRAM
	LCD->LCD_RAM=POINT_COLOR; 
}


/******************************************************************/

//快速画点
//x,y:坐标
//color:颜色

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/******************************************************************/


void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color)
{	   
	if(lcddev.id==0X9341)
	{
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(x>>8);
		LCD_WR_DATA(x&0XFF);  			 
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(y>>8);
		LCD_WR_DATA(y&0XFF); 	
		
		
		LCD->LCD_REG=lcddev.wramcmd; 
		LCD->LCD_RAM=color; 
		
	}
			 

}	


/******************************************************************/

//设置LCD显示方向
//输入参数：

//设置LCD显示方向

////dir:   0,竖屏  正
//         1,竖屏  反
//         2,横屏  左
//         3,横屏  右

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/******************************************************************/


void LCD_Display_Dir(u8 dir)
{
	u8 SCAN_DIR;
		
	if(dir==0)			     //竖屏  正
	{
		lcddev.dir=0;	     //竖屏
		lcddev.width=240;
		lcddev.height=320;
    
		lcddev.wramcmd=0X2C;
	 	lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;

    SCAN_DIR=L2R_U2D; //选择扫描方向		

	}
	
else if (dir==1)			 //横屏
	{	  				
		lcddev.dir=0;	     //竖屏
		lcddev.width=240;
		lcddev.height=320;
    
		lcddev.wramcmd=0X2C;
	 	lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;
		
    SCAN_DIR=R2L_D2U; //选择扫描方向		
	} 	
	
	
	else if (dir==2)			//横屏
	{	  				
		lcddev.dir=1;	     //横屏
		lcddev.width=320;
		lcddev.height=240;

		lcddev.wramcmd=0X2C;
	 	lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;  
		
    SCAN_DIR=U2D_R2L; //选择扫描方向		
		
	} 
 else if (dir==3)				  //横屏
	{	  				
		lcddev.dir=1;	        //横屏
		lcddev.width=320;
		lcddev.height=240;

		lcddev.wramcmd=0X2C;
	 	lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B; 
    
    SCAN_DIR=D2U_L2R; //选择扫描方向				

	} 	
 else //设置默认为竖屏--正
 {
	  lcddev.dir=0;	     //竖屏
		lcddev.width=240;
		lcddev.height=320;
    
		lcddev.wramcmd=0X2C;
	 	lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;

    SCAN_DIR=L2R_U2D; //选择扫描方向		
	 
 }	 


 /////设置屏幕显示--扫描方向
	
	   LCD_Scan_Dir(SCAN_DIR);	//设置屏幕显示--扫描方向
}	 


/******************************************************************/

//设置窗口,并自动设置画点坐标到窗口左上角(sx,sy).
//sx,sy:窗口起始坐标(左上角)
//width,height:窗口宽度和高度,必须大于0!!
//窗体大小:width*height. 

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/******************************************************************/


void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height)
{    

	u16 twidth,theight;
	
	twidth=sx+width-1;
	theight=sy+height-1;
	
	if(lcddev.id==0X9341)
	{
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(sx>>8); 
		LCD_WR_DATA(sx&0XFF);	 
		LCD_WR_DATA(twidth>>8); 
		LCD_WR_DATA(twidth&0XFF);  
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(sy>>8); 
		LCD_WR_DATA(sy&0XFF); 
		LCD_WR_DATA(theight>>8); 
		LCD_WR_DATA(theight&0XFF); 
	}

}


/******************************************************************/

//初始化lcd

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/******************************************************************/


void LCD_Init(void)
{ 					
 	GPIO_InitTypeDef GPIO_InitStructure;
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  readWriteTiming; 
	FSMC_NORSRAMTimingInitTypeDef  writeTiming;
	
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);	  //使能FSMC时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOG,ENABLE);//使能PB,PD,PE,PG以及AFIO复用功能时钟

 
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				     //PB0 推挽输出 背光
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
 	//PORTD复用推挽输出  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_14|GPIO_Pin_15;				 //	//PORTD复用推挽输出  
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //复用推挽输出   
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOD, &GPIO_InitStructure); 
  	 
	//PORTE复用推挽输出  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;				 //	//PORTD复用推挽输出  
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //复用推挽输出   
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOE, &GPIO_InitStructure);    	    	 											 

   	//	//PORTG12复用推挽输出 A0	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_12;	 //	//PORTD复用推挽输出  
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //复用推挽输出   
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOG, &GPIO_InitStructure); 

	readWriteTiming.FSMC_AddressSetupTime = 0x01;	 //地址建立时间（ADDSET）为2个HCLK 1/36M=27ns
  readWriteTiming.FSMC_AddressHoldTime = 0x00;	 //地址保持时间（ADDHLD）模式A未用到	
  readWriteTiming.FSMC_DataSetupTime = 0x0f;		 // 数据保存时间为16个HCLK,因为液晶驱动IC的读数据的时候，速度不能太快，尤其对1289这个IC。
  readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
  readWriteTiming.FSMC_CLKDivision = 0x00;
  readWriteTiming.FSMC_DataLatency = 0x00;
  readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //模式A 
    

	writeTiming.FSMC_AddressSetupTime = 0x00;	 //地址建立时间（ADDSET）为1个HCLK  
  writeTiming.FSMC_AddressHoldTime = 0x00;	 //地址保持时间（A		
  writeTiming.FSMC_DataSetupTime = 0x03;		 ////数据保存时间为4个HCLK	
  writeTiming.FSMC_BusTurnAroundDuration = 0x00;
  writeTiming.FSMC_CLKDivision = 0x00;
  writeTiming.FSMC_DataLatency = 0x00;
  writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //模式A 

 
  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;//  这里我们使用NE4 ，也就对应BTCR[6],[7]。
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; // 不复用数据地址
  FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;// FSMC_MemoryType_SRAM;  //SRAM   
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;//存储器数据宽度为16bit   
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;// FSMC_BurstAccessMode_Disable; 
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable; 
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;   
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;  
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;	//  存储器写使能
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;   
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable; // 读写使用不同的时序
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable; 
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming; //读写时序
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;  //写时序

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //初始化FSMC配置

 	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);  // 使能BANK1 
		
 
	delay_ms(50); 					// delay 50 ms 
  
	lcddev.id=LCD_ReadReg(0x0000);	//读ID  
  
	if(lcddev.id<0XFF||lcddev.id==0XFFFF||lcddev.id==0X9300)//读到ID不正确,新增lcddev.id==0X9300判断，因为9341在未被复位的情况下会被读成9300
	{	
 		//尝试9341 ID的读取		
		LCD_WR_REG(0XD3);				   
		lcddev.id=LCD_RD_DATA();	  //dummy read 	
 		lcddev.id=LCD_RD_DATA();	  //读到0X00
  	lcddev.id=LCD_RD_DATA();   	//读取93								   
 		lcddev.id<<=8;
		lcddev.id|=LCD_RD_DATA();  	//读取41 	  
		
 		if(lcddev.id!=0X9341)		//非9341,
		{	
      return;//退出初始化
 		}  	
	}
	
// 	printf(" LCD ID:%x\r\n",lcddev.id); //打印LCD ID  
	
	if(lcddev.id==0X9341)	//9341初始化
	{	 
		LCD_WR_REG(0xCF);  
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0xC1); 
		LCD_WR_DATA(0X30); 
		LCD_WR_REG(0xED);  
		LCD_WR_DATA(0x64); 
		LCD_WR_DATA(0x03); 
		LCD_WR_DATA(0X12); 
		LCD_WR_DATA(0X81); 
		LCD_WR_REG(0xE8);  
		LCD_WR_DATA(0x85); 
		LCD_WR_DATA(0x10); 
		LCD_WR_DATA(0x7A); 
		LCD_WR_REG(0xCB);  
		LCD_WR_DATA(0x39); 
		LCD_WR_DATA(0x2C); 
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x34); 
		LCD_WR_DATA(0x02); 
		LCD_WR_REG(0xF7);  
		LCD_WR_DATA(0x20); 
		LCD_WR_REG(0xEA);  
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x00); 
		LCD_WR_REG(0xC0);    //Power control 
		LCD_WR_DATA(0x1B);   //VRH[5:0] 
		LCD_WR_REG(0xC1);    //Power control 
		LCD_WR_DATA(0x01);   //SAP[2:0];BT[3:0] 
		LCD_WR_REG(0xC5);    //VCM control 
		LCD_WR_DATA(0x30); 	 //3F
		LCD_WR_DATA(0x30); 	 //3C
		LCD_WR_REG(0xC7);    //VCM control2 
		LCD_WR_DATA(0XB7); 
		LCD_WR_REG(0x36);    // Memory Access Control 
		LCD_WR_DATA(0x48); 
		LCD_WR_REG(0x3A);   
		LCD_WR_DATA(0x55); 
		LCD_WR_REG(0xB1);   
		LCD_WR_DATA(0x00);   
		LCD_WR_DATA(0x1A); 
		LCD_WR_REG(0xB6);    // Display Function Control 
		LCD_WR_DATA(0x0A); 
		LCD_WR_DATA(0xA2); 
		LCD_WR_REG(0xF2);    // 3Gamma Function Disable 
		LCD_WR_DATA(0x00); 
		LCD_WR_REG(0x26);    //Gamma curve selected 
		LCD_WR_DATA(0x01); 
		LCD_WR_REG(0xE0);    //Set Gamma 
		LCD_WR_DATA(0x0F); 
		LCD_WR_DATA(0x2A); 
		LCD_WR_DATA(0x28); 
		LCD_WR_DATA(0x08); 
		LCD_WR_DATA(0x0E); 
		LCD_WR_DATA(0x08); 
		LCD_WR_DATA(0x54); 
		LCD_WR_DATA(0XA9); 
		LCD_WR_DATA(0x43); 
		LCD_WR_DATA(0x0A); 
		LCD_WR_DATA(0x0F); 
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x00); 		 
		LCD_WR_REG(0XE1);    //Set Gamma 
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x15); 
		LCD_WR_DATA(0x17); 
		LCD_WR_DATA(0x07); 
		LCD_WR_DATA(0x11); 
		LCD_WR_DATA(0x06); 
		LCD_WR_DATA(0x2B); 
		LCD_WR_DATA(0x56); 
		LCD_WR_DATA(0x3C); 
		LCD_WR_DATA(0x05); 
		LCD_WR_DATA(0x10); 
		LCD_WR_DATA(0x0F); 
		LCD_WR_DATA(0x3F); 
		LCD_WR_DATA(0x3F); 
		LCD_WR_DATA(0x0F); 
		LCD_WR_REG(0x2B); 
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x01);
		LCD_WR_DATA(0x3f);
		LCD_WR_REG(0x2A); 
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xef);	 
		LCD_WR_REG(0x11); //Exit Sleep
		delay_ms(120);
		LCD_WR_REG(0x29); //display on	
	}
	

	LCD_Display_Dir(LCD_DIR_Mode);	//选择--屏幕显示方式
	LCD_LED=1;				//点亮背光
	LCD_Clear(WHITE);
	
	
}  


/******************************************************************/

//清屏函数
//color:要清屏的填充色

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/******************************************************************/


void LCD_Clear(u16 color)
{
	u32 index=0; 
	u32 totalpoint;
	
	LCD_Set_Window(0,0,lcddev.width,lcddev.height);//设置全屏窗口
	
  totalpoint=lcddev.width * lcddev.height; 			//得到总点数
	
  LCD_SetCursor(0x00,0x0000);	//设置光标位置 
	LCD_WriteRAM_Prepare();     //开始写入GRAM	 	  
	for(index=0;index<totalpoint;index++)
	{
		LCD->LCD_RAM=color;	
	}
} 


/******************************************************************/

//在指定区域内填充单个颜色
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/******************************************************************/


void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{          
		u16 i,j;
		u16 xlen=0;

	  xlen=ex-sx+1;	 
		for(i=sy;i<=ey;i++)
		{
		 	LCD_SetCursor(sx,i);      				//设置光标位置 
			LCD_WriteRAM_Prepare();     			//开始写入GRAM	  
			for(j=0;j<xlen;j++)LCD->LCD_RAM=color;	//显示颜色 	    
		}
	
}


/******************************************************************/

//在指定区域内填充指定颜色块			 
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/******************************************************************/


void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
{  
	u16 height,width;
	u16 i,j;
	width=ex-sx+1; 			//得到填充的宽度
	height=ey-sy+1;			//高度
 	for(i=0;i<height;i++)
	{
 		LCD_SetCursor(sx,sy+i);   	//设置光标位置 
		LCD_WriteRAM_Prepare();     //开始写入GRAM
		for(j=0;j<width;j++)LCD->LCD_RAM=color[i*width+j];//写入数据 
	}		  
}  

/******************************************************************/

//画线
//x1,y1:起点坐标
//x2,y2:终点坐标  
//Color;线条颜色

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/******************************************************************/


void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 Color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1;
	
	uRow=x1; 
	uCol=y1; 
	
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		LCD_Fast_DrawPoint(uRow,uCol,Color);//画点 --使用输入颜色参数 
		
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
} 


/*******************************************************************************/
//函数：void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
//函数功能：画矩形	  
//输入参数：
//(x1,y1),(x2,y2):矩形的对角坐标
//Color;线条颜色

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com
/*******************************************************************************/

void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 Color)
{
	LCD_DrawLine(x1,y1,x2,y1,Color);
	LCD_DrawLine(x1,y1,x1,y2,Color);
	LCD_DrawLine(x1,y2,x2,y2,Color);
	LCD_DrawLine(x2,y1,x2,y2,Color);
}


/*******************************************************************************/
//函数：void LCD_Draw_Circle(u16 x0,u16 y0,u8 r)
//函数功能：在指定位置画一个指定大小的圆
//输入参数：
//(x,y):中心点
//r    :半径
//Color;线条颜色

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com
/*******************************************************************************/

void LCD_Draw_Circle(u16 x0,u16 y0,u8 r, u16 Color)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		LCD_Fast_DrawPoint(x0+a,y0-b,Color);             //5
 		LCD_Fast_DrawPoint(x0+b,y0-a,Color);             //0           
		LCD_Fast_DrawPoint(x0+b,y0+a,Color);             //4               
		LCD_Fast_DrawPoint(x0+a,y0+b,Color);             //6 
		LCD_Fast_DrawPoint(x0-a,y0+b,Color);             //1       
 		LCD_Fast_DrawPoint(x0-b,y0+a,Color);             
		LCD_Fast_DrawPoint(x0-a,y0-b,Color);             //2             
  	LCD_Fast_DrawPoint(x0-b,y0-a,Color);             //7     	         
		a++;
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
}	


/*******************************************************************************/
//函数：LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u16 color,u8 mode)
//函数功能：在指定位置显示一个字符
//输入参数：
//x,y:起始坐标
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16/24
//color,字符颜色
//mode:叠加方式(1)还是非叠加方式(0)

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com
/*******************************************************************************/

void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u16 color,u8 mode)
{  							  
  u8 temp,t1,t;
	u16 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数	
	//设置窗口		   
	num=num-' ';//得到偏移后的值
	for(t=0;t<csize;t++)
	{   
		if(size==12)temp=asc2_1206[num][t]; 	 	//调用1206字体
		else if(size==16)temp=asc2_1608[num][t];	//调用1608字体
		else if(size==24)temp=asc2_2412[num][t];	//调用2412字体
		else return;								//没有的字库
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,color);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR);
			
			temp<<=1;
			y++;
			if(y>=lcddev.height)return;		//超区域了
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=lcddev.width)return;	//超区域了
				break;
			}
		}  	 
	}  	    	   	 	  
}


/******************************************************************/

//m^n函数
//返回值:m^n次方.

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/******************************************************************/


u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}	


/*******************************************************************************/
//函数：void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size,u16 color)
//函数功能：显示数字,高位为0,则不显示
//输入参数：

//x,y :起点坐标	 
//num:数值(0~4294967295);	
//len :数字的位数
//size:字体大小
//color:颜色 


//返回值:无

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com
/*******************************************************************************/

void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size,u16 color)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,' ',size,color,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,color,0); 
	}
} 


/*******************************************************************************/
//函数：void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u16 color,u8 mode)
//函数功能：显示数字,高位为0,还是显示
//输入参数：
//显示数字,高位为0,还是显示
//x,y:起点坐标
//num:数值(0~999999999);	 
//len:长度(即要显示的位数)
//size:字体大小
//color:颜色 
//mode:
//[7]:0,不填充;1,填充0.
//[6:1]:保留
//[0]:0,非叠加显示;1,叠加显示.

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com
/*******************************************************************************/

void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u16 color,u8 mode)
{  
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80)LCD_ShowChar(x+(size/2)*t,y,'0',size,color,mode&0X01);  
				else LCD_ShowChar(x+(size/2)*t,y,' ',size,color,mode&0X01);  
 				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,color,mode&0X01); 
	}
} 


/*******************************************************************************/
//函数：void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u16 color,u8 *p)
//函数功能：显示字符串
//输入参数：
//x,y:起点坐标
//width,height:区域大小  
//size:字体大小
//color:颜色 
//*p:字符串起始地址		  

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com
/*******************************************************************************/


void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u16 color,u8 *p)
{         
	u8 x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//退出
        LCD_ShowChar(x,y,*p,size,color,0);
        x+=size/2;
        p++;
    }  
}




//****************************************************************************************/
//函数：void DisplayButtonDown(u16 x1,u16 y1,u16 x2,u16 y2)
//功能描述: 在屏幕显示一凸起的按钮框
//输    入: u16 x1,y1,x2,y2 按钮框左上角和右下角坐标
//输    出: 无


//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com
/****************************************************************************************/

void DisplayButtonDown(u16 x1,u16 y1,u16 x2,u16 y2)
{
	
	LCD_DrawLine(x1,y1, x2,y1,GRAY2);  //H
		
	LCD_DrawLine(x1+1,y1+1,x2,y1+1,GRAY1);  //H
		
	LCD_DrawLine(x1,  y1,  x1,y2,GRAY2);    //V
		
	LCD_DrawLine(x1+1,y1+1,x1+1,y2,GRAY1);  //V
	
	LCD_DrawLine(x1, y2, x2, y2,WHITE);     //H
	LCD_DrawLine(x2, y1, x2, y2,WHITE);     //V
}

//****************************************************************************************/
//函数：void DisplayButtonUp(u16 x1,u16 y1,u16 x2,u16 y2)
//功能描述: 在屏幕显示一凹下的按钮框
//输    入: u16 x1,y1,x2,y2 按钮框左上角和右下角坐标
//输    出: 无
//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com
/****************************************************************************************/
void DisplayButtonUp(u16 x1,u16 y1,u16 x2,u16 y2)
{
	
	LCD_DrawLine(x1,  y1,  x2,y1,WHITE);    //H
	LCD_DrawLine(x1,  y1,  x1,y2,WHITE);    //V
	
	LCD_DrawLine(x1+1,y2-1,x2,y2-1,GRAY1);  //H
		
	LCD_DrawLine(x1,  y2,  x2,y2,GRAY2);    //H
		
	LCD_DrawLine(x2-1,y1+1,x2-1,y2,GRAY1);  //V
		
  LCD_DrawLine(x2  ,y1  ,x2,y2,GRAY2);    //V
	

}


//****************************************************************************************/
//函数：void Draw_Test(void)
//功能描述: 绘制图形函数测试


//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com
/****************************************************************************************/

void Draw_Test(void)
{
	
		LCD_Clear(WHITE); //清屏
		
	  LCD_DrawLine(20,64, 220,128,RED);//划线函数
	  LCD_DrawLine(20,128, 220,64,RED);//划线函数
	
	  LCD_DrawRectangle(20,64,220,128, BLUE);//绘制方形状
	  
	  LCD_Draw_Circle(120,96,81, BRED);//绘制圆形
	  LCD_Draw_Circle(120,96,80, BRED);//绘制圆形
	  LCD_Draw_Circle(120,96,79, BRED);//绘制圆形
			
		delay_ms(1000);//延时

}


//****************************************************************************************/
//函数：void Color_Test(void)
//功能描述: 颜色填充显示测试


//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com
/****************************************************************************************/

void Color_Test(void)
{
	
		LCD_Clear(GRAY0); //清屏
		
		Draw_Font16B(24,16,BLUE,"1: 颜色填充测试");
	
	  LCD_Fill(5,5,lcddev.width-5,lcddev.height-5,RED);//设置一个窗口，写入指定区域颜色
	
	  LCD_Fill(20,20,lcddev.width-20,lcddev.height-20,YELLOW);//设置一个窗口，写入指定区域颜色
	
	  LCD_Fill(50,50,lcddev.width-50,lcddev.height-50,BLUE);//设置一个窗口，写入指定区域颜色
	
	  LCD_Fill(80,100,lcddev.width-80,lcddev.height-80,MAGENTA);//设置一个窗口，写入指定区域颜色
	
		delay_ms(1000);

		LCD_Clear(WHITE);
	
	  Draw_Test();//绘图函数测试
	
		delay_ms(500);//延时
		LCD_Clear(BLACK);
		delay_ms(500);//延时
		LCD_Clear(RED);
		delay_ms(500);//延时
		LCD_Clear(YELLOW);
		delay_ms(500);//延时
		LCD_Clear(GREEN);
		delay_ms(500);//延时
		LCD_Clear(BLUE);
		delay_ms(500);//延时

}


uint16_t D_Color=BLUE; //点阵颜色
uint16_t B_Color=WHITE; //背景颜色

//****************************************************************************************/
//函数：void Font_Test(void)
//功能描述: 字体字形显示测试


//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com
/****************************************************************************************/

void Font_Test(void)
{
	LCD_Clear(GRAY0);
	Draw_Font16B(24,16,BLUE,"2: 文字显示测试");

	delay_ms(1000);
	LCD_Clear(GRAY0);
	
	Draw_Font24B(16,4,BLACK,"DevEBox-大越创新");
	Draw_Font24B(48,32,BLACK,"液晶屏测试");
	
	Draw_Font16B(24,80, BLACK,"嵌入式开发网");
	Draw_Font16B(24,96, GREEN,"中文测试");
	Draw_Font16B(24,112,BLUE,"专注液晶批发");
	Draw_Font16B(24,128,RED, "全程技术支持");
	Draw_Font16B(16,144,BLUE,"F: mcudev.taobao.com");
	Draw_Font16B(16,160,BLUE,"F: shop389957290.taobao.com");
	Draw_Font16B(16,176,RED,"0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz");

	delay_ms(3000);	
	
	LCD_Clear(GRAY0);


}


//****************************************************************************************/
//函数：void Demo_Menu(void)
//功能描述: 演示程序菜单


//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com
/****************************************************************************************/

void Demo_Menu(void)
{

	LCD_Clear(GRAY0); //清屏
	
	Draw_Font24B(16,4,MAGENTA,"DevEBox-大越创新");
	Draw_Font24B(48,32,MAGENTA,"液晶屏测试");


	DisplayButtonUp(16,64,224,88); //x1,y1,x2,y2
	Draw_Font16B(48,68,BLUE,"1: 颜色填充测试");

	DisplayButtonUp(16,104,224,128); //x1,y1,x2,y2
	Draw_Font16B(48,108,BLUE,"2: 文字显示测试");

	DisplayButtonUp(16,144,224,168); //x1,y1,x2,y2
	Draw_Font16B(48,148,BLUE,"3: 图片显示测试");
  
	Draw_Font16B(16,180,RED,"F: mcudev.taobao.com");
	Draw_Font16B(16,196,RED,"F: shop389957290.taobao.com");
	
	delay_ms(2000);//延时
		
	Color_Test();       //颜色填充测试
	Font_Test();        //字体字形显示测试
  GBK_LibFont_Test(); //GBK字库测试 -(如果使用不带字库的液晶屏版本，此处可以屏蔽，不做字库测试）
	Show_Picture();     //显示一张图片 (如果芯片FLASH存储空间有限，可以去掉此函数，减少图片占用空间）
	


}















































/******************************************************************/

//STM32F103ZE核心板
//2.4寸/2.8寸/3.2寸  TFT液晶驱动	  
//支持驱动IC型号包括:ILI9341等	

//DevEBox  大越创新
//淘宝店铺：mcudev.taobao.com
//淘宝店铺：shop389957290.taobao.com	

/******************************************************************/












