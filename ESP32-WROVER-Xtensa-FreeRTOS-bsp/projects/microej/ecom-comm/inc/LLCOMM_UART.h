/*
 * C
 *
 * Copyright 2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#ifndef LLCOMM_UART_H
#define LLCOMM_UART_H

#include "LLCOMM_BUFFERED_CONNECTION.h"
#include "LLCOMM_UART_configuration.h"
#include "driver/uart.h"

#ifdef __cplusplus
	extern "C" {
#endif

/**
 * @brief UART instance describing a physical connection.
 */
typedef struct {
	uart_port_t uart_num; /*!< UART number, mandatory. */
	int32_t tx_io_num;    /*!< TX GPIO, mandatory. */
	int32_t rx_io_num;    /*!< RX GPIO, mandatory. */
	int32_t rts_io_num;   /*!< RTS GPIO, optional, set UART_PIN_NO_CHANGE if not used. */
	int32_t cts_io_num;   /*!< CTS GPIO, optional, set UART_PIN_NO_CHANGE if not used. */
} uart_instance_t;

/**
 * @brief LLCOMM_UART connection.
 */
typedef struct LLCOMM_UART {
	struct LLCOMM_BUFFERED_CONNECTION header;      /*!< MicroEJ LLCOMM connection header. */
	uart_instance_t uart_instance;                 /*!< UART instance. */
	QueueHandle_t uart_rx_queue;                   /*!< UART Rx queue. */
	TaskHandle_t uart_rx_task;                     /*!< UART Rx task handle. */
	int32_t platform_id;                           /*!< UART platform id. */
	uint8_t tx_buffer[LLCOMM_UART_TX_BUFFER_SIZE]; /*!< Software FIFO size for transmission used by MicroEJ framework. */
	uint8_t rx_buffer[LLCOMM_UART_RX_BUFFER_SIZE]; /*!< Software FIFO size for reception used by MicroEJ framework. */
} LLCOMM_UART;

#ifdef __cplusplus
	}
#endif

#endif /* LLCOMM_UART_H */
