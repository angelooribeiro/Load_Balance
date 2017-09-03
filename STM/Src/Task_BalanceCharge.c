#include "stm32f4xx_conf.h"

int8_t i_pid_tunned;

/*behaviour of task responsible for mantain the charge in balance*/
void vTaskBalanceCharge(void *args)
{	
	//Instanciate the struct
	Pid_t PidStruct_t;

	//The angle that the system want to achive
	float32_t f_desired_angle = 0.0f;
	int8_t i_index = 0;		//auxiliar
	i_pid_tunned = 0; 		//when the task is created this values is always zero
	
	//variables for twiddle algortihm that doesn´t need to be sent
	float32_t f_twiddle_threshold = 1.0f;		//value for wich there is not necessary improve more
	float32_t f_best_error = 0.0f; 					//to save the last best error
	float32_t potencial_adj[3] = {0.1f, 0.1f, 0.1f};	//vector with the possible changes in the gains
	
	//Initialiaze an intance of the PID cmsis
	arm_pid_instance_f32 PID;
	
	//initialize the struct of the PID to the default values
	PidStruct_t.f_pid_error = 0.0f;
	PidStruct_t.f_current_angle = 0.0f;
	PidStruct_t.f_pid_gain[0] =  0.01f;
	PidStruct_t.f_pid_gain[1] =  0.1f;
	PidStruct_t.f_pid_gain[2] =  1.5f;

	//Passe the values to the PID struct
	PID.Kp = PidStruct_t.f_pid_gain[0];
	PID.Ki = PidStruct_t.f_pid_gain[1];
	PID.Kd = PidStruct_t.f_pid_gain[2]; 

	//Initialize the PID with the struct and with the flag reset enabled
	arm_pid_init_f32(&PID, 1);
	
	for(;;){
		
		//verify if is a message in the queue
		while(xQueueReceive(xQueue_AngleToPid, &PidStruct_t.f_current_angle, my_port_delay) != pdTRUE) {}
		PidStruct_t.f_pid_error = PidStruct_t.f_current_angle - f_desired_angle;		//calculate the error
		PidStruct_t.f_duty = arm_pid_f32(&PID, PidStruct_t.f_pid_error);
			
			if(i_pid_tunned){
				if( (PidStruct_t.f_current_angle  < (float32_t)0.0f ))
					vChangeVelocity(100);
				if( (PidStruct_t.f_current_angle > (float32_t)20.0f ))
					vChangeVelocity(65535);		
				
				/*Test the Motor limits*/
				if(PidStruct_t.f_duty < UP_LIMIT_M3)
					PidStruct_t.f_duty = UP_LIMIT_M3;
				if(PidStruct_t.f_duty > DOWN_LIMIT_M3)
					PidStruct_t.f_duty = DOWN_LIMIT_M3;
					vChangeAngle(PidStruct_t.f_duty, Pwm_ARMM3);		//change the pwm
				
				xQueueSend(xQueue_PidToCommunication ,&PidStruct_t, 50);			
			}//end tunned	

			if(!i_pid_tunned){						//if the system is not already tunned
				if( (potencial_adj[0] + potencial_adj[1] + potencial_adj[2]) > f_twiddle_threshold){		//if there is a need to improve the gains
					
					//Assiggn the last determained improvement
					PidStruct_t.f_pid_gain[i_index] += potencial_adj[i_index];		
					//actualize the state of the struct
					PID.Kp = PidStruct_t.f_pid_gain[0];
					PID.Ki = PidStruct_t.f_pid_gain[1];
					PID.Kd = PidStruct_t.f_pid_gain[2]; 
				
					arm_pid_init_f32(&PID, 0); //passes the struct to the PID do not reset
					//run th PID algorithm and determine each PWM must be applied
					PidStruct_t.f_duty = arm_pid_f32(&PID, PidStruct_t.f_pid_error);
					
					//If is insid the threshold
					if(PidStruct_t.f_duty < UP_LIMIT_M3)
						PidStruct_t.f_duty = UP_LIMIT_M3;
					if(PidStruct_t.f_duty > DOWN_LIMIT_M3)
						PidStruct_t.f_duty = DOWN_LIMIT_M3;
					vChangeAngle(PidStruct_t.f_duty, Pwm_ARMM3);		//change the pwm	
					xQueueSend(xQueue_PidToCommunication ,&PidStruct_t, 100 );
					
					//comeback to test the again
					while(xQueueReceive(xQueue_AngleToPid, &PidStruct_t.f_current_angle, (my_port_delay)) != pdTRUE) {}
					PidStruct_t.f_pid_error = PidStruct_t.f_current_angle - f_desired_angle;		//calculate the error
					if(PidStruct_t.f_pid_error < f_best_error){  		// There was some improvement
						f_best_error = PidStruct_t.f_pid_error;				//if the new error is the smallest ever
						potencial_adj[i_index] *= 1.1f;								//continue in the same direction to achive better
					}
					else{  // There was no improvement
						PidStruct_t.f_pid_gain[i_index] -= 2.0f*potencial_adj[i_index];  	//Go into the other direction
					}
					//actualize the state of the struct
					PID.Kp = PidStruct_t.f_pid_gain[0];
					PID.Ki = PidStruct_t.f_pid_gain[1];
					PID.Kd = PidStruct_t.f_pid_gain[2]; 
					
					arm_pid_init_f32(&PID, 0); //passes the struct to the PID do not reset
					//run th PID algorithm and determine each PWM must be applied
					PidStruct_t.f_duty = arm_pid_f32(&PID, PidStruct_t.f_pid_error);
					//If is insid the threshold
					if(PidStruct_t.f_duty < UP_LIMIT_M3)
						PidStruct_t.f_duty = UP_LIMIT_M3;
					if(PidStruct_t.f_duty > DOWN_LIMIT_M3)
						PidStruct_t.f_duty = DOWN_LIMIT_M3;
					vChangeAngle(PidStruct_t.f_duty, Pwm_ARMM3);		//change the pwm	
					xQueueSend(xQueue_PidToCommunication ,&PidStruct_t, 100 );
						
					//the secound cycle gives a more fine adjustment
					while(xQueueReceive(xQueue_AngleToPid, &PidStruct_t.f_current_angle, (my_port_delay)) != pdTRUE) {}
					PidStruct_t.f_pid_error = PidStruct_t.f_current_angle - f_desired_angle;		//calculate the error
					if (PidStruct_t.f_pid_error < f_best_error){  	//there was an improvement
						f_best_error = PidStruct_t.f_pid_error;				//actualize the best error value
						potencial_adj[i_index] *= 1.05f;		//calculate the new improvement
					}
					else{  //There was no improvement
						PidStruct_t.f_pid_gain[i_index] += potencial_adj[i_index];
						//As there was no improvement, the step size in either
						//direction, the step size might simply be too big.
						potencial_adj[i_index] *= 0.95f;
					}
					
					//goes to the next gain to be actualized
					i_index == 2 ? (i_index = 0) : (i_index++);		//actualize the index to pass to the next one	
				} else i_pid_tunned = 1; //the system is tunned, is declared as that
			}//end system no tunned
		}//end for
}//end behaviour function
