/**
 ******************************************************************************
 * @file    stm32f3xx_nucleo.c
 * @author  MCD Application Team
 * @brief   This file provides set of firmware functions to manage:
 *          - SD card
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
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
#include "stm32f3xx_nucleo.h"

#define SD_DUMMY_BYTE            0xFF    
#define SD_NO_RESPONSE_EXPECTED  0x80

/*<! Value of Timeout when SPI communication fails */
uint32_t SpixTimeout = NUCLEO_SPIx_TIMEOUT_MAX;

/* SPI IO functions */
static void       SPIx_Write(uint8_t Value);
static uint32_t   SPIx_Read(void);
static void       SPIx_Error(void);

/* SD IO functions */
void              SD_IO_Init(void);
HAL_StatusTypeDef SD_IO_WriteCmd(uint8_t Cmd, uint32_t Arg, uint8_t Crc, uint8_t Response);
HAL_StatusTypeDef SD_IO_WaitResponse(uint8_t Response);
void              SD_IO_WriteDummy(void);
void              SD_IO_CSState(uint8_t state);
void              SD_IO_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLength);
uint8_t           SD_IO_WriteByte(uint8_t Data);
uint8_t           SD_IO_ReadByte(void);

/******************************************************************************
                            BUS OPERATIONS
 *******************************************************************************/

/**
 * @brief  SPI Write a byte to device
 * @param  DataIn value to be written
 * @param  DataOut read value
 * @param  DataLegnth length of data value
 * @retval None
 */
static void SPIx_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLegnth)
{
	HAL_StatusTypeDef status = HAL_OK;

	status = HAL_SPI_TransmitReceive(&hnucleo_Spi, (uint8_t*) DataIn, DataOut, DataLegnth, SpixTimeout);

	/* Check the communication status */
	if(status != HAL_OK)
	{
		/* Execute user timeout callback */
		SPIx_Error();
	}
}
/**
 * @brief  SPI Read 4 bytes from device
 * @retval Read data
 */
static uint32_t SPIx_Read(void)
{
	HAL_StatusTypeDef status = HAL_OK;
	uint32_t readvalue = 0;
	uint32_t writevalue = 0xFFFFFFFF;

	status = HAL_SPI_TransmitReceive(&hnucleo_Spi, (uint8_t*) &writevalue, (uint8_t*) &readvalue, 1, SpixTimeout);

	/* Check the communication status */
	if(status != HAL_OK)
	{
		/* Execute user timeout callback */
		SPIx_Error();
	}

	return readvalue;
}

/**
 * @brief  SPI Write a byte to device.
 * @param  Value value to be written
 * @retval None
 */

static void SPIx_Write(uint8_t Value)
{
	HAL_StatusTypeDef status = HAL_OK;
	uint8_t data;

	status = HAL_SPI_TransmitReceive(&hnucleo_Spi, (uint8_t*) &Value, &data, 1, SpixTimeout);

	/* Check the communication status */
	if(status != HAL_OK)
	{
		/* Execute user timeout callback */
		SPIx_Error();
	}
}

/**
 * @brief  SPI error treatment function
 * @retval None
 */
static void SPIx_Error (void)
{
	/* De-initialize the SPI communication BUS */
	HAL_SPI_DeInit(&hnucleo_Spi);

	/* Re-Initiaize the SPI communication BUS */
	MX_SPI1_Init();
}
/**
 * @}
 */

/** @defgroup STM32F3XX_NUCLEO_LINK_OPERATIONS Link Operation functions
 * @{
 */

/******************************************************************************
                            LINK OPERATIONS
 *******************************************************************************/

/********************************* LINK SD ************************************/
/**
 * @brief  Initializes the SD Card and put it into StandBy State (Ready for
 *         data transfer).
 * @retval None
 */
