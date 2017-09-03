#define _serie_

#include "stm32f4xx_conf.h"


//USART 1 Pinout
//Tx = PB6
//Rx = PB7
//


void InitUSART(uint32_t baudrate)	
{
	//Struct Defenitions
	GPIO_InitTypeDef GPIO_InitStruct_t;
	USART_InitTypeDef USART_InitStruc_t;
  NVIC_InitTypeDef NVIC_InitStruct_t;
	
	//Enable Clocks
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	//Initialize GPIO
	//GPIO 6 Tx ; GPIO 7 Rx 
	GPIO_InitStruct_t.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStruct_t.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct_t.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct_t.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct_t.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init(GPIOB, &GPIO_InitStruct_t);

	//Assign the GPIOS to the usart
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);
	
	//Initialization of the USART
	USART_InitStruc_t.USART_BaudRate = baudrate;//Baudrate of the ESP
	USART_InitStruc_t.USART_WordLength = USART_WordLength_8b;
	USART_InitStruc_t.USART_StopBits = USART_StopBits_1;
	USART_InitStruc_t.USART_Parity = USART_Parity_No;
	USART_InitStruc_t.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//no control
	USART_InitStruc_t.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;			//enable transmission and reception
	USART_Init(USART1, &USART_InitStruc_t);
	
	//Enable Innterrupt
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	//Configure Interrupt
	NVIC_InitStruct_t.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct_t.NVIC_IRQChannelPreemptionPriority = configMAX_SYSCALL_INTERRUPT_PRIORITY + 10; //lower prioirty -> higher numeric value;
	//NVIC_InitStruct_t.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct_t.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct_t);
	
	//Start executing the Interrupt
	USART_Cmd(USART1, ENABLE);
	//to enable
	//USART2->CR1 |= USART_CR1_UE;

}

void USART1_IRQHandler(void)
{
	static portBASE_TYPE xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;  //do not woken a task at the start of the ISR
	char aux[2] = {0};
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){
    char read_caracter = USART_ReceiveData(USART1);
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    if(istart + ILEN != iend)
      inbuf[iend++ & (ILEN-1)] = read_caracter;
			//mean that a receiving communication 
		if( ((inbuf[(iend-3) & (ILEN-1)] == '!') && (inbuf[(iend-2) & (ILEN-1)] == 'C') && 
			((inbuf[(iend-1) & (ILEN-1)] == 'G') ||	(inbuf[(iend-1) & (ILEN-1)] == 'D')) ) ){
				aux[0] = inbuf[(iend-2) & (ILEN-1)];
				aux[1] = inbuf[(iend-1) & (ILEN-1)];
				xQueueOverwrite(xQueue_WifiReception,&aux);		
		}
		if( ( (inbuf[(iend-3) & (ILEN-1)] == '!') && (inbuf[(iend-2) & (ILEN-1)] == 'S') && 
				((inbuf[(iend-1) & (ILEN-1)] == 'T') || (inbuf[(iend-1) & (ILEN-1)] == 'P')) ) ){
				aux[0] = inbuf[(iend-2) & (ILEN-1)];
				aux[1] = inbuf[(iend-1) & (ILEN-1)];
				xQueueOverwrite(xQueue_WifiReception,&aux);
		}
	}
  
  if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET){
    USART_ClearITPendingBit(USART1, USART_IT_TXE);
    if(ostart != oend){
      USART_SendData(USART1, outbuf[ostart++ & (OLEN-1)]);
      sendfull = 0;
    }
    else{
      sendactive = 0;
      USART_ClearITPendingBit(USART1, USART_IT_TXE);
      USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
    }
  }
	 portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void Init_var(void)
{
  istart = 0;
  iend = 0;
  ostart = 0;
  oend = 0;
  sendactive = 0;
  sendfull = 0;
}

uint8_t RxBufferNotEmpty(void)
{
  return (iend != istart);
}

int fputc(int ch, FILE *f)
{
  while(sendfull);
  
  if(!sendfull){
    if(!sendactive){
      sendactive = 1;
      USART_SendData(USART1, (uint16_t) ch);
      USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
    }
    else{
      USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
      outbuf[oend++ & (OLEN-1)] = ch;
      if(((oend ^ ostart) & (OLEN-1)) == 0)
        sendfull = 1;
      USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
    }
  }
  return ch;
}

int fgetc(FILE *f)
{
  char ch;
  
  while(iend == istart);
  USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
  ch = inbuf[istart++ & (ILEN-1)];
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  
  if( (ch == '\r') || (ch == '\n')) 
    return EOF;
  else
    return ((int)ch);
}

void Manual_Inbuff_Fill(char* cmd)
{
  char i;
  
  Init_var();
  iend = strlen(cmd);
  
  Clear_Buff(inbuf);
  
  for(i = 0; i<ILEN; i++)
    inbuf[i] = *cmd++;
  return;
}

void Clear_Buff(uint8_t* buff)
{
  while(*buff)
    *buff++ = 0;
  return;
}

uint8_t CheckBufferNextCh(void)
{
  uint8_t pre_iend = iend;
 
  if(inbuf[iend - 1] == '\n') return 0;
  
  while(pre_iend == iend); //wait for the next character arrival
  
  if(('A' - inbuf[iend - 1]) > 0) return 1; //receive numeric char or (-) in this program
  return 2; //receive alfa ch
}


void USART_puts(USART_TypeDef* USARTx, volatile char *s){

	while(*s){
		// wait until data register is empty
		while( !(USARTx->SR & 0x00000040) ); 
		USART_SendData(USARTx, *s);
		*s++;
	}
}

