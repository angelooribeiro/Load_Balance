#ifndef HCSR04SENSOR_H
#define HCSR04SENSOR_H

//SR04 defines
#define SR04_GPIO 		GPIOA
#define ECHO_PIN 			GPIO_Pin_6
#define TRIG_PIN 			GPIO_Pin_7

//the timer just work for one period
#define TIMER_PERIOD_SR04 65535
#define TIMER_PRESCALER_SR04  640

#define SR04_Timer 		TIM3

#define SR04_EchoLine 	EXTI_Line6
#define SR04_EchoIRQ 		EXTI9_5_IRQn
#define SR04_TIMIRQ 		TIM3_IRQn

#define FallingEdge 0
#define RisingEdge 1

void vInitHcsr04(void);		/*peripheral initialization*/
void SR04_SendPulse(void);
void vInitTim3(void);
#endif


