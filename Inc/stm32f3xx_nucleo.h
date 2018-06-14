/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F3XX_NUCLEO_H
#define __STM32F3XX_NUCLEO_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"
#include "spi.h"

extern SPI_HandleTypeDef hspi1;
#define hnucleo_Spi hspi1

#if !defined (USE_STM32F3XX_NUCLEO)
#define USE_STM32F3XX_NUCLEO
#endif

#define NUCLEO_SPIx_TIMEOUT_MAX                   1000
/**
 * @}
 */


/** @defgroup STM32F3XX_NUCLEO_COMPONENT STM32F3XX-NUCLEO COMPONENT
 * @{
 */

/**
 * @brief  SD Control Lines management
 */
#define SD_CS_LOW()       HAL_GPIO_WritePin(SD_CS_GPIO_PORT, SD_CS_PIN, GPIO_PIN_RESET)
#define SD_CS_HIGH()      HAL_GPIO_WritePin(SD_CS_GPIO_PORT, SD_CS_PIN, GPIO_PIN_SET)

/**
 * @brief  SD Control Interface pins (shield D4)
 */

//TODO: cambia sti pin
#define SD_CS_PIN                                 GPIO_PIN_4
#define SD_CS_GPIO_PORT                           GPIOA
#define SD_CS_GPIO_CLK_ENABLE()                 __HAL_RCC_GPIOB_CLK_ENABLE()
#define SD_CS_GPIO_CLK_DISABLE()                __HAL_RCC_GPIOB_CLK_DISABLE()



#ifdef __cplusplus
}
#endif

#endif /* __STM32F3XX_NUCLEO_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

