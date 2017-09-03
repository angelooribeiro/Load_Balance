#include "stm32f4xx_conf.h"
#include "string.h"

void vTaskCommunication(void *args){
	
	//Instanciate the struct that will receive the information from the PID
	Pid_t PidStruct_t;
	//array that will receive the message from the queue communication
	char send[50] = {0};
	//array that will receive the message from the queue that receive the wifi communication
	char receive[10] = {0};
	char resultcom[20] = {0};
	uint8_t size = 0;
	
	for(;;){
		
		//If there is a last PID state to be sent
		if(xQueueReceive(xQueue_PidToCommunication, &PidStruct_t, 0) != pdFALSE){
			//Send the request to send the message
			printf("AT+CIPSEND=%d\r\n", 55);
			vTaskDelay(pdMS_TO_TICKS(40));
			//scanf("%s", resultcom);
			//Send the message
				printf("PID%lf+%lf+%lf+%lf+%lf        ", PidStruct_t.f_pid_error, PidStruct_t.f_pid_gain[0], 
						PidStruct_t.f_pid_gain[1], PidStruct_t.f_pid_gain[2], PidStruct_t.f_current_angle);
				vTaskDelay(pdMS_TO_TICKS(200));
		}
		
		//If data is availabe to be sent
		if(xQueueReceive(xQueue_Communication, &send, 0) != pdFALSE) {
			while(send[size++] != '\0');		//Count how many characteres exist to be sent
			printf("AT+CIPSEND=%d\r\n", size);
			vTaskDelay(pdMS_TO_TICKS(40));
			//scanf("%s", resultcom);
				//Send the message
				printf("%s", send);
				vTaskDelay(pdMS_TO_TICKS(1));
		}
		
		//If data is received on the wifi
		if(xQueueReceive(xQueue_WifiReception, &receive , 0) != pdFALSE) {
			//If is a Grab Charge
			if( ( (receive[0]=='C') && (receive[1] == 'G')) ){
				//Test to do a Grab
				if( (eTaskGetState(xTask_BalanceCharge)==eSuspended) 	&& 
						(eTaskGetState(xTask_MPUAcquisition)==eSuspended) &&
						(eTaskGetState(xTask_DropCharge)==eSuspended) 		&& 
						(eTaskGetState(xTask_GrabCharge)==eSuspended))		{
					vTaskResume(xTask_GrabCharge);		//Starting executing the task if all is ok
				}
				else
						xQueueSend(xQueue_Communication ,"Error\0", my_port_delay); 
			} //end CG
			
			//If is a Charge Drop
			if( ( (receive[0]=='C') && (receive[1] == 'D')) ){
				//Test if is possible to do the drop
				if( (eTaskGetState(xTask_BalanceCharge)==eSuspended) 		&&		//is running?
						(eTaskGetState(xTask_MPUAcquisition)==eSuspended)		&&
						(eTaskGetState(xTask_DropCharge)==eSuspended) 			&& 		//is with a charge
						(eTaskGetState(xTask_GrabCharge)==eSuspended) ) 		{ 
					vTaskResume(xTask_DropCharge);		//Start the Drop task		
				}
				else
					xQueueSend(xQueue_Communication ,"Error\0", my_port_delay);
			}//end CD
			
			//If is a Start
			if( ( (receive[0]=='S') && (receive[1] == 'T')) ){
				//Test to start the movement
				if(eTaskGetState(xTask_GrabCharge)==eSuspended &&
					(xSemaphoreTake( xSemaphore_Grab, my_port_delay ))){	//garanty that an boject is been already grabed
					vTaskResume(xTask_BalanceCharge);		//Suspend the tasks
					vTaskResume(xTask_MPUAcquisition);
				}
				else
					xQueueSend(xQueue_Communication ,"ErrorST\0", my_port_delay);
			} //end START
			
			//IF is a STOP
			if( ( (receive[0]=='S') && (receive[1] == 'P')) ){
			//Test to Stop a movement
			if( (eTaskGetState(xTask_BalanceCharge)!=eSuspended) ||	//if the movent is executing
					(eTaskGetState(xTask_MPUAcquisition)!=eSuspended)){
					vTaskSuspend(xTask_BalanceCharge);	//Stop the tasks
					vTaskSuspend(xTask_MPUAcquisition);
				}
				else
					xQueueSend(xQueue_Communication, "ErrorSP\0", my_port_delay);
			} //end stop
		} //end wifi receive
	} //end for
} //end behaviour function
