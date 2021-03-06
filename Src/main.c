
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f3xx_hal.h"
#include "can.h"
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

#define USE_PRINTF 1 //Deve essere 0 se si usa RUN, può essere 1 solo in Debug
extern void initialise_monitor_handles(void);
#if !USE_PRINTF
#define printf(fmt, ...) ;
#define initialise_monitor_handles() ;
#endif

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
int x=0;
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef *hcan){
	flag=!flag;
	//printf("ricevuto\n");
	x++;
	HAL_CAN_Receive_IT(hcan,CAN_FIFO0);

}

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
  MX_CAN_Init();
  /* USER CODE BEGIN 2 */

	// per usare printf ...
	initialise_monitor_handles();

	printf("CAN example:\n");

	//definisco lunghezza dei miei pacchetti
	int dlc = 8;

	//dati da trasmettere
	uint64_t payload = 0xDEADBEEFDEADBEEF;

	// variabile per eventuali codici errore
	HAL_StatusTypeDef canret;

	// struct messaggio da trasmettere
	CanTxMsgTypeDef canmsg;

	//passiamo alla HAL un puntatore al nostro messaggio.
	hcan.pTxMsg = &canmsg;

	// impostiamo il messaggio
	// IDE definisce la lunghezza dell'identificatore (std-11bit vs ext-29bit)
	canmsg.IDE = CAN_ID_STD;
	canmsg.RTR = CAN_RTR_DATA;

	// DLC definisce la lunghezza del pacchetto in Byte: min=0, max=8
	canmsg.DLC = dlc;

	//manderemo 100 messaggi, senza che creiamo un nuovo struct e passiamo il
	// puntatore ogni volta è possibile modificare lo stesso e chiederne l'invio
	for (int i = 0; i<100; i++){

		// scegliamo id per il pacchetto (in questo esempio ogni messaggio ha
		// l'id del precedente decrementato di 1)
		canmsg.StdId = 0x100;

		//metto i dati nel pacchetto (anche se sono sempre gli stessi in questo
		//esempio e potrei farne a meno di ripeterlo)
		memcpy(canmsg.Data, (void*)&payload,dlc);

		// chiediamo la tramsissione del messaggio di cui abbiamo fornito il
		// puntatore in precendenza
		canret = HAL_CAN_Transmit_IT(&hcan);

		//attesa di 1 millisecondo ogni 3 invii
		 HAL_Delay(10);

		//loggando con "candump can0 -td" su raspberry si nota che i pacchetti
		//arrivano a circa 80 uS di distanza se inviati insieme
		//spesso di meno, anche a 30 uS, ma mai più di 100 uS
		//le fluttuazioni sono probabilmente dovute a linux

		// se la funzione fallisce probabilmente abbiamo chiesto troppe
		// trasmissioni in troppo poco tempo, "cassetta delle lettere piena" o
		// "coda piena", dato che al massimo si possono accodare 3 messaggi.
		// non c'è stato tempo per sbrogliare tutte le richieste, forse il bus
		// è troppo impegnato con messaggi di priorità superiore (id inferiore)
		if (canret) printf("canret = %d\n",canret);

	}

	//configura i filtri al sorgente di questa funzione
	setupCANfilter();

	canret = HAL_CAN_Receive_IT(&hcan,CAN_FIFO0);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

	//loop di blink a 10Hz, il led smette/riprende a lampeggiare ogni volta che
	// si riceve il pacchetto che soddisfa uno dei filtri
	while (1)
	{
		HAL_Delay(100);
		if (flag) HAL_GPIO_TogglePin(LD3_GPIO_Port,LD3_Pin);

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
