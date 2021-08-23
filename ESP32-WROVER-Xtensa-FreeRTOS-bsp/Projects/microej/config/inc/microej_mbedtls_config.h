/*
 * C
 *
 * Copyright 2019-2021 MicroEJ Corp. All rights reserved.
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

#define MBEDTLS_PLATFORM_C
#define MBEDTLS_PLATFORM_MEMORY

void* microej_calloc4tls(size_t nmemb, size_t size);
void microej_free4tls(void *ptr);

extern time_t custom_mbedtls_time(time_t *time);
#define MBEDTLS_PLATFORM_TIME_MACRO custom_mbedtls_time /**< Custom time (seconds) macro to use. */
#define MBEDTLS_HAVE_TIME
#define MBEDTLS_HAVE_TIME_DATE
#define MBEDTLS_PLATFORM_CALLOC_MACRO microej_calloc4tls
#define MBEDTLS_PLATFORM_FREE_MACRO microej_free4tls

#define mbedtls_calloc MBEDTLS_PLATFORM_CALLOC_MACRO
#define mbedtls_free MBEDTLS_PLATFORM_FREE_MACRO

#define MBEDTLS_ENTROPY_C
#define MBEDTLS_CTR_DRBG_C

#endif // !defined __MICROEJ_MBEDTLS_CONFIG_H__
