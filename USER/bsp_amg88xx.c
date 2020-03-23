#include "bsp_amg88xx.h"
#include "myiic.h"
uint8_t xx;

void amg88xx_init(void)
{
	IIC_Init();
	amg88xx_write(AMG88xx_PCTL, AMG88xx_NORMAL_MODE);
	amg88xx_write(AMG88xx_RST, AMG88xx_INITIAL_RESET);
	amg88xx_write(AMG88xx_FPSC, AMG88xx_FPS_10);
}

uint8_t amg88xx_write(uint8_t reg,uint8_t data)
{
    IIC_Start();
    IIC_Send_Byte((AMG88xx_ADDRESS<<1)|0); //发送器件地址+写命令
    if(IIC_Wait_Ack())          //等待应答
    {
        IIC_Stop();
        return 1;
    }
    IIC_Send_Byte(reg);         //写寄存器地址
    IIC_Wait_Ack();             //等待应答
    IIC_Send_Byte(data);        //发送数据
    if(IIC_Wait_Ack())          //等待ACK
    {
        IIC_Stop();
        return 1;
    }
    IIC_Stop();
    return 0;
}

uint8_t amg88xx_read_len(uint8_t reg,uint8_t len,uint8_t *buf)
{ 
	IIC_Start();
	IIC_Send_Byte((AMG88xx_ADDRESS<<1)|0); //发送器件地址+写命令
	if(IIC_Wait_Ack())          //等待应答
	{
		IIC_Stop();
		return 1;
	}
	IIC_Send_Byte(reg);         //写寄存器地址
	IIC_Wait_Ack();             //等待应答
	IIC_Start();                
	IIC_Send_Byte((AMG88xx_ADDRESS<<1)|1); //发送器件地址+读命令
	IIC_Wait_Ack();             //等待应答
	while(len)
	{
		if(len==1)
		{
			*buf=IIC_Read_Byte(0);//读数据,发送nACK 
		}
		else 
		{
			*buf=IIC_Read_Byte(1);		//读数据,发送ACK  
		}
		len--;
		buf++;  
	}
	IIC_Stop();                 //产生一个停止条件
	return 0;          
}

float AMG88XX_signedMag12ToFloat(uint16_t val)
{
	//take first 11 bits as absolute val
	uint16_t absVal = (val & 0x7FF);
	
	return (val & 0x8000) ? 0 - (float)absVal : (float)absVal ;
}

float amg88xx_readThermistor()
{
	uint8_t raw[2];
	uint16_t recast;
	
	amg88xx_read_len(AMG88xx_TTHL, 2, raw);
	recast = ((uint16_t)raw[1] << 8) | ((uint16_t)raw[0]);
	return AMG88XX_signedMag12ToFloat(recast) * AMG88xx_THERMISTOR_CONVERSION;
}

void amg88xx_readPixels(float *buf, uint8_t size)
{
	uint16_t recast;
	float converted;
	uint8_t rawArray[128];
	
	amg88xx_read_len(AMG88xx_PIXEL_OFFSET, 128, rawArray);
	
	for(int i=0; i<size; i++)
	{
		uint8_t pos = i << 1;
		recast = ((uint16_t)rawArray[pos + 1] << 8) | ((uint16_t)rawArray[pos]);	
		
		converted = AMG88XX_signedMag12ToFloat(recast) * AMG88xx_PIXEL_TEMP_CONVERSION;
		buf[i] = converted;
	}
}

