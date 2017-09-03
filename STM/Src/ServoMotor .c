#include "stm32f4xx_conf.h"

// current angle position of arm motors
int16_t M1_pos = SP_M1;
int16_t M2_pos = SP_M2;
int16_t M3_pos = SP_M3;
int16_t M4_pos = SP_M4;

/*Private functions-----------------------------------------------------*/
//TIM1
void vInitPwm1(void);	/*initialization of pwm in timer1*/
void vInitTim1(void);	/*initialization of timer1*/
//TIM8
void vInitPwm8(void);	/*initialization of pwm in timer8*/
void vInitTim8(void);	/*initialization of timer8*/
//Init the Channel to the intial position
void vInitChannelServo(int16_t degrees, Pwm_Channel_t channel);



void vInitArm(void){
		
	//int the pwms to the arm
	vInitPwm1();
	vInitPwm8();
		
	//take the arm to the initial position
	vInitChannelServo(140, Pwm_ARMM1);
	vInitChannelServo(45, Pwm_ARMM2);
	vInitChannelServo(120, Pwm_ARMM3);
	vInitChannelServo(160, Pwm_ARMM4);
	vInitChannelServo(40, Pwm_CLAWM); //Open clawm position
	
	//Start the timers
	TIM_Cmd(TIM1, ENABLE);
	TIM_Cmd(TIM8, ENABLE);
}

void vDeInitArm()
{	
	vChangeAngle(110, Pwm_ARMM1); // Base Motor
	for(int i = 0; i <10000000; i++);
	vChangeAngle(27, Pwm_ARMM2);	// Motor 2
	for(int i = 0; i <10000000; i++);
	vChangeAngle(115, Pwm_ARMM3); // Motor 3
	vChangeAngle(110, Pwm_CLAWM); 	// Motor Claw (START posiiton)
	for(int i = 0; i <10000000; i++);
	TIM_Cmd(TIM1, DISABLE);
	TIM_Cmd(TIM8, DISABLE);
	
}

void vStartPos()
{
	vChangeAngle(140, Pwm_ARMM1); // Base Motor
	vTaskDelay(pdMS_TO_TICKS(200));
	vChangeAngle(45, Pwm_ARMM2);	// Motor 2
	vTaskDelay(pdMS_TO_TICKS(200));
	vChangeAngle(120, Pwm_ARMM3); // Motor 3
	vChangeAngle(160, Pwm_ARMM4);	// Motor 4 (Just have one position)
	vTaskDelay(pdMS_TO_TICKS(200));
	vChangeAngle(SP_CLAWM_MID, Pwm_CLAWM); 	// Motor Claw (START posiiton)
}

/*##########################################       TIM1      #####################################*/
/*initialization of pwm in timer1*/
void vInitPwm1(void)
{
		//to initialize the GPIOs for PWM1
		GPIO_InitTypeDef GPIOInitStruct_t;
		
		//Initialize the timer1
		vInitTim1();
		
		//Enable the clock
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
		
    // Alternating functions for pins
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_TIM1);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_TIM1);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_TIM1);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_TIM1);

		//Enables PWM output
		TIM_CtrlPWMOutputs(TIM1, ENABLE);
    
    /* Set pins */
    GPIOInitStruct_t.GPIO_Pin |= ARM_M1_PIN; 
		GPIOInitStruct_t.GPIO_Pin |= ARM_M2_PIN;
		GPIOInitStruct_t.GPIO_Pin |= ARM_M3_PIN;
		GPIOInitStruct_t.GPIO_Pin	|= ARM_M4_PIN;
    GPIOInitStruct_t.GPIO_OType = GPIO_OType_PP;
    GPIOInitStruct_t.GPIO_PuPd = GPIO_PuPd_UP;
    GPIOInitStruct_t.GPIO_Mode = GPIO_Mode_AF;
    GPIOInitStruct_t.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIO_TIM1, &GPIOInitStruct_t);
}

/*initialization of timer1*/
void vInitTim1(void)
{
		//Enable clock for timer1 84MHz
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
		//Instanceate Struct to initialize the timer
		TIM_TimeBaseInitTypeDef TIM_InitStruct_t;
		TIM_InitStruct_t.TIM_ClockDivision = TIM_CKD_DIV1;
		TIM_InitStruct_t.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_InitStruct_t.TIM_Period = TIMER_PERIOD ;
		TIM_InitStruct_t.TIM_Prescaler = TIMER_PRESCALER;
		
		//Init the timer with the struct
		TIM_TimeBaseInit(TIM1, &TIM_InitStruct_t);
}


