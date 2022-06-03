/*
 * C
 *
 * Copyright 2021-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief WATCHDOG TIMER generic implementation part.
 * @author MicroEJ Developer Team
 * @version 3.1.0
 */

/* Includes ------------------------------------------------------------------*/
#include <LLWATCHDOG_TIMER_configuration.h>
#include <stdlib.h>
#include <stdint.h>

#include "sni.h"
#include "osal.h"

#include "LLWATCHDOG_TIMER_impl.h"
#include "watchdog_timer_helper.h"

#ifdef __cplusplus
	extern "C" {
#endif

/* Macros and Defines --------------------------------------------------------*/

/**
 * Sanity check between the expected version of the configuration and the actual version of
 * the configuration.
 * If an error is raised here, it means that a new version of the CCO has been installed and
 * the configuration ecom_wifi_configuration.h must be updated based on the one provided
 * by the new CCO version.
 */
#if WATCHDOG_TIMER_CONFIGURATION_VERSION != 1
	#error "Version of the configuration file LLWATCHDOG_TIMER_TIMER_configuration.h is not compatible with this implementation."
#endif

/**
 * @brief These macros indicate the watchdog status.
 */
#define WATCHDOG_TIMER_STATUS_OFF             0 /*!< The watchdog timer is not used, it must be initialized. */
#define WATCHDOG_TIMER_STATUS_INITIALIZED     1 /*!< The watchdog timer has been initialized. Checkpoints can be registered. */
#define WATCHDOG_TIMER_STATUS_STARTED         2 /*!< The watchdog timer is running. It can be stopped. */

/**
 * The number max of checkpoint registrations.
 */
#define WATCHDOG_TIMER_MAX_CHECKPOINT_NR      32

/**
 * The smallest checkpoint ID that can be registered.
 */
#define WATCHDOG_TIMER_MIN_CHECKPOINT_NR      0

/* Globals -------------------------------------------------------------------*/

/**
 * @brief Status of watchdog timer initialization process.
 */
static uint32_t watchdog_timer_status = WATCHDOG_TIMER_STATUS_OFF;

/**
 * @brief Mutex to protect access to checkpoint registers.
 */
static OSAL_mutex_handle_t mutex_checkpoints;

/**
 * @brief Variable to store registered checkpoints.
 */
static volatile int32_t checkpoints_registered = 0;

/**
 * @brief Variable to store passed checkpoints.
 */
static volatile int32_t checkpoints_passed = 0;

/**
 * @brief Id given to a new checkpoint which registered in watchdog monitoring.
 */
static volatile int32_t checkpoint_id_given = 0;

/* Internal function prototypes ---------------------------------------------*/

/**
 * @brief Check all registered checkpoints status and if all are checked then refresh watchdog.
 *
 * @param[in] None
 *
 * @return None
 */
static int32_t LLWATCHDOG_TIMER_check(void);

/**
 * @brief Check whether the given checkpoint ID is registered or not.
 *
 * @param[in] checkpoint_id the checkpoint ID to be checked
 *
 * @return WATCHDOG_TIMER_TRUE if the checkpoint is registered, WATCHDOG_TIMER_FALSE otherwise.
 */
static uint8_t LLWATCHDOG_TIMER_is_registered_checkpoint(int32_t checkpoint_id);

/* Watchdog LLAPI functions ---------------------------------------------*/

int32_t LLWATCHDOG_TIMER_IMPL_init(void) {
	if (WATCHDOG_TIMER_STATUS_OFF == watchdog_timer_status) {
		OSAL_mutex_create((uint8_t*)"WatchdogMutex", &mutex_checkpoints);
		if (NULL == mutex_checkpoints) {
			LLWATCHDOG_TIMER_DEBUG_TRACE("Watchdog failed to create checkpoint mutex!\n");
			SNI_throwNativeIOException(WATCHDOG_TIMER_ERROR, "Watchdog failed to create checkpoint mutex");
			return WATCHDOG_TIMER_ERROR;
		}
		if (WATCHDOG_TIMER_OK != LLWATCHDOG_TIMER_IMPL_init_action()) {
			SNI_throwNativeIOException(WATCHDOG_TIMER_ERROR, "Watchdog initialization failed");
			return WATCHDOG_TIMER_ERROR;
		}
		watchdog_timer_status = WATCHDOG_TIMER_STATUS_INITIALIZED;
	} else {
		LLWATCHDOG_TIMER_DEBUG_TRACE("Watchdog already initialized\n");
	}
	return WATCHDOG_TIMER_OK;
}

int32_t LLWATCHDOG_TIMER_IMPL_start(void) {
	if (WATCHDOG_TIMER_STATUS_INITIALIZED == watchdog_timer_status) {
		if (WATCHDOG_TIMER_OK != LLWATCHDOG_TIMER_IMPL_start_action()) {
			SNI_throwNativeIOException(WATCHDOG_TIMER_ERROR, "Watchdog failed to start");
			return WATCHDOG_TIMER_ERROR;
		}
	} else {
		if (WATCHDOG_TIMER_STATUS_STARTED == watchdog_timer_status) {
			LLWATCHDOG_TIMER_DEBUG_TRACE("Watchdog already started\n");
			return WATCHDOG_TIMER_OK;
		}
		SNI_throwNativeIOException(WATCHDOG_TIMER_ERROR, "Watchdog not initialized");
		return WATCHDOG_TIMER_ERROR;
	}
	watchdog_timer_status = WATCHDOG_TIMER_STATUS_STARTED;
	return WATCHDOG_TIMER_OK;
}

int32_t LLWATCHDOG_TIMER_IMPL_stop(void) {
	if (WATCHDOG_TIMER_STATUS_STARTED != watchdog_timer_status) {
		LLWATCHDOG_TIMER_DEBUG_TRACE("Watchdog not started\n");
		return WATCHDOG_TIMER_OK;
	}
	if (WATCHDOG_TIMER_OK != LLWATCHDOG_TIMER_IMPL_stop_action()) {
		SNI_throwNativeIOException(WATCHDOG_TIMER_ERROR, "Error while stopping the watchdog");
		return WATCHDOG_TIMER_ERROR;
	}
	watchdog_timer_status = WATCHDOG_TIMER_STATUS_INITIALIZED;
	if (OSAL_OK == OSAL_mutex_take(&mutex_checkpoints, OSAL_INFINITE_TIME)) {
		checkpoints_registered = 0;
		checkpoints_passed = 0;
		checkpoint_id_given = 0;
		OSAL_mutex_give(&mutex_checkpoints);
		return WATCHDOG_TIMER_OK;
	}
	LLWATCHDOG_TIMER_DEBUG_TRACE("OSAL_mutex_take fail\n");
	SNI_throwNativeIOException(WATCHDOG_TIMER_ERROR, "OSAL_mutex_take fail");
	return WATCHDOG_TIMER_ERROR;
}

int32_t LLWATCHDOG_TIMER_IMPL_refresh(void) {
	return LLWATCHDOG_TIMER_IMPL_refresh_action();
}

int32_t LLWATCHDOG_TIMER_IMPL_registerCheckpoint(void) {
	int32_t checkpoint_id = 0;
	if (WATCHDOG_TIMER_STATUS_OFF != watchdog_timer_status) {
		if (WATCHDOG_TIMER_MAX_CHECKPOINT_NR > checkpoint_id_given) {
			/* Poll the mutex */
			if (OSAL_OK == OSAL_mutex_take(&mutex_checkpoints, OSAL_INFINITE_TIME)) {
				checkpoint_id = checkpoint_id_given;
				int32_t tmp_checkpoint_id_given = checkpoint_id_given;
				checkpoints_registered |= (1 << tmp_checkpoint_id_given);
				checkpoint_id_given++; // Update value for the next registration
				OSAL_mutex_give(&mutex_checkpoints);
			} else {
				LLWATCHDOG_TIMER_DEBUG_TRACE("OSAL_mutex_take fail\n");
				SNI_throwNativeIOException(WATCHDOG_TIMER_ERROR, "OSAL_mutex_take fail");
				return WATCHDOG_TIMER_ERROR;
			}
		} else {
			LLWATCHDOG_TIMER_DEBUG_TRACE("Too many registered checkpoint IDs (32)\n");
			SNI_throwNativeIOException(WATCHDOG_TIMER_ERROR, "Too many registered checkpoint IDs (32)");
			return WATCHDOG_TIMER_ERROR;
		}
	} else {
		SNI_throwNativeIOException(WATCHDOG_TIMER_ERROR, "Watchdog not initialized");
		return WATCHDOG_TIMER_ERROR;
	}
	return checkpoint_id;
}

int32_t LLWATCHDOG_TIMER_IMPL_unregisterCheckpoint(int32_t checkpoint_id) {
	if (WATCHDOG_TIMER_STATUS_OFF != watchdog_timer_status) {
		if (OSAL_OK == OSAL_mutex_take(&mutex_checkpoints, OSAL_INFINITE_TIME)) {
			if (LLWATCHDOG_TIMER_is_registered_checkpoint(checkpoint_id)) {
				checkpoints_registered &= ~(1 << checkpoint_id);
				checkpoints_passed &= ~(1 << checkpoint_id);
				OSAL_mutex_give(&mutex_checkpoints);
			} else {
				OSAL_mutex_give(&mutex_checkpoints);
				LLWATCHDOG_TIMER_DEBUG_TRACE("Wrong checkpoint_id = %d\n", checkpoint_id);
				SNI_throwNativeIOException(WATCHDOG_TIMER_ERROR, "Wrong checkpoint id");
				return WATCHDOG_TIMER_ERROR;
			}
		} else {
			LLWATCHDOG_TIMER_DEBUG_TRACE("OSAL_mutex_take fail\n");
			SNI_throwNativeIOException(WATCHDOG_TIMER_ERROR, "OSAL_mutex_take fail");
			return WATCHDOG_TIMER_ERROR;
		}
	} else {
		SNI_throwNativeIOException(WATCHDOG_TIMER_ERROR, "Watchdog not initialized");
		return WATCHDOG_TIMER_ERROR;
	}
	return WATCHDOG_TIMER_OK;
}

int32_t LLWATCHDOG_TIMER_IMPL_checkpoint(int32_t checkpoint_id) {
	if (WATCHDOG_TIMER_STATUS_OFF != watchdog_timer_status) {
		/* Poll the mutex */
		if (OSAL_OK == OSAL_mutex_take(&mutex_checkpoints, OSAL_INFINITE_TIME)) {
			if (!LLWATCHDOG_TIMER_is_registered_checkpoint(checkpoint_id)) {
				OSAL_mutex_give(&mutex_checkpoints);
				SNI_throwNativeIOException(WATCHDOG_TIMER_ERROR, "The checkpointId is not registered");
				return WATCHDOG_TIMER_ERROR;
			}
			checkpoints_passed |= (1 << checkpoint_id);
			// Check all checkpoints and refresh the watchdog if all passed
			int32_t ret = LLWATCHDOG_TIMER_check();
			OSAL_mutex_give(&mutex_checkpoints);
			if(ret != WATCHDOG_TIMER_OK){
				SNI_throwNativeIOException(ret, "Error while refreshing the watchdog!");
			}
			return ret;
		} else {
			LLWATCHDOG_TIMER_DEBUG_TRACE("OSAL_mutex_take fail\n");
			SNI_throwNativeIOException(WATCHDOG_TIMER_ERROR, "OSAL_mutex_take fail");
			return WATCHDOG_TIMER_ERROR;
		}
	} else {
		SNI_throwNativeIOException(WATCHDOG_TIMER_ERROR, "Watchdog not initialized");
		return WATCHDOG_TIMER_ERROR;
	}
}

uint8_t LLWATCHDOG_TIMER_IMPL_isResetCause(void) {
	return LLWATCHDOG_TIMER_IMPL_isResetCause_action();
}

int32_t LLWATCHDOG_TIMER_IMPL_getWatchdogTimeoutMs() {
	return LLWATCHDOG_TIMER_IMPL_getWatchdogTimeoutMs_action();
}

/* Internal function definitions ---------------------------------------------*/

static int32_t LLWATCHDOG_TIMER_check(void) {
	int32_t checkpoints_passed_tmp = checkpoints_passed;
	checkpoints_passed_tmp &= checkpoints_registered;
	if (checkpoints_registered == checkpoints_passed_tmp) {
		// all checkpoints have been passed
		//refresh the watchdog
		if(WATCHDOG_TIMER_OK == LLWATCHDOG_TIMER_IMPL_refresh()){
			//refresh succeed
			checkpoints_passed = 0; // clear passed checkpoints
			LLWATCHDOG_TIMER_DEBUG_TRACE("All checkpoints passed => Watchdog refresh!\n");
		}else{
			LLWATCHDOG_TIMER_DEBUG_TRACE("Error while refreshing the watchdog!\n");
			return WATCHDOG_TIMER_ERROR;
		}
	} else {
		//all checkpoints not passed
		LLWATCHDOG_TIMER_DEBUG_TRACE("All checkpoints not passed => Watchdog NOT refreshed! (checkpoints_registered=0x%08x, checkpoints_passed=0x%08x)\n",checkpoints_registered,checkpoints_passed);
	}
	return WATCHDOG_TIMER_OK;
}

static uint8_t LLWATCHDOG_TIMER_is_registered_checkpoint(int32_t checkpoint_id) {
	if (WATCHDOG_TIMER_MAX_CHECKPOINT_NR > checkpoint_id
			&& WATCHDOG_TIMER_MIN_CHECKPOINT_NR <= checkpoint_id) {
		int32_t mask_checkpoint_id = 1 << checkpoint_id;
		LLWATCHDOG_TIMER_DEBUG_TRACE("In is_registered_checkpoint, (mask_checkpoint_id = 0x%08x, checkpoints_registered = 0x%08x) \n", mask_checkpoint_id, checkpoints_registered);
		if (0 < (mask_checkpoint_id & checkpoints_registered)) {
			return WATCHDOG_TIMER_TRUE;
		}
	}
	return WATCHDOG_TIMER_FALSE;
}

#ifdef __cplusplus
	}
#endif

