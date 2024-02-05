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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
int _write(int file,char *ptr,int len)
{

    int i=0;
for(i=0;i<len;i++)
  ITM_SendChar((*ptr++));
return len;
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
char buf_to_extract[700];
//char GGA_buffer[80], RMC_buffer[80];
char json_str[256]; // Adjust the size as needed
//char *ptr,ptr1;
//int length;
float latitude, longitude;
int time,date,flag=0,gps_fix;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
//int times;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_UART4_Init(void);
static void MX_UART5_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
void extract_data_for_GGA(char *);
void json_format_for_GGA(char *);
void json_format_for_RMC(char *);
void extract_data_for_RMC(char *);
void get_data_from_gps(void);
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
  MX_UART5_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
 //  HAL_UART_Receive(&huart3, rx_data, sizeof(rx_data), 1000);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	 //HAL_UART_Receive_IT(&huart3, rx_data,sizeof(rx_data), 1000);

	                get_data_from_gps();

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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
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
  huart4.Init.BaudRate = 9600;
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
  huart2.Init.BaudRate = 9600;
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
  huart3.Init.BaudRate = 9600;
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

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();



}

/* USER CODE BEGIN 4 */
void get_data_from_gps()
{
	char UART_receive_data[700];
	char *ptrgga=NULL, *ptrrmc=NULL;
	char GGA_buffer[53],RMC_buffer[80];
			int index,gga_length,rmc_length;

		  	 	// length=strlen(rx_data);
		  	 	//   HAL_UART_Transmit(&huart2, rx_data,sizeof(rx_data), 2000);
		  	 	 // HAL_Delay(1000);
			 printf("loop\n");
			memset(UART_receive_data,0,sizeof(UART_receive_data));

			HAL_UART_Receive(&huart3,UART_receive_data,sizeof(UART_receive_data),1000);
			printf("received data from UART %s\n",UART_receive_data);
			strcpy(buf_to_extract,UART_receive_data);
			 	printf(" data for extraction %s\n",buf_to_extract);

		  	 	if( ptrgga=strstr(buf_to_extract,"GPGGA"))
		  	 	{
		  	 	//memset(GGA_buffer,0,sizeof(GGA_buffer));

		  	 	 if(*ptrgga=='G')
		  	 	 {
		  	 		 index=0;
		  	 		 while(*(ptrgga+index)!='*')
		  	 		 {

		  	 			 GGA_buffer[index]=*(ptrgga+index);
		  	 			 index++;
		  	 		 }
		  	 		GGA_buffer[index]='\0';
		  	 	 }
		  	 	  printf("GGA string is %s\n", GGA_buffer);
		  	 	  gga_length=strlen(GGA_buffer);


	  	 	 if(GGA_buffer!=NULL)
	  	     {
	  	 		  HAL_UART_Transmit(&huart3, GGA_buffer, gga_length, 100);
	  	 		 // HAL_Delay(1000);
	  	 		 for(int i=0;GGA_buffer[i]!='\0';i++)
	  	 		  {
	  	 			  if(GGA_buffer[i]=='M')
	  	 			  {

	  	 				  extract_data_for_GGA(GGA_buffer);
	  	 				  // Generating the JSON string
	  	 				  json_format_for_GGA(json_str);
	  	 			  }
	  	 		  }
	  	     }}
	  	 		  else
	  	 		  {
	  	 			  printf("GGA guffer is empty\n");
	  	 		  }

	  	 	if(ptrrmc=strstr(UART_receive_data,"GPRMC"))
	  	 	{
	  	 	  if(*ptrrmc=='G')
	  	 	   {
	  	 	     index=0;
	  	 	     while(*(ptrrmc+index)!='*')
	  	 		 {

	  	 			  	RMC_buffer[index]=*(ptrrmc+index);
	  	 			  	 index++;
	  	 		 }
	  	 	     RMC_buffer[index]='\0';
	  	 	   }

	  	 	   printf("RMC string is %s\n", RMC_buffer);
	 	 	   rmc_length=strlen(RMC_buffer);
	  	       //HAL_UART_Transmit(&huart2, RMC_buffer,lengthrmc, 1000);
	  	 	    if(RMC_buffer!=NULL)
	  	 		  {
	  	 		  	// HAL_UART_Transmit(&huart2, RMC_buffer, rmc_length, 1000);
	  	 		  	 		 // HAL_Delay(1000);
	  	 		  	 for(int i=0;RMC_buffer[i]!='\0';i++)
	  	 		  	 {
	  	 		  		 if(RMC_buffer[i]=='E')
	  	 		  		 {

	  	 		  			 extract_data_for_RMC(RMC_buffer);

	  	 		  			 // Generating the JSON string
	  	 		  			 json_format_for_RMC(json_str);
	  	 		  		 }
	  	 		  	 }

	  	 		  }	  			// Printing the JSON string

	  	 		  	 	  	 // HAL_Delay(1000);
	  	 		  	 	  	//  memset(rx_data,0,sizeof(rx_data));
	  	 		  }
	  	 	    else
	  	 	    {
	  	 	    	printf("rmc buffer is empty\n");
	  	 	    }

	  	 	  // else
	  	 	   //{
	  	 		 //  flag=1;
	  	 	   //}
	  	 	 /*if(gps_fix==1)
	  	 	 		{
	  	 	 			printf("gps is fixed\n");
	  	 	 		}
	  	 	 		else if(gps_fix==0)
	  	 	 		{
	  	 	 			printf("gps is not fixed\n");
	  	 	 		}*/

	  	 	  //HAL_UART_Transmit(&huart5, "hello", 6, 100);
	  	 	 // printf("transmit data:%s\n",rx_data);
	  	 	  //HAL_Delay(1000);
		  memset(UART_receive_data,0,sizeof(UART_receive_data));
		  memset(GGA_buffer,0,sizeof(GGA_buffer));
		  memset(RMC_buffer,0,sizeof(RMC_buffer));
		//  memset(buf_to_extract,0,sizeof(buf_to_extract));
}
/* USER CODE BEGIN 4 */
void extract_data_for_GGA( char *GGA_buffer)
{

	int tokencnt = 0;
	printf("in gga fun %s\n",GGA_buffer);
	char utc_time[7], latitude_str[10], longitude_str[11],gpsfix[2];
	char *token = strtok(GGA_buffer, ",");
	while (token != NULL && tokencnt < 9)
	{
        //printf("tokencnt=%d\n",tokencnt);
		switch (tokencnt)
		{

			case 1: // Time
				strncpy(utc_time, token, 6);
				utc_time[6] = '\0';
				// Extract hours, minutes, and seconds
								    int utc_hours = atoi(utc_time) / 10000;
								    int utc_minutes = (atoi(utc_time) % 10000) / 100;
								    int utc_seconds = atoi(utc_time) % 100;

								    // Calculate IST (Indian Standard Time) with a time zone offset of 5 hours and 30 minutes
								    int ist_hours = (utc_hours + 5) % 24;
								    int ist_minutes = (utc_minutes + 30) % 60;
								    int ist_seconds = utc_seconds;

								    // Print the result in the format "hrs:mins:secs"
					//			    printf("UTC time: %s\n", utc_time);
								    time = ist_hours * 10000 + ist_minutes * 100 + ist_seconds;
								  //  sprintf(time,"%02d:%02d:%02d\n",ist_hours * 10000 + ist_minutes * 100 + ist_seconds);
					//			    printf("time: %d\n", time);
				break;
			case 2: // Latitude
				strncpy(latitude_str, token, 9);
				latitude_str[9] = '\0';
				latitude = atof(latitude_str);
				//printf("latitude=%f\n",latitude);
				break;
			case 4: // Longitude
				strncpy(longitude_str, token, 10);
				longitude_str[10] = '\0';
				longitude = atof(longitude_str);
				//printf("longitude=%f\n",longitude);
				break;
			case 6: // gps fix
				strncpy(gpsfix,token,1);
				gpsfix[1]='\0';
				gps_fix =atoi(gpsfix);
				//printf("gpsfix=%d\n",gps_fix);
				break;
		}
		token = strtok(NULL, ",");
		tokencnt++;
	}
}


