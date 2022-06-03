/*
 * C
 *
 * Copyright 2021-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#ifndef WATCHDOG_TIMER_FREERTOS_CONF_H
#define WATCHDOG_TIMER_FREERTOS_CONF_H

/**
 * @file
 * @brief WATCHDOG TIMER FreeRTOS CCO configuration file.
 * @author MicroEJ Developer Team
 * @version 2.0.1
 * @date 5 August 2021
 */

#ifdef __cplusplus
	extern "C" {
#endif

/**
 * @brief Compatibility sanity check value.
 * This define value is checked in the implementation to validate that the version of this configuration
 * is compatible with the implementation.
 *
 * This value must not be changed by the user of the CCO.
 * This value must be incremented by the implementor of the CCO when a configuration define is added, deleted or modified.
 */
#define WATCHDOG_TIMER_FREERTOS_CONFIGURATION_VERSION (1)


/**
 * @brief Period (in milliseconds) of the FreeRTOS timer that will attest the FreeRTOS scheduler activity.
 * Warning This value must be lower than the watchdog timeout period.
 */
#ifndef FREERTOS_TIMER_PERIOD_MS
#define FREERTOS_TIMER_PERIOD_MS 15000	// ms
#endif


#ifdef __cplusplus
	}
#endif

#endif /* WATCHDOG_TIMER_FREERTOS_CONF_H */

