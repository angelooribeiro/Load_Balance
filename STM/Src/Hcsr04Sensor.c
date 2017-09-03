#include "stm32f4xx_conf.h"

/*
@verbatim
HCSR04		STM32F4xx	Descrption
 
ECHO 		PA6
TRIG 		PA7
VCC			5V
GND			GND
*/

/*Private Variables--------------------------------------------------*/
volatile uint8_t InterruptEdge = 3;

/*Private Functions--------------------------------------------------*/

//int the timer that counts the value in high
void vInitTim3(void)
{
		//Enable clock for timer4 42MHz
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

		//Instanceate Struct to initialize the timer
		TIM_TimeBaseInitTypeDef TIM_InitStruct_t;
		TIM_InitStruct_t.TIM_ClockDivision = TIM_CKD_DIV1;
		TIM_InitStruct_t.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_InitStruct_t.TIM_Period = TIMER_PERIOD_SR04;
		TIM_InitStruct_t.TIM_Prescaler = TIMER_PRESCALER_SR04;
		//Init the timer with the struct
		TIM_TimeBaseInit(TIM3, &TIM_InitStruct_t);
	
		//the start of the timer must be done when nedded
		TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
			
		//struct no itialize the interrupt
		NVIC_InitTypeDef NVICInit_t;
		//Enable the EXTI interrupt for the timer
		NVICInit_t.NVIC_IRQChannel = TIM3_IRQn;
		NVICInit_t.NVIC_IRQChannelCmd = ENABLE;
		NVICInit_t.NVIC_IRQChannelPreemptionPriority = configMAX_SYSCALL_INTERRUPT_PRIORITY + 10; //lower prioirty -> higher numeric value;
		//NVICInit_t.NVIC_IRQChannelSubPriority = 0x00;
		NVIC_Init(&NVICInit_t);
}


//sends a pulse and initialize the timer3 to count the time in high
void SR04_SendPulse(void){
	//Ensure that the timer is initially disabled
	TIM_Cmd(SR04_Timer, DISABLE);
	//Set the timer count to 0
	TIM_SetCounter(SR04_Timer, 0);
	//the next changing on state will be rising
	InterruptEdge = RisingEdge;

	//Set the trigger pin high
	GPIO_SetBits(SR04_GPIO, TRIG_PIN);

	//send the pulse
	vTaskDelay(pdMS_TO_TICKS(1));
	
	//After 10us has passed => Trig pin low
	GPIO_ResetBits(SR04_GPIO, TRIG_PIN);

}



//Response to external interrup from line 5 to 9
void EXTI9_5_IRQHandler(void){
	
	//If the interrupt has been triggered
	if(EXTI_GetITStatus(EXTI_Line6) == SET){
		//Clear the interrupt pending bit
		EXTI_ClearITPendingBit(EXTI_Line6);
		
		uint8_t iDetectStatus = 0;		//to indicate the not detection
		static portBASE_TYPE xHigherPriorityTaskWoken;
		xHigherPriorityTaskWoken = pdFALSE;  //do not woken a task at the start of the ISR
		
		//if is a falling edge => the pulse end
		if(InterruptEdge == FallingEdge){
			//calculate the dustance
			TIM_Cmd(SR04_Timer, DISABLE);		//disable the timer
			uint16_t timer_value = TIM_GetCounter(TIM3);
			//test the value directly will increase the speed of execution
			//uint16_t distance_cm = (uint16_t)((float16_t)timer_value*(float16_t)(0.1234));
			

			if(	( ( timer_value >= 0x3E) && (timer_value <= 0x5A) ) ){ //detects
				iDetectStatus = 1;
				//Save data on the Queue and overwrite it
				xQueueOverwriteFromISR(xQueue_Detection,(void*)&iDetectStatus,&xHigherPriorityTaskWoken);
			}
			if( ( (timer_value < 0x3E) || (timer_value > 0x5A) ) ) {
				//distance > that the desired
				xQueueOverwriteFromISR(xQueue_Detection,(void*)&iDetectStatus,&xHigherPriorityTaskWoken);
			}
		}
		
		//if is a rising edge => the pulse start
		if(InterruptEdge == RisingEdge){
		//set the timer to 0 and enable timer
			TIM_Cmd(SR04_Timer, ENABLE);		//enable the timer to start counting
			InterruptEdge = FallingEdge;		//actualize state
		}
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken); //do not statrt a context switching
	}
}


//If the timer gives an overflow means that no object is in front
void TIM3_IRQHandler(void){
	
	//Timer overflowed => Out of Range
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET){
		static portBASE_TYPE xHigherPriorityTaskWoken;
		xHigherPriorityTaskWoken = pdFALSE;  //do not woken a task at the start of the ISR
		//Clear the pending interrupt bit
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		TIM_Cmd(TIM3, DISABLE);		//disable the timer
		
		uint8_t iDetectStatus = 0;		//to indicate the not detection

		//Save data on the Queue and overwrite it
		xQueueOverwriteFromISR(xQueue_Detection,(void*)&iDetectStatus,&xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken); //do not statrt a context switching
	}
}

//Init the external interrupt
void vInitHcsr04(void)
{
	GPIO_InitTypeDef GPIOInitStruct_t;
	//Enable clocks to GPIO and Timer 3(16 bits) (42MHz max freq)
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	//struct no itialize the interrupt
	NVIC_InitTypeDef NVICInit_t;
	
	//Configure Trig pin as output
	GPIOInitStruct_t.GPIO_Pin = TRIG_PIN;
	GPIOInitStruct_t.GPIO_OType = GPIO_OType_PP;
	GPIOInitStruct_t.GPIO_Mode = GPIO_Mode_OUT;
	GPIOInitStruct_t.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SR04_GPIO, &GPIOInitStruct_t);

	//Configure Echo pin as input
	GPIOInitStruct_t.GPIO_Pin = ECHO_PIN;
	GPIOInitStruct_t.GPIO_PuPd = GPIO_PuPd_UP;
	GPIOInitStruct_t.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(SR04_GPIO, &GPIOInitStruct_t);

	//tell the system that this port is associated with this pin
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource6);
	//Interrupt Enabled for ECHO, Rissing Falling to mesure the pulse
	EXTI_InitTypeDef EXTIInitStruct_t;
	EXTIInitStruct_t.EXTI_Line = EXTI_Line6;
	EXTIInitStruct_t.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTIInitStruct_t.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTIInitStruct_t.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTIInitStruct_t);

	//Enable EXTI for external interrupt
	NVICInit_t.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVICInit_t.NVIC_IRQChannelPreemptionPriority = configMAX_SYSCALL_INTERRUPT_PRIORITY + 10; //lower prioirty -> higher numeric value
	//NVICInit_t.NVIC_IRQChannelSubPriority = 0; //not necessary
	NVICInit_t.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVICInit_t);

}

