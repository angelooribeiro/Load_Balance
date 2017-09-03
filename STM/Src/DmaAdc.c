#include "DmaAdc.h"
#include "CcMotor.h"
uint16_t data_buffer[BUFFERSIZE] = {0};

uint8_t size_buffer = 0;
uint8_t buffer_type = 0;

void Init_DMA(void)
{
  DMA_InitTypeDef DMA_InitStruct;
  NVIC_InitTypeDef NVIC_InitStruct;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
  
  
  DMA_DeInit(DMA2_Stream2);
  
  //DMA->buffertype == 0 ? (DMA_InitStruct.DMA_Mode = DMA_Mode_Normal) : (DMA_InitStruct.DMA_Mode = DMA_Mode_Circular);
  
  DMA_InitStruct.DMA_Channel = DMA_Channel_1;
  DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
  DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)&data_buffer;
  DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStruct.DMA_BufferSize = 1;
  DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStruct.DMA_Priority = DMA_Priority_High;
  
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
	
  DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream2, &DMA_InitStruct);
  
  DMA_Cmd(DMA2_Stream2, ENABLE);
  
  DMA_ITConfig(DMA2_Stream2, DMA_IT_TC | DMA_IT_HT, ENABLE);  
  
  NVIC_InitStruct.NVIC_IRQChannel = DMA2_Stream2_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);
  
//  free(DMA);
}


