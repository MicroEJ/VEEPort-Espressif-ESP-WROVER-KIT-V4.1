/*
 * C
 *
 * Copyright 2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#include "LLCOMM_UART.h"
#define   LLCOMM_BUFFERED_CONNECTION_IMPL LLCOMM_UART
#include "LLCOMM_BUFFERED_CONNECTION_impl.h"

#ifdef __cplusplus
	extern "C" {
#endif

/**
 * @brief UART Rx queue size.
 * A minimum of (50 * 1) bytes and a maximum of (50 * UART_FIFO_LEN) can be queued by the UART driver.
 */
#define UART_RX_QUEUE_SIZE (50)

/**
 * UART Rx task definitions
 */
#define UART_RX_STACK_SIZE       (8 * 1024)
#define UART_RX_TASK_PRIORITY    (11)
#define UART_RX_TASK_STACK_SIZE  (UART_RX_STACK_SIZE / 4)
#define UART_RX_TASK_NAME        "ECOM-COMM UART%d Rx Task"

/** @brief Private functions. */
static void uart_rx_task(void* pvParameters);

void LLCOMM_BUFFERED_CONNECTION_IMPL_initialize(LLCOMM_BUFFERED_CONNECTION* env) {
	uart_config_t uart_config = {
			.baud_rate = 115200,
			.data_bits = UART_DATA_8_BITS,
			.parity = UART_PARITY_DISABLE,
			.stop_bits = UART_STOP_BITS_1,
			.source_clk = UART_SCLK_APB
		};
	LLCOMM_UART* llcomm = (LLCOMM_UART*) env;
	uint8_t task_name[33] = {0};
	esp_err_t err = ESP_FAIL;

	if (llcomm->uart_instance.rts_io_num == UART_PIN_NO_CHANGE) {
		uart_config.rx_flow_ctrl_thresh = 0;
		if (llcomm->uart_instance.cts_io_num == UART_PIN_NO_CHANGE) {
			uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
		} else {
			uart_config.flow_ctrl = UART_HW_FLOWCTRL_CTS;
		}
	} else {
		uart_config.rx_flow_ctrl_thresh = (UART_FIFO_LEN * 3) / 4;
		if (llcomm->uart_instance.cts_io_num == UART_PIN_NO_CHANGE) {
			uart_config.flow_ctrl = UART_HW_FLOWCTRL_RTS;
		} else {
			uart_config.flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS;
		}
	}

	err = uart_param_config(llcomm->uart_instance.uart_num, &uart_config);
	if (err != ESP_OK) {
		printf("[ERROR] Configuring UART parameters = [%d]\n", err);
	}

	err = uart_set_pin(llcomm->uart_instance.uart_num, llcomm->uart_instance.tx_io_num, llcomm->uart_instance.rx_io_num, llcomm->uart_instance.rts_io_num, llcomm->uart_instance.cts_io_num);
	if (err != ESP_OK) {
		printf("[ERROR] Configuring UART pins = [%d]\n", err);
	}

	err = uart_driver_install(llcomm->uart_instance.uart_num, LLCOMM_UART_RX_BUFFER_SIZE, LLCOMM_UART_TX_BUFFER_SIZE, UART_RX_QUEUE_SIZE, &llcomm->uart_rx_queue, 0);
	if (err != ESP_OK) {
		printf("[ERROR] Installing UART driver = [%d]\n", err);
	}

	err = uart_set_mode(llcomm->uart_instance.uart_num, UART_MODE_UART);
	if (err != ESP_OK) {
		printf("[ERROR] Setting UART mode = [%d]\n", err);
	}

	err = uart_disable_rx_intr(llcomm->uart_instance.uart_num);
	if (err != ESP_OK) {
		printf("[ERROR] Disabling Rx interrupts = [%d]\n", err);
	}

	err = uart_disable_tx_intr(llcomm->uart_instance.uart_num);
	if (err != ESP_OK) {
		printf("[ERROR] Disabling Tx interrupts = [%d]\n", err);
	}

	sprintf((char*)task_name, UART_RX_TASK_NAME, llcomm->uart_instance.uart_num);
	BaseType_t ret = xTaskCreate(uart_rx_task, (char const *)task_name, UART_RX_TASK_STACK_SIZE, llcomm, UART_RX_TASK_PRIORITY, &llcomm->uart_rx_task);
	if (ret != pdPASS) {
		printf("[ERROR] Creating the UART Rx task = [%d]\n", ret);
	}
}

