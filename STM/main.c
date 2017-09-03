#include "stm32f4xx_conf.h"

//Gloabal Variable max ticks of clock to wait
TickType_t my_port_delay = pdMS_TO_TICKS(10);		//wait 10ms

//Global synchornization objects
xQueueHandle xQueue_AcquisitionToAngle;
xQueueHandle xQueue_AngleToPid;
xQueueHandle xQueue_PidToCommunication;
xQueueHandle xQueue_Detection;
xQueueHandle xQueue_Communication;
xQueueHandle xQueue_WifiReception;
SemaphoreHandle_t xSemaphore_Grab;

//Gloabl TaskHandle
TaskHandle_t xTask_BalanceCharge;
TaskHandle_t xTask_MPUAcquisition;
TaskHandle_t xTask_GrabCharge;
TaskHandle_t xTask_DropCharge;
TaskHandle_t xTask_Communication;

int main(){
	
	//set the priority group	
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_4);
	
	vInitArm();			//Should be hear to give time to the arm to initialize
	//Synchornization objects creation
	//Data from Acquisition to the Angle calculation
	xQueue_AcquisitionToAngle = xQueueCreate(1, sizeof(MPU6050_t));
	//angle that goes to the PID control
	xQueue_AngleToPid = xQueueCreate(1, sizeof(float32_t));
	//passes the PID info for the communication queue
	xQueue_PidToCommunication = xQueueCreate( 10, sizeof(Pid_t));
	//passes the PWM info for the communication queue
	xQueue_Detection = xQueueCreate(1, sizeof(uint8_t));
	//passes messages to the Communication
	xQueue_Communication = xQueueCreate(10, sizeof(char[10]));
	//To pass the comand from the Reception to the Task
	xQueue_WifiReception = xQueueCreate(1, sizeof(char[2]));
	//Indicates that the Grab is completed
	xSemaphore_Grab = xSemaphoreCreateBinary();
	
	//Peripherals initialization
	MPU6050_Init(MPU6050_Accelerometer_8G, MPU6050_Gyroscope_250s);
	vInitESP();
	vInitHcsr04();
	vInitPwm4();
	vInitTim3();
	
	//Tasks creation
	xTaskCreate( vTaskBalanceCharge, "BalCharge", configMINIMAL_STACK_SIZE, NULL, 4, &xTask_BalanceCharge);
	xTaskCreate( vTaskMpuAcquisition, "MPUAcq", configMINIMAL_STACK_SIZE, NULL, 3, &xTask_MPUAcquisition);
	xTaskCreate( vTaskCommunication, "Communication", configMINIMAL_STACK_SIZE, NULL, 2, &xTask_Communication);
	xTaskCreate( vTaskGrabCharge, "GrabCharge", configMINIMAL_STACK_SIZE, NULL, 1, &xTask_GrabCharge); 
	xTaskCreate( vTaskDropCharge, "DropCharge", configMINIMAL_STACK_SIZE, NULL, 1, &xTask_DropCharge);

	//Suspend all the tasks, they will be abilitated by the task Communication
	vTaskSuspend(xTask_MPUAcquisition);
	vTaskSuspend(xTask_BalanceCharge);
	vTaskSuspend(xTask_GrabCharge);
	vTaskSuspend(xTask_DropCharge);
	
	//Start the Scheduler
	vTaskStartScheduler();

	while(1);
}
