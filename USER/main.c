#include "delay.h"
#include "tim3.h"
#include "Sys_Config.h"
#include "RTOS_task.h"
#include "MDM_RTU_RW_Man.h"
#if MD_USD_SALVE
#include "MDS_RTU_APP.h"
#include "MDS_RTU_APP_1.h"
#else
#include "MDM_RTU_APP.h"
#include "MDM_RTU_Fun.h"
#include "MDM_RTU_User_Fun.h"
#endif

#if MD_RTU_USED_OS
PTASK_TCB task0;
PTASK_TCB task1;
PTASK_TCB task2;
extern Modbus_RTU_CB modbusRWRTUCB;
extern Modbus_RTU_CB modbusRWRTUCB1;
void mdTestTask0(void *arg){
	uint16 temp;
	for(;;){
		if(ERR_RW_FIN==MDM_RTU_ReadHoldReg(&modbusRWRTUCB,1,0,1)){
			MDM_RTU_ReadRegs(modbusRWRTUCB.pModbus_RTU,0,1, (&temp),HOLD_REGS_TYPE,0x1);
		}
		OSTaskDelay(500);
	}
}
void mdTestTask1(void *arg){
	uint16 temp=0;
	for(;;){	
		if(ERR_RW_FIN==MDM_RTU_WriteSingleReg(&modbusRWRTUCB1,2,0,temp)){
			//MDM_RTU_ReadRegs(modbusRWRTUCB1.pModbus_RTU,0,1, (&temp),HOLD_REGS_TYPE,2);
		}
		temp++;
		OSTaskDelay(500);
	}
}
#endif

int main(void)
 {	 
	 #if MD_RTU_USED_OS
	OSInit();	 
	 	#endif
	//delay_init();
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
#if MD_USD_SALVE
		MDS_RTU_APPInit();
		MDS_RTU_APPInit_1();
#else
		MDM_RTU_APPInit();
	  
#endif
	TIM3_Int_Init(72-1,100-1);
	
	#if MD_RTU_USED_OS
	task1=OSCreateTask(mdTestTask0,NULL,6,256);
	task2=OSCreateTask(mdTestTask1,NULL,6,256);
	SysTick_Config(SystemCoreClock / OS_TICKS_PER_SEC);
	OSStart();
	#endif
	while(1){
		#if !MD_RTU_USED_OS
			#if MD_USD_SALVE
			MDS_RTU_Loop();
			MDS_RTU_Loop_1();
			#else
			MDM_RTU_Loop();
			MDM_RW_CtrlLoop();
			#endif
		#endif
	}
}





