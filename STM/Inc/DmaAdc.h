#ifndef DMAADC_H
#define DMAADC_H

#include "stm32f4xx.h"
#include <stdlib.h>

#define BUFFERSIZE 255

extern uint16_t data_buffer[BUFFERSIZE];
extern uint8_t size_buffer;
extern uint8_t buffer_type;
extern uint8_t received_ch;
extern uint8_t half_transfer_ch;

typedef struct 
{
  uint8_t buffersize;
  uint8_t buffertype : 1;
}DMA_Attach;

void Init_DMA(void);

#endif
