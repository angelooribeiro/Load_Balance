#include "stm32f4xx_conf.h"

/*behaviour of task responsible for communication*/
void vTaskGrabCharge(void *args)
{
	//Task Variables
	uint8_t iDetectStatus;		//variable to store the returned value in the notify
	int16_t iAux_m1 = SP_M1;
	int16_t iAux_m3 = SP_M3;
	int i = 0;
	
	for( ;; ){
		
		if( (i!=25) && (iDetectStatus!=0x01))
		{	
			//Take the arm to the next lower position
			vChangeAngle(iAux_m1, Pwm_ARMM1); 
			vChangeAngle(iAux_m3, Pwm_ARMM3);
			//Delay the arm
			vTaskDelay(pdMS_TO_TICKS(300));
			//Send the pulse to the HCSR 04
			SR04_SendPulse();
			
			//wait for the reception of the message with the distance
 			while(xQueueReceive(xQueue_Detection, &iDetectStatus, 
																						(portMAX_DELAY)) != pdTRUE) {}

 			if(iDetectStatus == 0x00){		//If not detect
				iAux_m1+=2.7;				//keep going down
				iAux_m3-=1.5;
				i++;
			}
			
			if(iDetectStatus == 0x01)			//If detected
			{
				//function to gow down the arm a little bit
				for(int i = 0; i<9; i++){
					iAux_m1+=2.7;				//keep going down
					iAux_m3-=1.5;
					vChangeAngle(iAux_m1, Pwm_ARMM1);
					vTaskDelay(pdMS_TO_TICKS(200));
				}
				
				// Close the clawm
				vChangeAngle(SP_CLAWM_MID,Pwm_CLAWM);
				vTaskDelay(pdMS_TO_TICKS(300)); //wait 200ms
				
				while( (iAux_m1 > SP_M1) || (iAux_m3 > SP_M3) ){
					vChangeAngle(iAux_m1-=2, Pwm_ARMM1); 
					vChangeAngle(iAux_m3+=2, Pwm_ARMM3);
					vTaskDelay(pdMS_TO_TICKS(100));
				}
				vStartPos();
				i=0;					//actulaize the index to the next execution
				
				//Communicate the success
				xQueueSend(xQueue_Communication, "Detected\0", my_port_delay);
				xSemaphoreGive(xSemaphore_Grab); 	//indicates that the charge as been grab
				vTaskSuspend(xTask_GrabCharge);		//Suspend the own task
			}//if detected
		}	//end first if
		
		//The charge was not detected
		if( (i == 25) && (iDetectStatus!=0x01)){
			//send message not detected
			xQueueSend(xQueue_Communication, "Not Detected\0", my_port_delay);
			vTaskSuspend(xTask_GrabCharge);		//Suspend the own task
		}
	}	//end for
}	//end task behaviour
