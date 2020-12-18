/*
 * C
 *
 * Copyright 2019-2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */

#include "rom/gpio.h"
#include "rom/uart.h"
#include "soc/gpio_sig_map.h"
#include "soc/io_mux_reg.h"

#include "esp_log.h"

#include "sdkconfig.h"

#include "uart_switch.h"

// When using external UART, GPIO 4 and 39 are used.
#define CONFIG_CONSOLE_UART_TX_GPIO 4
#define CONFIG_CONSOLE_UART_RX_GPIO 39

// This function should be called early in the bootloader
// In bootloader_init.c:uart_console_configure
void uart_switch(void) {
	const int uart_num = CONFIG_CONSOLE_UART_NUM;

	// Do nothing if traces should not be redirected
	// This uses a link constante that is usually set by the MicroEJ application linker.
	if (!MEJ_UART_EXT()) {
		return;
	}

	// Some constants to make the following code less upper-case
	const int uart_tx_gpio = CONFIG_CONSOLE_UART_TX_GPIO;
	const int uart_rx_gpio = CONFIG_CONSOLE_UART_RX_GPIO;
	// Switch to the new UART (this just changes UART number used for
	// ets_printf in ROM code).
	uart_tx_switch(uart_num);
	// If console is attached to UART1 or if non-default pins are used,
	// need to reconfigure pins using GPIO matrix
	if (uart_num != 0 || uart_tx_gpio != 1 || uart_rx_gpio != 3) {
		// Change pin mode for GPIO1/3 from UART to GPIO
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0RXD_U, FUNC_U0RXD_GPIO3);
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0TXD_U, FUNC_U0TXD_GPIO1);
		// Route GPIO signals to/from pins
		// (arrays should be optimized away by the compiler)
		const uint32_t tx_idx_list[3] = { U0TXD_OUT_IDX, U1TXD_OUT_IDX, U2TXD_OUT_IDX };
		const uint32_t rx_idx_list[3] = { U0RXD_IN_IDX, U1RXD_IN_IDX, U2RXD_IN_IDX };
		const uint32_t tx_idx = tx_idx_list[uart_num];
		const uint32_t rx_idx = rx_idx_list[uart_num];
		gpio_matrix_out(uart_tx_gpio, tx_idx, 0, 0);
		gpio_matrix_in(uart_rx_gpio, rx_idx, 0);
	}
}
