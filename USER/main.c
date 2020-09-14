#include "delay.h"
#include "tim3.h"
#include "Sys_Config.h"

#include "MDM_RTU_RW_Man.h"
#if MD_USD_SALVE
#include "MDS_RTU_APP.h"
#include "MDS_RTU_APP_1.h"
#else
#include "MDM_RTU_APP.h"
#endif

int main(void)
 {	 		    
	delay_init();
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
#if MD_USD_SALVE
		MDS_RTU_APPInit();
		MDS_RTU_APPInit_1();
#else
		MDM_RTU_APPInit();
#endif
	TIM3_Int_Init(72-1,100-1);
	while(1){
		#if MD_USD_SALVE
		MDS_RTU_Loop();
		MDS_RTU_Loop_1();
		#else
		MDM_RTU_Loop();
		#endif
		
		MDM_RW_CtrlLoop();
	}
}





