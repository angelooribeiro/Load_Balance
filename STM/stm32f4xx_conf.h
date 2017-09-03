/**
  ******************************************************************************
  * @file    ADC/ADC_DualModeInterleaved/stm32f4xx_conf.h  
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    19-September-2013
  * @brief   Library configuration file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4xx_CONF_H
#define __STM32F4xx_CONF_H

/* Includes ------------------------------------------------------------------*/
/* Uncomment the line below to enable peripheral header file inclusion */
#include "stm32f4xx_adc.h"
#include "stm32f4xx_crc.h"
#include "stm32f4xx_dbgmcu.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_flash.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_iwdg.h"
#include "stm32f4xx_pwr.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_rtc.h"
#include "stm32f4xx_sdio.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_wwdg.h"
#include "misc.h" /* High level functions for NVIC and SysTick (add-on to CMSIS functions) */

#if defined (STM32F429_439xx)
#include "stm32f4xx_cryp.h"
#include "stm32f4xx_hash.h"
#include "stm32f4xx_rng.h"
#include "stm32f4xx_can.h"
#include "stm32f4xx_dac.h"
#include "stm32f4xx_dcmi.h"
#include "stm32f4xx_dma2d.h"
#include "stm32f4xx_fmc.h"
#include "stm32f4xx_ltdc.h"
#include "stm32f4xx_sai.h"
#endif /* STM32F429_439xx */

#if defined (STM32F427_437xx)
#include "stm32f4xx_cryp.h"
#include "stm32f4xx_hash.h"
#include "stm32f4xx_rng.h"
#include "stm32f4xx_can.h"
#include "stm32f4xx_dac.h"
#include "stm32f4xx_dcmi.h"
#include "stm32f4xx_dma2d.h"
#include "stm32f4xx_fmc.h"
#include "stm32f4xx_sai.h"
#endif /* STM32F427_437xx */

#if defined (STM32F40_41xxx)
#include "stm32f4xx_cryp.h"
#include "stm32f4xx_hash.h"
#include "stm32f4xx_rng.h"
#include "stm32f4xx_can.h"
#include "stm32f4xx_dac.h"
#include "stm32f4xx_dcmi.h"
#include "stm32f4xx_fsmc.h"
#endif /* STM32F40_41xxx */

/* Free RTOS Includes ------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* User Includes ------------------------------------------------------------*/
#include "Hcsr04Sensor.h"
#include "ServoMotor.h"
#include "CcMotor.h"
#include "MPU6050.h"
#include "ESP8266.h"
#include "AdcBatteries.h"
#include "I2C.h"
#include <stdlib.h>
#include <stdio.h>
#include "serie.h"

/* DSP includes -------------------------------------------------------------------*/
#include "arm_math.h"

/* Synchronization objects --------------------------------------------------------*/
extern xQueueHandle xQueue_AcquisitionToAngle;
extern xQueueHandle xQueue_AngleToPid;
extern xQueueHandle xQueue_PidToCommunication;
extern xQueueHandle xQueue_Detection;
extern xQueueHandle xQueue_Communication;
extern xQueueHandle xQueue_WifiReception;
extern SemaphoreHandle_t xSemaphore_Grab;
extern SemaphoreHandle_t xSemaphore_Reception;

/*Handles used in diferent tasks----------------------------------------------------*/
extern TaskHandle_t xTask_BalanceCharge;
extern TaskHandle_t xTask_MPUAcquisition;
extern TaskHandle_t xTask_GrabCharge;
extern TaskHandle_t xTask_DropCharge;
extern TaskHandle_t xTask_Communication;

/*Taks------------------------------------------------------------------------------*/
extern void vTaskMpuAcquisition(void* args);
extern void vTaskBalanceCharge(void* args);
extern void vTaskCommunication(void *args);
extern void vTaskGrabCharge(void *args);
extern void vTaskDropCharge(void *args);

/*Global Variables------------------------------------------------------------------*/
extern int8_t i_pid_tunned; 
extern uint32_t my_port_delay;
/*Struct used to pass the valuesof the PID on the Queue*/
typedef struct {
	float32_t f_pid_error;				//diference between the actual angle and the desired
	float32_t f_current_angle;		//to store the angle taht were reat
	float32_t f_pid_gain[3];			//array with the gain of the pid
	float32_t f_duty;							//dutycle that result from the pid calculation
} Pid_t;


/* If an external clock source is used, then the value of the following define 
   should be set to the value of the external clock source, else, if no external 
   clock is used, keep this define commented */
/*#define I2S_EXTERNAL_CLOCK_VAL   12288000 */ /* Value of the external clock in Hz */


/* Uncomment the line below to expanse the "assert_param" macro in the 
   Standard Peripheral Library drivers code */
// #define USE_FULL_ASSERT    1 

/* Exported macro ------------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT

/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function
  *   which reports the name of the source file and the source
  *   line number of the call that failed. 
  *   If expr is true, it returns no value.
  * @retval None
  */
  #define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
  void assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */

#endif /* __STM32F4xx_CONF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
