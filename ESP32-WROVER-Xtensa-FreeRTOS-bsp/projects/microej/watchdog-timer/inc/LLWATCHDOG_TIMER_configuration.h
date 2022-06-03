/*
 * C
 *
 * Copyright 2021-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#ifndef LLWATCHDOG_TIMER_CONF_H
#define LLWATCHDOG_TIMER_CONF_H

/**
 * @file
 * @brief WATCHDOG TIMER configuration file.
 * @author MicroEJ Developer Team
 * @version 3.0.1
 * @date 5 August 2021
 */


#ifdef __cplusplus
	extern "C" {
#endif

/* Macros and Defines --------------------------------------------------------*/

/**
 * @brief Compatibility sanity check value.
 * This define value is checked in the implementation to validate that the version of this configuration
 * is compatible with the implementation.
 *
 * This value must not be changed by the user of the CCO.
 * This value must be incremented by the implementor of the CCO when a configuration define is added, deleted or modified.
 */
#define WATCHDOG_TIMER_CONFIGURATION_VERSION (1)

/**
 * @brief Set this define to print debug traces.
 */
//#define LLWATCHDOG_TIMER_DEBUG
#ifdef LLWATCHDOG_TIMER_DEBUG
#include <stdio.h>
#define LLWATCHDOG_TIMER_DEBUG_TRACE printf("[DEBUG] ");printf
#else
#define LLWATCHDOG_TIMER_DEBUG_TRACE(...) ((void) 0)
#endif

#ifdef __cplusplus
	}
#endif

#endif /* LLWATCHDOG_TIMER_CONF_H */
