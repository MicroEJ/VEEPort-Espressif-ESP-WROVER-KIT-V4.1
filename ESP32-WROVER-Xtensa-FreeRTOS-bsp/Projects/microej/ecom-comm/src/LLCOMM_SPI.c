/*
 * C
 *
 * Copyright 2018-2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */

/* Includes ------------------------------------------------------------------*/

#include <string.h>
#include "LLCOMM_SPI.h"
#define   LLCOMM_CUSTOM_CONNECTION_IMPL LLCOMM_UART
#include "LLCOMM_CUSTOM_CONNECTION_impl.h"
#include "interrupts.h"
#include "comm_spi.h"
#include "soc/rtc_cntl_reg.h"
#include "LLMJVM.h"

// Un-comment the below define to enable SPI Master task debugging
//#define LLCOMM_SPI_DEBUG

/* SPI Master task constants */
#define SPI_STACK_SIZE                    (7 * 1024)
#define SPI_TASK_PRIORITY                 ( 9 ) /** Should be > tskIDLE_PRIORITY & < configTIMER_TASK_PRIORITY */
#define SPI_TASK_STACK_SIZE               (SPI_STACK_SIZE / 4)
#define SPI_TASK_NAME                     "SPI Master"

/* Local variables */
uint16_t readIndex;
uint16_t writeIndex;
int32_t availableData;

/* Interrupt callback --------------------------------------------------------*/

static void spi_master_thread_main(void * pvParameters);

/* API -----------------------------------------------------------------------*/

void LLCOMM_CUSTOM_CONNECTION_IMPL_initialize(LLCOMM_CUSTOM_CONNECTION* env)
{
	microej_spi_master_config();

	// Create SPI master thread
	xTaskCreate(spi_master_thread_main, SPI_TASK_NAME, SPI_TASK_STACK_SIZE, (void *)env, SPI_TASK_PRIORITY, NULL);
}

int32_t LLCOMM_CUSTOM_CONNECTION_IMPL_getPlatformId(LLCOMM_CUSTOM_CONNECTION* env)
{
	// Platform ID is the spi_host number, but 1-based
	return (((LLCOMM_UART *)env)->spi_host + 1);
}

void* LLCOMM_CUSTOM_CONNECTION_IMPL_getName(LLCOMM_CUSTOM_CONNECTION* env)
{
	// This kind of connection cannot be open from a name (only from a platform port number)
	return 0;
}

int32_t LLCOMM_CUSTOM_CONNECTION_IMPL_configureDevice(LLCOMM_CUSTOM_CONNECTION* env, int32_t baudrate, int32_t bitsperchar, int32_t stopbits, int32_t parity)
{
	// Spi connection cannot be configured with uart parameters
	return LLCOMM_CUSTOM_CONNECTION_SUCCESS;
}

void LLCOMM_CUSTOM_CONNECTION_IMPL_enableRX(LLCOMM_CUSTOM_CONNECTION* env)
{
	// esp-idf doesn't provide API for enableRX
}

void LLCOMM_CUSTOM_CONNECTION_IMPL_disableRX(LLCOMM_CUSTOM_CONNECTION* env)
{
	// esp-idf doesn't provide API for disableRX
}

void LLCOMM_CUSTOM_CONNECTION_IMPL_enableRXDeviceInterrupt(LLCOMM_CUSTOM_CONNECTION* env)
{
	// esp-idf doesn't provide API for enableRXDeviceInterrupt
}

void LLCOMM_CUSTOM_CONNECTION_IMPL_disableRXDeviceInterrupt(LLCOMM_CUSTOM_CONNECTION* env)
{
	// esp-idf doesn't provide API for disableRXDeviceInterrupt
}

void LLCOMM_CUSTOM_CONNECTION_IMPL_enableTX(LLCOMM_CUSTOM_CONNECTION* env)
{
	// esp-idf doesn't provide API for enableTX
}

void LLCOMM_CUSTOM_CONNECTION_IMPL_disableTX(LLCOMM_CUSTOM_CONNECTION* env)
{
	// esp-idf doesn't provide API for disableTX
}

void LLCOMM_CUSTOM_CONNECTION_IMPL_enableTXDeviceInterrupt(LLCOMM_CUSTOM_CONNECTION* env)
{
	// esp-idf doesn't provide API for enableTXDeviceInterrupt
}

