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
 * @version 2.0.2
 * @date 27 February, 2022
 */

/* Includes ------------------------------------------------------------------*/
#include <LLWATCHDOG_TIMER_configuration.h>
#include <stdlib.h>
#include <stdint.h>
#include "esp_task_wdt.h"

#include "sni.h"
#include "osal.h"

#include "LLWATCHDOG_TIMER_impl.h"
#include "watchdog_timer_helper.h"

#define TWDT_TIMEOUT_S          3

#ifdef __cplusplus
	extern "C" {
#endif


/**
 * @brief Initializes here the watchdog timer.
 * Action requested by <code>LLWATCHDOG_TIMER_IMPL_init()</code>.
 *
 * @param[in] None
 *
 * @return
 *   - WATCHDOG_TIMER_OK on success.
 *   - WATCHDOG_TIMER_ERROR if an error occurs.
 */
int32_t LLWATCHDOG_TIMER_IMPL_init_action(void)
{
    LLWATCHDOG_TIMER_DEBUG_TRACE("LLWATCHDOG_TIMER_IMPL_init_action\n");
	if (ESP_OK == esp_task_wdt_init(TWDT_TIMEOUT_S, true))
	{
        return WATCHDOG_TIMER_OK;	
	}
	else
	{
        return WATCHDOG_TIMER_ERROR;	
	}
}

/**
 * @brief Starts the watchdog timer counting.
 * Action requested by <code>LLWATCHDOG_TIMER_IMPL_start()</code>.
 *
 * @param[in] None
 *
 * @return
 *   - WATCHDOG_TIMER_OK on success.
 *   - WATCHDOG_TIMER_ERROR if an error occurs.
 *
 */
int32_t LLWATCHDOG_TIMER_IMPL_start_action(void)
{
    LLWATCHDOG_TIMER_DEBUG_TRACE("LLWATCHDOG_TIMER_IMPL_start_action\n");
	if (ESP_OK == esp_task_wdt_add(xTaskGetCurrentTaskHandle()))
	{
        return WATCHDOG_TIMER_OK;	
	}
	else
	{
        return WATCHDOG_TIMER_ERROR;	
	}
}

/**
 * @brief Stops the watchdog timer.
 * Action requested by <code>LLWATCHDOG_TIMER_IMPL_stop()</code>.
 *
 * @param[in] None
 *
 * @return
 *   - WATCHDOG_TIMER_OK on success.
 *   - WATCHDOG_TIMER_ERROR if an error occurs.
 */
int32_t LLWATCHDOG_TIMER_IMPL_stop_action(void)
{
    LLWATCHDOG_TIMER_DEBUG_TRACE("LLWATCHDOG_TIMER_IMPL_stop_action\n");
	if (ESP_OK == esp_task_wdt_delete(xTaskGetCurrentTaskHandle()))
	{
        return WATCHDOG_TIMER_OK;	
	}
	else
	{
        return WATCHDOG_TIMER_ERROR;	
	}
}

/**
 * @brief Check whether the last reset was caused by the watchdog timer or not.
 * Action requested by <code>LLWATCHDOG_TIMER_IMPL_isResetCause()</code>.
 *
 * @param[in] None
 *
 * @return WATCHDOG_TIMER_TRUE if watchdog was reset condition, WATCHDOG_TIMER_FALSE otherwise.
 */
uint8_t LLWATCHDOG_TIMER_IMPL_isResetCause_action(void)
{
    LLWATCHDOG_TIMER_DEBUG_TRACE("LLWATCHDOG_TIMER_IMPL_isResetCause_action %d\n",esp_reset_reason());
	if (ESP_RST_TASK_WDT == esp_reset_reason())
	{
        return WATCHDOG_TIMER_TRUE;	
	}
	else
	{
        return WATCHDOG_TIMER_FALSE;	
	}	
}

/**
 * @brief Resets the watchdog timer counter. This function is called if all checkpoints passed.
 * Action requested by <code>LLWATCHDOG_TIMER_IMPL_refresh()</code>.
 *
 * @param[in] None
 *
 * @return
 *   - WATCHDOG_TIMER_OK on success.
 *   - WATCHDOG_TIMER_ERROR if an error occurs.
 */
int32_t LLWATCHDOG_TIMER_IMPL_refresh_action(void)
{
    LLWATCHDOG_TIMER_DEBUG_TRACE("LLWATCHDOG_TIMER_IMPL_refresh_action\n");
	if (ESP_OK == esp_task_wdt_reset())
	{
        return WATCHDOG_TIMER_OK;	
	}
	else
	{
        return WATCHDOG_TIMER_ERROR;	
	}
}

/**
 * @brief Return the watchdog timeout period configuration parameter. It's the watchdog timeout period in milliseconds.
 * Action requested by <code>LLWATCHDOG_TIMER_IMPL_getWatchdogTimeoutMs()</code>.
 *
 * @param[in] None
 *
 * @return the integer value of watchdog timeout period.
 */
int32_t LLWATCHDOG_TIMER_IMPL_getWatchdogTimeoutMs_action(void)
{
    LLWATCHDOG_TIMER_DEBUG_TRACE("LLWATCHDOG_TIMER_IMPL_getWatchdogTimeoutMs_action\n");
    return TWDT_TIMEOUT_S;
}

#ifdef __cplusplus
	}
#endif

