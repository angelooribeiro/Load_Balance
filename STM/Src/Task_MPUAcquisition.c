#include "stm32f4xx_conf.h"
#include "arm_math.h"

/*############################################################################*/
/*missing assert the gyroscope sensability (why not accelarometer also?) and the offset bias of the sensor*/

float32_t acc_values[20] = {0};
float32_t weight = 0.96;   //0.986
float32_t previous_filtered_angle = 0;
float32_t gyroscale = 250;


/*behaviour of task responsible for acquisition from the MPU6050*/
void vTaskMpuAcquisition(void* args){
	MPU6050_t MPU_Read;
	portTickType xLastWakeTime;		//nedded for the integration
	xLastWakeTime = xTaskGetTickCount();
	float32_t pitch = 0;
	float32_t filtered_angle = 0;
	float32_t f_readed_values[10] = {0.0f};
	int8_t index = 0;
	
	for(;;)	{
		//Start a conversion
		while(MPU6050_ReadAll() == 0);
		//waits a message after success 
		
		//wait that the read is complete 
		while(xQueueReceive(xQueue_AcquisitionToAngle,( void * )&MPU_Read, my_port_delay) != pdTRUE) {};
	
		//The sensor doesn't apresent gaps in the range for the project, so there is no need for this step
		//Offset bias
		/*	
		MPU_Read.Gyroscope_X += 0; 
		MPU_Read.Accelerometer_X += 0;
		MPU_Read.Accelerometer_Y += 0;
		MPU_Read.Accelerometer_Z += 0;
		*/
			
		//calculate pitch accelaration => X
		pitch = (float32_t) atan2f(MPU_Read.Accelerometer_X, sqrtf( ( powf(MPU_Read.Accelerometer_Y, 2.f) + powf(MPU_Read.Accelerometer_Z, 2.f)))) ;
		//convert to degrees
		pitch = (180/PI) * pitch;
		float32_t gyro = (float32_t) (MPU_Read.Gyroscope_X / gyroscale);
		//complementary filter
		filtered_angle = weight * (previous_filtered_angle + MPU_Read.Gyroscope_X * 0.001f) + (1.0f - weight) * (pitch); 
		//save converted value
		previous_filtered_angle = filtered_angle;
		
		f_readed_values[index] = filtered_angle;
		
		(index==10) ? (index=0) : (index++);

		for(int i = 0; i< 10; i++)
			filtered_angle += f_readed_values[i];
			filtered_angle /= 10;
			//arm_mean_f32(f_readed_values, 9, &filtered_angle);
			//write to the queue to be used in the PID
			xQueueOverwrite( xQueue_AngleToPid, &filtered_angle);
		//This task should execute with a fixed interval of 1 ms => 1KHz
		vTaskDelayUntil( &xLastWakeTime, ( 1 /	 portTICK_PERIOD_MS ) );	
	}
}
