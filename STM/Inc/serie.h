#ifndef SERIE_H
#define SERIE_H

#define BAUDRATE 9600

#ifdef _serie_
  #define ILEN 256
  uint8_t inbuf[ILEN];
  uint8_t istart, iend;

  #define OLEN 256
  uint8_t ostart, oend;
  uint8_t outbuf[OLEN];
  
  uint8_t sendfull;
  uint8_t sendactive;
#endif

uint8_t RxBufferNotEmpty(void);
void InitUSART(uint32_t baudrate);
void Init_var(void);
void Manual_Inbuff_Fill(char* cmd);
void Clear_Buff(uint8_t* buff);
uint8_t CheckBufferNextCh(void);
void USART_puts(USART_TypeDef* USARTx, volatile char *s);
#endif
