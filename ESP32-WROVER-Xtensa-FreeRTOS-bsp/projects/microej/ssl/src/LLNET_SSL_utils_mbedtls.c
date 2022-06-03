/*
 * C
 *
 * Copyright 2018-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief LLNET_SSL_utils_mbedtls implementation over mbedtls.
 * @author MicroEJ Developer Team
 * @version 2.1.5
 * @date 20 December 2021
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif
#include "mbedtls/error.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/x509_crt.h"
#if defined(MBEDTLS_CTR_DRBG_C)
#include "mbedtls/ctr_drbg.h"
#endif
#include "LLNET_Common.h"
#include "LLNET_SSL_utils_mbedtls.h"
#include "LLNET_SSL_ERRORS.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
 extern "C" {
#endif

 /* ---- constants ---- */

 /**
  * @brief null terminated character
  */
#define NULL_CHARACTER '\0'

/* ---- external functions ---- */
/**
 * @brief External function used to retrieve currentTime (defined in LLMJVM)
 */
extern int64_t LLMJVM_IMPL_getCurrentTime__Z(uint8_t system);

/**
 * @brief External function used to map mbedtls error code to MicroEJ SSL error code
 */
extern int32_t LLNET_SSL_TranslateReturnCode(int32_t mbedtls_error);

/* ---- private functions ---- */
static uint8_t * microej_get_str_from_array(uint8_t * array, uint32_t offset, uint32_t * len) {
	uint8_t * p_str;

	p_str = (uint8_t *) array + offset;

	/* Check if array is null terminated */
	if (NULL_CHARACTER != p_str[*len - 1]) {
		int32_t trueLength = SNI_getArrayLength(array) - offset;

		/* len should be one byte longer to store the null terminated key */
		(*len)++;

		/* Check if there is enough space in the byte array to insert a null character */
		if (trueLength > *len) {
			p_str[*len - 1] = NULL_CHARACTER;
		} else {
			/*
			 * Duplicate the array but with a null terminated character,
			 * /!\ this new string must be freed
			 */

			/* Try first internal memory because it is faster */
			p_str = malloc(*len);

			if (NULL == p_str) {
				return NULL;
			}

			memcpy((void *)p_str, (void *)(array + offset), *len - 1);
			p_str[*len - 1] = NULL_CHARACTER;

		}
	}

	return (uint8_t *) p_str;
}

/* ---- Specific C Time function ---- */

time_t custom_mbedtls_time(time_t *time)
{
	time_t currentTime = (time_t)(LLMJVM_IMPL_getCurrentTime__Z(0) / 1000);

	if (NULL != time)
	{
		*time = currentTime;
	}

	return currentTime;
}

/* ---- Specific net layer connection functions ---- */

/**
 * Receive callback for mbed TLS
 */
int LLNET_SSL_utils_mbedtls_recv(void *ctx, unsigned char *buf, size_t len) {
	LLNET_SSL_DEBUG_TRACE("%s(ctx=%d, length=%d)\n", __func__, (int)ctx, (int)len);
    int recv_bytes = -1;

    int fd = *((int*)ctx);

    if( fd < 0 )
        return( MBEDTLS_ERR_NET_INVALID_CONTEXT );

    int32_t selectRes = non_blocking_select( fd, SELECT_READ );

    if( selectRes == 0 ) {
    	LLNET_SSL_DEBUG_TRACE("%s RECV WANT READ(ctx=%d, length=%d)\n", __func__, (int)ctx, (int)len);
    	return( MBEDTLS_ERR_SSL_WANT_READ );
    } else {
    	recv_bytes = llnet_recv( fd, buf, len, 0 );
    	int netError = errno;

    	LLNET_SSL_DEBUG_TRACE("%s RECV (ctx=%d, length=%d) bytes read=%d, netError=%d\n", __func__, (int)ctx, (int)len, (int)recv_bytes, (int)netError);

		if( recv_bytes < 0 )
		{
			if( netError == EPIPE || netError == ECONNRESET )
				return( MBEDTLS_ERR_NET_CONN_RESET );

			if( netError == EINTR )
				return( MBEDTLS_ERR_SSL_WANT_READ );

			return( MBEDTLS_ERR_NET_RECV_FAILED );
		}
    }

    return recv_bytes;
}

