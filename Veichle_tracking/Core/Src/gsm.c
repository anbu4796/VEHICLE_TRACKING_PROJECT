/*
 * gsm.c
 *
 *  Created on: Feb 6, 2024
 *      Author: ram77
 *
 *
 *
 */


#include "main.h"

#include "stdio.h"
#include "string.h"
#include "gsm.h"

#define RXSIZE 200
uint8_t RxBuf[RXSIZE];
char *Rx_string;
extern UART_HandleTypeDef huart4;


int DATA_SEND_READY(void)
{
	int ret;
	int ret1;
	 ret= GSM_SIM_INIT();
	  	 if (ret==0)
	  	 {
	  		 printf("GSM INIT DONE!\n");
	  	 }
	  	 else
	  	 {
	  			 printf("GSM INIT FAILED!!!\n");
	  	}
	  	 ret = GSM_APN_SET();
	  	 if (ret==0)
	  		 {
	  			 printf("APN SET DONE!\n");
	  			ret1= GSM_TCP_init();
	  		 }
	  		 else
	  		 {
	  				 printf("APN SET FAILED\n");
	  		}
	  	 if (ret1==0)
	  	 	 {
	  	 		 printf("TCP INIT DONE\n");
	  	 		 return 0;

	  	 	 }
	  	 	 else
	  	 	 {
	  	 			 printf("TCP INIT FAILED\n");
	  	 			 return 1;
	  	 	}
}
char* GSM_Send_AT_Command(char* AT_CMD)
{
	memset(RxBuf,0,RXSIZE);
	printf("Sent command %s\n",AT_CMD);
	HAL_UART_Transmit_IT(&huart4,(uint8_t*)AT_CMD, strlen(AT_CMD));
	HAL_UART_Receive(&huart4, RxBuf, RXSIZE,500);
	printf("Received response %s\n",RxBuf);
	return (char *)RxBuf;

}
int GSM_TCP_init(void)
{
	uint8_t state=0,substate=0;
	uint8_t counter=0;

	while(1)
	{
		switch(state)
		{
			case 0:

					switch(substate)
					{
						case 0:
							Rx_string=GSM_Send_AT_Command("AT+CIPMODE?\r");
							if(strstr(Rx_string,"+CIPMODE: 0"))
							{

								state=1;
								substate=0;
								counter=0;
							}
							else
							{
								substate=1;
							}
							break;
						case 1:
							Rx_string=GSM_Send_AT_Command("AT+CIPMODE=0\r");
							if(strstr(Rx_string,"OK"))
							{
								state=1;
								substate=0;
								counter=0;
							}
							else
							{
								substate=0;
								counter++;
									   			  	  if(counter>=10)
									   			  	  {
									   			  		  printf("CONNECTIVITY PROBLEM\n");
									   			  		  return 1;
									   			  	  }
							}
							break;
					}
					break;

			case 1:

					switch(substate)
					{
						case 0:
							Rx_string=GSM_Send_AT_Command("AT+NETOPEN\r");
							if(strstr(Rx_string,"NETOPEN"))
							{
								state=2;
								substate=0;
								counter=0;
							}
							else
							{
								substate=1;
							}
							break;
						case 1:
							Rx_string=GSM_Send_AT_Command("AT+NETOPEN\r");
							if(strstr(Rx_string,"opened"))
							{
									state=2;
									substate=0;
									counter=0;
							}
							else
							{
									substate=0;
									counter++;
										   			  	  if(counter>=10)
										   			  	  {
										   			  		  printf("CONNECTIVITY PROBLEM\n");
										   			  		  return 1;
										   			  	  }
							}
					}
					break;

			case 2:
					return 0;
					break;
			default:
					return 1;
		}
	}
}

