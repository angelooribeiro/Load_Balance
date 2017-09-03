#ifndef ESP8266_H
#define ESP8266_H

#define SSID		"Vodafone-D12146"		//Name of the connection point
#define PASS		"CxtvRJKb4x"				//Password of the connectioj point
#define DST_IP	"169.254.87.175"		//IP of server
#define	PORT		"80"								//Allocated Port


//to initialize the module
void vInitESP(void);
//to send a message
int8_t iSendESP(char message[]);

#endif

