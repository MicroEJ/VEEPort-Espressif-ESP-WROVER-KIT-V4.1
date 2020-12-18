/*
 * C
 *
 * Copyright 2018-2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */

/* Includes ------------------------------------------------------------------*/

#include "interrupts.h"
#include "microej.h"

/* Global --------------------------------------------------------------------*/

static volatile uint8_t is_in_ISR = MICROEJ_FALSE;

/* Public functions ----------------------------------------------------------*/

uint8_t interrupt_is_in(void)
{
	return is_in_ISR;
}

uint8_t interrupt_enter(void)
{
	if (is_in_ISR == MICROEJ_TRUE)
	{
		return MICROEJ_FALSE;
	}
	else
	{
		is_in_ISR = MICROEJ_TRUE;
		return MICROEJ_TRUE;
	}
}

void interrupt_leave(uint8_t leave)
{
	if (leave == MICROEJ_TRUE)
	{
		is_in_ISR = MICROEJ_FALSE;
	}
}
