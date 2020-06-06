#include "delay.h"
#include "tim3.h"

#include "MD_RTU_APP.h"
#include "MDM_RTU_APP.h"

 int main(void)
 {	 		    
	delay_init();	    	 //延时函数初始化	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	MDS_RTU_APPInit();
	//MDM_RTU_APPInit();
	TIM3_Int_Init(72-1,100-1);
	while(1){
		MDS_RTU_Loop();
		//MDM_RTU_Loop();
	}
}