void LLCOMM_BUFFERED_CONNECTION_IMPL_enableRX(LLCOMM_BUFFERED_CONNECTION* env) {
	/* No support in esp-idf to enable the reception. */
	(void)env;
}

void LLCOMM_BUFFERED_CONNECTION_IMPL_disableRX(LLCOMM_BUFFERED_CONNECTION* env) {
	/* No support in esp-idf to disable the reception. */
	(void)env;
}

void LLCOMM_BUFFERED_CONNECTION_IMPL_enableRXDeviceInterrupt(LLCOMM_BUFFERED_CONNECTION* env) {
	LLCOMM_UART* llcomm = (LLCOMM_UART*) env;

	esp_err_t err = uart_enable_rx_intr(llcomm->uart_instance.uart_num);
	if (err != ESP_OK) {
		printf("[ERROR] Enabling Rx interrupts = [%d]\n", err);
	}
}

void LLCOMM_BUFFERED_CONNECTION_IMPL_disableRXDeviceInterrupt(LLCOMM_BUFFERED_CONNECTION* env) {
	LLCOMM_UART* llcomm = (LLCOMM_UART*) env;

	esp_err_t err = uart_disable_rx_intr(llcomm->uart_instance.uart_num);
	if (err != ESP_OK) {
		printf("[ERROR] Disabling Rx interrupts = [%d]\n", err);
	}
}

void LLCOMM_BUFFERED_CONNECTION_IMPL_enableTX(LLCOMM_BUFFERED_CONNECTION* env) {
	/* No support in esp-idf to enable the transmission. */
	(void)env;
}

void LLCOMM_BUFFERED_CONNECTION_IMPL_disableTX(LLCOMM_BUFFERED_CONNECTION* env) {
	/* No support in esp-idf to disable the transmission. */
	(void)env;
}

void LLCOMM_BUFFERED_CONNECTION_IMPL_enableTXDeviceInterrupt(LLCOMM_BUFFERED_CONNECTION* env) {
	LLCOMM_UART* llcomm = (LLCOMM_UART*) env;

	esp_err_t err = uart_enable_tx_intr(llcomm->uart_instance.uart_num, 1, UART_FIFO_LEN / 4);
	if (err != ESP_OK) {
		printf("[ERROR] Enabling Tx interrupts = [%d]\n", err);
	}
}

void LLCOMM_BUFFERED_CONNECTION_IMPL_disableTXDeviceInterrupt(LLCOMM_BUFFERED_CONNECTION* env) {
	LLCOMM_UART* llcomm = (LLCOMM_UART*) env;

	esp_err_t err = uart_disable_tx_intr(llcomm->uart_instance.uart_num);
	if (err != ESP_OK) {
		printf("[ERROR] Disabling Tx interrupts = [%d]\n", err);
	}
}

