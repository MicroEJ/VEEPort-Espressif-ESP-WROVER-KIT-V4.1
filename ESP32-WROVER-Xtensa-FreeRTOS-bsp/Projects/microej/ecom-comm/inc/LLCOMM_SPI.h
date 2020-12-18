/*
 * C
 *
 * Copyright 2018-2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */

#ifndef _LLCOMM_SPI
#define _LLCOMM_SPI

/* Includes ------------------------------------------------------------------*/

#include "LLCOMM_CUSTOM_CONNECTION.h"
#include "LLCOMM_SPI_configuration.h"
#include "comm_spi.h"

/* Defines and Macros --------------------------------------------------------*/

/*
 * Defines the software FIFO size for transmission used by MicroEJ framework.
 * Default size is 5 bytes.
 */
#ifndef LLCOMM_SPI_TX_BUFFER_SIZE
#define LLCOMM_SPI_TX_BUFFER_SIZE (COMM_SPI_MAX_DMA_LEN)
#endif

/*
 * Defines the software FIFO size for reception used by MicroEJ framework
 * Default size is 1024 bytes.
 */
#ifndef LLCOMM_SPI_RX_BUFFER_SIZE
#define LLCOMM_SPI_RX_BUFFER_SIZE (COMM_SPI_MAX_DMA_LEN)
#endif

/* Structures ----------------------------------------------------------------*/

typedef struct LLCOMM_UART
{
	// MicroEJ LLCOMM connection header
	struct LLCOMM_CUSTOM_CONNECTION header;

	// HAL SPI instance
	spi_host_device_t spi_host;

	// Software FIFO size for transmission used by MicroEJ framework
	uint8_t txBuffer[LLCOMM_SPI_TX_BUFFER_SIZE];

	// Software FIFO size for reception used by MicroEJ framework
	uint8_t rxBuffer[LLCOMM_SPI_RX_BUFFER_SIZE];

} LLCOMM_UART;

/* Public functions ----------------------------------------------------------*/

/*
 * Generic SPI interrupt callback
 */
void LLCOMM_SPI_callback(LLCOMM_UART* llcomm);

/* EOF -----------------------------------------------------------------------*/

#endif // _LLCOMM_SPI
