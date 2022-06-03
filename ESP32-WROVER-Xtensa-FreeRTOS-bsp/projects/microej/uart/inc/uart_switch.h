/*
 * C
 *
 * Copyright 2019-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#if !defined __UART_SWITCH_H__
# define __UART_SWITCH_H__

extern int _usart_traces_switch;

#define USART_TRACES_USB 1
#define USART_TRACES_EXT 2

static inline bool MEJ_UART(int mode) {
	const volatile uint32_t usart_traces_switch = ((uint32_t)(& _usart_traces_switch));

	return (usart_traces_switch == mode);
}

#define MEJ_UART_USB() MEJ_UART(USART_TRACES_USB)
#define MEJ_UART_EXT() MEJ_UART(USART_TRACES_EXT)

void uart_switch(void);

#endif // !defined __UART_SWITCH_H__