int32_t LLCOMM_BUFFERED_CONNECTION_IMPL_configureDevice(LLCOMM_BUFFERED_CONNECTION* env, int32_t baudrate, int32_t bitsperchar, int32_t stopbits, int32_t parity) {
	int32_t status = LLCOMM_BUFFERED_CONNECTION_SUCCESS;
	uart_word_length_t data_bit = UART_DATA_BITS_MAX;
	uart_stop_bits_t stop_bits = UART_STOP_BITS_MAX;
	uart_parity_t parity_mode = UART_PARITY_DISABLE;
	LLCOMM_UART* llcomm = (LLCOMM_UART*) env;
	esp_err_t err = ESP_FAIL;

	if (baudrate > UART_BITRATE_MAX) {
		printf("[ERROR] Unsupported baudrate = [%d]\n", baudrate);
		status = LLCOMM_BUFFERED_CONNECTION_ERROR_INIT_UNSUPPORTED_BAUDRATE;
	}

	switch (bitsperchar) {
	case LLCOMM_BUFFERED_CONNECTION_DATABITS_5:
		data_bit = UART_DATA_5_BITS;
		break;
	case LLCOMM_BUFFERED_CONNECTION_DATABITS_6:
		data_bit = UART_DATA_6_BITS;
		break;
	case LLCOMM_BUFFERED_CONNECTION_DATABITS_7:
		data_bit = UART_DATA_7_BITS;
		break;
	case LLCOMM_BUFFERED_CONNECTION_DATABITS_8:
		data_bit = UART_DATA_8_BITS;
		break;
	default:
		printf("[ERROR] Unsupported bitsperchar = [%d]\n", bitsperchar);
		status = LLCOMM_BUFFERED_CONNECTION_ERROR_INIT_UNSUPPORTED_BITSPERCHAR;
		break;
	}

	switch (stopbits) {
	case LLCOMM_BUFFERED_CONNECTION_STOPBITS_1:
		stop_bits = UART_STOP_BITS_1;
		break;
	case LLCOMM_BUFFERED_CONNECTION_STOPBITS_1_5:
		stop_bits = UART_STOP_BITS_1_5;
		break;
	case LLCOMM_BUFFERED_CONNECTION_STOPBITS_2:
		stop_bits = UART_STOP_BITS_2;
		break;
	default:
		printf("[ERROR] Unsupported stopbits = [%d]\n", stopbits);
		status = LLCOMM_BUFFERED_CONNECTION_ERROR_INIT_UNSUPPORTED_STOPBITS;
		break;
	}

	switch (parity) {
	case LLCOMM_BUFFERED_CONNECTION_PARITY_NO:
		parity_mode = UART_PARITY_DISABLE;
		break;
	case LLCOMM_BUFFERED_CONNECTION_PARITY_EVEN:
		parity_mode = UART_PARITY_EVEN;
		break;
	case LLCOMM_BUFFERED_CONNECTION_PARITY_ODD:
		parity_mode = UART_PARITY_ODD;
		break;
	default:
		printf("[ERROR] Unsupported parity = [%d]\n", parity);
		status = LLCOMM_BUFFERED_CONNECTION_ERROR_INIT_UNSUPPORTED_PARITY;
		break;
	}

	if (status == LLCOMM_BUFFERED_CONNECTION_SUCCESS) {
		err = uart_set_baudrate(llcomm->uart_instance.uart_num, baudrate);
		if (err != ESP_OK) {
			printf("[ERROR] Setting UART baudrate = [%d]\n", err);
			status = LLCOMM_BUFFERED_CONNECTION_ERROR_CANNOT_OPEN;
		}

		err = uart_set_word_length(llcomm->uart_instance.uart_num, data_bit);
		if (err != ESP_OK) {
			printf("[ERROR] Setting UART bitsperchar = [%d]\n", err);
			status = LLCOMM_BUFFERED_CONNECTION_ERROR_CANNOT_OPEN;
		}

		err = uart_set_stop_bits(llcomm->uart_instance.uart_num, stop_bits);
		if (err != ESP_OK) {
			printf("[ERROR] Setting UART stopbits = [%d]\n", err);
			status = LLCOMM_BUFFERED_CONNECTION_ERROR_CANNOT_OPEN;
		}

		err = uart_set_parity(llcomm->uart_instance.uart_num, parity_mode);
		if (err != ESP_OK) {
			printf("[ERROR] Setting UART parity = [%d]\n", err);
			status = LLCOMM_BUFFERED_CONNECTION_ERROR_CANNOT_OPEN;
		}
	}

	return status;
}

