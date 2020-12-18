/*
 * C
 *
 * Copyright 2019-2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
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
