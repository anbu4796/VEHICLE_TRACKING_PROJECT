/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define RXSIZE 200
uint8_t RxBuf[RXSIZE];
char *Rx_string;
char *str;
int GSM_SIM_INIT(void);
int DATA_SEND_READY(void);
int GSM_TCP_init(void);
int GSM_APN_SET(void);
int GSM_Send_Data(uint8_t * data);


typedef struct gpsdata
{
	double time;
	//char time[8];
	float latitude;
	float longitude;
	double Knots;
	char Date[9];
	char N_OR_S;
	char E_OR_W;
	int fix;
	char Data[750];
	char buffer[100];
	char particular_flag_data[100];
}gpsdata;
uint8_t flag = 0;



#define Min_To_Degree  0.01666666667
#define Sec_To_Degree	 0.000277777778
char rx_data[60],json_str[100],*ptr;
int Flag=0;
gpsdata gps;
void gpsData(gpsdata* data);
static void gps_data_extraction(gpsdata* data);


int _write(int file,char *ptr,int len)

{

    int i=0;

    for(i=0;i<len;i++)

      ITM_SendChar((*ptr++));

    return len;

}


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */

//char Rx_Buffer[200];
//char Tx_Buffer[200];
//char *str;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_UART4_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_UART5_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	/* NO OPENARTAION  */
}







