/*
 * C
 *
 * Copyright 2021-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#ifndef WATCHDOG_TIMER_FREERTOS_H
#define WATCHDOG_TIMER_FREERTOS_H

/**
 * @file
 * @brief WATCHDOG TIMER FreeRTOS scheduler checkpoint CCO header file.
 * @author MicroEJ Developer Team
 * @version 2.0.1
 * @date 5 August 2021
 */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

#ifdef __cplusplus
	extern "C" {
#endif


/* Macros and Defines --------------------------------------------------------*/

/** @brief Error codes constants */
#define CHECKPOINT_FREERTOS_OK			0	/*!< Error code returned on success. */
#define CHECKPOINT_FREERTOS_ERROR 		-1	/*!< Error code returned on failure. */

/* Globals -------------------------------------------------------------------*/



/* Project functions----------------------------------------------------------*/

/**
 * @brief Starts the FreeRTOS scheduler checkpoint.
 *
 * This function creates a FreeRTOS software timer and registers a watchdog checkpoint for the FreeRTOS scheduler activity.
 * Each time the timer expires, it calls <code>LLWATCHDOG_TIMER_IMPL_checkpoint(unt32_t checkpointId)</code> with the recorded ID to attest the FreeRTOS scheduler activity.
 *
 * @return
 *   - CHECKPOINT_FREERTOS_OK 			on success.
 *   - CHECKPOINT_FREERTOS_ERROR  		if an error occurs.
 *
 * @note This function does nothing if the checkpoint is already started.
 *
 */
int32_t watchdog_timer_freertos_scheduler_checkpoint_start(void);


/**
 * @brief Stops the FreeRTOS scheduler checkpoint
 *
 * This function stops the software timer and then unregisters the FreeRTOS scheduler checkpoint created in <code>watchdog_timer_freertos_scheduler_checkpoint_start(void)</code>.
 *
 * @return
 *   - CHECKPOINT_FREERTOS_OK 			on success.
 *   - CHECKPOINT_FREERTOS_ERROR  		if an error occurs.
 *
 * @note If this function fails when attempting to unregister the checkpoint ID, the software timer that has been stopped can no longer attest the scheduler activity and this may trigger the watchdog.
 *
 */
int32_t watchdog_timer_freertos_scheduler_checkpoint_stop(void);

/**
 * @brief Prints the current FreeRTOS tasks state.
 *
 * @note The macros <code>configUSE_TRACE_FACILITY</code> and <code>configUSE_STATS_FORMATTING_FUNCTIONS</code> need to be
 * defined and set to 1 in FreeRTOSConfig.h otherwise no task state will be printed.
 *
 */
void watchdog_timer_freertos_print_tasks_state(void);

#ifdef __cplusplus
	}
#endif

#endif /* WATCHDOG_TIMER_FREERTOS_H */
