/*
 * C
 *
 * Copyright 2018-2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */

/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include "LLCOMM_SPI.h"
#include "LLCOMM_impl.h"
#include "microej.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/xtensa_api.h"
#include "interrupts.h"

/* Methods declaration -------------------------------------------------------*/

void LLCOMM_UART_new(LLCOMM_CUSTOM_CONNECTION* env);

/* Enumerations --------------------------------------------------------------*/

/*
 * Defines the mapping between the HAL SPI instance and MicroEJ LLCOMM_SPIs
 * table. The indexes are 0-based.
 */
typedef enum
{
	SPI2_INDEX = 0,
} hal_to_microej_spi_table;

/* Fields --------------------------------------------------------------------*/

/*
 * Array of all static LLCOMM SPIs.
 */
static LLCOMM_UART LLCOMM_SPIs[1];

static SemaphoreHandle_t sync_connections_sem;

/* Private functions ---------------------------------------------------------*/
/*
 * Initialize LLCOMM_SPIs array for a specific instance and initialize
 * corresponding MicroEJ LLCOMM_UART
 */
static void COMMS_initialize_llcomm_spi(int32_t index, const spi_host_device_t spi_host)
{
	// store SPI instance
	LLCOMM_SPIs[index].spi_host = spi_host;

	// create MicroEJ SPI connection
	LLCOMM_UART_new(&LLCOMM_SPIs[index].header);

	// add MicroEJ SPI connection into MicroEJ framework as generic connection
	LLCOMM_addConnection((LLCOMM_CONNECTION*)&LLCOMM_SPIs[index].header);
}

/* Interrupt functions -------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/

/*
 * The semaphore must be created before the first call of LLCOMM_IMPL_syncConnectionsEnter()
 * which is called BEFORE LLCOMM_IMPL_initialize().
 */
void LLCOMM_stack_initialize(void)
{
	vSemaphoreCreateBinary(sync_connections_sem);	
	xSemaphoreGive(sync_connections_sem);
}

void LLCOMM_IMPL_initialize(void)
{
	COMMS_initialize_llcomm_spi(SPI2_INDEX, HSPI_HOST);
}

void LLCOMM_IMPL_syncConnectionsEnter(void)
{
	xSemaphoreTake(sync_connections_sem, 0);
}

void LLCOMM_IMPL_syncConnectionsExit(void)
{
	if (interrupt_is_in() == MICROEJ_TRUE)
	{
		portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
		xSemaphoreGiveFromISR(sync_connections_sem, &xHigherPriorityTaskWoken);
		if (xHigherPriorityTaskWoken != pdFALSE)
		{
			// Force a context switch here.
			portYIELD_FROM_ISR();
		}
	}
	else
	{
		xSemaphoreGive(sync_connections_sem);
	}
}
