/*
 * C
 *
 * Copyright 2018-2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/******************************************************************************
Includes   <System Includes> , "Project Includes"
 ******************************************************************************/

#include "LLBSP_impl.h"
#include "microej.h"
#include <stdio.h>
#include "esp_log.h"
#include "soc/compare_set.h"

extern void* SOAR_START;
extern void* SOAR_END;

/*******************************************************************************
 * Function Name: LLBSP_IMPL_isInReadOnlyMemory
 * Description  : checks if the given pointer is in a read only memory or not.
 *******************************************************************************/
uint8_t LLBSP_IMPL_isInReadOnlyMemory(void* ptr)
{
	if ((uint32_t)ptr >= (uint32_t)&SOAR_START && (uint32_t)ptr <= (uint32_t)&SOAR_END)
	{
		return MICROEJ_TRUE;
	}
	// RAM
	return MICROEJ_FALSE;
}

/*******************************************************************************
 * Function Name: LLBSP_IMPL_putchar
 * Description  : puts the character to stdout stream.
 *******************************************************************************/
void LLBSP_IMPL_putchar(int32_t c)
{
	ets_write_char_uart(c);
}

/*******************************************************************************
 * Function Name: LLBSP_IMPL_atomic_exchange
 * Description  : performs an atomic exchange
 *******************************************************************************/
int32_t LLBSP_IMPL_atomic_exchange(int32_t* ptr, int32_t value) {
	void (*compare_and_set_func)(volatile uint32_t *, uint32_t, uint32_t *) = NULL;
	volatile uint32_t old = *((volatile uint32_t*)ptr);
	uint32_t val = 0;

#if defined(CONFIG_ESP32_SPIRAM_SUPPORT)
	if (esp_ptr_external_ram(ptr)) {
		compare_and_set_func = compare_and_set_extram;
	} else {
#endif
		compare_and_set_func = compare_and_set_native;
#if defined(CONFIG_ESP32_SPIRAM_SUPPORT)
	}
#endif

	do {
		val = value;
		compare_and_set_func((volatile uint32_t *)ptr, old, &val);
		if (val == old) {
			break;
		} else {
			// we lost a race setting *ptr
			old = *((volatile uint32_t*)ptr);
		}
	} while (1);

	return old;
}

/* End of File */
