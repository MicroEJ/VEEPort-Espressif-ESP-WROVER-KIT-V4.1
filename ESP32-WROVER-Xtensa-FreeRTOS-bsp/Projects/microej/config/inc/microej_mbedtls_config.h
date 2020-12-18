/*
 * C
 *
 * Copyright 2019-2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 *
 */

#if !defined __MICROEJ_MBEDTLS_CONFIG_H__
# define __MICROEJ_MBEDTLS_CONFIG_H__

// Define maximum certificate length to 4 (2 intermediate + 1 leaf + 1 root)
#define MBEDTLS_X509_MAX_INTERMEDIATE_CA 2

// Use microej allocator to allocate SSL contextes in external ram
#include "microej_allocator.h"
#include <time.h>
#include "ssl_utils.h"

#define MBEDTLS_PLATFORM_MEMORY

void* microej_calloc4tls(size_t nmemb, size_t size);
void microej_free4tls(void *ptr);

#define MBEDTLS_PLATFORM_TIME_MACRO custom_mbedtls_time_sec /**< Custom time (seconds) macro to use. */
#define MBEDTLS_PLATFORM_CALLOC_MACRO microej_calloc4tls
#define MBEDTLS_PLATFORM_FREE_MACRO microej_free4tls

#endif // !defined __MICROEJ_MBEDTLS_CONFIG_H__
