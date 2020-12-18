/*
 * C
 *
 * Copyright 2018-2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */

#include <string.h>
#include <esp_types.h>
#include <esp_clk.h>
#include "comm_spi.h"

/* Rx/Tx buffers */
DRAM_ATTR static uint8_t __attribute__ ((aligned(4))) myRxBuffer[COMM_SPI_MAX_DMA_LEN] = {};
DRAM_ATTR static uint8_t __attribute__ ((aligned(4))) myTxBuffer[COMM_SPI_MAX_DMA_LEN] = {};

/* Function to transfer data via spi with dma */
static int32_t microej_spi_dma_transfer_bytes(uint16_t size);

void LLSPI_open(void) {

}

int32_t LLSPI_write(uint8_t *sendBuffer, uint16_t size) {
	int32_t transfer_status = SPI_TRANSFER_UNDEFINED;

	if (size > 0) {
		// Copy the data to the SPI buffer
		memcpy(myTxBuffer, sendBuffer, size);

		// Transmit the data
		transfer_status = microej_spi_dma_transfer_bytes(size);

		// Clear the SPI buffers
		memset(myTxBuffer, 0, size);
		memset(myRxBuffer, 0, size);
	} else {
		printf("%s wanted to send 0 bytes\n", __func__);
	}

	return transfer_status;
}

int32_t LLSPI_read(uint8_t *receiveBuffer, uint16_t size) {
	int32_t transfer_status = SPI_TRANSFER_UNDEFINED;

	if (size > 0) {
		// Receive the data
		transfer_status = microej_spi_dma_transfer_bytes(size);

		// Copy the data from the receive buffer and clear the SPI buffers
		memcpy(receiveBuffer, myRxBuffer, size);
		memset(myTxBuffer, 0, size);
		memset(myRxBuffer, 0, size);
	} else {
		printf("%s wanted to receive 0 bytes\n", __func__);
	}

	return transfer_status;
}

int32_t LLSPI_transmit(uint8_t *sendBuffer, uint8_t *receiveBuffer, uint16_t size) {
	int32_t transfer_status = SPI_TRANSFER_UNDEFINED;

	if (size > 0) {
		// Copy the data to the SPI buffer
		memcpy(myTxBuffer, sendBuffer, size);

		// Transmit the data
		transfer_status = microej_spi_dma_transfer_bytes(size);

		// Copy the data from the receive buffer and clear the SPI buffers
		memcpy(receiveBuffer, myRxBuffer, size);
		memset(myTxBuffer, 0, size);
		memset(myRxBuffer, 0, size);
	} else {
		printf("%s wanted to send 0 bytes\n", __func__);
	}

	return transfer_status;
}

void LLSPI_close(void) {

}

// Initialize the SPI2 device in master mode
void microej_spi_master_config(void) {
	// Configuration for the SPI bus
	spi_bus_config_t buscfg = {
		.mosi_io_num = SPI_MOSI_GPIO,
		.miso_io_num = SPI_MISO_GPIO,
		.sclk_io_num = SPI_CLK_GPIO,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1,
		.max_transfer_sz = COMM_SPI_MAX_DMA_LEN,
	};

	// Configuration for the SPI master interface
	spi_device_interface_config_t devcfg = {
		.command_bits = 0,
		.address_bits = 0,
		.dummy_bits = 0,
		.duty_cycle_pos = 128,
		.cs_ena_pretrans = 1,  // Keep the CS low 1 cycle before transaction, to stop the master from missing the first bit when CS has less propagation delay than CLK
		.cs_ena_posttrans = 1, // Keep the CS low 1 cycle after transaction, to stop the slave from missing the last bit when CS has less propagation delay than CLK
		.clock_speed_hz = SPI_CLOCK,
		.mode = SPI_MODE,
		.spics_io_num = SPI_CS_GPIO,
		.queue_size = 1,
		.flags = 0,
		.pre_cb = NULL,
		.post_cb = NULL,
	};

	// Initialize and enable SPI
	spi_bus_initialize(SPI_CHANNEL, &buscfg, SPI_DMA);
	spi_bus_add_device(SPI_CHANNEL, &devcfg, &platform_spi_master_drivers[ESP32_SPI2].spi);

	// Create mutex/semaphore and set initial driver state
	platform_spi_master_drivers[ESP32_SPI2].state = SPI_MASTER_STATE_IDLE;
	platform_spi_master_drivers[ESP32_SPI2].transfer_pending = xSemaphoreCreateMutex();
	vSemaphoreCreateBinary(platform_spi_master_drivers[ESP32_SPI2].transfer_ready);
	xSemaphoreTake(platform_spi_master_drivers[ESP32_SPI2].transfer_ready, 0);
}

// Full buffer DMA transfer
static int32_t microej_spi_dma_transfer_bytes(uint16_t size) {
	esp_err_t trans_result = ESP_OK;
	spi_transaction_t trans_t;

	// Prepare transaction parameters
	trans_t.rx_buffer = myRxBuffer;
	trans_t.tx_buffer = myTxBuffer;
	trans_t.rxlength = 0;
	trans_t.length = 8 * size;
	trans_t.flags = 0;
	trans_t.cmd = 0;
	trans_t.addr = 0;
	trans_t.user = NULL;

	// Transfer pending
	xSemaphoreTake(platform_spi_master_drivers[ESP32_SPI2].transfer_pending, portMAX_DELAY);

	// Perform transaction
	trans_result = spi_device_transmit(platform_spi_master_drivers[ESP32_SPI2].spi, &trans_t);

	// Transfer finished
	xSemaphoreGive(platform_spi_master_drivers[ESP32_SPI2].transfer_pending);

	// Return
	if (ESP_OK != trans_result) {
		return SPI_TRANSFER_CANCELLED;
	} else {
		return size;
	}
}
