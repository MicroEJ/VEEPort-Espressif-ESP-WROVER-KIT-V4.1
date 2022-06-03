/*
 * C
 *
 * Copyright 2021-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */
#ifndef LLWATCHDOG_TIMER_IMPL_H
#define LLWATCHDOG_TIMER_IMPL_H

/**
 * @file
 * @brief MicroEJ Watchdog Low Level API
 * @author MicroEJ Developer Team
 * @version 3.0.0
 * @date 28 July 2021
 */

#include <stdint.h>
#include <intern/LLWATCHDOG_TIMER_impl.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Macros and Defines --------------------------------------------------------*/

/**
 * Watchdog timer error codes.
 */
#define WATCHDOG_TIMER_OK			0	/*!< Error code returned on success. */
#define WATCHDOG_TIMER_ERROR		-1	/*!< Error code returned on failure. */

/**
 * Watchdog timer boolean macros.
 */
#define WATCHDOG_TIMER_FALSE		0	/*!< Value returned if False. */
#define WATCHDOG_TIMER_TRUE			1	/*!< Value returned if True. */

/* Prototype functions----------------------------------------------------------*/

/**
 * @brief Initializes the watchdog timer and the checkpoints registration system.
 *
 * This method does nothing if the watchdog is already initialized.
 *
 * @return WATCHDOG_TIMER_OK on success or WATCHDOG_TIMER_ERROR if an error occurs.
 *
 * @note Throws NativeException on error when called from Java.
 */
int32_t LLWATCHDOG_TIMER_IMPL_init(void);

/**
 * @brief Starts the watchdog timer.
 *
 * This method does nothing if the watchdog is already started.
 *
 * @return WATCHDOG_TIMER_OK on success or WATCHDOG_TIMER_ERROR if an error occurs.
 *
 * @note Throws NativeException on error when called from Java.
 */
int32_t LLWATCHDOG_TIMER_IMPL_start(void);

/**
 * @brief Stops the watchdog timer and unregisters all existing checkpoints.
 *
 * This method does nothing if the watchdog is not started.
 *
 * @return WATCHDOG_TIMER_OK on success or WATCHDOG_TIMER_ERROR if an error occurs.
 *
 * @note Throws NativeException on error when called from Java.
 */
int32_t LLWATCHDOG_TIMER_IMPL_stop(void);

/**
 * @brief Refreshes the watchdog.
 *
 * @return WATCHDOG_TIMER_OK on success or WATCHDOG_TIMER_ERROR if an error occurs.
 *
 * @note Throws NativeException on error when called from Java.
 *
 * @warning Do not use this function on your own! If you want to refresh the watchdog,
 * 			always use this function: <code>LLWATCHDOG_TIMER_IMPL_checkpoint</code>.
 */
int32_t LLWATCHDOG_TIMER_IMPL_refresh(void);

/**
 * @brief Checks whether the last reset was caused by the watchdog or not.
 *
 * @return WATCHDOG_TIMER_TRUE if the last reset was caused by the watchdog, WATCHDOG_TIMER_FALSE otherwise.
 *
 * @note Throws NativeException on error when called from Java.
 *
 */
uint8_t LLWATCHDOG_TIMER_IMPL_isResetCause(void);

/**
 * @brief Registers a watchdog timer checkpoint and returns its corresponding Id.
 *
 * The maximum number of checkpoints that can be registered is 32 and the
 * checkpoint Id values are comprise between 0 and 31.
 *
 * @return the identifier of the registered checkpoint or WATCHDOG_TIMER_ERROR if an error occurs.
 *
 * @note Throws NativeException on error when called from Java.
 */
int32_t LLWATCHDOG_TIMER_IMPL_registerCheckpoint(void);

/**
 * @brief Unregisters the checkpoint with the given <code>checkpoint_id</code>.
 *
 * @param[in] checkpoint_id the identifier of the checkpoint to unregister.
 *
 * @return WATCHDOG_TIMER_OK on success or WATCHDOG_TIMER_ERROR if an error occurs.
 *
 * @note Throws NativeException on error when called from Java.
 */
int32_t LLWATCHDOG_TIMER_IMPL_unregisterCheckpoint(int32_t checkpoint_id);

/**
 * @brief Validates the checkpoint identified by the given <code>checkpoint_id</code> and
 * refreshes the watchdog timer if all checkpoints are passed.
 *
 * Be sure that all the checkpoints are validated before the watchdog timeout
 * expires otherwise a hardware reset will be performed.
 *
 * @param[in] checkpoint_id id used to identify each checkpoint using the watchdog checking routine.
 * 						   Must be between 0 and 31.
 *
 * @return WATCHDOG_TIMER_OK on success or WATCHDOG_TIMER_ERROR if an error occurs.
 *
 * @note Throws NativeException on error when called from Java.
 */
int32_t LLWATCHDOG_TIMER_IMPL_checkpoint(int32_t checkpoint_id);

/**
 * @brief Gets the configured watchdog timeout period (in milliseconds).
 *
 * @return the watchdog timeout period in milliseconds.
 *
 * @note Throws NativeException on error when called from Java.
 */
int32_t LLWATCHDOG_TIMER_IMPL_getWatchdogTimeoutMs(void);

#ifdef __cplusplus
}
#endif

#endif // LLWATCHDOG_TIMER_IMPL_H
