/*
 * C
 *
 * Copyright 2014-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/*
 * Implementation for FreeRTOS
 */

#include "framerate_conf.h"
#ifdef FRAMERATE_ENABLED

/* Includes ------------------------------------------------------------------*/
 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "framerate_impl.h"

/* Defines -------------------------------------------------------------------*/

#define FRAMERATE_STACK_SIZE ( 3 * 1024 )
#define FRAMERATE_TASK_PRIORITY      ( 3 )
#define FRAMERATE_TASK_STACK_SIZE     FRAMERATE_STACK_SIZE/4

/* Private API ---------------------------------------------------------------*/

static void _framerate_task(void * pvParameters)
{
	// launch framerate job
	framerate_task_work();
	// job end, cleanup resources
	vTaskDelete(xTaskGetCurrentTaskHandle());
}

/* API -----------------------------------------------------------------------*/

int32_t framerate_impl_start_task(void)
{
	BaseType_t xReturn = xTaskCreate( _framerate_task, "FRAMERATE", FRAMERATE_TASK_STACK_SIZE, NULL, FRAMERATE_TASK_PRIORITY, NULL );
	return xReturn == pdPASS ? FRAMERATE_OK : FRAMERATE_ERROR;
}

void framerate_impl_sleep(uint32_t ms)
{
	TickType_t ticks = ms / portTICK_PERIOD_MS;
	vTaskDelay(ticks ? ticks : 1);          /* Minimum delay = 1 tick */
	return;
}

#endif

