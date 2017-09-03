#include "stm32f4xx_conf.h"
#include "stdlib.h"

/*behaviour of task responsible for communication*/
void vTaskDropCharge(void *args)
{
	int16_t iAux_m1 = SP_M1;
	int16_t iAux_m3 = SP_M3;
	int i = 0;
	
	for(;;)
	{
		
		if(i<25){
			vChangeAngle(iAux_m1+=2.7, Pwm_ARMM1); 
			vChangeAngle(iAux_m3-=1.5, Pwm_ARMM3);
			//Delay the arm
			vTaskDelay(pdMS_TO_TICKS(300));
			i++;
		}
		if(i>=25)
		{
			vChangeAngle(SP_CLAWM_MID,Pwm_CLAWM);
			vTaskDelay(pdMS_TO_TICKS(300)); //wait 200ms
			
			while( (iAux_m1 > SP_M1) || (iAux_m3 > SP_M3) ){
					vChangeAngle(iAux_m1-=2, Pwm_ARMM1); 
					vChangeAngle(iAux_m3+=2, Pwm_ARMM3);
					vTaskDelay(pdMS_TO_TICKS(100));
				}
				vStartPos();
				i=0;					//actulaize the index to the next execution
			vTaskSuspend(xTask_DropCharge);
		}
	}
}
