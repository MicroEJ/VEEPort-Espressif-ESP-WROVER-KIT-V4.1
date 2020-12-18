/*
 * C
 *
 * Copyright 2018-2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */

#include "microej_time.h"
#include "freertos/FreeRTOS.h"
#include "esp_freertos_hooks.h"

#include "microej.h"
#include "stdint.h"
#include "freertos/task.h"


// Simple time implementation that uses OS ticks to compute time.

/*
 *********************************************************************************************************
 *                                             DEFINES
 *********************************************************************************************************
 */

#if ( configUSE_TICKLESS_IDLE != 0 )

// This implementation is not compatible with the tickless mode of FreeRTOS because
// the tick counter will not be incremented correctly when the system sleeps.
#error "OS Tickless mode not managed by this implementation"

#endif


/*
 *********************************************************************************************************
 * 	                                      INTERNAL FUNCTIONS
 *********************************************************************************************************
 */

/** Offset in ms from system time to application time */
static uint64_t microej_application_time_offset = 0;



/*
 *********************************************************************************************************
 * 	                                      PUBLIC FUNCTIONS
 *********************************************************************************************************
 */


/*
 * Convert a time in milliseconds in ticks.
 * Result is round up to the next tick (ie. converting back the resulting ticks
 * to milliseconds results in a value greater than or equals to given time).
 */
int64_t microej_time_timeToTick(int64_t time)
{
	int64_t timeus = time * 1000;
	int64_t os_tick = 1000000 / configTICK_RATE_HZ; // tick in microseconds
	return (timeus + (os_tick-1)) / os_tick;
}

/*
 * Convert a nb of ticks in milliseconds.
 * Result is round up to the next millisecond (ie. converting back the resulting ms
 * to ticks results in a value greater than or equals to given time).
 */
int64_t microej_time_tickToTime(int64_t ticks)
{
	int64_t timems = (ticks * 1000) / configTICK_RATE_HZ;

	return timems;
}


int64_t microej_time_getcurrenttime(uint8_t isPlatformTime){
	/*
	 * /!\
	 * isPlatformTime == true when ej.bon.Util.platformTimeMillis
	 * isPlatformTime == false when java.lang.System.currentTimeMillis
	 * /!\
	 */

	int64_t ticks = (int64_t)xTaskGetTickCount();
	int64_t time = microej_time_tickToTime(ticks);

	if(isPlatformTime == MICROEJ_TRUE){
		return time;
	}
	else {
		int64_t applicationTime = time + microej_application_time_offset;
		return applicationTime;
	}
}

int64_t microej_time_gettimenanos(){
	return microej_time_getcurrenttime(MICROEJ_TRUE) * 1000000;
}

void microej_time_setapplicationtime(int64_t t)
{
    int64_t currentTime = (int64_t)microej_time_getcurrenttime(MICROEJ_TRUE);
    microej_application_time_offset = t - currentTime;
}

