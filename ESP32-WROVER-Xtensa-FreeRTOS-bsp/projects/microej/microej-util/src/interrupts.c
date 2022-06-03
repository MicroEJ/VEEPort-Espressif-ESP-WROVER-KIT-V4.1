/*
 * C
 *
 * Copyright 2018-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
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
