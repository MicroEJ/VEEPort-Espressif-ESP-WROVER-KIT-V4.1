/*
 * C
 *
 * Copyright 2018-2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */

/******************************************************************************
Includes   <System Includes> , "Project Includes"
 ******************************************************************************/

#include "LLBSP_impl.h"
#include "microej.h"
#include <stdio.h>
#include "esp_log.h"

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

/* End of File */
