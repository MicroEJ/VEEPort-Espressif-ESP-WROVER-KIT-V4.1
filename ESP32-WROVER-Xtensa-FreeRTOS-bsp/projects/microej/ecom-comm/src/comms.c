/*
 * C
 *
 * Copyright 2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#include "LLCOMM_UART.h"
#include "LLCOMM_impl.h"
#include "driver/gpio.h"

#ifdef __cplusplus
	extern "C" {
#endif

/**
 * @brief ECOM-COMM platform id for UART1.
 */
#define UART1_PLATFORM_ID (6)

/**
 * @brief ECOM-COMM platform id for UART2.
 */
#define UART2_PLATFORM_ID (7)

/**
 * @brief Defines the MicroEJ LLCOMM_UARTs list.
 */
typedef enum {
	UART1_INDEX,
	UART2_INDEX,
	UART_MAX_INDEX
} microej_uart_t;

/**
 * @brief Structure describing the physical UART connection, associated to UART1_PLATFORM_ID.
 *
 * If RTS and CTS are not set, hardware flow control will be disabled.
 * If RTS-only is set, Rx hardware flow control will be enabled.
 * If CTS-only is set, Tx hardware flow control will be enabled.
 * If both RTS and CTS are set, full hardware flow control will be enabled.
 */
static const uart_instance_t uart1_instance = {
	.uart_num = UART_NUM_1,
	.tx_io_num = GPIO_NUM_4,
	.rx_io_num = GPIO_NUM_5,
	.rts_io_num = UART_PIN_NO_CHANGE,
	.cts_io_num = UART_PIN_NO_CHANGE
};

/**
 * @brief Structure describing the physical UART connection, associated to UART2_PLATFORM_ID.
 *
 * If RTS and CTS are not set, hardware flow control will be disabled.
 * If RTS-only is set, Rx hardware flow control will be enabled.
 * If CTS-only is set, Tx hardware flow control will be enabled.
 * If both RTS and CTS are set, full hardware flow control will be enabled.
 */
static const uart_instance_t uart2_instance = {
	.uart_num = UART_NUM_2,
	.tx_io_num = GPIO_NUM_18,
	.rx_io_num = GPIO_NUM_19,
	.rts_io_num = UART_PIN_NO_CHANGE,
	.cts_io_num = UART_PIN_NO_CHANGE
};

/**
 * @brief Array of all static LLCOMM UARTs.
 */
static LLCOMM_UART LLCOMM_UARTs[UART_MAX_INDEX];

/**
 * @brief Rx queue used by UART1 driver.
 */
static QueueHandle_t uart1_rx_queue;

/**
 * @brief Rx queue used by UART2 driver.
 */
static QueueHandle_t uart2_rx_queue;

/** @brief Prototype of external function. */
void LLCOMM_UART_new(LLCOMM_BUFFERED_CONNECTION* env);

/** @brief Private functions. */
static void COMMS_initialize_llcomm_uart(microej_uart_t index, uart_instance_t uart_instance, QueueHandle_t uart_rx_queue, uint8_t platform_id);

void LLCOMM_stack_initialize(void) {
	/* No dynamic connections (e.g. USB) available on this VEE Port, nothing to do. */
}

void LLCOMM_IMPL_initialize(void) {
	COMMS_initialize_llcomm_uart(UART1_INDEX, uart1_instance, uart1_rx_queue, UART1_PLATFORM_ID);
	COMMS_initialize_llcomm_uart(UART2_INDEX, uart2_instance, uart2_rx_queue, UART2_PLATFORM_ID);
}

void LLCOMM_IMPL_syncConnectionsEnter(void) {
	/* No dynamic connections (e.g. USB) available on the controller, nothing to do. */
}

void LLCOMM_IMPL_syncConnectionsExit(void) {
	/* No dynamic connections (e.g. USB) available on the controller, nothing to do. */
}

/**
 * @brief Initialize an ECOM-COMM UART connection.
 *
 * @param[in] index the index in the static LLCOMM UARTs.
 * @param[in] uart_instance structure describing the physical UART connection.
 * @param[in] uart_rx_queue global queue used by the UART driver to receive data.
 * @param[in] platform_id id of the ECOM-COMM connection.
 */
static void COMMS_initialize_llcomm_uart(microej_uart_t index, uart_instance_t uart_instance, QueueHandle_t uart_rx_queue, uint8_t platform_id) {
	/* Store UART instance. */
	LLCOMM_UARTs[index].uart_instance = uart_instance;

	/* Store UART Rx queue. */
	LLCOMM_UARTs[index].uart_rx_queue = uart_rx_queue;

	/* Store UART Id. */
	LLCOMM_UARTs[index].platform_id = platform_id;

	/* Create MicroEJ UART connection. */
	LLCOMM_UART_new(&LLCOMM_UARTs[index].header);

	/* Add MicroEJ UART connection into MicroEJ framework as generic connection. */
	LLCOMM_addConnection((LLCOMM_CONNECTION*)&LLCOMM_UARTs[index].header);
}

#ifdef __cplusplus
	}
#endif
