/*this module is done to aberviate the use of the i2c peripheral drive*/
#ifndef I2C_H
#define I2C_H


#include "stm32f4xx.h"


#define I2C_TIMEOUT					20000


/* Private defines */
#define I2C_TRANSMITTER_MODE   0
#define I2C_RECEIVER_MODE      1
#define I2C_ACK_ENABLE         1
#define I2C_ACK_DISABLE        0


//to read a single byte from the slave
uint8_t I2CRead(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg);
//Reads multiple bytes from slave
void I2CReadMulti(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg, uint8_t *data, uint16_t count);
//Writes single byte to slave
void I2CWrite(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg, uint8_t data);
//Writes multi bytes to slave
void I2CWriteMulti(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg, uint8_t *data, uint16_t count);
//test if the divice is connected returns 0 if not
uint8_t I2CIsDeviceConnected(I2C_TypeDef* I2Cx, uint8_t address);
//send a START condition
int16_t I2CStart(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction, uint8_t ack);
//send a STOP condition
uint8_t I2CStop(I2C_TypeDef* I2Cx);
//Read bytes without ACK
uint8_t I2CReadNack(I2C_TypeDef* I2Cx);
//Reads bytes with the ACK
uint8_t I2CReadAck(I2C_TypeDef* I2Cx);
//write to the slave
void I2CWriteData(I2C_TypeDef* I2Cx, uint8_t data);

#endif
