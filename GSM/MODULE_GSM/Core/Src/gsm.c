/*
 * gsm.c
 *
 *  Created on: Jan 29, 2024
 *      Author: ram77
 */
#include "main.h"
#include "gsm.h"

#include <string.h>
#include <stdint.h>


extern UART_HandleTypeDef huart3;




/* "RX buffer" to store incoming data from GSM module */
char RX_Buffer[RX_BUFFER_SIZE];


//GSM initialization

void gsm_init(void)
{
	 /* Send Basic AT Commmand */
	 gsm_send_AT_cmd("AT\r\n"); //responce is "ok"
	 HAL_Delay(300);
	 gsm_send_AT_cmd("AT+CPIN?\r\n"); //"responce +CPIN: READY
	 HAL_Delay(300);
	 gsm_send_AT_cmd("AT+CSQ\r\n");
	 HAL_Delay(300);
	 gsm_send_AT_cmd("AT+CCID\r\n");
	 HAL_Delay(300);
	 gsm_send_AT_cmd("AT+CREG?\r\n");
	 HAL_Delay(300);
	 gsm_send_AT_cmd("AT+CREG?\r\n");
	 HAL_Delay(300);
	 gsm_send_AT_cmd("ATI\r\n");
	 HAL_Delay(300);
	 gsm_send_AT_cmd("AT+CBC\r\n");
	 HAL_Delay(300);
}

// GSM sening
void gsm_send(char *msg)
{
	gsm_send_AT_cmd("AT+CMGF=1\r\n"); //// Configuring TEXT mode
	HAL_Delay(300);
	gsm_send_AT_cmd("AT+CMGS=\"+919014941406\"");// phone number to sms
	HAL_Delay(300);
	HAL_UART_Transmit(&huart3,(uint8_t *)msg,strlen(msg),2000);
	HAL_Delay(1000);
}

//GSM Recieve
char * gsm_recv(void)
{
/* recv the sms from any number*/
	return NULL;
}

//GSM send AT cmd

void gsm_send_AT_cmd(char *AT_cmd)
{
	HAL_UART_Transmit(&huart3,(uint8_t*)AT_cmd, strlen(AT_cmd),2000);
}

//GSM TCP init

void gsm_TCP_init(void)
{
	gsm_send_AT_cmd("AT\r\n"); //responce is "ok"
	 HAL_Delay(300);
	 gsm_send_AT_cmd("AT+CPIN?\r\n"); //"responce +CPIN: READY
	 HAL_Delay(300);



}

//GSM TCP Send

void gsm_TCP_send(char *)
{
}