void LLCOMM_CUSTOM_CONNECTION_IMPL_disableTXDeviceInterrupt(LLCOMM_CUSTOM_CONNECTION* env)
{
	// esp-idf doesn't provide API for disableTXDeviceInterrupt
}

int32_t LLCOMM_CUSTOM_CONNECTION_IMPL_getNextRXData(LLCOMM_CUSTOM_CONNECTION* env)
{
	int32_t readValue = -1;

	// The application developer has to be aware of the ECOM-COMM implementation over SPI!
	// The LLCOM_SPI* API is designed to work on the full-duplex feature of the SPI bus architecture:
	// - when a write request is coming, a "transfer" is made on the SPI bus, meaning that at the same time the data is received
	//   from the connected peripheral, which is kept in the Rx buffer
	// - when a read request is coming, the data is already present, so just fetch it from the Rx buffer
	// - a new write request will overwrite any previously received data with new received data
	// Dedicated half-duplex read/write would need custom APIs and are currently not supported by the LLCOM_SPI module
	if (readIndex < LLCOMM_SPI_RX_BUFFER_SIZE) {
		// When data is available, read it
		xSemaphoreTake(platform_spi_master_drivers[ESP32_SPI2].transfer_pending, portMAX_DELAY);
		readValue = ((LLCOMM_UART *)env)->rxBuffer[readIndex++];
		availableData--;
		xSemaphoreGive(platform_spi_master_drivers[ESP32_SPI2].transfer_pending);
	}

	return (int32_t)readValue;
}

int32_t LLCOMM_CUSTOM_CONNECTION_IMPL_initializeRXData(LLCOMM_CUSTOM_CONNECTION* env)
{
	memset(((LLCOMM_UART *)env)->rxBuffer, 0, LLCOMM_SPI_RX_BUFFER_SIZE);
	return LLCOMM_CUSTOM_CONNECTION_SUCCESS;
}

int32_t LLCOMM_CUSTOM_CONNECTION_IMPL_initializeTXData(LLCOMM_CUSTOM_CONNECTION* env)
{
	memset(((LLCOMM_UART *)env)->txBuffer, 0, LLCOMM_SPI_RX_BUFFER_SIZE);
	return LLCOMM_CUSTOM_CONNECTION_SUCCESS;
}

int32_t LLCOMM_CUSTOM_CONNECTION_IMPL_isRXDataAvailable(LLCOMM_CUSTOM_CONNECTION* env)
{
	return availableData;
}

int32_t LLCOMM_CUSTOM_CONNECTION_IMPL_sendData(LLCOMM_CUSTOM_CONNECTION* env, int32_t b)
{
	if (writeIndex > LLCOMM_SPI_TX_BUFFER_SIZE) {
		return LLCOMM_CUSTOM_CONNECTION_TRANSMISSION_ERROR;
	} else {
		// Wait for any ongoing transfer to be finished and prepare the new one
		xSemaphoreTake(platform_spi_master_drivers[ESP32_SPI2].transfer_pending, portMAX_DELAY);
		((LLCOMM_UART *)env)->txBuffer[writeIndex++] = (uint8_t) b;
		xSemaphoreGive(platform_spi_master_drivers[ESP32_SPI2].transfer_pending);

		// Prepare the spi thread for data transmitting and unlock it
		platform_spi_master_drivers[ESP32_SPI2].state = SPI_MASTER_STATE_TRANSMIT_DATA;
		xSemaphoreGive(platform_spi_master_drivers[ESP32_SPI2].transfer_ready);
		return LLCOMM_CUSTOM_CONNECTION_TRANSMISSION_COMPLETE;
	}
}

int32_t LLCOMM_CUSTOM_CONNECTION_IMPL_isTransmissionComplete(LLCOMM_CUSTOM_CONNECTION* env)
{
	xSemaphoreTake(platform_spi_master_drivers[ESP32_SPI2].transfer_pending, portMAX_DELAY);
	xSemaphoreGive(platform_spi_master_drivers[ESP32_SPI2].transfer_pending);
	return LLCOMM_CUSTOM_CONNECTION_TRANSMISSION_COMPLETE;
}

int32_t LLCOMM_CUSTOM_CONNECTION_IMPL_getNbProperties(LLCOMM_CUSTOM_CONNECTION* env)
{
	// This implementation does not provide Device properties
	return 0;
}

void* LLCOMM_CUSTOM_CONNECTION_IMPL_getProperty(LLCOMM_CUSTOM_CONNECTION* env, void* propertyName)
{
	// This implementation does not provide Device properties
	return 0;
}

