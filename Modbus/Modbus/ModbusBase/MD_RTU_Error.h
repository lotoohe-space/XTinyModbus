#ifndef _MDM_RTU_ERROR_H__
#define _MDM_RTU_ERROR_H__

typedef enum{
	ERR_VOID=0,					/*Null pointer error*/
	ERR_NONE=1,					/*No error*/
	ERR_CTE_OBJ,				/*Object creation error*/
	ERR_IDLE,						/*The block is currently free*/
	ERR_QUEUE,					/*Queue write error*/
	ERR_OVER_TIME,			/*Timeout error*/
	ERR_SEND_FIN,				/*Send complete*/
	ERR_RW_OV_TIME_ERR,	/*Send timeout error*/
	ERR_DEV_DIS,				/*Device disconnection*/
	ERR_SLAVE_ADDR,			/*The slave address does not match*/
	ERR_DATA_LEN,				/*Data length error*/
	ERR_DATA_SAVE,			/*Data saving error*/
	
	/*The following error is returned from the slave machine.*/
	ERR_READ_COIL,			/*Read coil error*/
	ERR_READ_INPUT,			/*Read input error*/
	ERR_READ_HOLD_REG,	/*Read holding register error*/
	ERR_READ_INPUT_REG,	/*Error reading input register*/
	ERR_WRITE_COIL,			/*Write coil error*/
	ERR_WRITE_REG,			/*Write register error*/
	ERR_RW_FIN,					/*The slave receives and the master receives the correct response*/
}MDError;

#endif
