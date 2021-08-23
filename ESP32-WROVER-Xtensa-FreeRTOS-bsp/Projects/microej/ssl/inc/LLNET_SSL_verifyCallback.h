/*
 * C
 *
 * Copyright 2018-2021 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */

/**
 * @file
 * @brief LLNET_SSL_verifyCallback functions for mbedtls.
 * @author MicroEJ Developer Team
 * @version 2.1.4
 * @date 23 August 2021
 */

#ifndef LLNET_SSL_VERIFY_CALLBACK
#define LLNET_SSL_VERIFY_CALLBACK

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif
#include "mbedtls/x509.h"

#ifdef __cplusplus
	extern "C" {
#endif

/*
 * Verify Callback context
*/
typedef struct
{
	mbedtls_ssl_config* conf;
	uint8_t             isUnTrustCA;
}cert_verify_ctx;


/*
 * Certificate verification callback
*/
int LLNET_SSL_VERIFY_verifyCallback(void *data, mbedtls_x509_crt *crt, int depth, uint32_t *flags);


#ifdef __cplusplus
	}
#endif

#endif //LLNET_SSL_VERIFY_CALLBACK