int GSM_APN_SET(void)
{

uint8_t state=0;
uint8_t substate=0;
uint8_t counter=0;
 while (1)
  {
	  switch(state)
	 	  {
	 	case 0:
	 		  switch(substate)
	   		  {
	   		  case 0:
	   			  Rx_string = GSM_Send_AT_Command("AT+COPS?\r");
	   			  if(strstr(Rx_string,"40449"))
	   			  {
	   				  counter=0;
	   				 state=1;
	   				printf("RESPONSE MATCHED\n");
	   			  }
	   			  else
	   			  {
	   				  substate=1;
	   				counter++;
	   					   			  	  if(counter>=10)
	   					   			  	  {
	   					   			  		  printf("CONNECTIVITY PROBLEM\n");
	   					   			  		  return 1;
	   					   			  	  }
	   			  }
	   			  break;
	   		  case 1:
	   			  if(strstr(Rx_string,"405854"))
	   			  {
	   				  state=2;
	   				  substate=0;
	   				  counter=0;
	   			  	  printf("RESPONSE MATCHED\n");
	   			  }
	   			  else
	   			  {
	   			  	  substate=0;
	   			  	counter++;
	   			  		   			  	  if(counter>=10)
	   			  		   			  	  {
	   			  		   			  		  printf("CONNECTIVITY PROBLEM\n");
	   			  		   			  		  return 1;
	   			  		   			  	  }
	   			  }
	   			  break;
			  case 2:
				  if(strstr(Rx_string,"ERROR"))
	   			  {
	   				  state=0;
	   			  	  substate=0;
	   			  	counter++;
	   			  		   			  	  if(counter>=10)
	   			  		   			  	  {
	   			  		   			  		  printf("CONNECTIVITY PROBLEM\n");
	   			  		   			  		  return 1;
	   			  		   			  	  }
	   			  }
	   			  else
	   			  {
	   			  	  state=0;
	   			  	  substate=0;
	   			  	counter++;
	   			  		   			  	  if(counter>=10)
	   			  		   			  	  {
	   			  		   			  		  printf("CONNECTIVITY PROBLEM\n");
	   			  		   			  		  return 1;
	   			  		   			  	  }
	   			  }
	   			  break;

	   		  default:
	   			  break;
	   		  }
	 		  break;
	 	case 1:
	 		  switch(substate)
	   		  {
	   		  case 0:
	   			  Rx_string = GSM_Send_AT_Command("AT+CGDCONT=1,\"IP\",\"airtelgprs.com\"\r");
	   			  if(strstr(Rx_string,"OK"))
	   			  {
	   				 state=2;
	   				 substate=0;
	   				printf("RESPONSE MATCHED\n");
	   				counter=0;
	   				return 0;
	   			  }
	   			  else
	   			  {
	   				  substate=1;
	   			  }
	   			  break;
	   		  case 1:
	   			  if(!(strncmp(Rx_string,"\r\nERROR\r\n",6)))
	   			  {
	   				  state=0;
	   			  	  substate=0;
	   			  	counter++;
	   			  		   			  	  if(counter>=10)
	   			  		   			  	  {
	   			  		   			  		  printf("CONNECTIVITY PROBLEM\n");
	   			  		   			  		  return 1;
	   			  		   			  	  }
	   			  }
	   			  else
	   			  {
	   			  	  state=0;
	   			  	  substate=0;
	   			  	counter++;
	   			  		   			  	  if(counter>=10)
	   			  		   			  	  {
	   			  		   			  		  printf("CONNECTIVITY PROBLEM\n");
	   			  		   			  		  return 1;
	   			  		   			  	  }
	   			  }
	   			  break;
	   		  default:
	   			  break;
	   		  }
	 		  break;
	 	case 2:
	 		  switch(substate)
	   		  {
	   		  case 0:
	   			  Rx_string = GSM_Send_AT_Command("AT+CGDCONT=1,\"IP\",\"jionet\"\r");
	   			  if(strstr(Rx_string,"OK"))
	   			  {
	   				 state=3;
	   				 substate=0;
	   				 counter=0;
	   				printf("RESPONSE MATCHED\n");
					return 0;
	   			  }
	   			  else
	   			  {
	   				  substate=1;
	   			  }
	   			  break;
	   		  case 1:
			          if(!(strncmp(Rx_string,"\r\nERROR\r\n",6)))
	   			  {
	   			  	  substate=0;
	   			  	  state=2;
	   			  	counter++;
	   			  		   			  	  if(counter>=10)
	   			  		   			  	  {
	   			  		   			  		  printf("CONNECTIVITY PROBLEM\n");
	   			  		   			  		  return 1;
	   			  		   			  	  }
	   			  }
	   			  else
	   			  {
	   				  substate=0;
	   			  	  state=0;
	   			  	counter++;
	   			  		   			  	  if(counter>=10)
	   			  		   			  	  {
	   			  		   			  		  printf("CONNECTIVITY PROBLEM\n");
	   			  		   			  		  return 1;
	   			  		   			  	  }
	   			  }
	   			  break;
	   		  default:
	   			  break;
	   		  }
	 		  break;
	}
}
}
int GSM_SIM_INIT(void)
{

uint8_t state=0;
uint8_t substate=0;
uint8_t counter=0;

 while (1)
  {
	  switch(state)
	 	  {
	 	case 0:
	 		  switch(substate)
	   		  {
	   		  case 0:
	   			  Rx_string = GSM_Send_AT_Command("ATE0\r");
	   			  if(!(strncmp(Rx_string,"\r\nOK\r\n",6)))
	   			  {
	   				 state=1;
	   				printf("RESPONSE MATCHED\n");
	   				counter=0;
	   			  }
	   			  else
	   			  {
	   				  substate=1;
	   			  }
	   			  break;
	   		  case 1:
	   			  if(!(strncmp(Rx_string,"\r\nERROR\r\n",6)))
	   			  {
	   				  state=0;
	   			  	  substate=0;
	   			  	counter++;
	   			  	if(counter>=10)
	   			  	{
	   			  		printf("CONNECTIVITY PROBLEM\n");
	   			  		return 1;
	   			  	}
	   			  }
	   			  else
	   			  {
	   			  	  state=0;
	   			  	  substate=0;
	   			  	  counter++;
	   			  	  if(counter>=10)
	   			  	  {
	   			  		  printf("CONNECTIVITY PROBLEM\n");
	   			  		  return 1;
	   			  	  }
	   			  }
	   			  break;
	   		  default:
	   			  break;
	   		  }
	 		  break;
	 	case 1:
	 		  switch(substate)
	   		  {
	   		  case 0:
	   			  Rx_string = GSM_Send_AT_Command("AT+CPIN?\r");
	   			  if(strstr(Rx_string,"READY"))
	   			  {
	   				 state=2;
	   				 substate=0;
	   				 counter=0;
	   				printf("RESPONSE MATCHED\n");
	   			  }
	   			  else
	   			  {
	   				  substate=1;
	   			  }
	   			  break;
	   		  case 1:
	   			  if(!(strncmp(Rx_string,"\r\nERROR\r\n",6)))
	   			  {
	   				  state=0;
	   			  	  substate=0;
	   			  	counter++;
	   			  		   			  	  if(counter>=10)
	   			  		   			  	  {
	   			  		   			  		  printf("CONNECTIVITY PROBLEM\n");
	   			  		   			  		  return 1;
	   			  		   			  	  }
	   			  }
	   			  else
	   			  {
	   			  	  state=0;
	   			  	  substate=0;
	   			  	counter++;
	   			  		   			  	  if(counter>=10)
	   			  		   			  	  {
	   			  		   			  		  printf("CONNECTIVITY PROBLEM\n");
	   			  		   			  		  return 1;
	   			  		   			  	  }
	   			  }
	   			  break;
	   		  default:
	   			  break;
	   		  }
	 		  break;
	 	case 2:
	 		  switch(substate)
	   		  {
	   		  case 0:
	   			  Rx_string = GSM_Send_AT_Command("AT+CSQ\r");
	   			  if(strstr(Rx_string,"99")&& (Rx_string[8]>='2'))
	   			  {
	   				 state=3;
	   				 substate=0;
	   				 counter=0;
	   				printf("RESPONSE MATCHED\n");
	   			  }
	   			  else
	   			  {
	   				  substate=1;
	   			  }
	   			  break;
	   		  case 1:
			          Rx_string = GSM_Send_AT_Command("AT+CFUN=1\r");
	   			  if(strstr(Rx_string,"0,1"))
	   			  {
	   			  	  substate=0;
	   			  	  state=2;
	   			  	  counter=0;
	   			  }
	   			  else
	   			  {
	   				  substate=0;
	   			  	  state=0;
	   			  	counter++;
	   			  		   			  	  if(counter>=10)
	   			  		   			  	  {
	   			  		   			  		  printf("CONNECTIVITY PROBLEM\n");
	   			  		   			  		  return 1;
	   			  		   			  	  }
	   			  }
	   			  break;
	   		  default:
	   			  break;
	   		  }
	 		  break;
	 case 3:
	 		  switch(substate)
	   		  {
	   		  case 0:
	   			  Rx_string = GSM_Send_AT_Command("AT+CMEE=2\r");
	   			  if(strstr(Rx_string,"OK"))
	   			  {
	   				 state=4;
	   				 substate=0;
	   				 counter=0;
	   				printf("RESPONSE MATCHED\n");
	   			  }
	   			  else
	   			  {
	   				  substate=1;
	   			  }
	   			  break;
	   		  case 1:
	   			  if(!(strncmp(Rx_string,"\r\nERROR\r\n",6)))
	   			  {
	   				  state=0;
	   			  	  substate=0;
	   			  	counter++;
	   			  		   			  	  if(counter>=10)
	   			  		   			  	  {
	   			  		   			  		  printf("CONNECTIVITY PROBLEM\n");
	   			  		   			  		  return 1;
	   			  		   			  	  }
	   			  }
	   			  else
	   			  {
	   			  	  state=0;
	   			  	  substate=0;
	   			  	counter++;
	   			  		   			  	  if(counter>=10)
	   			  		   			  	  {
	   			  		   			  		  printf("CONNECTIVITY PROBLEM\n");
	   			  		   			  		  return 1;
	   			  		   			  	  }
	   			  }
	   			  break;
	   		  default:
	   			  break;
	   		  }
	 		  break;
	 	case 4:
	 		  switch(substate)
	   		  {
	   		  case 0:
	   			  Rx_string = GSM_Send_AT_Command("AT+CREG?\r");
	   			  if((strstr(Rx_string,"0,1"))||(strstr(Rx_string,"0,6")))
	   			  {
	   				 state=5;
	   				 substate=0;
	   				 counter=0;
	   				printf("RESPONSE MATCHED\n");
	   			  }
	   			  else
	   			  {
	   				  substate=1;
	   			  }
	   			  break;
	   		  case 1:
	   			  if(!(strncmp(Rx_string,"\r\nERROR\r\n",6)))
	   			  {
	   				  state=0;
	   			  	  substate=0;
	   			  	counter++;
	   			  		   			  	  if(counter>=10)
	   			  		   			  	  {
	   			  		   			  		  printf("CONNECTIVITY PROBLEM\n");
	   			  		   			  		  return 1;
	   			  		   			  	  }
	   			  }
	   			  else
	   			  {
	   			  	  state=0;
	   			  	  substate=0;
	   			  	counter++;
	   			  		   			  	  if(counter>=10)
	   			  		   			  	  {
	   			  		   			  		  printf("CONNECTIVITY PROBLEM\n");
	   			  		   			  		  return 1;
	   			  		   			  	  }
	   			  }
	   			  break;
	   		  default:
	   			  break;
	   		  }
 			  break;
		case 5:
	 		  switch(substate)
	   		  {
	   		  case 0:
	   			  Rx_string = GSM_Send_AT_Command("AT+CGREG?\r");
	   			  if((strstr(Rx_string,"0,1"))||(strstr(Rx_string,"1,1")))
	   			  {
	   				 state=6;
	   				 substate=0;
	   				 counter=0;
	   				printf("RESPONSE MATCHED\n");
	   			  }
	   			  else
	   			  {
	   				  substate=1;
	   			  }
	   			  break;
	   		  case 1:
				  Rx_string = GSM_Send_AT_Command("AT+CGREG=0\r");
	   			  if(strstr(Rx_string,"OK"))
	   			  {
	   				  state=5;
	   			  	  substate=0;
	   			  	  counter=0;
	   			  }
	   			  else
	   			  {
	   			  	  state=0;
	   			  	  substate=0;
	   			  	counter++;
	   			  		   			  	  if(counter>=10)
	   			  		   			  	  {
	   			  		   			  		  printf("CONNECTIVITY PROBLEM\n");
	   			  		   			  		  return 1;
	   			  		   			  	  }
	   			  }
	   			  break;
	   		  default:
	   			  break;
	   		  }
 			  break;
		case 6:
	 		  switch(substate)
	   		  {
	   		  case 0:
	   			  Rx_string = GSM_Send_AT_Command("AT+CPSI?\r");
	   			  if((strstr(Rx_string,"LTE,Online")))
	   			  {
	   				 state=0;
	   				 substate=0;
	   				printf("RESPONSE MATCHED\n");
	   				counter=0;
					return 0;
	   			  }
	   			  else
	   			  {
	   				  substate=1;
	   			  }
	   			  break;
	   		 case 1:
	   			  if(GSM_APN_SET()==0)
	   			  {
	   			     substate=0;
	   			  }
	   			  else
	   			  {
	   				  substate=0;
	   			   	  state=6;
	   			   counter++;
	   			   	   			  	  if(counter>=10)
	   			   	   			  	  {
	   			   	   			  		  printf("CONNECTIVITY PROBLEM\n");
	   			   	   			  		  return 1;
	   			   	   			  	  }
	   			  }

	   		  default:
	   			  break;
	   		  }
 			  break;

	  default:
	 	 break;
  	}
	}
}


