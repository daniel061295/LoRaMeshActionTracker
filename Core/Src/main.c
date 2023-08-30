/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "LoRa.h"
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
LoRa myLoRa;
uint16_t LoRa_stat = 0;
uint8_t RxBuffer[128];



int RSSI ;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
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
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  myLoRa = newLoRa();

  myLoRa.CS_port         = NSS_GPIO_Port;
  myLoRa.CS_pin          = NSS_Pin;
  myLoRa.reset_port      = RST_GPIO_Port;
  myLoRa.reset_pin       = RST_Pin;
  myLoRa.DIO0_port       = DIO0_GPIO_Port;
  myLoRa.DIO0_pin        = DIO0_Pin;
  myLoRa.hSPIx           = &hspi1;

  myLoRa.frequency             = 868;             // default = 433 MHz
  myLoRa.spredingFactor        = SF_7;            // default = SF_7
  myLoRa.bandWidth             = BW_125KHz;       // default = BW_125KHz
  myLoRa.crcRate               = CR_4_5;          // default = CR_4_5
  myLoRa.power                 = POWER_20db;      // default = 20db
  myLoRa.overCurrentProtection = 130;             // default = 100 mA
  myLoRa.preamble              = 9;              // default = 8;

//  LoRa_reset(&myLoRa);
  if (LoRa_init(&myLoRa) == LORA_OK ){
	  LoRa_stat = 1;
  }

  LoRa_startReceiving(&myLoRa);

//  uint8_t TxBuffer[16] = "Hola desde stm32" ;
//  uint8_t TxBuffer[4];
//  TxBuffer[0] = 'H';
//  TxBuffer[1] = 'O';
//  TxBuffer[2] = 'l';
//  TxBuffer[3] = 'a';
//
//  LoRa_transmit(&myLoRa, TxBuffer, 4, 500);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {

	HAL_UART_Transmit(&huart1, (uint8_t *)"Enviando mensaje\n", 17, HAL_MAX_DELAY);
	LoRa_sendPing(&myLoRa);
	HAL_UART_Transmit(&huart1, (uint8_t *)"Esperando respuesta...\n", 23, HAL_MAX_DELAY);
	HAL_Delay(1000);




    /* USER CODE END WHILE */

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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL5;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == DIO0_Pin){
	LoRa_receive(&myLoRa, RxBuffer, 128);
	RSSI = LoRa_getRSSI(&myLoRa);
	}
	uint8_t uartBuff[128];
	int len_message = sizeof(RxBuffer) / sizeof(RxBuffer[0]);
	char message[len_message];
	for (int i=0;i<len_message;i++){
		message[i] = (char)RxBuffer[i];
	}
	message[len_message] = '\0';
	sprintf(uartBuff, "%s rssi:%d\n",message,RSSI);
	HAL_UART_Transmit(&huart1, (uint8_t *)uartBuff, strlen(uartBuff), HAL_MAX_DELAY);

	char command[20];
	int id, rssi;
	LoRa_getNeighborRssi(message,command, &id, &rssi);

	if(strcmp(command,"ANSPING")==0){
		sprintf(uartBuff, "ID: %d rssi: %d\n",id, rssi);
		HAL_UART_Transmit(&huart1, (uint8_t *)uartBuff, strlen(uartBuff), HAL_MAX_DELAY);
		LoRa_deleteAll();
		LoRa_addNeighbor(id, rssi);
//		LoRa_printNeighbors();
	}
	if(strcmp(command,"PING")==0){

	    uint8_t response_ping[20];
	    sprintf(response_ping,"ANSPING;%d;%d",LORA_NODE_ID,RSSI);
	    HAL_UART_Transmit(&huart1, (uint8_t *)response_ping, strlen(response_ping), HAL_MAX_DELAY);
	    HAL_Delay(1000);
		LoRa_transmit(&myLoRa, response_ping, strlen(response_ping), 1000);
		HAL_UART_Transmit(&huart1, (uint8_t *)"\nMensaje enviado", 16, HAL_MAX_DELAY);

	}

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
