#include "stm32f4xx_conf.h"


uint8_t ADC1Value[10] = {0};

/*Pins
	PC0
	PC1
	PC2
*/


//Private Function
void vInitDmaAdc(void);


//Initialization fo the ADC1
void vInitADC(void)
{
	//GPIO Structure
	GPIO_InitTypeDef GPIO_InitStruct_t;
	//Common ADC configuration
	ADC_CommonInitTypeDef ADC_CommonInitStruct_t;
	//ADC Structure
	ADC_InitTypeDef ADC_InitStruct_t;
	
	/* Enable GPIOA's AHB interface clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);								//Initialize the clock
	GPIO_InitStruct_t.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;	//PC0, PC1, PC2, PC3
	GPIO_InitStruct_t.GPIO_Mode = GPIO_Mode_AIN;												//Analog In
	GPIO_InitStruct_t.GPIO_PuPd = GPIO_PuPd_NOPULL;											//No pull resistors
	GPIO_Init(GPIOA, &GPIO_InitStruct_t);																//Init the peripheral
	
	//Initialize the ADC clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	//common configuration for all the ADCs
	ADC_CommonInitStruct_t.ADC_Mode = ADC_Mode_Independent;	//single adc mode
	ADC_CommonInitStruct_t.ADC_Prescaler = ADC_Prescaler_Div2;	//addcclk = pclk2/2
	ADC_CommonInitStruct_t.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //availabe only for multi adc mode
	ADC_CommonInitStruct_t.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles; //delay between tow samples
	//itialization of the common adc features
	ADC_CommonInit(&ADC_CommonInitStruct_t);
	
	//ADC initialization
  ADC_InitStruct_t.ADC_Resolution = ADC_Resolution_8b;			//8bits resolution
  ADC_InitStruct_t.ADC_ScanConvMode = DISABLE;							//multi channel
  ADC_InitStruct_t.ADC_ContinuousConvMode = ENABLE;					//sigle mode conversion
	ADC_InitStruct_t.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	//ADC_InitStruct_t.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;	//no trigger to conversion	must be assigned a tomer
  ADC_InitStruct_t.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStruct_t.ADC_NbrOfConversion = 1;
	//initialization of the adc
	ADC_Init(ADC1, &ADC_InitStruct_t);
	
	//configure the adc1 with the regular chanells 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_3Cycles);
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_3Cycles);
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_3Cycles);
	
	//Enable DMA request after last transfer (single adc mode)
	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
	
	//Enable ADC1 DMA interface
	ADC_DMACmd(ADC1, ENABLE);
	
	//Enable ADC1
	ADC_Cmd(ADC1, ENABLE);
	
	//start ADC1 software conversion
	ADC_SoftwareStartConv(ADC1);
	
	//Init the DMA
	vInitDmaAdc();
	
}

void vInitDmaAdc(void)
{
	
	DMA_InitTypeDef DMA_InitStruct_t;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	/* Enable DMA2's AHB1 interface clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	/* Configure DMA2 Stream0 channel0 to transfer, in circular mode,
	the converted data from ADC1 DR register to the
	ADCConvertedValue variable */
	DMA_InitStruct_t.DMA_Channel = DMA_Channel_0;
	DMA_InitStruct_t.DMA_PeripheralBaseAddr = ADC1_DR_ADDRESS;
	DMA_InitStruct_t.DMA_Memory0BaseAddr = (uint32_t)&ADC1Value;
	DMA_InitStruct_t.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStruct_t.DMA_BufferSize = 1;
	DMA_InitStruct_t.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStruct_t.DMA_MemoryInc = DMA_MemoryInc_Disable;
	DMA_InitStruct_t.DMA_PeripheralDataSize =
	DMA_PeripheralDataSize_HalfWord;
	DMA_InitStruct_t.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStruct_t.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStruct_t.DMA_Priority = DMA_Priority_High;
	DMA_InitStruct_t.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStruct_t.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStruct_t.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStruct_t.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream0, &DMA_InitStruct_t);
	/* Enable DMA2 Stream0 */
	DMA_Cmd(DMA2_Stream0, ENABLE);
		
	DMA_ITConfig(DMA2_Stream0, DMA_IT_TC | DMA_IT_HT, ENABLE);  
  
  NVIC_InitStruct.NVIC_IRQChannel = DMA2_Stream0_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);
}

void DMA2_Stream0_IRQHandler(void)
{
	//printf("%d", ADC1Value);
	//PC0
	if(ADC1Value[0] == 0x00)
		vChangeVelocity(0);
	//PC1
	if(ADC1Value[1] == 0x00)
		vChangeVelocity(0);
	//PC2
	if(ADC1Value[2] == 0x00)
		vChangeVelocity(0);
}

