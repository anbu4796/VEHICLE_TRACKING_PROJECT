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
#include <stdlib.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "gps.h"
#include "gsm.h"
#include "internal_flash.h"
#include "stdio.h"
#include "string.h"
#define JSON_SIZE 256
#define NMEA_SIZE 750
#define FLASH_START_ADDRESS 0x08080000UL
#define OFFESET 56
#define Buffer_For_Read 100
#define Buffer_for_Stored_Loc 100
#define Buffer_For_Send 100


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

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_UART4_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
extern char json_str[JSON_SIZE];
extern char buf_to_extract[NMEA_SIZE];
extern char RxBuffer[NMEA_SIZE];
extern char readData[Buffer_For_Read];
int Read_Location=0,Write_Location=0,No_Of_Locations=0;
int ret=1;
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
  MX_SPI1_Init();
  MX_UART4_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */

  eraseFlashPage(FLASH_START_ADDRESS); // Erasing Flash
  while (1)
  {
    /*/* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  if(DATA_SEND_READY()==0)
	  {
		  	 if(No_Of_Locations!=0) // Block for sending stored locations to server
		  	 {
		  		Read_Location=0; // Read location counter making zero
		  		char Buffer_to_Send[Buffer_for_Stored_Loc]; // Buffer to Send the stored locations
		  		 	 for(uint8_t instance=0;instance < No_Of_Locations;instance++)
		  		 	 {
		  		 		readDataFromFlash((FLASH_START_ADDRESS)+(OFFESET*Read_Location),readData,OFFESET);
		  		 		HAL_Delay(500); // Random Delay to read
		  		 		strcpy(Buffer_to_Send,readData);

		  		 		printf("Reading Done ");
		  		 		printf("Read data : %s\n ",Buffer_to_Send);
SEND_AGAIN:		  		ret=GSM_Send_Data((uint8_t *)Buffer_to_Send);
		  		 		if(ret==0)
		  		 		{
		  		 			Read_Location++; // Increment the read location counter
		  		 		printf("No of locations sent to server after server got connected %d\n",Read_Location);

		  		 		}
		  		 		else
		  		 		{
		  		 			printf("Restoring from flash to server was failed\n,Check the server \n");
		  		 			goto SEND_AGAIN;
		  		 		}
		  		 	 }
		  		 	 Read_Location=0;
		  		 	 No_Of_Locations=0;
		  		 	 Write_Location=0;
		  		 	 eraseFlashPage(FLASH_START_ADDRESS); // Erasing the flash after send the stored locations to server

		  	  }

	  	  	 		while(1)  /// Sending GPS data to server
	  	  	 		{
	  	  	 		  if(HAL_UART_Receive_IT(&huart1, (unsigned char *)RxBuffer, 750)==HAL_OK)
	  	  	 		  {
	  	  	 			printf("Received NMEA %s\n",RxBuffer);
	  	  	 			strcpy(buf_to_extract,RxBuffer);
	  	  	 			memset(RxBuffer,0,NMEA_SIZE);
	  	  	 			get_data_from_gps(buf_to_extract);


	  	  	 			if(*json_str)
	  	  	 			{
	  	  	 				printf("GPS Received %s \n",json_str);
	  	  	 				if(GSM_Send_Data((uint8_t *)json_str)==0)
	  	  	 				{
	  	  	 					printf("Sending worked \n");

	  	  	 				}
	  	  	 				else
	  	  	 				{
	  	  	 					printf("Sending  not worked \n");

	  	  	 				}
	  	  	 			}
	  	  	 			else
	  	  	 			{
	  	  	 				printf("GPS Received No DATA\n");

	  	  	 			}
	  	  	 			memset(json_str,0,JSON_SIZE);


	  	  	 		  }
	  	  	 		}
	  	  	 } //  GSM Sending block ends here
	  	  	 else
	  	  	 {
	  	  		 printf("GSM Init Failed in Main\n Storing in Flash \n");
 			  	  	//eraseFlashPage(FLASH_START_ADDRESS);

	  	  		while(1)
	  	  		{
	  	  			uint8_t json_str_len=0;
	  	  			if(HAL_UART_Receive_IT(&huart1,(unsigned char *)RxBuffer, 750)==HAL_OK)
	  	  			{

	  	  				printf("Received NMEA %s\n",RxBuffer);
	  	  			  	strcpy(buf_to_extract,RxBuffer);
	  	  			  	memset(RxBuffer,0,NMEA_SIZE);
	  	  			  	get_data_from_gps(buf_to_extract);

	  	  			  	  if(*json_str)
	  	  			  		{
	  	  			  	  	 printf("GPS Received %s \n",json_str);
	  	  			  	  	 json_str_len=strlen(json_str);
	  	  			  	  	 printf(" json string length %d",json_str_len);
	  	  			  	  // Write the json string into the flash
	  	  	  		  	  	 writeStringToFlash((FLASH_START_ADDRESS)+(OFFESET*Write_Location),(char * )json_str);
	  	  	  		  	  	 Write_Location++;

	  	  			  	  	 HAL_Delay(1000);
	  	  			  	  	 printf("Writing into flash done\n");
	  	  			  	  	 	 readDataFromFlash((FLASH_START_ADDRESS)+(OFFESET*Read_Location),readData,OFFESET);
	  	  			  	  		 HAL_Delay(500);
	  	  			  	  		 Read_Location++;
	  	  			  	  		 printf("Reading Done ");
	  	  			  	  		 printf("Read data : %s\n ",readData);
	  	  			  	  		 printf("Read PTR : %s\n ",json_str);
	  	  			  	  		 char Buffer_to_Send[Buffer_For_Send];
	  	  			  	  		 strcpy(Buffer_to_Send,readData);
	  	  			  	  		 if(strncmp(Buffer_to_Send,json_str,json_str_len)==0)
	  	  			  	  		 {
	  	  			  	  			 printf("Storing worked \n");
	  	  			  	  			 printf("Read from flash :%d :%s",Read_Location,readData);
	  	  			  	  			 No_Of_Locations++;
	  	  			  	  			 break;

	  	  			  	  		 }
	  	  			  	  		 else
	  	  			  	  		 {
	  	  			  	  			 printf("Storing  not worked \n");

	  	  			  	  		 }

	  	  			  	 }
	  	  			  	  else
	  	  			  	  {
	  	  			  	  	  printf("GPS Received No DATA\n");

	  	  			  	  }
	  	  			  	  	memset(json_str,0,JSON_SIZE);

	  	  			}
	  	  		}



	  	  	 }

	  } // Main While ends here


} // Main function ends here



  /* USER CODE END 3 */



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
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_4BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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