void* LLCOMM_BUFFERED_CONNECTION_IMPL_getName(LLCOMM_BUFFERED_CONNECTION* env) {
	/* This kind of connection cannot be open from a name (only from a platform port number). */
	(void)env;
	return 0;
}

int32_t LLCOMM_BUFFERED_CONNECTION_IMPL_getPlatformId(LLCOMM_BUFFERED_CONNECTION* env) {
	LLCOMM_UART* llcomm = (LLCOMM_UART*) env;
	return llcomm->platform_id;
}

void LLCOMM_BUFFERED_CONNECTION_IMPL_released(LLCOMM_BUFFERED_CONNECTION* env) {
	LLCOMM_UART* llcomm = (LLCOMM_UART*) env;

	esp_err_t err = uart_driver_delete(llcomm->uart_instance.uart_num);
	if (err != ESP_OK) {
		printf("[ERROR] Deleting the driver = [%d]\n", err);
	}

	vTaskDelete(llcomm->uart_rx_task);
}

int32_t LLCOMM_BUFFERED_CONNECTION_IMPL_getNbProperties(LLCOMM_BUFFERED_CONNECTION* env) {
	(void)env;
	return 0;
}

void* LLCOMM_BUFFERED_CONNECTION_IMPL_getProperty(LLCOMM_BUFFERED_CONNECTION* env, void* propertyName) {
	(void)env;
	(void)propertyName;
	return 0;
}

void* LLCOMM_BUFFERED_CONNECTION_IMPL_getPropertyName(LLCOMM_BUFFERED_CONNECTION* env, int32_t propertyId) {
	(void)env;
	(void)propertyId;
	return 0;
}

int32_t LLCOMM_BUFFERED_CONNECTION_IMPL_getRXBufferStartAddress(LLCOMM_BUFFERED_CONNECTION* env) {
	LLCOMM_UART* llcomm = (LLCOMM_UART*) env;
	return (int32_t)(llcomm->rx_buffer);
}

int32_t LLCOMM_BUFFERED_CONNECTION_IMPL_getRXBufferLength(LLCOMM_BUFFERED_CONNECTION* env) {
	(void)env;
	return LLCOMM_UART_RX_BUFFER_SIZE;
}

int32_t LLCOMM_BUFFERED_CONNECTION_IMPL_getTXBufferStartAddress(LLCOMM_BUFFERED_CONNECTION* env) {
	LLCOMM_UART* llcomm = (LLCOMM_UART*) env;
	return (int32_t)(llcomm->tx_buffer);
}

int32_t LLCOMM_BUFFERED_CONNECTION_IMPL_getTXBufferLength(LLCOMM_BUFFERED_CONNECTION* env) {
	(void)env;
	return LLCOMM_UART_TX_BUFFER_SIZE;
}

int32_t LLCOMM_BUFFERED_CONNECTION_IMPL_useTXHardwareFIFO(LLCOMM_BUFFERED_CONNECTION* env) {
	/* "FIFO" (or ring buffers) available, no need for managing Tx interrupts for single-byte transfers. */
	(void)env;
	return LLCOMM_BUFFERED_CONNECTION_SUCCESS;
}

int32_t LLCOMM_BUFFERED_CONNECTION_IMPL_txFifoFull(LLCOMM_BUFFERED_CONNECTION* env) {
	/* UART driver is installed using ring buffers, that can always accept Tx data, therefore the "FIFO" can be seen as always not full. */
	(void)env;
	return LLCOMM_BUFFERED_CONNECTION_HARDWARE_FIFO_NOT_FULL;
}

