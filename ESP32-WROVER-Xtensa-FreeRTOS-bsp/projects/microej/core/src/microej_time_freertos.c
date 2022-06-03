/*
 * C
 *
 * Copyright 2018-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
* @file
* @brief MicroEJ Time APIs implementation for FreeRTOS.
* @author MicroEJ Developer Team
* @version 0.1.0
*/

#include <stdint.h>

#include "microej.h"
#include "microej_time_freertos_configuration.h"
#include "microej_time.h"

#include FREERTOS_HEADER
#include FREERTOS_TASK_HEADER

#ifdef __cplusplus
	extern "C" {
#endif

#define MICROSECONDS_PER_TICK (1000000 / configTICK_RATE_HZ) // Number of microseconds per tick.
#define MILLISECONDS_PER_TICK (1000 / configTICK_RATE_HZ); // Number of milliseconds per tick.
#define MILLISECONDS_TO_MICROSECONDS (1000) // Converts milliseconds to microseconds.
#define MILLISECONDS_TO_NANOSECONDS (1000000) // Converts milliseconds to nanoseconds.

#if ( configUSE_TICKLESS_IDLE != 0 )

// This implementation is not compatible with the tickless mode of FreeRTOS because
// the tick counter will not be incremented correctly when the system sleeps.
#error "OS Tickless mode not managed by this implementation"

#endif

/** Offset in milliseconds from system time to application time. */
static uint64_t microej_application_time_offset = 0;

/**
 * @brief Initialisation of the timebase used by MicroEJ. Initialize in this function everything that is needed to implement the API.
 *
 */
void microej_time_init(void) {
	/* Nothing to do here. */
}

/**
 * @brief Converts a time in milliseconds to a number of system ticks.
 * The result is round up to the next tick (ie. converting back the resulting ticks to milliseconds results in a value greater than or equals to given time).
 * If an overflow occurs during the time conversion, a saturated number of ticks is returned.
 *
 * @param time the time to convert in milliseconds,
 * @return int64_t the number of ticks.
 */

int64_t microej_time_time_to_tick(int64_t time) {
	if (time < 0) {
		return 0;
	}

	int64_t time_microseconds  = time * MILLISECONDS_TO_MICROSECONDS;

	// Check for no overflow
	if (time_microseconds >= 0) {
		int64_t ticks64 = (time_microseconds + (MICROSECONDS_PER_TICK-1)) / MICROSECONDS_PER_TICK;

		// Check for no overflow
		if (ticks64 >= 0) {
			portTickType ticks = (portTickType)ticks64;

			// Check for no overflow
			if (ticks == ticks64) {
				return (int64_t)ticks;
			}
		}
	}

	// An overflow occurs: saturate the value to the max value for a portTickType
	return (int64_t)portMAX_DELAY;
}

/**
 * @brief Converts a number of system ticks in milliseconds.
 * If an overflow occurs during the conversion, a saturated number of milliseconds is returned.
 * @param ticks the number of ticks to convert in milliseconds,
 * @return int64_t the time in milliseconds.
 */
int64_t microej_time_tick_to_time(int64_t ticks) {
    if(ticks < 0){
       return 0;
    }
    int64_t time_milliseconds = ticks * MILLISECONDS_PER_TICK;
    // Check for no overflow
    if(time_milliseconds < 0){
        // An overflow occurs: saturate the value to int64_t max value
        return INT64_MAX;
    }
    return time_milliseconds;
}

/**
 * @brief Gets the current platform or application time.
 *
 * @param is_platform_time set to 1 to get the platform time or set to 0 to get the time elapsed since midnight, January 1, 1970 UTC.
 * @return int64_t the time in milliseconds.
 */
int64_t microej_time_get_current_time(uint8_t is_platform_time) {
	int64_t ticks = (int64_t) xTaskGetTickCount();
	int64_t time = microej_time_tick_to_time(ticks);

	if (is_platform_time == MICROEJ_TRUE) {
		return time;
	} else {
		int64_t applicationTime = time + microej_application_time_offset;
		return applicationTime;
	}
}

int64_t microej_time_get_time_nanos() {
	return microej_time_get_current_time(MICROEJ_TRUE) * MILLISECONDS_TO_NANOSECONDS;
}

/**
 * @brief Sets the application time.
 *
 * @param time value in milliseconds of the time elapsed since midnight, January 1, 1970 UTC.
 */
void microej_time_set_application_time(int64_t time) {
	int64_t currentTime = (int64_t) microej_time_get_current_time(MICROEJ_TRUE);
	microej_application_time_offset = time - currentTime;
}

#ifdef __cplusplus
	}
#endif