void* LLCOMM_CUSTOM_CONNECTION_IMPL_getPropertyName(LLCOMM_CUSTOM_CONNECTION* env, int32_t propertyId)
{
	// This implementation does not provide Device properties
	while(1);
}

void LLCOMM_CUSTOM_CONNECTION_IMPL_released(LLCOMM_CUSTOM_CONNECTION* env)
{
	// This implementation does not support dynamic connections add/removal.
	// To enable dynamic connections, synchronization functions must be implemented.
	// See LLCOMM_impl.h
	while(1);
}

int32_t LLCOMM_CUSTOM_CONNECTION_IMPL_sendBytes(LLCOMM_CUSTOM_CONNECTION* env, int8_t* buffer, int32_t offset, int32_t length)
{
	if ((length + writeIndex) > LLCOMM_SPI_TX_BUFFER_SIZE) {
		return LLCOMM_CUSTOM_CONNECTION_TRANSMISSION_ERROR;
	} else {
		// Wait for any ongoing transfer to be finished and prepare the new one
		xSemaphoreTake(platform_spi_master_drivers[ESP32_SPI2].transfer_pending, portMAX_DELAY);
		memcpy(&((LLCOMM_UART *)env)->txBuffer[writeIndex], &buffer[offset], length);
		writeIndex += length;
		xSemaphoreGive(platform_spi_master_drivers[ESP32_SPI2].transfer_pending);

		// Prepare the spi thread for data transmitting and unlock it
		platform_spi_master_drivers[ESP32_SPI2].state = SPI_MASTER_STATE_TRANSMIT_DATA;
		xSemaphoreGive(platform_spi_master_drivers[ESP32_SPI2].transfer_ready);
		return LLCOMM_CUSTOM_CONNECTION_SUCCESS;
	}
}

static void spi_master_thread_main(void * pvParameters)
{
	LLCOMM_CUSTOM_CONNECTION* env = (LLCOMM_CUSTOM_CONNECTION*)pvParameters;

	// Implement the spi thread as a simple state machine, waiting in idle for a transfer to be ready,
	// and when ready processing it and going back to idle
	while (1) {
		switch (platform_spi_master_drivers[ESP32_SPI2].state) {
		case SPI_MASTER_STATE_IDLE: {
#ifdef LLCOMM_SPI_DEBUG
			printf("[INFO] %s - SPI_MASTER_STATE_IDLE\n", __func__);
#endif
			xSemaphoreTake(platform_spi_master_drivers[ESP32_SPI2].transfer_ready, portMAX_DELAY);
			break;
		}
		case SPI_MASTER_STATE_READ_DATA: {
#ifdef LLCOMM_SPI_DEBUG
			printf("[INFO] %s - SPI_MASTER_STATE_READ_DATA\n", __func__);
#endif
			LLSPI_open();
			LLSPI_read(((LLCOMM_UART *)env)->rxBuffer, readIndex);
			readIndex = 0;
			LLSPI_close();
			platform_spi_master_drivers[ESP32_SPI2].state = SPI_MASTER_STATE_IDLE;
			break;
		}
		case SPI_MASTER_STATE_WRITE_DATA: {
#ifdef LLCOMM_SPI_DEBUG
			printf("[INFO] %s - SPI_MASTER_STATE_WRITE_DATA\n", __func__);
#endif
			LLSPI_open();
			LLSPI_write(((LLCOMM_UART *)env)->txBuffer, writeIndex);
			writeIndex = 0;
			LLSPI_close();
			platform_spi_master_drivers[ESP32_SPI2].state = SPI_MASTER_STATE_IDLE;
			break;
		}
		case SPI_MASTER_STATE_TRANSMIT_DATA: {
#ifdef LLCOMM_SPI_DEBUG
			printf("[INFO] %s - SPI_MASTER_STATE_TRANSMIT_DATA\n", __func__);
#endif
			LLSPI_open();
			LLSPI_transmit(((LLCOMM_UART *)env)->txBuffer, ((LLCOMM_UART *)env)->rxBuffer, writeIndex);
			availableData = writeIndex;
			writeIndex = 0;
			readIndex = 0;
			LLSPI_close();
			platform_spi_master_drivers[ESP32_SPI2].state = SPI_MASTER_STATE_IDLE;
			break;
		}
		default:
			break;
		}
	}
}
