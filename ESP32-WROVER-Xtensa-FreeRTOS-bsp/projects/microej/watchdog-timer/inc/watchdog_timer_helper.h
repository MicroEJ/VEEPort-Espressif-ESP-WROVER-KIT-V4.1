/*
 * C
 *
 * Copyright 2021-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */
#ifndef WATCHDOG_TIMER_HELPER_H
#define WATCHDOG_TIMER_HELPER_H

/**
 * @file
 * @brief WATCHDOG TIMER hardware dependent interface.
 * @author MicroEJ Developer Team
 * @version 3.1.0
 */

#include <stdint.h>
#include "LLWATCHDOG_TIMER_impl.h"

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
int32_t LLWATCHDOG_TIMER_IMPL_init_action(void);

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
int32_t LLWATCHDOG_TIMER_IMPL_start_action(void);

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
int32_t LLWATCHDOG_TIMER_IMPL_stop_action(void);

/**
 * @brief Check whether the last reset was caused by the watchdog timer or not.
 * Action requested by <code>LLWATCHDOG_TIMER_IMPL_isResetCause()</code>.
 *
 * @param[in] None
 *
 * @return WATCHDOG_TIMER_TRUE if watchdog was reset condition, WATCHDOG_TIMER_FALSE otherwise.
 */
uint8_t LLWATCHDOG_TIMER_IMPL_isResetCause_action(void);

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
int32_t LLWATCHDOG_TIMER_IMPL_refresh_action(void);

/**
 * @brief Return the watchdog timeout period configuration parameter. It's the watchdog timeout period in milliseconds.
 * Action requested by <code>LLWATCHDOG_TIMER_IMPL_getWatchdogTimeoutMs()</code>.
 *
 * @param[in] None
 *
 * @return the integer value of watchdog timeout period.
 */
int32_t LLWATCHDOG_TIMER_IMPL_getWatchdogTimeoutMs_action(void);


#ifdef __cplusplus
	}
#endif

#endif /* WATCHDOG_TIMER_HELPER_H */
