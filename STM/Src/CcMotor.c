#include "stm32f4xx_conf.h"

//private functions
void vInitTim4(void);			//initialization of timer1
void vInitChannelCc(uint16_t velocity);	//initialization of the channel


//initialization of pwm in timer4
void vInitPwm4(void)
{
	//to initialize the GPIOs for PWM4
		GPIO_InitTypeDef GPIOInitStruct_t;
		
		//Initialize the timer4
		vInitTim4();
		
		//Enable the clock
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
		
    // Alternating functions for the pin
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
		
		/*The initialization is made on a stop state*/
		vInitChannelCc(0);
	
		//Enables PWM output
		TIM_CtrlPWMOutputs(TIM4, ENABLE);
    
    /* Set pins */
    GPIOInitStruct_t.GPIO_Pin = CC_M_PIN; 
    GPIOInitStruct_t.GPIO_OType = GPIO_OType_PP;
    GPIOInitStruct_t.GPIO_PuPd = GPIO_PuPd_UP;
    GPIOInitStruct_t.GPIO_Mode = GPIO_Mode_AF;
    GPIOInitStruct_t.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIO_TIM4, &GPIOInitStruct_t);
}

//initialization of timer1
void vInitTim4(void)
{
		//Enable clock for timer4 42MHz
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
		//Instanceate Struct to initialize the timer
		TIM_TimeBaseInitTypeDef TIM_InitStruct_t;
		TIM_InitStruct_t.TIM_ClockDivision = TIM_CKD_DIV1;
		TIM_InitStruct_t.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_InitStruct_t.TIM_Period = TIMER_PERIOD_CC;
		TIM_InitStruct_t.TIM_Prescaler = TIMER_PRESCALER_CC;
		
		//Init the timer with the struct
		TIM_TimeBaseInit(TIM4, &TIM_InitStruct_t);
	
		//Start Timer
		TIM_Cmd(TIM4, ENABLE);
}

//initialization of the channel
void vInitChannelCc(uint16_t velocity)
{
		//Intance Struct to initialize the output compare mode
		TIM_OCInitTypeDef TIM_OCInitStruct_t;
		TIM_OCInitStruct_t.TIM_OCMode = TIM_OCMode_PWM1;		//set pulse when CNT == CCRx
		TIM_OCInitStruct_t.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStruct_t.TIM_OutputNState = TIM_OutputNState_Enable;
		TIM_OCInitStruct_t.TIM_Pulse = velocity; //value to be placed in the compare register
    TIM_OCInitStruct_t.TIM_OCPolarity = TIM_OCPolarity_High;		//active high 

		//configure the channel with the configuration
		TIM_OC1Init(TIM4, &TIM_OCInitStruct_t);
}

//to change the velocity of the weels
void vChangeVelocity(uint16_t velocity) 
{
	TIM4->CCR1 = velocity;
}
