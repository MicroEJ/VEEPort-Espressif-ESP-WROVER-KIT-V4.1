/*
 * C
 *
 * Copyright 2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#ifndef LLCOMM_UART_CONFIGURATION_H
#define LLCOMM_UART_CONFIGURATION_H

#ifdef __cplusplus
	extern "C" {
#endif

/**
 * @brief Defines the software FIFO size for transmission used by MicroEJ framework.
 */
#define LLCOMM_UART_TX_BUFFER_SIZE (2*1024)

/**
 * @brief Defines the software FIFO size for reception used by MicroEJ framework.
 */
#define LLCOMM_UART_RX_BUFFER_SIZE (2*1024)

#ifdef __cplusplus
	}
#endif

#endif /* LLCOMM_UART_CONFIGURATION_H */
