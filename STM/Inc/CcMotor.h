#ifndef CCMOTOR_H 
#define CCMOTOR_H

/*
@verbatim
CcMotor	Descrption
 
APB1 42MHz
Driver 20Kz
PWM port: PD12
*/

#define TIMER_PERIOD_CC 		49
#define TIMER_PRESCALER_CC  42

/*GPIO PORT Define*/
#define GPIO_TIM4	GPIOD 

/*Pins Define*/
#define CC_M_PIN GPIO_Pin_12


void vInitPwm4(void);			//initialization of pwm for the wheels
void vChangeVelocity(uint16_t velocity); //to change the velocity of the weels

#endif