int DATA_SEND_READY(void)
{
	int ret;
	int ret1;
	 ret= GSM_SIM_INIT();
	  	 printf("Hello \n");
	  	 if (ret==0)
	  	 {
	  		 printf("init worked\n");
	  	 }
	  	 else
	  	 {
	  			 printf("init NOT worked\n");
	  	}
	  	 ret = GSM_APN_SET();
	  	 if (ret==0)
	  		 {
	  			 printf("APN SET worked\n");
	  			ret1= GSM_TCP_init();
	  		 }
	  		 else
	  		 {
	  				 printf("APN SET NOT worked\n");
	  		}
	  	 if (ret1==0)
	  	 	 {
	  	 		 printf("tcp worked\n");
	  	 		 return 0;

	  	 	 }
	  	 	 else
	  	 	 {
	  	 			 printf("tcp NOT worked\n");
	  	 			 return 1;
	  	 	}
}
char* GSM_Send_AT_Command(char* AT_CMD)
{
	memset(RxBuf,0,200);
	printf("sent command %s\n",AT_CMD);
	HAL_UART_Transmit_IT(&huart4,(uint8_t*)AT_CMD, strlen(AT_CMD));
	HAL_UART_Receive(&huart4, RxBuf, 200,2000);
	printf("Received data %s\n",RxBuf);
	return (char *)RxBuf;

}
int GSM_TCP_init(void)
{
	int state =0,substate=0;
	while(1)
	{
		switch(state)
		{
			case 0:

					switch(substate)
					{
						case 0:
							str=GSM_Send_AT_Command("AT+CIPMODE?\r");
							if(strstr(str,"+CIPMODE: 0"))
							{

								state=1;
								substate=0;
							}
							else
							{
								substate=1;
							}
							break;
						case 1:
							str=GSM_Send_AT_Command("AT+CIPMODE=0\r");
							if(strstr(str,"OK"))
							{
								state=1;
								substate=0;
							}
							else
							{
								substate=0;
							}
							break;
					}
					break;

			case 1:

					switch(substate)
					{
						case 0:
							str=GSM_Send_AT_Command("AT+NETOPEN\r");
							if(strstr(str,"NETOPEN"))
							{
								state=2;
								substate=0;
							}
							else
							{
								substate=1;
							}
							break;
						case 1:
							str=GSM_Send_AT_Command("AT+NETOPEN\r");
							if(strstr(str,"opened"))
							{
									state=2;
									substate=0;
							}
							else
							{
									substate=0;
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

 while (1)
  {
    /* USER CODE END WHILE */
	  switch(state)
	 	  {
	 	case 0:
	 		  switch(substate)
	   		  {
	   		  case 0:
	   			  Rx_string = GSM_Send_AT_Command("AT+COPS?\r");
	   			  if(strstr(Rx_string,"40449"))
	   			  {
	   				 state=1;
	   				printf("match found\n");
	   			  }
	   			  else
	   			  {
	   				  substate=1;
	   			  }
	   			  break;
	   		  case 1:
	   			  if(strstr(Rx_string,"405854"))
	   			  {
	   				  state=2;
	   				  substate=0;
	   			  	  printf("match found\n");
	   			  }
	   			  else
	   			  {
	   			  	  substate=0;
	   			  }
	   			  break;
			  case 2:
				  if(strstr(Rx_string,"ERROR"))
	   			  {
	   				  state=0;
	   			  	  substate=0;
	   			  }
	   			  else
	   			  {
	   			  	  state=0;
	   			  	  substate=0;
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
	   				printf("match found\n");
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
	   			  }
	   			  else
	   			  {
	   			  	  state=0;
	   			  	  substate=0;
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
	   				printf("match found\n");
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
	   			  }
	   			  else
	   			  {
	   				  substate=0;
	   			  	  state=0;
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

 while (1)
  {
    /* USER CODE END WHILE */
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
	   				printf("match found\n");
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
	   			  }
	   			  else
	   			  {
	   			  	  state=0;
	   			  	  substate=0;
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
	   				printf("match found\n");
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
	   			  }
	   			  else
	   			  {
	   			  	  state=0;
	   			  	  substate=0;
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
	   			  if(strstr(Rx_string,"99")&& (Rx_string[5]>='2'))
	   			  {
	   				 state=3;
	   				 substate=0;
	   				printf("match found\n");
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
	   			  }
	   			  else
	   			  {
	   				  substate=0;
	   			  	  state=0;
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
	   				printf("match found\n");
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
	   			  }
	   			  else
	   			  {
	   			  	  state=0;
	   			  	  substate=0;
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
	   				printf("match found\n");
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
	   			  }
	   			  else
	   			  {
	   			  	  state=0;
	   			  	  substate=0;
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
	   				printf("match found\n");
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
	   			  }
	   			  else
	   			  {
	   			  	  state=0;
	   			  	  substate=0;
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
	   				printf("match found\n");
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
	   			   	  state=6;
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
	int substate=0,cnt=0;
	char z=26;


		while(1)
		{			switch(substate)
					{
						case 0:
							str=GSM_Send_AT_Command("AT+CIPOPEN?\r");
							if(strstr(str,"139.59.78.252"))
							{
									//state=2;
									//substate=0;
									printf("ip OPEN WORKED\n");
									memset(RxBuf,0,200);
									HAL_UART_Transmit_IT(&huart4,(uint8_t*)"AT+CIPSEND=1\r", strlen("AT+CIPSEND=1\r"));
									HAL_UART_Receive(&huart4, (uint8_t *)RxBuf, 200,3000);
									printf("Received data1 %s\n",RxBuf);
									if(strstr(RxBuf,">"))
									{
									memset(RxBuf,0,200);
									//HAL_UART_Transmit_IT(&huart4,(uint8_t*)"$GPGSV,2,1,08,02,74,042,45,04,18,190,36,07,67,279,42,12,29,323,36*77", strlen("$GPGSV,2,1,08,02,74,042,45,04,18,190,36,07,67,279,42,12,29,323,36*77"));
									HAL_UART_Transmit_IT(&huart4,(uint8_t *)data,strlen(data));
									HAL_Delay(2000);
									HAL_UART_Transmit_IT(&huart4,(uint8_t *)&z,1);
									HAL_UART_Receive(&huart4, (uint8_t *)RxBuf, 200,3000);
									printf("Received data1 %s\n",RxBuf);
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
						    	str=GSM_Send_AT_Command("AT+CIPOPEN=1,\"TCP\",\"139.59.78.252\",49791\r");
						    	if(strstr(str,"+CIPOPEN: 1,0"))
						    	{
						    		//state=2;
						    		//substate=0;
						    		printf("ip OPEN WORKED\n");
						    		memset(RxBuf,0,200);
						    		HAL_UART_Transmit_IT(&huart4,(uint8_t*)"AT+CIPSEND=1\r", strlen("AT+CIPSEND=1\r"));
						    		HAL_UART_Receive(&huart4, (uint8_t *)RxBuf, 200,3000);
						    		printf("Received data1 %s\n",RxBuf);
						    		if(strstr(RxBuf,">"))
						    		{
						    		memset(RxBuf,0,200);
						    		HAL_UART_Transmit_IT(&huart4,(uint8_t *)data,strlen(data));
						    		HAL_Delay(2000);
						    		//HAL_UART_Transmit_IT(&huart4,(uint8_t*)"$GPGSV,2,1,08,02,74,042,45,04,18,190,36,07,67,279,42,12,29,323,36*77", strlen("$GPGSV,2,1,08,02,74,042,45,04,18,190,36,07,67,279,42,12,29,323,36*77"));
									HAL_UART_Transmit_IT(&huart4,(uint8_t *)&z,1);
									HAL_UART_Receive(&huart4, (uint8_t *)RxBuf, 200,3000);
						    		printf("Received data1 %s\n",RxBuf);
						    		 memset(RxBuf,0,200);
						    		return 0;
						    		}
						    		else
						    		{
						    			substate=1;
						    		}

							}
							else
							{
								substate=0;
								cnt++;
								if(cnt>5)
								{
									substate=2;
								}
							}
							break;
						case 2:
								return 1;  // GSM SEND failed
								break;



					}

		}


}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	int ret=0;


  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_UART4_Init();
  MX_USART3_UART_Init();
  MX_UART5_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  char *ptr=malloc(100);





  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
 // uint8_t d[100];
  while (1)
  {
    /* USER CODE END WHILE */
	  /*HAL_UART_Receive_IT(&huart1, d, 100);
	  HAL_Delay(3000);*/

	 if(DATA_SEND_READY()==0)
	 {

		while( HAL_UART_Receive_IT(&huart1, gps.Data,750)==HAL_OK)
		{
		 if(*gps.Data)
		 {
			 //printf("GPS Recieved %s \n",gps.Data);
			 gpsData(&gps);
			 printf("GPS Recieved %s \n",json_str);

			 if(GSM_Send_Data((uint8_t *)json_str)==0)
			 {
				 printf("Sending Worked to server\n");

			 }
			 else
			 {
				 printf("Sending to server not worked\n");
				 DATA_SEND_READY();
			 }
		 }
		 else
		 {
			 printf("GPS Recieved No DATA\n");
			 //
		 }
	 }
	 }
    /* USER CODE BEGIN 3 */




  /* USER CODE END 3 */
}
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
  * @brief UART5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART5_Init(void)
{

  /* USER CODE BEGIN UART5_Init 0 */

  /* USER CODE END UART5_Init 0 */

  /* USER CODE BEGIN UART5_Init 1 */

  /* USER CODE END UART5_Init 1 */
  huart5.Instance = UART5;
  huart5.Init.BaudRate = 9600;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  huart5.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart5.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART5_Init 2 */

  /* USER CODE END UART5_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */





  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void gpsData(gpsdata* data)
{
	char *current_flag;
	int counter;
	int comma = 0;
	printf("in func\n");
			//HAL_UART_Receive(&GpsData->UartPort,(uint8_t*)GpsData->Data,700,HAL_MAX_DELAY);
	current_flag=strstr(data->Data,"GNGGA");
	//printf("string found\n");
	if(*current_flag=='G')
	{
	   counter=0;
	   while(*(current_flag+counter)!='*')
	   {
		   data->particular_flag_data[counter]=*(current_flag+counter);
		   counter++;
	   }
	    printf(" string is %s", data->particular_flag_data);
		sprintf(data->buffer,"%s",data->particular_flag_data);
		sscanf(data->buffer,"GNGGA,%lf,%f,%c,%f,%c,%d,",&data->time,&data->latitude,&data->N_OR_S,&data->longitude,&data->E_OR_W,&data->fix);
        printf("lat:%f\n",data->latitude);
        printf("lon:%f\n",data->longitude);


		current_flag=strstr(data->Data,"RMC");
			//printf("string found\n");
			if(*current_flag=='R')
			{
				printf("\nrmc ->%s\n",*current_flag);
				counter=0;

				while(comma < 9) {
					if(*(current_flag + counter) == ',')
						comma++;
					counter++;
					if(comma == 9)
						break;

				}

				for(int i=counter; i < counter+7; i++)
					data->Date[i-counter]= *(current_flag+i);

//					   while((*(current_flag+n)!='*') && (comma < 9) )
//					   {
//						   n++;
//
//						   if(*(current_flag+n) != ',')
//							   comma++;
//					   }
//
//					   for(int  i = 0 ; i < 9 ; i++ ,n++)
//					   {
//						   if(i== 8)
//						   {
//							   data->Date[i]= '\0';
//						   }
//						   else if((i != 2) && (i != 4) )
//						   {
//							   data->Date[i]= *(current_flag+n);
//						   }
//						   else
//						   {
//							   data->Date[i]= '/';
//						   }
//
//					   }

			}


		if(data->fix==1||data->fix==2||data->fix==6)
		{
			printf("valid fix\n");
			gps_data_extraction(data);
		}
		else
		{
			printf("invalid signal no fix\n");
		}
	 }
	else
	{
		printf("no data\n");
	}
}

static void gps_data_extraction(gpsdata* data)
{
	double Deg_Val=0.0,Min_Val=0.0,Sec_Val=0.0,lon=0.0,lat=0.0;
	int hr,min,sec;
	//int hours, minutes, seconds;

	lon=data->longitude;
	lat=data->latitude;
	if((data->E_OR_W=='E' && data->N_OR_S=='S')||(data->E_OR_W=='W' && data->N_OR_S=='N'))
	{
		Deg_Val=(int)(lon/100);
		Min_Val=(int)(lon-(Deg_Val*100));
		Sec_Val=((lon-(Deg_Val*100))-Min_Val)*100;
		data->longitude=(Deg_Val+(Min_Val*Min_To_Degree)+(Sec_Val*Sec_To_Degree))*-1;
		printf("longitude : %f\n",data->longitude);

		Deg_Val=(int)((lat/100));
		Min_Val=(int)(lat-(Deg_Val*100));
		Sec_Val=((lat-(Deg_Val*100))-Min_Val)*10;
		data->latitude=(Deg_Val+(Min_Val*Min_To_Degree)+(Sec_Val*Sec_To_Degree));
		printf("latitude : %f\n",data->latitude);
	}
    hr=(int)((data->time)/10000);
   	min=(int)(data->time-(hr*10000))/100;
   	sec=(int)(data->time-((hr*10000)+(min*100)));
   	int ist_hr = hr + 5;  // 5 hours difference
   	int ist_min = min + 30;  // 30 minutes difference
    if(ist_hr>=24)
    {
    	ist_hr-=24;
    }
   	// Adjusting for overflow
   	if (ist_min >= 60) {
   	    ist_hr++;
   	    ist_min -= 60;
   	}

	    //sscanf(data->time, "%2d%2d%2d", &hours, &minutes, &seconds);
//	    // Convert to IST
//	    hours += 5;
//	    minutes += 30;
//
//	        // Adjust for overflow
//	    if (minutes >= 60) {
//	         minutes -= 60;
//	         hours++;
//	     }
//
//	        // Adjust for 24-hour clock format
//	     if (hours >= 24) {
//	         hours -= 24;
//	     }
//
//	        // Format and store in gpsData
//	     sprintf(data->time, "%02d:%02d:%02d", hours, minutes, seconds);
			//sprintf(json_str,"{\"latitude\":%f %c,\n \"longitude\":%f %c,\n \"time\":%s,\nData = %s\n }",data->latitude,data->N_OR_S, data->longitude,data->E_OR_W,data->time,data->Date);

		sprintf(json_str,"{\"latitude\":%f %c,\n \"longitude\":%f %c,\n \"time\":%d:%d:%d\nData = %s\n }",data->latitude,data->N_OR_S, data->longitude,data->E_OR_W,hr,min,sec,data->Date);
	     //sprintf(json_str,"{\"latitude\":%f %c,\n \"longitude\":%f %c,\n \"time\":\"%s\" \nData = %s\n }",data->latitude,data->N_OR_S, data->longitude,data->E_OR_W,data->time,data->Date);
	     printf("json format is %s\n",json_str);

}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