void LLCOMM_BUFFERED_CONNECTION_IMPL_sendData(LLCOMM_BUFFERED_CONNECTION* env, int32_t data) {
	LLCOMM_UART* llcomm = (LLCOMM_UART*) env;

	esp_err_t err = uart_write_bytes(llcomm->uart_instance.uart_num, &data, 1);
	if (err < 0) {
		printf("[ERROR] Writing bytes = [%d]\n", err);
	}
}

int32_t LLCOMM_BUFFERED_CONNECTION_IMPL_isTransmissionComplete(LLCOMM_BUFFERED_CONNECTION* env) {
	int32_t status = LLCOMM_BUFFERED_CONNECTION_TRANSMISSION_COMPLETE;
	LLCOMM_UART* llcomm = (LLCOMM_UART*) env;

	esp_err_t err = uart_wait_tx_done(llcomm->uart_instance.uart_num, 0);
	if (err != ESP_OK) {
		status = LLCOMM_BUFFERED_CONNECTION_TRANSMISSION_TIMEOUT;
	}

	return status;
}

/**
 * @brief This is the implementation of the UART RX task.
 *
 * @param[in] pvParameters task's parameter.
 */
static void uart_rx_task(void* pvParameters) {
	LLCOMM_UART* llcomm = (LLCOMM_UART*) pvParameters;
	uint8_t rx_buffer[UART_FIFO_LEN] = {0};
	uart_event_t event = {0};
	esp_err_t err = ESP_FAIL;

	for (;;) {
		if (xQueueReceive(llcomm->uart_rx_queue, &event, portMAX_DELAY)) {
			switch (event.type) {
			case UART_DATA:
				err = uart_read_bytes(llcomm->uart_instance.uart_num, rx_buffer, event.size, portMAX_DELAY);
				if (err >= 0) {
					for(int32_t byte = 0; byte < err; byte++) {
						int32_t res = LLCOMM_BUFFERED_CONNECTION_dataReceived((LLCOMM_BUFFERED_CONNECTION*) llcomm, rx_buffer[byte]);
						switch (res) {
						case LLCOMM_BUFFERED_CONNECTION_SUCCESS:
							/* Nothing to do. */
							break;
						case LLCOMM_BUFFERED_CONNECTION_FIFO_FULL:
							printf("[ERROR] ECOM-COMM Rx buffer full, consider increasing it\n");
							break;
						case LLCOMM_BUFFERED_CONNECTION_RX_CLOSED:
							printf("[ERROR] Receiving data while Rx connection is closed\n");
							break;
						default:
							printf("[ERROR] Unsupported ECOM-COMM API return code = [%d]\n", res);
							break;
						}
					}
				} else {
					printf("[ERROR] Reading bytes = [%d]\n", err);
				}
				break;
			case UART_FIFO_OVF:
				printf("[ERROR] UART FIFO overflow, consider adding flow control\n");

				/*
				 * If FIFO overflow happened, you should consider adding flow control for your application.
				 * The ISR has already reset the RX FIFO.
				 */
				break;
			case UART_BUFFER_FULL:
				printf("[ERROR] UART Rx ring buffer full, consider increasing the Rx buffer size\n");

				/*
				 * If buffer full happened, you should consider increasing your RX buffer size.
				 * We directly flush the RX buffer here in order to read more data.
				 */
				uart_flush_input(llcomm->uart_instance.uart_num);
				break;
			case UART_BREAK:
				printf("[ERROR] UART break\n");
				break;
			case UART_FRAME_ERR:
				printf("[ERROR] UART frame error\n");
				break;
			case UART_PARITY_ERR:
				printf("[ERROR] UART parity error\n");
				break;
			case UART_DATA_BREAK:
				printf("[ERROR] UART Tx data and break\n");
				break;
			case UART_PATTERN_DET:
				printf("[ERROR] UART pattern detection\n");
				break;
			default:
				printf("[ERROR] Unknown UART event type = [%d]", event.type);
				break;
			}
		}
	}
}

#ifdef __cplusplus
	}
#endif
