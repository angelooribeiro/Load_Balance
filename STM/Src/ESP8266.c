#include "stm32f4xx_conf.h"
#include "string.h"
//USSEFUL BUT NOT USED
/*
//AT commands to the PORT
//To change the configuration to station mode
char StationMode[] = {"AT+CWMODE=1\r\n"};
//To create the connection with the router
//Add the SSID and the PASSWORD and the terminator
char RouterConnect[] = {"AT+CWJAP=\"HIGHWAY 4G\",\"robot1234\"\r\n"};
//Request the IP Addr that was atribuited 
char RequestIP[] = {"AT+CIFSR\r\n"};
//Enable multiconnections
char MultConn[] = {"AT+CIPMUX=0\r\n"};
//Connect to a TCP Port
//Add "TCP", "IP", PORT
char TCPConn[] = {"AT+CIPSTART=\"TCP\",\"192.168.43.164\",1234\r\n"};
char CIPSend[] = {"AT+CIPSEND=\"TCP\",\"192.168.0.2\",3553\r\n"};
*/

void vInitESP(void){
	
	InitUSART(9600);
	//To store the data that returns from the ESP
	char result[20] = {0};
	
	//Test the connection
	while(strcmp(result, "AT\r\r\n\r\nOK\r\n")) {
		printf("AT\r\n");			//Send
		for(int i = 0; i < 1000000; i++);
		scanf("%s", result);		//Receive
	}
	
	//Establish connection to the ESP
	while( strcmp(result, "AT+CIPSTART=\"TCP\",\"192.168.43.164\",1234\r\r\nOK\r\nLinkd\r\n")){
		printf("AT+CIPSTART=\"TCP\",\"192.168.43.164\",1234\r\n");
		for(int i = 0; i < 10000000; i++);	//Delay
		scanf("%s", result);
	}
	//If the system doesn't get a valid connection the system will never start
}

int8_t iSendESP(char message[])
{
	char result[20] = {0};
	//The ESP need to know how much bytes will send
	uint8_t aux = 0;
	while(message[aux++] != '\0');

	//Send the request to send the message
	printf("AT+CIPSEND=%d\n\r", aux);
	vTaskDelay(pdMS_TO_TICKS(1));
	scanf("%s", result);
	//If ESP is ready to receive
	if(strcmp(result,">")){
		//Send the message
		printf("%s\r\n", message);
	//must be made the scanf to ensure that was made correctly
	return 1;		//success
	}
	else 
		return 0; //error
}