int GSM_Send_Data(uint8_t * data)
{
	int substate=0,counter=0;
	char CTRL_Z=26;


		while(1)
		{			switch(substate)
					{		case 0:
							Rx_string=GSM_Send_AT_Command("AT+CIPOPEN?\r");
							if(strstr(Rx_string,"139.59.78.252"))
							{
									printf("IP OPENED\n");
									counter=0;
									memset(RxBuf,0,200);
									HAL_UART_Transmit_IT(&huart4,(uint8_t*)"AT+CIPSEND=1\r", strlen("AT+CIPSEND=1\r"));
									HAL_UART_Receive(&huart4, (uint8_t *)RxBuf, 200,3000);
									printf("RECEIVED RESPONSE FROM SERVER %s\n",RxBuf);
									if(strstr(RxBuf,(char *)">"))
									{
									memset(RxBuf,0,200);
									HAL_UART_Transmit_IT(&huart4,(uint8_t *)data,strlen(data));
									HAL_Delay(2000);
									HAL_UART_Transmit_IT(&huart4,(uint8_t *)&CTRL_Z,1);
									HAL_UART_Receive(&huart4, (uint8_t *)RxBuf, 200,3000);
									printf("RECEIVED RESPONSE FROM SERVER %s\n",RxBuf);
									memset(RxBuf,0,200);
									return 0;
									}
									else
									{
										substate=0;
									}

							}
							else
							{
								substate=1;
							}
							break;
						    case 1:
						    	Rx_string=GSM_Send_AT_Command("AT+CIPOPEN=1,\"TCP\",\"139.59.78.252\",52102\r");
						    	if(strstr(Rx_string,"+CIPOPEN: 1,0"))
						    	{
						    		printf("IP OPENED\n");
						    		counter=0;
						    		substate=0;
						    		memset(RxBuf,0,200);
						    		HAL_UART_Transmit_IT(&huart4,(uint8_t*)"AT+CIPSEND=1\r", strlen("AT+CIPSEND=1\r"));
						    		HAL_UART_Receive(&huart4, (uint8_t *)RxBuf, 200,1000);
						    		printf("RECEIVED RESPONSE FROM SERVER %s\n",RxBuf);
						    				if(strstr(RxBuf,(char *)">"))
						    					{
						    					memset(RxBuf,0,200);
						    					HAL_UART_Transmit_IT(&huart4,(uint8_t *)data,strlen(data));
						    					HAL_Delay(2000);
						    					HAL_UART_Transmit_IT(&huart4,(uint8_t *)&CTRL_Z,1);
						    					HAL_UART_Receive(&huart4, (uint8_t *)RxBuf, 200,1000);
						    					printf("RECEIVED RESPONSE FROM SERVER %s\n",RxBuf);
						    					memset(RxBuf,0,200);
						    					}						    								    								    					else
						    					{
													substate=1;

						    					}
								}
								else
								{
									counter++;
									if(counter>5)
									{
									  return 1;
									}
								}
						    	break;
						case 2:
								return 1;  // GSM SEND failed
								break;
					  }
		}
}











