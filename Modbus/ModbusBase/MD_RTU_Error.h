#ifndef _MDM_RTU_ERROR_H__
#define _MDM_RTU_ERROR_H__

typedef enum{
	ERR_VOID=0,					/*空指针错误*/
	ERR_NONE=1,					/*没有错误*/
	ERR_IDLE,						/*代表该块当前被空闲*/
	ERR_QUEUE,					/*队列写入错误*/
	ERR_OVER_TIME,			/*超时错误*/
	ERR_SEND_FIN,				/*发送完成*/
	ERR_RW_OV_TIME_ERR,	/*发送超时错误*/
	ERR_SLAVE_ADDR,			/*从机地址不匹配*/
	ERR_DATA_LEN,				/*数据长度错误*/
	ERR_DATA_SAVE,			/*数据保存错误*/
	
	/*下面的错误是从机返回的*/
	ERR_READ_COIL,			/*读线圈错误*/
	ERR_READ_INPUT,			/*读输入错误*/
	ERR_READ_HOLD_REG,	/*读保持寄存器错误*/
	ERR_READ_INPUT_REG,	/*读输入寄存器错误*/
	ERR_WRITE_COIL,			/*写线圈错误*/
	ERR_WRITE_REG,			/*写寄存器错误*/
	ERR_RW_FIN,					/*代表从机收到且主机收到正确回应*/
}MDError;

#endif