void json_format_for_GGA(char *json_str)
{
	// Creating the JSON string
	sprintf(json_str, "{ \"Time\": %d,\n  \"Latitude\": %0.3f,\n  \"Longitude\": %0.3f\n}\n", time, latitude, longitude);
	//HAL_UART_Transmit(&huart2, (uint8_t *)json_str, strlen(json_str), 1000);
	printf("JSON string format=%s\n",json_str);
}

void extract_data_for_RMC(char *RMC_buffer)
{
	int tokencnt = 0;
	char utc_time[7], latitude_str[10], longitude_str[11],Date_str[7];
	printf("rmc buffer in function = %s\n",RMC_buffer);
	char *token = strtok(RMC_buffer, ",");
	printf("token in function = %s\n",token);
	while (token != NULL && tokencnt < 12)
	{
		switch (tokencnt)
		{
			case 1: // Time
				strncpy(utc_time, token, 6);
				utc_time[6] = '\0';
				//time = atoi(utc_time);
				// Extract hours, minutes, and seconds
				    int utc_hours = atoi(utc_time) / 10000;
				    int utc_minutes = (atoi(utc_time) % 10000) / 100;
				    int utc_seconds = atoi(utc_time) % 100;

				    // Calculate IST (Indian Standard Time) with a time zone offset of 5 hours and 30 minutes
				    int ist_hours = (utc_hours + 5) % 24;
				    int ist_minutes = (utc_minutes + 30) % 60;
				    int ist_seconds = utc_seconds;

				    // Print the result in the format "hrs:mins:secs"
				    //printf("UTC time: %s\n", utc_time);
				    time = ist_hours * 10000 + ist_minutes * 100 + ist_seconds;
				  //  sprintf(time,"%02d:%02d:%02d\n",ist_hours * 10000 + ist_minutes * 100 + ist_seconds);
				    //printf("time: %d\n", time);

				break;
			case 3: // Latitude
				strncpy(latitude_str, token, 9);
				latitude_str[9] = '\0';
				latitude = atof(latitude_str);
				break;
			case 5: // Longitude
				strncpy(longitude_str, token, 10);
				longitude_str[10] = '\0';
				longitude = atof(longitude_str);
				break;
			case 9: // Date
				strncpy(Date_str, token, 7);
                printf("datedhg=%s\n",Date_str);
				Date_str[6] = '\0';
				date = atoi(Date_str);
				printf("date=%06d\n",date);
				break;

		}
		token = strtok(NULL, ",");
		tokencnt++;
		//printf("extracted time,latitude,longitude\n");
	}
}

void json_format_for_RMC(char *json_str)
{
	// Creating the JSON string
	sprintf(json_str, "{\n  \"Date\":%06d,\n  \"Time\": %d,\n  \"Latitude\": %0.3f,\n  \"Longitude\": %0.3f\n}\n", date, time, latitude, longitude);
	//HAL_UART_Transmit(&huart2, (uint8_t *)json_str, strlen(json_str), 1000);
	printf("JSON String:\n%s\n", json_str);
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