/**
 * Send callback for mbed TLS
 */
int LLNET_SSL_utils_mbedtls_send(void *ctx, const unsigned char *buf, size_t len) {
	LLNET_SSL_DEBUG_TRACE("%s(ctx=%d, length=%d)\n", __func__, (int)ctx, (int)len);
    int size_sent = -1;

    int fd = *((int*)ctx);
    if( fd < 0 )
        return( MBEDTLS_ERR_NET_INVALID_CONTEXT );

    int32_t selectRes = non_blocking_select( fd, SELECT_WRITE );

    if( selectRes == 0 ) {
    	LLNET_SSL_DEBUG_TRACE("%s SEND WANT WRITE(ctx=%d, length=%d)\n", __func__, (int)ctx, (int)len);
    	return( MBEDTLS_ERR_SSL_WANT_WRITE );
    } else {
    	size_sent = llnet_send( fd, buf, len, 0 );
    	int netError = errno;

    	LLNET_SSL_DEBUG_TRACE("%s SEND (ctx=%d, length=%d) bytes read=%d netError=%d\n", __func__, (int)ctx, (int)len, (int)size_sent, (int)netError);

		if( size_sent < 0 )
		{
			if( netError == EPIPE || netError == ECONNRESET )
				return( MBEDTLS_ERR_NET_CONN_RESET );

			if( netError == EINTR )
				return( MBEDTLS_ERR_SSL_WANT_WRITE );

			return( MBEDTLS_ERR_NET_SEND_FAILED );
		}
    }

	return size_sent;
}

/* ---- mbedtls custom function for error printing ---- */

/**
 * Helper for pretty-printing mbed TLS error codes
 */
 void print_mbedtls_error(const char *name, int err) {
    char buf[128];
    mbedtls_strerror(err, buf, sizeof (buf));
    printf("%s() failed: -0x%04x (%d): %s\r\n", name, -err, err, buf);
}

/* ---- random number generator callback function. ---- */

/*
 * Random Number Generator (RNG) callback function.
 * This function generates a random data.
 * If entropy pool and CTR_DRBG AES-256 random number generator are not supported,
 * this function uses the custom function <code>microej_custom_random_func</code> for random number generation;
 * and this custom function need to be defined in <code>LLNET_SSL_utils_mbedtls.h</code>.
 * @param p_rng : RNG parameter
 * @param output : output buffer to fill.
 * @param output_len : output buffer length.
 * @return 0 on success, negative value on error.
 */
int LLNET_SSL_utils_mbedtls_random(void *p_rng, unsigned char *output, size_t output_len)
{
#if defined(MBEDTLS_ENTROPY_C) && defined(MBEDTLS_CTR_DRBG_C)
 	return mbedtls_ctr_drbg_random(p_rng, output, output_len);
#else
 	(void) p_rng;
 	return microej_custom_random_func(output, output_len);
#endif
}

/* ---- Parsing certificate helper ---- */

/**
 * Parse PEM-encoded Certificate.
 * PEM certificate content should be null-terminated otherwise mbedtls_x509_crt_parse() method call will fail.
 * This method wrap mbedtls_x509_crt_parse() by adding a terminating null byte ('\0') to the certificate data if there is none before calling mbedtls_x509_crt_parse().
 * @param cert : the chain to which to add the parsed certificate.
 * @param array : the buffer holding the certificate data.
 * @param offset : the offset in the buffer at which the certificate data started.
 * @param len : the certificate data length.
 *
 */
int LLNET_SSL_utils_mbedtls_x509_crt_parse(mbedtls_x509_crt *cert, uint8_t * array, uint32_t offset, uint32_t len) {
	LLNET_SSL_DEBUG_TRACE("%s()\n", __func__);
	uint8_t * strCert = microej_get_str_from_array(array, offset, &len);

	if (NULL == strCert) {
		return J_MEMORY_ERROR;
	}

	int ret = mbedtls_x509_crt_parse(cert, strCert, (uint32_t) len);

	// Free strCert if it has been allocated by microej_get_str_from_array
	if (strCert != (array + offset)) {
		free(strCert);
	}

	return LLNET_SSL_TranslateReturnCode(ret);
}

#ifdef __cplusplus
}
#endif
