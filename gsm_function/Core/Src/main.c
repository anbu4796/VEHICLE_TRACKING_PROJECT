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
#include "stdio.h"
#include "string.h"
UART_HandleTypeDef huart4;
#define RXSIZE 200
uint8_t RxBuf[RXSIZE];
char *Rx_string;
char *str;
int _write(int file,char *ptr,int len)
{
    int i=0;
    for(i=0;i<len;i++)
    	ITM_SendChar((*ptr++));
    return len;
}

char* GSM_Send_AT_Command(char* AT_CMD)
{
	memset(RxBuf,0,sizeof(RxBuf));
	HAL_UART_Transmit_IT(&huart4,(uint8_t*)AT_CMD, strlen(AT_CMD));
	HAL_UART_Receive(&huart4, RxBuf, 200,1000);
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
					switch(substate)
					{
						case 0:
							str=GSM_Send_AT_Command("AT+CIPOPEN?\r");
							if(strstr(str,"139.59.78.252"))
							{
									state=2;
									substate=0;
									printf("ip OPEN WORKED\n");
									memset(RxBuf,0,200);
									HAL_UART_Transmit_IT(&huart4,(uint8_t*)"AT+CIPSEND=0,67\r", strlen("AT+CIPSEND=0,67\r"));
									HAL_UART_Receive(&huart4, RxBuf, 200,3000);
									printf("Received data %s\n",RxBuf);
									memset(RxBuf,0,200);
									HAL_UART_Transmit_IT(&huart4,(uint8_t*)"$GPGSV,2,1,08,02,74,042,45,04,18,190,36,07,67,279,42,12,29,323,36*77", strlen("$GPGSV,2,1,08,02,74,042,45,04,18,190,36,07,67,279,42,12,29,323,36*77"));
									HAL_UART_Receive(&huart4, RxBuf, 200,3000);
									printf("Received data %s\n",RxBuf);
									// memset(RxBuf,0,200);
									//return 0;

							}
							else
							{
								substate=1;
							}
							break;
						    case 1:
						    	str=GSM_Send_AT_Command("AT+CIPOPEN=0,\"TCP\",\"139.59.78.252\",49791\r");
						    	if(strstr(str,"+CIPOPEN: 0,0"))
						    	{
						    		state=2;
						    		substate=0;
						    		printf("ip OPEN WORKED\n");
						    		memset(RxBuf,0,200);
						    		HAL_UART_Transmit_IT(&huart4,(uint8_t*)"AT+CIPSEND=0,7\r", strlen("AT+CIPSEND=0,7\r"));
						    		HAL_UART_Receive(&huart4, RxBuf, 200,3000);
						    		printf("Received data %s\n",RxBuf);
						    		memset(RxBuf,0,200);
						    		HAL_UART_Transmit_IT(&huart4,(uint8_t*)"Anburaj", strlen("Anburaj"));
						    		HAL_UART_Receive(&huart4, RxBuf, 200,3000);
						    		printf("Received data %s\n",RxBuf);
						    		// memset(RxBuf,0,200);
						    		//return 0;
							}
							else
							{
								substate=0;
							}
							break;

					}
					break;
			case 3:
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
	   			  if(strstr(Rx_string,"99"))
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
				  Rx_string = GSM_Send_AT_Command("AT+CGREG=1\r");
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

	  default:
	 	 break;
  	}
	}
}
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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


/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_UART4_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

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
  MX_UART4_Init();
  /* USER CODE BEGIN 2 */
  int ret;
  int ret1;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	 ret= GSM_SIM_INIT();
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
	 		 break;
	 	 }
	 	 else
	 	 {
	 			 printf("tcp NOT worked\n");
	 	}
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
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
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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