/*##########################################       TIM8      #####################################*/
void vInitPwm8(void)
{
			//to initialize the GPIOs for PWM1
		GPIO_InitTypeDef GPIOInitStruct_t;
		
		//Initialize the timer1
		vInitTim8();
		
		//Enable the clock
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		
    // Alternating functions for pins
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM8);
	
		//Enables PWM output
		TIM_CtrlPWMOutputs(TIM8, ENABLE);
    
    /* Set pins */
    GPIOInitStruct_t.GPIO_Pin = CLAWM_PIN; 
    GPIOInitStruct_t.GPIO_OType = GPIO_OType_PP;
    GPIOInitStruct_t.GPIO_PuPd = GPIO_PuPd_UP;
    GPIOInitStruct_t.GPIO_Mode = GPIO_Mode_AF;
    GPIOInitStruct_t.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIO_TIM8, &GPIOInitStruct_t);
}

/*initialization of timer8*/
void vInitTim8(void)
{
		//Enable clock for timer8 84MHz
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
	
		//Instanceate Struct to initialize the timer
		TIM_TimeBaseInitTypeDef TIM_InitStruct_t;
		TIM_InitStruct_t.TIM_ClockDivision = TIM_CKD_DIV1;
		TIM_InitStruct_t.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_InitStruct_t.TIM_Period = TIMER_PERIOD;
		TIM_InitStruct_t.TIM_Prescaler = TIMER_PRESCALER;
		
		//Init the timer with the struct
		TIM_TimeBaseInit(TIM8, &TIM_InitStruct_t);
	
}

/*###################################    General ################################# */

void vInitChannelServo(int16_t degrees, Pwm_Channel_t channel)
{		
		int32_t duty_cycle = 0;
		duty_cycle = 600+13.3333*(uint32_t)degrees;
		
	/*if threre is an error nothing is made*/
		if(duty_cycle < (int32_t)MIN_DUTY_CYCLE || duty_cycle > (int32_t)MAX_DUTY_CYCLE)
			return;

		//Intance Struct to initialize the output compare mode
		TIM_OCInitTypeDef TIM_OCInitStruct_t;
		TIM_OCInitStruct_t.TIM_OCMode = TIM_OCMode_PWM1;		//set pulse when CNT == CCRx
		TIM_OCInitStruct_t.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStruct_t.TIM_OutputNState = TIM_OutputNState_Enable;
		TIM_OCInitStruct_t.TIM_Pulse = duty_cycle; //value to be placed in the compare register
    TIM_OCInitStruct_t.TIM_OCPolarity = TIM_OCPolarity_High;		//active high 
	
		switch(channel){
			case Pwm_ARMM1:
				//configure the channel with the configuration
				TIM_OC1Init(TIM1, &TIM_OCInitStruct_t);
			break;
			
			case Pwm_ARMM2:
				//configure the channel with the configuration
				TIM_OC2Init(TIM1, &TIM_OCInitStruct_t);
			break;
			
			case Pwm_ARMM3:
				//configure the channel with the configuration
				TIM_OC3Init(TIM1, &TIM_OCInitStruct_t);
			break;
			
			case Pwm_ARMM4:
				//configure the channel with the configuration
				TIM_OC4Init(TIM1, &TIM_OCInitStruct_t);
			break;
			
			case Pwm_CLAWM:
				//configure the channel with the configuration
				TIM_OC3Init(TIM8, &TIM_OCInitStruct_t);	 //-->> COMENTADO !!!!!!!**********************
			break;
			
			default:
			break;
		}
}

void vChangeAngle(int16_t degrees, Pwm_Channel_t channel)
{
		int32_t duty_cycle = 0;
		duty_cycle = 600+13.3333*degrees;
		
	/*if threre is an error nothing is made*/
		if(duty_cycle < (int32_t)MIN_DUTY_CYCLE || duty_cycle > (int32_t)MAX_DUTY_CYCLE)
			return;
	
		switch(channel){
			case Pwm_ARMM1:
				//while(MP1_pos <= degrees){
					TIM1->CCR1 = duty_cycle;
				//	M1_pos = degrees;
				//}		
			break;
			
			case Pwm_ARMM2:
				TIM1->CCR2 = duty_cycle;
				//M2_pos= degrees;
			break;
			
			case Pwm_ARMM3:
				TIM1->CCR3 = duty_cycle;
				//M3_pos= degrees;
			break;
			
			case Pwm_ARMM4:
				TIM1->CCR4 = duty_cycle;
				//M4_pos= degrees;
			break;
			
			case Pwm_CLAWM:
				TIM8->CCR3 = duty_cycle;
			break;
			
			default:
			break;
		}
}
