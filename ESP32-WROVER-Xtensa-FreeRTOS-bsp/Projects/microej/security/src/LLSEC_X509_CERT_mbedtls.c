/*
 * C
 *
 * Copyright 2017-2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */

#include <LLSEC_X509_CERT_impl.h>
#include <LLSEC_ERRORS.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sni.h>

#include "mbedtls/ssl.h"
#include "ssl_parse.h"

/**
 * @file
 * @brief MicroEJ Security low level API implementation for mbed TLS library
 * @author @CCO_AUTHOR@
 * @version @CCO_VERSION@
 * @date @CCO_DATE@
 */


/**
 * Parses the given certificate to validate it and returns its encoded format type.
 * @param cert the certificate buffer
 * @param off the offset in the buffer at which the certificate content started
 * @param len the certificate content length
 * @return the certificate encoded format type (0 if PEM format or 1 if DER format); {@link J_CERT_PARSE_ERROR} if the certificate is not valid or an error occurs.
 */
int32_t LLSEC_X509_CERT_IMPL_parse(int8_t* cert, int32_t off, int32_t len){
	int32_t format = J_CERT_PARSE_ERROR;
	mbedtls_x509_crt * tmp_cert = NULL;

//	printf("[INFO] %s cert=0x%x, off=%d, len=%d\n", __func__, (unsigned int)cert, off, len);

	do {
		int ret;

		/* Check parameters */
		if (((uint8_t *)(cert + off) == NULL) || (len == 0)) {
			format = J_BAD_FUNC_ARG;
			break;
		}

		/* Allocate a new X509 certificate */
		tmp_cert = (mbedtls_x509_crt *) microej_malloc(sizeof(*tmp_cert));
		if (tmp_cert == NULL) {
			format = J_MEMORY_ERROR;
			break;
		}

		/* Initialize the X509 certificate */
		mbedtls_x509_crt_init(tmp_cert);

		/* Parse the X509 DER certificate */
		ret = mbedtls_x509_crt_parse_der(tmp_cert, (const uint8_t *) (cert + off), len);
		if (ret == 0) {
			/* Encoded DER certificate */
			format = CERT_DER_FORMAT;
			break;
		}

		/* Free the X509 certificate */
		mbedtls_x509_crt_free(tmp_cert);

		/* Initialize the X509 certificate */
		mbedtls_x509_crt_init(tmp_cert);

		/* Parse the X509 PEM certificate */
		ret = microej_x509_crt_parse(tmp_cert, (uint8_t*)cert, off, len);
		if (ret == 0) {
			/* Encoded PEM certificate */
			format = CERT_PEM_FORMAT;
			break;
		}

	} while (0);

//	printf("%s : return format=%d\n", __func__, format);

	/* Free the X509 certificate */
	if (tmp_cert != NULL) {
		mbedtls_x509_crt_free(tmp_cert);
		microej_free((void *) tmp_cert);
	}

	return format;
}

/**
 *
 * @param cert
 * @param certLen
 * @param keyData
 * @param keyDataLength inparameter. Contains the length of keyData.
 * @return the number of bytes copied into keyData
 *
 ** Warning: cert_data must not be used outside of the VM task or saved
 ** Warning: key must not be used outside of the VM task or saved
 *
 * @throws NativeException on error.
 */
int32_t LLSEC_X509_CERT_IMPL_get_x500_principal_data(int8_t* cert_data, int32_t cert_data_length, uint8_t* principal_data, int32_t principal_data_length, uint8_t get_issuer){
	// TODO not implemented yet
	SNI_throwNativeException(J_UNKNOWN_ERROR, "Native not implemented yet");
	return -1;
}


/**
 *
 * @param cert
 * @param certLen
 * @param keyData
 * @param keyDataLength inparameter. Contains the length of keyData.
 * @return the number of bytes copied into keyData
 *
 ** Warning: cert_data must not be used outside of the VM task or saved
 ** Warning: key must not be used outside of the VM task or saved
 *
 * @throws NativeException on error.
 */
int32_t LLSEC_X509_CERT_IMPL_get_key(int8_t* cert_data, int32_t cert_data_length, uint8_t* key, int32_t key_length)
{
	// TODO not implemented yet
	SNI_throwNativeException(J_UNKNOWN_ERROR, "Native not implemented yet");
	return -1;
}

int32_t LLSEC_X509_CERT_IMPL_verify(int8_t* cert_data, int32_t cert_data_length, uint8_t* key, int32_t key_length)
{
	// TODO not implemented yet
	SNI_throwNativeException(J_UNKNOWN_ERROR, "Native not implemented yet");
	return -1;
}

