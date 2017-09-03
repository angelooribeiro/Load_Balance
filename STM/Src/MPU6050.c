#include "stm32f4xx_conf.h"

//private functions
void vInitDMA(void);
//void vInitI2C(void);

//private variables
unsigned char MpuBuffer[15];
MPU6050_t DataStruct;
	
//DMA interrupt
void DMA1_Stream2_IRQHandler(void)
{	
	static portBASE_TYPE xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;  //do not woken a task at the start of the ISR
	
	if (DMA_GetFlagStatus(DMA1_Stream2,DMA_IT_TCIF2)) {
		//Clear transmission complete flag
		DMA_ClearFlag(DMA1_Stream2,DMA_FLAG_TCIF2);
		I2C_DMACmd(I2C2, DISABLE);
		// Send I2C1 STOP Condition
		I2C_GenerateSTOP(I2C2, ENABLE);
		// Disable DMA channel
		DMA_Cmd(DMA1_Stream2, DISABLE);
	
		//DataStruct.INT_Status = MpuBuffer[0]; 		 //Interrupt Status
		DataStruct.Accelerometer_X = ((short)MpuBuffer[1] << 8)+MpuBuffer[2]; // Accelerometer value X axis 
		DataStruct.Accelerometer_Y = ((short)MpuBuffer[3] << 8)+MpuBuffer[4]; // Accelerometer value Y axis 
		DataStruct.Accelerometer_Z = ((short)MpuBuffer[5] << 8)+MpuBuffer[6]; // Accelerometer value Z axis 
		DataStruct.Gyroscope_X = ((short)MpuBuffer[9] << 8)+MpuBuffer[10];     // Gyroscope value X axis 
		
		//Save data on the Queue and overwrite it, passed by copy
		xQueueOverwriteFromISR(xQueue_AcquisitionToAngle,(void*)&DataStruct,&xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken); //do not statrt a context switching
	} //end if
}


//used to configure the I2C of the MPU6050 module
void vInitI2C(void) {
	//structs for the I2C initialization and GPIO
	I2C_InitTypeDef I2C_InitStruct;
	GPIO_InitTypeDef  GPIOInitStruct_t;
	
	//Enable the GPIO clock
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	//configuration of the PB10 and PB11 for the I2C
	GPIOInitStruct_t.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIOInitStruct_t.GPIO_Mode = GPIO_Mode_AF;
	GPIOInitStruct_t.GPIO_Speed = GPIO_Speed_50MHz;
	GPIOInitStruct_t.GPIO_PuPd =  GPIO_PuPd_NOPULL;
	GPIOInitStruct_t.GPIO_OType = GPIO_OType_OD;
	
	//Enable Clock for the I2C
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	
	//GPIO Alternate functions
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_I2C2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_I2C2);
	
	//Init the GPIOs
	GPIO_Init(GPIOB, &GPIOInitStruct_t);
		
	// Set values
	I2C_InitStruct.I2C_ClockSpeed = 400000;		//MPU clock
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_OwnAddress1 = 0x00;
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_Init(I2C2, &I2C_InitStruct);
	
}


//configure the DMA to be used with the I2C to read the MPU6050 values
void vInitDMA(void)
{
	///to intialize the DMA interrupt and the DMA
	NVIC_InitTypeDef NVIC_InitStruct_t;
	DMA_InitTypeDef DMA_InitStruct_t;
	
	//enable the clock for the DMA
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	
	//Initialization of the struct
	DMA_InitStruct_t.DMA_Channel = DMA_Channel_7;	//channel selection
	DMA_InitStruct_t.DMA_PeripheralBaseAddr = (uint32_t)&(I2C2->DR); //adrress of I2C read register
	DMA_InitStruct_t.DMA_Memory0BaseAddr = (uint32_t)&MpuBuffer; //first addr of space to save data
	DMA_InitStruct_t.DMA_DIR = DMA_DIR_PeripheralToMemory;	//receives from the peripheral and save in memory
	DMA_InitStruct_t.DMA_BufferSize = 15;				//sizeof frame from the MPU
	DMA_InitStruct_t.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//doesn't need to increment
	DMA_InitStruct_t.DMA_MemoryInc = DMA_MemoryInc_Enable;	//automatically increment on memory
	DMA_InitStruct_t.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//the size of each element sent by the MPU
	DMA_InitStruct_t.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	//space of each element in memory
	DMA_InitStruct_t.DMA_Mode = DMA_Mode_Normal;				//non circular
	DMA_InitStruct_t.DMA_Priority = DMA_Priority_High;	//priority over other DMAs
	DMA_InitStruct_t.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStruct_t.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;	//one transferation at time
	DMA_InitStruct_t.DMA_MemoryBurst = DMA_MemoryBurst_Single;	//one trasnferation at interrupt
	//Init the DMA with the values of the struct
	DMA_Init(DMA1_Stream2, &DMA_InitStruct_t);
	
	//Enable the interrupt associated to the DMA
	DMA_ITConfig(DMA1_Stream2, DMA_IT_TC, ENABLE);
	
	NVIC_InitStruct_t.NVIC_IRQChannel = DMA1_Stream2_IRQn; //I2C2 connect to channel 7 of DMA1
	NVIC_InitStruct_t.NVIC_IRQChannelPreemptionPriority = configMAX_SYSCALL_INTERRUPT_PRIORITY + 10; //lower prioirty -> higher numeric value
	//CRITICAL : to use SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT mus be less priority that defined by configMax...
	NVIC_InitStruct_t.NVIC_IRQChannelSubPriority = 0;			//Is not used in this architecture with FreeRTOS
	NVIC_InitStruct_t.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct_t);
	
}