void SD_IO_Init(void)
{
	uint8_t counter;

	/* SD chip select high */
	SD_CS_HIGH();

	/* Send dummy byte 0xFF, 10 times with CS high */
	/* Rise CS and MOSI for 80 clocks cycles */
	for (counter = 0; counter <= 9; counter++)
	{
		/* Send dummy byte 0xFF */
		SD_IO_WriteByte(SD_DUMMY_BYTE);
	}
}

/**
 * @brief  Set the SD_CS pin.
 * @param  val pin value.
 * @retval None
 */
void SD_IO_CSState(uint8_t val)
{
	if(val == 1)
	{
		SD_CS_HIGH();
	}
	else
	{
		SD_CS_LOW();
	}
}

/**
 * @brief  Write a byte on the SD.
 * @param  DataIn byte to be written
 * @param  DataOut read value
 * @param  DataLength length of data value
 * @retval None
 */
void SD_IO_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLength)
{
	//  /* SD chip select low */
	//  SD_CS_LOW();
	/* Send the byte */
	SPIx_WriteReadData(DataIn, DataOut, DataLength);
}

/**
 * @brief  Reads a byte from the SD.
 * @retval The received byte.
 */
uint8_t SD_IO_ReadByte(void)
{
	uint8_t data = 0;

	/* Get the received data */
	data = SPIx_Read();

	/* Return the shifted data */
	return data;
}

/**
 * @brief  Sends 5 bytes command to the SD card and get response
 * @param  Cmd The user expected command to send to SD card.
 * @param  Arg The command argument.
 * @param  Crc The CRC.
 * @param  Response Expected response from the SD card
 * @retval HAL_StatusTypeDef HAL Status
 */
HAL_StatusTypeDef SD_IO_WriteCmd(uint8_t Cmd, uint32_t Arg, uint8_t Crc, uint8_t Response)
{
	uint32_t counter = 0x00;
	uint8_t frame[6];

	/* Prepare Frame to send */
	frame[0] = (Cmd | 0x40);         /* Construct byte 1 */
	frame[1] = (uint8_t)(Arg >> 24); /* Construct byte 2 */
	frame[2] = (uint8_t)(Arg >> 16); /* Construct byte 3 */
	frame[3] = (uint8_t)(Arg >> 8);  /* Construct byte 4 */
	frame[4] = (uint8_t)(Arg);       /* Construct byte 5 */
	frame[5] = (Crc);                /* Construct byte 6 */

	/* SD chip select low */
	SD_CS_LOW();

	/* Send Frame */
	for (counter = 0; counter < 6; counter++)
	{
		SD_IO_WriteByte(frame[counter]); /* Send the Cmd bytes */
	}

	if(Response != SD_NO_RESPONSE_EXPECTED)
	{
		return SD_IO_WaitResponse(Response);
	}

	return HAL_OK;
}

/**
 * @brief  Waits response from the SD card
 * @param  Response Expected response from the SD card
 * @retval HAL_StatusTypeDef HAL Status
 */
HAL_StatusTypeDef SD_IO_WaitResponse(uint8_t Response)
{
	uint32_t timeout = 0xFFFF;

	/* Check if response is got or a timeout is happen */
	while ((SD_IO_ReadByte() != Response) && timeout)
	{
		timeout--;
	}

	if (timeout == 0)
	{
		/* After time out */
		return HAL_TIMEOUT;
	}
	else
	{
		/* Right response got */
		return HAL_OK;
	}
}

/**
 * @brief  Writes a byte on the SD.
 * @param  Data byte to send.
 * @retval None
 */
uint8_t SD_IO_WriteByte(uint8_t Data)
{
	uint8_t tmp;
	/* Send the byte */
	SPIx_WriteReadData(&Data,&tmp,1);
	return tmp;
}

/**
 * @brief  Sends dummy byte with CS High
 * @retval None
 */
void SD_IO_WriteDummy(void)
{
	/* SD chip select high */
	SD_CS_HIGH();

	/* Send Dummy byte 0xFF */
	SD_IO_WriteByte(SD_DUMMY_BYTE);
}
