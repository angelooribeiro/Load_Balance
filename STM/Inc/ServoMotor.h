#ifndef SERVOMOTOR_H
#define SERVOMOTOR_H

#include "stm32f4xx_conf.h"

#define TIMER_PERIOD 		19999
#define TIMER_PRESCALER	84

#define MAX_DUTY_CYCLE 3000
#define MIN_DUTY_CYCLE 600

/*GPIO PORT Define*/
#define GPIO_TIM1	GPIOE 
#define GPIO_TIM8 GPIOC

/*Pins Define*/
#define ARM_M1_PIN GPIO_Pin_9
#define ARM_M2_PIN GPIO_Pin_11
#define ARM_M3_PIN GPIO_Pin_13
#define ARM_M4_PIN GPIO_Pin_14
#define CLAWM_PIN  GPIO_Pin_8

/*Arms motors Start Position*/
#define SP_M1						140
#define SP_M2						45
#define SP_M3						120
#define SP_M4						160
#define SP_CLAWM_OPEN		40  

/*Arms motors Limits Position*/
#define UP_LIMIT_M1			100
#define DOWN_LIMIT_M1		180
#define UP_LIMIT_M2			70
#define DOWN_LIMIT_M2		25
#define UP_LIMIT_M3			50
#define DOWN_LIMIT_M3		150
#define SP_CLAWM_MID 		75	
#define SP_CLAWM_CLOSE 	110	

//to chose the specific chanel
typedef enum {
	Pwm_ARMM1 = 0x00, /* Operate with channel 1 */
	Pwm_ARMM2 = 0x01, /* Operate with channel 2 */
	Pwm_ARMM3 = 0x02, /* Operate with channel 3 */
	Pwm_ARMM4 = 0x03,  /* Operate with channel 4 */
	Pwm_CLAWM = 0x04,	/* Operate with channel 1 */
} Pwm_Channel_t;

void vInitArm(void); //Start position and Init the PWMs
void vDeInitArm(void); //DeInit the arm and go to a save position
void vChangeAngle(int16_t degrees, Pwm_Channel_t channel);
void vStartPos(void);
	
#endif 