void MPU6050_Init(MPU6050_Accelerometer_t AccelerometerSensitivity, MPU6050_Gyroscope_t GyroscopeSensitivity) {
	
	uint8_t temp;
	
	// Format I2C address
	DataStruct.Address = MPU6050_I2C_ADDR;
	
	// Initialize I2C
	vInitI2C();
	
	I2CWrite(I2C2, MPU6050_I2C_ADDR, MPU6050_PWR_MGMT_1, 0x00);		// Wakeup MPU6050
	I2CWrite(I2C2,MPU6050_I2C_ADDR, MPU6050_PWR_MGMT_2, 0x00);		//Enable all axes
	
	//Sample Rate
	I2CWrite(I2C2, MPU6050_I2C_ADDR, MPU6050_SMPLRT_DIV, 15); //8k/(1+15)=500Hz
	
	// Config accelerometer
	temp = I2CRead(I2C2, MPU6050_I2C_ADDR, MPU6050_ACCEL_CONFIG);
	temp = (temp & 0xE7) | (uint8_t)AccelerometerSensitivity << 3;
	I2CWrite(I2C2, MPU6050_I2C_ADDR, MPU6050_ACCEL_CONFIG, temp);
	
	// Config gyroscope
	temp = I2CRead(I2C2, MPU6050_I2C_ADDR, MPU6050_GYRO_CONFIG);
	temp = (temp & 0xE7) | (uint8_t)GyroscopeSensitivity << 3;
	I2CWrite(I2C2, MPU6050_I2C_ADDR, MPU6050_GYRO_CONFIG, temp);
	
	// Set sensitivities for multiplying gyro and accelerometer data
	switch (AccelerometerSensitivity) {
		case MPU6050_Accelerometer_2G:
			DataStruct.Acce_Mult = (float)1 / MPU6050_ACCE_SENS_2; 
			break;
		case MPU6050_Accelerometer_4G:
			DataStruct.Acce_Mult = (float)1 / MPU6050_ACCE_SENS_4; 
			break;
		case MPU6050_Accelerometer_8G:
			DataStruct.Acce_Mult = (float)1 / MPU6050_ACCE_SENS_8; 
			break;
		case MPU6050_Accelerometer_16G:
			DataStruct.Acce_Mult = (float)1 / MPU6050_ACCE_SENS_16; 
		default:
			break;
	}
	
	switch (GyroscopeSensitivity) {
		case MPU6050_Gyroscope_250s:
			DataStruct.Gyro_Mult = (float)1 / MPU6050_GYRO_SENS_250; 
			break;
		case MPU6050_Gyroscope_500s:
			DataStruct.Gyro_Mult = (float)1 / MPU6050_GYRO_SENS_500; 
			break;
		case MPU6050_Gyroscope_1000s:
			DataStruct.Gyro_Mult = (float)1 / MPU6050_GYRO_SENS_1000; 
			break;
		case MPU6050_Gyroscope_2000s:
			DataStruct.Gyro_Mult = (float)1 / MPU6050_GYRO_SENS_2000; 
		default:
			break;
	}
	
	//Interrupt Pin Config (ActiveLow, Flag Clear after INT_STATUS read)
	I2CWrite(I2C2, MPU6050_I2C_ADDR,MPU6050_INT_PIN_CFG, 0x20);
	////interupt occurs when data is ready
	I2CWrite(I2C2, MPU6050_I2C_ADDR,MPU6050_INT_ENABLE, 0x1);
	
	vInitDMA();

}


int8_t MPU6050_ReadAll(void) {
	
	int8_t aux = 0;
	
	//Disable the DMA
	DMA_Cmd(DMA1_Stream2, DISABLE);
	//Restart the DMA data counter
	DMA_SetCurrDataCounter(DMA1_Stream2, 15);
	
	//To test the status of the interrupt
	I2CStart(I2C2, MPU6050_I2C_ADDR, 0, 1);
	I2CWriteData(I2C2, MPU6050_INT_STATUS);
	I2CStop(I2C2);
	
	//Enable DMA NACK automatic generation
	I2C_DMALastTransferCmd(I2C2, ENABLE);
	
	//Send start a secound time
	I2C_GenerateSTART(I2C2, ENABLE);
	
	//Test ON ev5 and clear it
	while((!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT)) || (aux++ == 10));
	if(aux == 10)
		return 0;
	
	aux = 0;
	
	//I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);
	//Send the read addr
	I2C_Send7bitAddress(I2C2, MPU6050_I2C_ADDR, I2C_Direction_Receiver);
	
	//Test on EV6 and clear
	while((!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) || (aux++ == 10));
	if(aux == 10)
		return 0;
	
	//Start the DMA associated to I2C2
	DMA_Cmd(DMA1_Stream2, ENABLE);
	I2C_DMACmd(I2C2, ENABLE);
	
	return 1;
}

//test the connection of the MPU 6050
int8_t iMPU6050TestConnection(void)
{
	//to test the connection call only this function and no initialization 
	vInitI2C();
	
	//Check if device is connected
	if (!I2CIsDeviceConnected(I2C2, MPU6050_I2C_ADDR)) {
		//Return error
		return 1;
	}
	
	// Check who I am
	if (I2CRead(I2C2, MPU6050_I2C_ADDR, MPU6050_WHO_AM_I) != MPU6050_I_AM) {
		// Return error
		return 2;
	}
	
	return 0;
}
