/*
 * C
 *
 * Copyright 2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */
#include "ssl_utils.h"
#include "microej.h"

#include "lwip/sys.h"

#ifdef __cplusplus
 extern "C" {
#endif

 /**
  * @brief External function used to retrieve currentTime (defined in LLMJVM)
  */
 extern int64_t LLMJVM_IMPL_getCurrentTime__Z(uint8_t system);

/* ---- Specific C Time function ---- */

time_t custom_mbedtls_time_sec(time_t *time)
{
	time_t currentTime = (time_t)(LLMJVM_IMPL_getCurrentTime__Z(MICROEJ_FALSE) / 1000);

	if (time != NULL)
	{
		*time = currentTime;
	}

	return currentTime;
}

#ifdef __cplusplus
}
#endif
