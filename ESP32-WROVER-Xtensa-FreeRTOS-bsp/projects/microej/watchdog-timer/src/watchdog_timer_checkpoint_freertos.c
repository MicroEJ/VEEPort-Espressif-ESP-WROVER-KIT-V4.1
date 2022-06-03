/*
 * C
 *
 * Copyright 2021-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief WATCHDOG TIMER FreeRTOS CCO implementation file.
 * @author MicroEJ Developer Team
 * @version 2.0.1
 * @date 5 August 2021
 */


/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <watchdog_timer_freertos.h>
#include <watchdog_timer_freertos_configuration.h>

#include "FreeRTOS.h"
#include "timers.h"

#include "LLWATCHDOG_TIMER_configuration.h"
#include "LLWATCHDOG_TIMER_impl.h"


#ifdef __cplusplus
	extern "C" {
#endif

/* Macros and Defines --------------------------------------------------------*/

/**
* Sanity check between the expected version of the configuration and the actual version of
* the configuration.
* If an error is raised here, it means that a new version of the CCO has been installed and
* the configuration watchdog_timer_freertos_configuration.h must be updated based on the one provided
* by the new CCO version.
*/
#if WATCHDOG_TIMER_FREERTOS_CONFIGURATION_VERSION != 1
	#error "Version of the configuration file watchdog_timer_freertos_configuration.h is not compatible with this implementation."
#endif


/* Static variables -------------------------------------------------------------------*/
static xTimerHandle wtd_rtos_timer = NULL;

/**
 * @brief Id of RTOS scheduler checkpoint
 */
static volatile int32_t checkpoint_id_rtos_scheduler = WATCHDOG_TIMER_ERROR;

/* Internal functions ------------------------------------------------*/

/**
 * @brief Callback function that refreshes the watchdog timer if freeRTOS scheduler works
 *
 * @param[in] pxTimer, required parameter for TimerCallbackFunction_t
 */
static void watchdog_timer_freertos_check_callback(xTimerHandle pxTimer);


/* API functions ----------------------------------------------------------*/

int32_t watchdog_timer_freertos_scheduler_checkpoint_start(void){
	if (NULL != wtd_rtos_timer && WATCHDOG_TIMER_ERROR != checkpoint_id_rtos_scheduler) {
		LLWATCHDOG_TIMER_DEBUG_TRACE("Watchdog FreeRTOS scheduler checkpoint already started.\n");
		return CHECKPOINT_FREERTOS_OK;
	}

	if (NULL == wtd_rtos_timer && WATCHDOG_TIMER_ERROR == checkpoint_id_rtos_scheduler){
		wtd_rtos_timer = xTimerCreate(
					"watchdog_timer_rtos_check",                   /* name */
					pdMS_TO_TICKS(FREERTOS_TIMER_PERIOD_MS), /* period in ms */
					pdTRUE,                                  /* set auto reload */
					(void*)0,                                /* timer ID = 0 */
					watchdog_timer_freertos_check_callback);       /* callback function pointer */

		if (NULL == wtd_rtos_timer) {
			LLWATCHDOG_TIMER_DEBUG_TRACE("Watchdog FreeRTOS SW timer creation failed\n");
			return CHECKPOINT_FREERTOS_ERROR;
		}

		if(pdFAIL == xTimerStart( wtd_rtos_timer, 0 )){
			LLWATCHDOG_TIMER_DEBUG_TRACE("Fail to start Watchdog FreeRTOS SW timer\n");
			xTimerDelete( wtd_rtos_timer, 0 );
			wtd_rtos_timer = NULL;
			return CHECKPOINT_FREERTOS_ERROR;
		}else{
			checkpoint_id_rtos_scheduler = LLWATCHDOG_TIMER_IMPL_registerCheckpoint();
			if(WATCHDOG_TIMER_ERROR == checkpoint_id_rtos_scheduler){
				LLWATCHDOG_TIMER_DEBUG_TRACE("Fail to register a checkpoint for Watchdog FreeRTOS scheduler activity\n");
				xTimerDelete( wtd_rtos_timer, 0 );
				wtd_rtos_timer = NULL;
				return CHECKPOINT_FREERTOS_ERROR;
			}
			return CHECKPOINT_FREERTOS_OK;
		}
	}

	// Here, the timer is null (was deleted in stop function) and the checkpoint is still valid (not unregistered)
	// This may be caused by a failure of a previous call of watchdog_timer_freertos_scheduler_checkpoint_stop() function.
	// We cannot restart the scheduler checkpoint when it was not correctly stopped.
	LLWATCHDOG_TIMER_DEBUG_TRACE("Cannot restart the scheduler checkpoint when it was not correctly stopped.\n");
	return CHECKPOINT_FREERTOS_ERROR;
}

int32_t watchdog_timer_freertos_scheduler_checkpoint_stop(void){
	if(NULL != wtd_rtos_timer){
		if(pdFAIL == xTimerStop( wtd_rtos_timer, 0 )){
			LLWATCHDOG_TIMER_DEBUG_TRACE("Watchdog FreeRTOS fails to stop the SW timer\n");
			return CHECKPOINT_FREERTOS_ERROR;
		}else{
			xTimerDelete( wtd_rtos_timer, 0 );
			wtd_rtos_timer = NULL;
		}
	}
	if(WATCHDOG_TIMER_ERROR != checkpoint_id_rtos_scheduler){
		if(WATCHDOG_TIMER_ERROR ==  LLWATCHDOG_TIMER_IMPL_unregisterCheckpoint(checkpoint_id_rtos_scheduler)){
			LLWATCHDOG_TIMER_DEBUG_TRACE("Fail to unregister the FreeRTOS scheduler checkpoint!\n");
			return CHECKPOINT_FREERTOS_ERROR;
		}
		checkpoint_id_rtos_scheduler = WATCHDOG_TIMER_ERROR;
	}

	return CHECKPOINT_FREERTOS_OK;
}

void watchdog_timer_freertos_print_tasks_state(void){
#if (configUSE_TRACE_FACILITY == 1) && (configUSE_STATS_FORMATTING_FUNCTIONS == 1)
	char* tasks_log_buffer;
	uint16_t number_of_tasks = 0;
	/* Take a snapshot of the number of tasks in case it changes while this
	function is executing. */
	number_of_tasks = uxTaskGetNumberOfTasks();

	if(0 < number_of_tasks){
		/* Allocate a 50 bytes for each task to print data. */
		tasks_log_buffer = pvPortMalloc(number_of_tasks * 50 * sizeof(char));
		vTaskList(tasks_log_buffer);
		printf("******************************************\n");
		printf("Task \t    State    Prio    Stack     Num\n");
		printf("******************************************\n");
		printf("%s \n", tasks_log_buffer);
		printf("******************************************\n");
		printf("X-runnning, B–Blocked, R–Ready, D–Deleted, S–Suspended\n");
		/* The array is no longer needed, free the memory it consumes. */
		vPortFree(tasks_log_buffer);
	} else {
		LLWATCHDOG_TIMER_DEBUG_TRACE("No tasks retrieved with uxTaskGetNumberOfTasks()\n");
	}
#endif
}

/* Internal functions definition ---------------------------------------------*/

static void watchdog_timer_freertos_check_callback(xTimerHandle pxTimer){
	LLWATCHDOG_TIMER_DEBUG_TRACE("Watchdog FreeRTOS SW timer callback: attest the FreeRTOS scheduler activity with the checkpoint ID %d", checkpoint_id_rtos_scheduler);
	if(WATCHDOG_TIMER_ERROR == LLWATCHDOG_TIMER_IMPL_checkpoint(checkpoint_id_rtos_scheduler)){
		LLWATCHDOG_TIMER_DEBUG_TRACE("Watchdog FreeRTOS fail to attest the scheduler activity\n");
	}
}


#ifdef __cplusplus
	}
#endif

