
#ifndef _MD_RTU_CONFIG_H__
#define _MD_RTU_CONFIG_H__

/*********************************配置参数**************************************/

#define MD_RTU_CRC16_FAST_MODE 			1					/*CRC校验方式配置*/

#define MD_RTU_QUEUE_SIZE 					256				/*缓冲队列大小*/

/*与从机相关配置*/
#define MDS_REG_COIL_ITEM_NUM 			20				/*离散映射最大数量*/
#define MDS_RTU_CMD_SIZE						256				/*单条指令长度*/
#define MDS_RTU_SEND_CACHE_SIZE			256				/*发送缓存长度*/
#define MDS_USE_SEND_CACHE 					1					/*是否启用发送缓存*/

/*与主机相关配置*/
#define MDM_REG_COIL_ITEM_NUM 			20				/*离散映射最大数量*/
#define MDM_RTU_SEND_CACHE_SIZE			256				/*发送缓存大小*/
#define MDM_USE_SEND_CACHE					1					/*是否开启发送缓存*/


/*********************************结束******************************************/


#endif

