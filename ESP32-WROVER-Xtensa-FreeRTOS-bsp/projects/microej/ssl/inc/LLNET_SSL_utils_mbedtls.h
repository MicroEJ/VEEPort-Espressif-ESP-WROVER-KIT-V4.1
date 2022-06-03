/*
 * C
 *
 * Copyright 2018-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief LLNET_SSL_utils_mbedtls functions for mbedtls.
 * @author MicroEJ Developer Team
 * @version 2.1.5
 * @date 20 December 2021
 */

#ifndef LLNET_SSL_UTILS_MBEDTLS
#define LLNET_SSL_UTILS_MBEDTLS

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif
#include "mbedtls/x509_crt.h"
#include "LLNET_SSL_CONSTANTS.h"
#include <time.h>
#include <stdio.h>

#ifdef __cplusplus
 extern "C" {
#endif

#if !defined(MBEDTLS_ENTROPY_C) || !defined(MBEDTLS_CTR_DRBG_C)
#error "Please set your custom random number generator function in the next #define by replacing my_custom_random_func with the appropriate one. Remove this #error when done."
/*
 * Custom random number generator function.
 * Used when the platform does not support entropy pool and CTR_DRBG AES-256 random number generator.
 * Returns 0 on success, negative value on error.
 */
#define microej_custom_random_func my_custom_random_func
#endif

#ifdef LLNET_SSL_DEBUG
/* ---- Define the mbedtls debug level ---- */
#define MBEDTLS_DEBUG_LEVEL 2
 /* ---- Define the mbedtls debug printer ---- */
#define LLNET_SSL_DEBUG_MBEDTLS_TRACE 	   print_mbedtls_error
#else
#define LLNET_SSL_DEBUG_MBEDTLS_TRACE(...) ((void) 0)
#endif

/* ---- Specific C Time function ---- */
/*
 * time function to get current JVM application time.
 * @param time pointer to time_t struct that will be updated with the current time in seconds
 * @return the current time in seconds
 */
time_t custom_mbedtls_time(time_t *time);

/* ---- Specific net layer connection functions ---- */

/*
 * Net receive layer adaptation function for mbedtls .
 * @param ctx Net context (pointer to the socket fd)
 * @param buf buffer for receiving data
 * @param len length to receive
 * @return -1 on error, number of received bytes otherwise
 */
int LLNET_SSL_utils_mbedtls_recv(void *ctx, unsigned char *buf, size_t len);

/*
 * Net send layer adaptation function for mbedtls .
 * @param ctx Net context (pointer to the socket fd)
 * @param buf buffer of data to send
 * @param len length to send
 * @return -1 on error, number of sent bytes otherwise
 */
int LLNET_SSL_utils_mbedtls_send(void *ctx, const unsigned char *buf, size_t len);

/* ---- mbedtls custom function for error printing ---- */
void print_mbedtls_error(const char *name, int err);

/* ---- mbedtls Random Number Generator callback function. ---- */

/*
 * Random Number Generator (RNG) callback function.
 * This function generates a random data.
 * If entropy pool and CTR_DRBG AES-256 random number generator are not supported,
 * this function uses the custom function <code>microej_custom_random_func</code> for random number generation;
 * and this custom function need to be defined in <code>LLNET_SSL_utils_mbedtls.h</code>.
 * @param p_rng RNG parameter
 * @param output output buffer to fill.
 * @param output_len output buffer length.
 * @return 0 on success, negative value on error.
 */
int LLNET_SSL_utils_mbedtls_random(void *p_rng, unsigned char *output, size_t output_len);

/* ---- Parsing certificate helper ---- */

/**
 * Parse PEM-encoded Certificate.
 * PEM certificate content should be null-terminated otherwise mbedtls_x509_crt_parse() method call will fail.
 * This method wrap mbedtls_x509_crt_parse() by adding a terminating null byte ('\0') to the certificate data if there is none before calling mbedtls_x509_crt_parse().
 * @param cert the chain to which to add the parsed certificate.
 * @param array the buffer holding the certificate data.
 * @param offset the offset in the buffer at which the certificate data started.
 * @param len the certificate data length.
 *
 */
int LLNET_SSL_utils_mbedtls_x509_crt_parse(mbedtls_x509_crt *cert, uint8_t * array, uint32_t offset, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif //LLNET_SSL_UTILS_MBEDTLS
