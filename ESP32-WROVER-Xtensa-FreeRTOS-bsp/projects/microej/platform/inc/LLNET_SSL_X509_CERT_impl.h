/*
 * C
 *
 * Copyright 2015-2017 IS2T. All rights reserved.
 * For demonstration purpose only.
 * IS2T PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */
#ifndef __LLNET_SSL_X509_CERT_IMPL__
#define __LLNET_SSL_X509_CERT_IMPL__

/**
 * @file
 * @brief MicroEJ SSL low level API
 * @author MicroEJ Developer Team
 * @version 4.0.0
 * @date 29 January 2019
 */

#include <sni.h>
#include <intern/LLNET_SSL_X509_CERT_impl.h>
#include <LLNET_SSL_CONSTANTS.h>
#include <LLNET_SSL_ERRORS.h>
#include <stdint.h>

#ifdef __cplusplus
	extern "C" {
#endif

/**
 * Parses the given certificate to validate it and returns its encoded format type.
 * @param cert the certificate buffer
 * @param off the offset in the buffer at which the certificate content started
 * @param len the certificate content length
 * @param retry true if the calling process repeats the call to this operation for its completion when the previous call
 * has returned {@link J_NATIVE_CODE_BLOCKED_WITHOUT_RESULT} to indicate that the operation was not completed.
 * @return the certificate encoded format type (0 if PEM format or 1 if DER format); {@link J_CERT_PARSE_ERROR} if the certificate is not valid or an error occurs.
 */
int32_t LLNET_SSL_X509_CERT_IMPL_parse(int8_t *cert, int32_t off, int32_t len, uint8_t retry);

#ifdef __cplusplus
	}
#endif

#endif //__LLNET_SSL_X509_CERT_IMPL__
