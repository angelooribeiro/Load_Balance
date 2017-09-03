#include "I2C.h"

/* Private variables */
static uint32_t I2C_Timeout;

uint8_t I2CRead(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg) {
	uint8_t received_data;
	I2CStart(I2Cx, address, I2C_TRANSMITTER_MODE, I2C_ACK_DISABLE);
	I2CWriteData(I2Cx, reg);
	I2CStop(I2Cx);
	I2CStart(I2Cx, address, I2C_RECEIVER_MODE, I2C_ACK_DISABLE);
	received_data = I2CReadNack(I2Cx);
	return received_data;
}

void I2CReadMulti(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg, uint8_t* data, uint16_t count) {
	I2CStart(I2Cx, address, I2C_TRANSMITTER_MODE, I2C_ACK_ENABLE);
	I2CWriteData(I2Cx, reg);
	//TM_I2C_Stop(I2Cx);
	I2CStart(I2Cx, address, I2C_RECEIVER_MODE, I2C_ACK_ENABLE);
	while (count--) {
		if (!count) {
			/* Last byte */
			*data++ = I2CReadNack(I2Cx);
		} else {
			*data++ = I2CReadAck(I2Cx);
		}
	}
}


void I2CWrite(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg, uint8_t data) {
	I2CStart(I2Cx, address, I2C_TRANSMITTER_MODE, I2C_ACK_DISABLE);
	I2CWriteData(I2Cx, reg);
	I2CWriteData(I2Cx, data);
	I2CStop(I2Cx);
}

void I2CWriteMulti(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg, uint8_t* data, uint16_t count) {
	I2CStart(I2Cx, address, I2C_TRANSMITTER_MODE, I2C_ACK_DISABLE);
	I2CWriteData(I2Cx, reg);
	while (count--) {
		I2CWriteData(I2Cx, *data++);
	}
	I2CStop(I2Cx);
}

uint8_t I2CIsDeviceConnected(I2C_TypeDef* I2Cx, uint8_t address) {
	uint8_t connected = 0;
	/* Try to start, function will return 0 in case device will send ACK */
	if (!I2CStart(I2Cx, address, I2C_TRANSMITTER_MODE, I2C_ACK_ENABLE)) {
		connected = 1;
	}
	
	/* STOP I2C */
	I2CStop(I2Cx);
	
	/* Return status */
	return connected;
}

/* Private functions */
int16_t I2CStart(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction, uint8_t ack) {
	/* Generate I2C start pulse */
	I2Cx->CR1 |= I2C_CR1_START;
	
	/* Wait till I2C is busy */
	I2C_Timeout = I2C_TIMEOUT;
	while (!(I2Cx->SR1 & I2C_SR1_SB)) {
		if (--I2C_Timeout == 0x00) {
			return 1;
		}
	}

	/* Enable ack if we select it */
	if (ack) {
		I2Cx->CR1 |= I2C_CR1_ACK;
	}

	/* Send write/read bit */
	if (direction == I2C_TRANSMITTER_MODE) {
		/* Send address with zero last bit */
		I2Cx->DR = address & ~I2C_OAR1_ADD0;
		
		/* Wait till finished */
		I2C_Timeout = I2C_TIMEOUT;
		while (!(I2Cx->SR1 & I2C_SR1_ADDR)) {
			if (--I2C_Timeout == 0x00) {
				return 1;
			}
		}
	}
	if (direction == I2C_RECEIVER_MODE) {
		/* Send address with 1 last bit */
		I2Cx->DR = address | I2C_OAR1_ADD0;
		
		/* Wait till finished */
		I2C_Timeout = I2C_TIMEOUT;
		while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {
			if (--I2C_Timeout == 0x00) {
				return 1;
			}
		}
	}
	
	/* Read status register to clear ADDR flag */
	I2Cx->SR2;
	
	/* Return 0, everything ok */
	return 0;
}

void I2CWriteData(I2C_TypeDef* I2Cx, uint8_t data) {
	/* Wait till I2C is not busy anymore */
	I2C_Timeout = I2C_TIMEOUT;
	while (!(I2Cx->SR1 & I2C_SR1_TXE) && I2C_Timeout) {
		I2C_Timeout--;
	}
	
	/* Send I2C data */
	I2Cx->DR = data;
}

uint8_t I2CReadAck(I2C_TypeDef* I2Cx) {
	uint8_t data;
	
	/* Enable ACK */
	I2Cx->CR1 |= I2C_CR1_ACK;
	
	/* Wait till not received */
	I2C_Timeout = I2C_TIMEOUT;
	while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)) {
		if (--I2C_Timeout == 0x00) {
			return 1;
		}
	}
	
	/* Read data */
	data = I2Cx->DR;
	
	/* Return data */
	return data;
}

uint8_t I2CReadNack(I2C_TypeDef* I2Cx) {
	uint8_t data;
	
	/* Disable ACK */
	I2Cx->CR1 &= ~I2C_CR1_ACK;
	
	/* Generate stop */
	I2Cx->CR1 |= I2C_CR1_STOP;
	
	/* Wait till received */
	I2C_Timeout = I2C_TIMEOUT;
	while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)) {
		if (--I2C_Timeout == 0x00) {
			return 1;
		}
	}

	/* Read data */
	data = I2Cx->DR;
	
	/* Return data */
	return data;
}

uint8_t I2CStop(I2C_TypeDef* I2Cx) {
	/* Wait till transmitter not empty */
	I2C_Timeout = I2C_TIMEOUT;
	while (((!(I2Cx->SR1 & I2C_SR1_TXE)) || (!(I2Cx->SR1 & I2C_SR1_BTF)))) {
		if (--I2C_Timeout == 0x00) {
			return 1;
		}
	}
	
	/* Generate stop */
	I2Cx->CR1 |= I2C_CR1_STOP;
	
	/* Return 0, everything ok */
	return 0;
}



