/*
 * C
 *
 * Copyright 2018-2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */

#ifndef SOURCES_MICROEJ_INC_SPI_COMM_SPI_H_
#define SOURCES_MICROEJ_INC_SPI_COMM_SPI_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/xtensa_api.h"
#include "driver/spi_master.h"

/* SPI channel and clock default configuration */
#define SPI_CHANNEL    VSPI_HOST
#define SPI_CLOCK      5000000 // Default SPI clock 5MHz

#define SPI_DMA        2

/*
 * SPI master modes for ESP32:
 * - Mode 0: CPOL = 0, CPHA = 0
 * - Mode 1: CPOL = 0, CPHA = 1
 * - Mode 2: CPOL = 1, CPHA = 1
 * - Mode 3: CPOL = 1, CPHA = 0
 */
#define SPI_MODE       0 // Default SPI mode 0

/*
 * The ECOM-COMM will work only if:
 * - JTAG is not used (no jumpers present on JP8)
 * - Camera is not used (nothing plugged on JP4)
 * - MicroSD can be used but only with SDMMC peripheral in 1- line mode
 */
#define SPI_CLK_GPIO   GPIO_NUM_12 // Shared with JTAG/MicroSD in 4- line mode
#define SPI_CS_GPIO    GPIO_NUM_13 // Shared with JTAG/MicroSD in 4- line mode
#define SPI_MISO_GPIO  GPIO_NUM_26 // Shared with Camera
#define SPI_MOSI_GPIO  GPIO_NUM_27 // Shared with Camera

/* The DMA buffer is limited to 4092 bytes, and DMA descriptor chaining is not supported by esp-idf */
#define COMM_SPI_MAX_DMA_LEN     4092

/* SPI transfer error codes */
#define SPI_TRANSFER_OK          0x00
#define SPI_TRANSFER_CANCELLED  -0x42
#define SPI_TRANSFER_UNDEFINED  -0xFF

/* SPI task states */
typedef enum {
	SPI_MASTER_STATE_IDLE,
	SPI_MASTER_STATE_READ_DATA,
	SPI_MASTER_STATE_WRITE_DATA,
	SPI_MASTER_STATE_TRANSMIT_DATA
} platform_spi_master_state_t;

/* SPI driver context type */
typedef struct {
	SemaphoreHandle_t transfer_pending;
	SemaphoreHandle_t transfer_ready;
	platform_spi_master_state_t state;
	spi_device_handle_t spi;
} platform_spi_master_driver_t;

/* User SPI */
typedef enum {
	ESP32_SPI2 = 0,
	ESP32_MAX_SPI
} platform_user_spi_t;

/* SPI driver context */
platform_spi_master_driver_t platform_spi_master_drivers[ESP32_MAX_SPI];

/* Function prototypes */
void LLSPI_open(void);
int32_t LLSPI_write(uint8_t *sendBuffer, uint16_t size);
int32_t LLSPI_read(uint8_t *receiveBuffer, uint16_t size);
int32_t LLSPI_transmit(uint8_t *sendBuffer, uint8_t *receiveBuffer, uint16_t size);
void LLSPI_close(void);
void microej_spi_master_config(void);

#endif /* SOURCES_MICROEJ_INC_SPI_COMM_SPI_H_ */
