/*
 * C
 *
 * Copyright 2021-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/* Includes ------------------------------------------------------------------*/

#include "LLUI_INPUT_impl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

static xSemaphoreHandle g_sem_input;

/* API -----------------------------------------------------------------------*/

void LLUI_INPUT_IMPL_initialize(void)
{
	g_sem_input = xSemaphoreCreateBinary();
	xSemaphoreGive(g_sem_input);

}

int32_t LLUI_INPUT_IMPL_getInitialStateValue(int32_t stateMachinesID, int32_t stateID)
{
	// no state on this BSP
	return 0;
}

void LLUI_INPUT_IMPL_enterCriticalSection()
{
	xSemaphoreTake(g_sem_input, portMAX_DELAY);
}

void LLUI_INPUT_IMPL_leaveCriticalSection()
{
    xSemaphoreGive(g_sem_input);
}
