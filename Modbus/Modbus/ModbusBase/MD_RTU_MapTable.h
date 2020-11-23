/**@file  MD_RTU_MapTable.h
* @brief       Modbus RTU Discrete mapping management module
* @author      zspace
* @date        2020-4-10
* @version     V1.0
**********************************************************************************
* @par Open source address
*	https://github.com/lotoohe-space/XTinyModbus
* @par modify log:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020-4-10  <td>1.0      <td>zspace  		<td>First version
* </table>
*
**********************************************************************************
*/

#ifndef _MD_RTU_REGCOIL_H__
#define _MD_RTU_REGCOIL_H__

/*********************************HEAD FILE************************************/
#include "MD_RTU_Type.h"
/*********************************END******************************************/
	
/*********************************CUSTOM DATA TYPE************************************/

/**@enum AddrType
* @brief Modbus RTU types
*/
typedef enum{		
	COILS_TYPE=1,			///< coil type
	INPUT_TYPE=2,			///< input type
	HOLD_REGS_TYPE=3,	///< hold reg type
	INPUT_REGS_TYPE=4	///< input reg type
}AddrType;

/**@struct MapTableItem
* @brief Discrete mapping structure \n
* Map modbus addresses to memory addresses
*/
typedef struct{
	uint16 		modbusAddr;			///< Modbus address
	uint16*		modbusData;			///< Stored data
	uint16		modbusDataSize;	///< Map size
	AddrType	addrType;				///< Address type,Reference @see :: AddrType
	uint8			devAddr;				///< Indicate which device the mapping item belongs to. For the time being, only the master uses this variable and the slave ignores it
}*PMapTableItem,MapTableItem;
/*********************************END******************************************/

/*********************************FUNCTION DECLARATION************************************/
uint8 MapTableAdd(void* obj,PMapTableItem pMapTableItem,uint16 tabSize);
uint8 MapTableDel(void* obj,PMapTableItem pMapTableItem,uint16 tabSize);
/*********************************END******************************************/

#endif
