/*
 * C
 *
 * Copyright 2018-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

#include "LLHAL_GPIO_impl.h"

/* Macros --------------------------------------------------------------------*/


/* Private functions ---------------------------------------------------------*/


/* API -----------------------------------------------------------------------*/

int32_t LLHAL_GPIO_IMPL_setPinMode(int32_t port, int32_t pin, int32_t mode)
{
	// NOT IMPLEMENTED
	return HAL_GPIO_NOK;
}

int32_t LLHAL_GPIO_IMPL_getDigitalValue(int32_t port, int32_t pin)
{
	// NOT IMPLEMENTED: always LLHAL_GPIO_DIGITAL_LOW
	return LLHAL_GPIO_DIGITAL_LOW;
}

int32_t LLHAL_GPIO_IMPL_setDigitalValue(int32_t port, int32_t pin, int32_t value)
{
	// NOT IMPLEMENTED
	return HAL_GPIO_NOK;
}

int32_t LLHAL_GPIO_IMPL_getAnalogValue(int32_t port, int32_t pin)
{
	// NOT IMPLEMENTED: always 0
	return 0;
}

int32_t LLHAL_GPIO_IMPL_setAnalogValue(int32_t port, int32_t pin, int32_t percentage)
{
	// NOT IMPLEMENTED
	return HAL_GPIO_NOK;
}

/* EOF -----------------------------------------------------------------------*/
