#include "delay.h"
#include "tim3.h"
#include "Sys_Config.h"
#if MD_USD_SALVE
#include "MDS_RTU_APP.h"
#else
#include "MDM_RTU_APP.h"
#endif

int main(void)
 {	 		    
	delay_init();	    	 //延时函数初始化	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
#if MD_USD_SALVE
		MDS_RTU_APPInit();
#else
		MDM_RTU_APPInit();
#endif
	TIM3_Int_Init(72-1,100-1);
	while(1){
		#if MD_USD_SALVE
		MDS_RTU_Loop();
		#else
		MDM_RTU_Loop();
		#endif
	}
}





