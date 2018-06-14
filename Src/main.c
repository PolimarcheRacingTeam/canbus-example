
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * This notice applies to any and all portions of this file
 * that are not between comment pairs USER CODE BEGIN and
 * USER CODE END. Other portions of this file, whether
 * inserted by the user or by software development tools
 * are owned by their respective copyright owners.
 *
 * Copyright (c) 2018 STMicroelectronics International N.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted, provided that the following conditions are met:
 *
 * 1. Redistribution of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of other
 *    contributors to this software may be used to endorse or promote products
 *    derived from this software without specific written permission.
 * 4. This software, including modifications and/or derivative works of this
 *    software, must execute solely and exclusively on microcontroller or
 *    microprocessor devices manufactured by or for STMicroelectronics.
 * 5. Redistribution and use of this software other than as permitted under
 *    this license is void and will automatically terminate your rights under
 *    this license.
 *
 * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
 * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
 * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f3xx_hal.h"
#include "can.h"
#include "fatfs.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include <string.h> //per usare memcpy

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
int flag = 1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
FATFS SDFatFs;  /* File system object for SD card logical drive */
FIL MyFile;     /* File object */
char SDPath[4]; /* SD card logical drive path */

#define USE_PRINTF 1 //Deve essere 0 se si usa RUN, può essere 1 solo in Debug
extern void initialise_monitor_handles(void);
#if !USE_PRINTF
#define printf(fmt, ...) ;
#define initialise_monitor_handles() ;
#endif

void SDInit(void);
void SDFormat(void);
void SDNew(char new[]);
void SDClose(void);

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
int x=0;
int ciclo1 = 0;
int sticazzi;

FRESULT res;


/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 *
 * @retval None
 */
int main(void)
{
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration----------------------------------------------------------*/

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
	MX_FATFS_Init();
	MX_SPI1_Init();
	MX_CAN_Init();
	/* USER CODE BEGIN 2 */
	//configura i filtri al sorgente di questa funzione
	setupCANfilter();

	// per usare printf ...
	initialise_monitor_handles();

	if (HAL_CAN_Receive_IT(&hcan, CAN_FIFO0) != HAL_OK) // Abilita la ricezione del CAN tramite Interrupt
	{
		/* Reception Error */\
		Error_Handler();
	}

	SDInit();
	SDFormat();
	SDNew("File.txt");

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		// Qu� si memorizzano i dati da inviare, 8bit per data, fino ad un massimo
		// di Data[8] per un totale di 8 byte
		sticazzi++;
		//		hcan.pTxMsg->Data[0] = 0b1010;
		//		hcan.pTxMsg->Data[1] = 0b0110;

		/*##-3- Inizia il processo di trasmissione ###############################*/
		//		if ((res = HAL_CAN_Transmit(&hcan, 10)) != HAL_OK)
		//		{
		/* Transmition Error */
		//			Error_Handler();
		//		}
	}

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */

	//loop di blink a 10Hz, il led smette/riprende a lampeggiare ogni volta che
	// si riceve il pacchetto che soddisfa uno dei filtri
	/* USER CODE END 3 */

}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure the Systick interrupt time
	 */
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	/**Configure the Systick
	 */
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

void SDInit()
{
	/*##-2- Register the file system object to the FatFs module ##############*/
	printf("SDInit->");
	res = f_mount(&SDFatFs, (TCHAR const*)SDPath, 0);
	if(res != FR_OK)
	{
		/* FatFs Initialization Error */
		Error_Handler();
	}
	else
	{
		printf("OK\n");
	}
}

void SDFormat()
{
	/*##-3- Create a FAT file system (format) on the logical drive #########*/
	/* WARNING: Formatting the uSD card will delete all content on the device */
	printf("SDFormat->");
	res = f_mkfs((TCHAR const*)SDPath, 0, 0);
	if(res != FR_OK)
	{
		/* FatFs Format Error */
		Error_Handler();
	}
	else
	{
		printf("OK\n");
	}
}

void SDNew(char new[])
{
	/*##-4- Create and Open a new text file object with write access #####*/
	printf("SDNew->");
	res = f_open(&MyFile, new, FA_CREATE_ALWAYS | FA_WRITE);
	if(res != FR_OK)
	{
		/* 'STM32.TXT' file Open for write Error */
		Error_Handler();
	}
	else
	{
		printf("OK\n");
	}
}

void SDClose()
{
	/*##-6- Close the open text file #################################*/
	printf("SDClose->");
	res = f_close(&MyFile);
	if (res != FR_OK )
	{
		/* 'STM32.TXT' file Open for write Error */
		Error_Handler();
	}
	else
	{
		printf("OK\n");
	}
}
// Questa funzione � un Interrupt e viene letta ogni volta che il CAN riceve un messaggio

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef *hcan)
{
	ciclo1++;
	  if(ciclo1 < 5)
  {
	  f_printf(&MyFile,"%d  ",ciclo1);
	  f_printf(&MyFile,"%x ",hcan->pRxMsg->Data[0]);
	  f_printf(&MyFile,"%x  ",hcan->pRxMsg->Data[1]);
	  f_printf(&MyFile,"%x ",hcan->pRxMsg->StdId);
	  f_printf(&MyFile,"%d\n",hcan->pRxMsg->DLC);
  }
	//  else if ((hcan->pRxMsg->StdId == 0x642) && (hcan->pRxMsg->Data[0] == 0b1111) && (hcan->pRxMsg->Data[1] == 0b0000))
	//  {
	//	  SDClose();
	//  }
	  else
	  {
		  SDClose();
		  printf("\nclose");
	  }
	HAL_CAN_Receive_IT(hcan,CAN_FIFO0);
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  file: The file name as string.
 * @param  line: The line in file as a number.
 * @retval None
 */
void _Error_Handler(char *file, int line)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	while(1)
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
void assert_failed(uint8_t* file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
