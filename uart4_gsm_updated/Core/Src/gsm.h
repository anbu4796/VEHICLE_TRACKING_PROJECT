/*
 * gsm.h
 *
 *  Created on: Jan 30, 2024
 *      Author: Lenovo
 */

#ifndef SRC_GSM_H_
#define SRC_GSM_H_
#define DATA_LENGTH 64
extern uint8_t RxBuf[30];
extern uint8_t string[100];

#endif /* SRC_GSM_H_ */

void GSM_Send_AT_Command(char* AT_CMD)
{
	HAL_UART_Transmit(&huart2,(uint8_t*)AT_CMD, strlen(AT_CMD),2000);
}

int GSM_Compare_GSMData_With(const char* string)
{
	char* ptr = NULL;
  /*Compare given string with GSM data*/
	ptr = strstr(RxBuf, string);
	/* if ptr = NULL, then no match found else match found*/
  if(ptr!=NULL)
		return 1;
	else
		return 0;
}

void GSM_SIM_CARD_STATUS(void)
{
	GSM_Send_AT_Command("AT\r\n");
	GSM_Send_AT_Command("AT+CPIN?\r\n");
}

void GSM_SIGNAL_QUALITY(void)
{
	GSM_Send_AT_Command("AT+CSQ\r\n");
}

void FUNCTION_QUALITY(void)
{
	GSM_Send_AT_Command("AT+CFUN?\r\n");
}

void GSM_CS_SERVICE(void)
{
	GSM_Send_AT_Command("AT+CREG?\r\n");
}

void GSM_PS_SERVICE(void)
{
	GSM_Send_AT_Command("AT+CEREG?\r\n");
}

void GSM_UE_INFO(void)
{
	GSM_Send_AT_Command("AT+CPSI?\r\n");
}

void GSM_CHECK_CARRIER(void)
{
	GSM_Send_AT_Command("AT+CPOS?\r\n");
}

void TCP_IP_CONFIGURE(void)
{
    GSM_Send_AT_Command("AT+CGDCONT=1,\"IP\",\"airtelgprs.com\"");
}

void TCP_IP_TRANSPARENT_MODE(void)
{
    GSM_Send_AT_Command("AT+CIPMODE=1");
}

void TCP_IP_ACTIVATE(void)
{
    GSM_Send_AT_Command("AT+NETOPEN");  //OK    +NETOPEN: 0
    GSM_Send_AT_Command("AT+IPADDR");   //+IPADDR: 10.148.0.17      OK
}

void TCP_IP_SETUP(void)
{
	 GSM_Send_AT_Command("AT+CIPOPEN=1,\"TCP\",\"117.131.85.139\",5253");
}

void TCP_IP_DATA_SEND(void)
{
	GSM_Send_AT_Command("AT+CIPSEND=1,DATA_LENGTH");
}

void TCP_IP_DEACTIVATE(void)
{
    GSM_Send_AT_Command("AT+NETCLOSE"); //OK
    GSM_Send_AT_Command("AT+IPADDR");  //+NETCLOSE: 0   +IPADDR: Network not opened ERROR
}

