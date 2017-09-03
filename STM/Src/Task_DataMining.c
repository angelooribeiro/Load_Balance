#include "stm32f4xx_conf.h"


const int16_t velocity[5] = {100, 150, 200, 300, 400};

/*behaviour of task responsible for determine each is the best velocity for the system*/
void vTaskDatamining(void *args){
	float32_t angles[10] = {0};			//array with the received angles
	int16_t level = 0; 							//level of velocity that must be applied
	float32_t result_angle;					//to determine the level
	
	for(;;){
		
		while(ulTaskNotifyTake(pdTRUE, portMAX_DELAY)) {} //waits the notification to execute
		for(int i = 0; i < 10; i++)	//reads all the values ??????????????????????????????????????????
			while(xQueueReceive(xQueue_AngleToMining, &angles[i], (portMAX_DELAY)) != pdTRUE);
		
		/*#################		datamining		#######################*/	
		//determin each should be the PWM apllied to the weels of the car
			
		//chose the aproprieted angle
		
			
		vChangeVelocity(velocity[level]); //Change the velocity of the system
	}
}

