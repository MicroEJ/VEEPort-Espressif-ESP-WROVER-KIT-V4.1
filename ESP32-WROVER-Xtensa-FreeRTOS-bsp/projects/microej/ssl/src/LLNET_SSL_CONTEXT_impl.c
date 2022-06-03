/*
 * C
 *
 * Copyright 2018-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief LLNET_SSL_CONTEXT implementation over mbedtls.
 * @author MicroEJ Developer Team
 * @version 2.1.5
 * @date 20 December 2021
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif
#include "mbedtls/debug.h"
#include "mbedtls/net.h"
#include "mbedtls/ssl.h"
#include "mbedtls/ssl_internal.h"
#include "mbedtls/error.h"
#include "mbedtls/platform.h"
#if defined(MBEDTLS_ENTROPY_C) && defined(MBEDTLS_CTR_DRBG_C)
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#endif
#include "LLNET_SSL_utils_mbedtls.h"
#include "LLNET_SSL_verifyCallback.h"
#include "LLNET_SSL_CONTEXT_impl.h"
#include "LLNET_SSL_CONSTANTS.h"
#include "LLNET_SSL_ERRORS.h"
#include <stdlib.h>
#include <string.h>


#ifdef __cplusplus
	extern "C" {
#endif

/* ----------- external function and variables ----------- */
extern int32_t LLNET_SSL_TranslateReturnCode(int32_t mbedtls_error);
#if defined(MBEDTLS_ENTROPY_C) && defined(MBEDTLS_CTR_DRBG_C)
extern mbedtls_ctr_drbg_context ctr_drbg;
#endif

/* ----------- Definitions  -----------*/
#define PEM_END "-----END CERTIFICATE-----"

/* ----------- Private API  -----------*/
#if MBEDTLS_DEBUG_LEVEL > 0
/**
 * Debug callback for mbed TLS
 * Just prints on the USB serial port
 */
void microej_mbedtls_debug(void *ctx, int level, const char *file, int line,
					 const char *str)
{
	const char *p, *basename;
	(void) ctx;

	/* Extract basename from file */
	for(p = basename = file; *p != '\0'; p++) {
		if(*p == '/' || *p == '\\') {
			basename = p + 1;
		}
	}

	printf("%s:%04d: |%d| %s", basename, line, level, str);
}
#endif

/* ----------- API  -----------*/

int32_t LLNET_SSL_CONTEXT_IMPL_createContext(int32_t protocol, uint8_t isClientContext, uint8_t retry){
	LLNET_SSL_DEBUG_TRACE("%s(protocol=%d, isClientContext=%d, retry=%d)\n", __func__, (int)protocol, isClientContext, retry);

	mbedtls_ssl_config* conf = (mbedtls_ssl_config*)mbedtls_calloc(1, sizeof(mbedtls_ssl_config));
	void* p_rng = NULL;
	if (NULL != conf)
	{
		mbedtls_ssl_config_init(conf);

		int endpoint;
		if (isClientContext)
			endpoint = MBEDTLS_SSL_IS_CLIENT;
		else
			endpoint = MBEDTLS_SSL_IS_SERVER;

		int ret;
		if ((ret = mbedtls_ssl_config_defaults(conf,
						endpoint,
						MBEDTLS_SSL_TRANSPORT_STREAM,
						MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
			LLNET_SSL_DEBUG_MBEDTLS_TRACE("mbedtls_ssl_config_defaults", ret);
		}

		switch (protocol) {

			case SSLv3_PROTOCOL:
				mbedtls_ssl_conf_min_version(conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_0 );
				mbedtls_ssl_conf_max_version(conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_0 );
				break;
			case TLSv1_PROTOCOL:
				mbedtls_ssl_conf_min_version(conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_1 );
				mbedtls_ssl_conf_max_version(conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_1 );
				break;
			case TLSv1_1_PROTOCOL:
				mbedtls_ssl_conf_min_version(conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_2 );
				mbedtls_ssl_conf_max_version(conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_2 );
				break;
			case TLSv1_2_PROTOCOL:
				mbedtls_ssl_conf_min_version(conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3 );
				mbedtls_ssl_conf_max_version(conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3 );
				break;
			default:
				break;
		}

#if defined(MBEDTLS_ENTROPY_C) && defined(MBEDTLS_CTR_DRBG_C)
		p_rng = &ctr_drbg;
#endif
		mbedtls_ssl_conf_rng(conf, LLNET_SSL_utils_mbedtls_random, p_rng);

		/* It is possible to disable authentication by passing
		 * MBEDTLS_SSL_VERIFY_NONE in the call to mbedtls_ssl_conf_authmode()
		 */
		mbedtls_ssl_conf_authmode(conf, MBEDTLS_SSL_VERIFY_UNSET);

		/*
		 * Change certificate verification profile, allocate and initialize new profile
		 */
		mbedtls_x509_crt_profile* crt_profile = (mbedtls_x509_crt_profile*)mbedtls_calloc(1, sizeof(mbedtls_x509_crt_profile));
		if (NULL == crt_profile){
			mbedtls_ssl_config_free(conf);
			mbedtls_free(conf);
			return J_CREATE_SSL_CONTEXT_ERROR;
		}
		/* Hashes from MD5 and above */
		crt_profile->allowed_mds = MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_MD5 ) |
								   MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA1 ) |
								   MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_RIPEMD160 ) |
								   MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA224 ) |
								   MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA256 ) |
								   MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA384 ) |
								   MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA512 );
		/* Any PK alg    */
		crt_profile->allowed_pks = 0xFFFFFFF;
		/* Any curve     */
		crt_profile->allowed_curves = 0xFFFFFFF;
		/* RSA min lenght */
		crt_profile->rsa_min_bitlen = 1024;

		mbedtls_ssl_conf_cert_profile(conf, crt_profile);

		/* Allocate and initialize certificate verify context*/
		cert_verify_ctx* verify_ctx = (cert_verify_ctx*)mbedtls_calloc(1, sizeof(cert_verify_ctx));
		if (NULL == verify_ctx){
			mbedtls_ssl_conf_cert_profile(conf, NULL);
			mbedtls_free(crt_profile);
			mbedtls_ssl_config_free(conf);
			mbedtls_free(conf);
			return J_CREATE_SSL_CONTEXT_ERROR;
		}

        verify_ctx->conf = conf;
        verify_ctx->isUnTrustCA = 0;

		mbedtls_ssl_conf_verify(conf, LLNET_SSL_VERIFY_verifyCallback, (void*)verify_ctx);

#if MBEDTLS_DEBUG_LEVEL > 0
			mbedtls_ssl_conf_dbg(conf, microej_mbedtls_debug, NULL);
#if defined(MBEDTLS_DEBUG_C)
			mbedtls_debug_set_threshold(MBEDTLS_DEBUG_LEVEL);
#endif
#endif

		LLNET_SSL_DEBUG_TRACE("%s(method=%d) return ctx=%p\n", __func__, protocol,conf);

		return (int32_t)conf;
	}

	return J_CREATE_SSL_CONTEXT_ERROR; //error
}

int32_t LLNET_SSL_CONTEXT_IMPL_addTrustedCert(int32_t contextID, uint8_t *cert, int32_t off, int32_t len, int32_t format, uint8_t retry){

	LLNET_SSL_DEBUG_TRACE("%s(context=%d, retry=%d)\n", __func__, (int)contextID, retry);

	int32_t ret = J_SSL_NO_ERROR;

	/* Check parameters */
	if ((NULL == (uint8_t *)(cert + off)) || (0 == len) ||
		((CERT_DER_FORMAT != format) && (CERT_PEM_FORMAT != format)))
	{
		return J_BAD_FUNC_ARG;
	}

	mbedtls_ssl_config* conf = (mbedtls_ssl_config*)(contextID);
	mbedtls_x509_crt* cacert = NULL;
	if (NULL == conf->ca_chain)
	{
		cacert = (mbedtls_x509_crt*)mbedtls_calloc(1, sizeof(mbedtls_x509_crt));
		mbedtls_x509_crt_init(cacert);
	}
	else
	{
		cacert = conf->ca_chain;
	}

	if (NULL != conf && NULL != cacert)
	{
		int ret;

		if (CERT_DER_FORMAT == format) {
			ret = mbedtls_x509_crt_parse_der(cacert, (const unsigned char *) (cert + off), len);
            ret = LLNET_SSL_TranslateReturnCode(ret);
		} else {
			/* Find end of first certificate. */
			char* end = strstr((char*) cert, PEM_END);
			if (NULL == end) {
				ret = J_CREATE_SSL_CONTEXT_ERROR;
			} else {
				end += sizeof(PEM_END);
				int end_index = end - (char*) cert;
				ret = LLNET_SSL_utils_mbedtls_x509_crt_parse(cacert, (uint8_t *) cert, off, end_index);
			}
		}

		if(J_SSL_NO_ERROR != ret){
			mbedtls_free(cacert);
			return ret;
		}

		if (NULL == conf->ca_chain){
			mbedtls_ssl_conf_ca_chain(conf, cacert, NULL);
		}
	}
	else
	{
		ret = J_CREATE_SSL_CONTEXT_ERROR;
	}

    return ret;
}


int32_t LLNET_SSL_CONTEXT_IMPL_clearTrustStore(int32_t contextID, uint8_t retry){

	LLNET_SSL_DEBUG_TRACE("%s(context=%d, retry=%d)\n", __func__, (int)contextID, retry);
	mbedtls_ssl_config* conf = (mbedtls_ssl_config*)(contextID);

	if (NULL != conf)
	{
		if (NULL != conf->ca_chain)
		{
			void* chain_ptr = (void*)conf->ca_chain;
			mbedtls_ssl_conf_ca_chain(conf, NULL, NULL);
			mbedtls_x509_crt_free(chain_ptr);
			mbedtls_free(chain_ptr);
		}
	}

	return J_SSL_NO_ERROR;
}

int32_t LLNET_SSL_CONTEXT_IMPL_clearKeyStore(int32_t contextID, uint8_t retry){
	LLNET_SSL_DEBUG_TRACE("%s(context=%d, retry=%d)\n", __func__, (int)contextID, retry);
	mbedtls_ssl_config* conf = (mbedtls_ssl_config*)(contextID);

	/* Free the private key */
	if (NULL != conf)
	{
		if (NULL != conf->key_cert)
		{
			if (NULL != conf->key_cert->key)
			{
				mbedtls_pk_free(conf->key_cert->key);
				mbedtls_free(conf->key_cert->key);
			}
		}
	}

	return J_SSL_NO_ERROR;
}

int32_t LLNET_SSL_CONTEXT_IMPL_setCertificate(int32_t contextID, uint8_t* cert, int32_t offset, int32_t len, int32_t format, uint8_t retry){
	LLNET_SSL_DEBUG_TRACE("%s(context=%d, retry=%d)\n", __func__, (int)contextID, retry);
	mbedtls_ssl_config* conf = (mbedtls_ssl_config*)(contextID);
	mbedtls_x509_crt *clicert;
	int ret;

	/* Check parameters */
	if ((NULL == (uint8_t *)((int32_t)cert + offset)) || (0 == len) ||
		((CERT_DER_FORMAT != format) && (CERT_PEM_FORMAT != format))) {
		return J_BAD_FUNC_ARG;
	}

	/* Allocate a new keycert if needed, otherwise free the existing certificate */
	if (NULL == conf->key_cert) {
		conf->key_cert = mbedtls_calloc( 1, sizeof( mbedtls_ssl_key_cert ) );
	} else {
		mbedtls_x509_crt_free(conf->key_cert->cert);
		mbedtls_free(conf->key_cert->cert);
	}

	if (NULL == conf->key_cert){
		return J_MEMORY_ERROR;
	}

	/* The LLNET_SSL_* interface does not support multiple keycert (only one private key can be set) */
	conf->key_cert->next = NULL;

	/* Allocate a new certificate */
	clicert = mbedtls_calloc( 1, sizeof( mbedtls_x509_crt ) );
	if (NULL == clicert){
		return J_MEMORY_ERROR;
	}

	/* Try to parse the certificate */
	mbedtls_x509_crt_init(clicert);
	if (CERT_DER_FORMAT == format) {
		ret = mbedtls_x509_crt_parse_der(clicert, (uint8_t *) ((int32_t)cert + offset), (uint32_t) len);
		ret = LLNET_SSL_TranslateReturnCode(ret);
	} else {
		ret = LLNET_SSL_utils_mbedtls_x509_crt_parse(clicert, (uint8_t *) cert, offset, len);
	}

	/* Check parse result */
	if (J_SSL_NO_ERROR != ret) {
		/* Free the certificate */
		mbedtls_x509_crt_free(clicert);
		mbedtls_free(clicert);

		return J_CERT_PARSE_ERROR;
	}

	/* Link the certificate in the keycert */
	conf->key_cert->cert = clicert;

	return J_SSL_NO_ERROR;
}

int32_t LLNET_SSL_CONTEXT_IMPL_setPrivateKey(int32_t contextID, uint8_t* privateKey, int32_t offset, int32_t len, uint8_t* password,
		int32_t passwordOffset, int32_t passwordLen, uint8_t retry){
	LLNET_SSL_DEBUG_TRACE("%s(context=%d, retry=%d)\n", __func__, (int)contextID, retry);
	mbedtls_ssl_config* conf = (mbedtls_ssl_config*)(contextID);
	mbedtls_pk_context *key;
	uint8_t * pwd = NULL;

	/* Check parameters */
	if ((NULL == (uint8_t *)((int32_t)privateKey + offset)) || (0 == len)) {
		return J_BAD_FUNC_ARG;
	}

	/* Check if the password has to be used for decryption */
	if (passwordLen > 0) {
		pwd = (uint8_t *) ((int32_t)password + passwordOffset);
	}

	/* Allocate a new keycert if needed, otherwise free the existing private key */
	if (NULL == conf->key_cert) {
		conf->key_cert = mbedtls_calloc( 1, sizeof( mbedtls_ssl_key_cert ) );
	} else {
		mbedtls_pk_free(conf->key_cert->key);
		mbedtls_free(conf->key_cert->key);
	}

	if (NULL == conf->key_cert){
		return J_MEMORY_ERROR;
	}

	/* The LLNET_SSL_* interface does not support multiple keycert (only one private key can be set) */
	conf->key_cert->next = NULL;

	/* Allocate a new private key */
	key = mbedtls_calloc( 1, sizeof( mbedtls_pk_context ) );
	if (NULL == key){
		return J_MEMORY_ERROR;
	}

	/* Try to parse the private key */
	mbedtls_pk_init(key);
	int mbed_err = mbedtls_pk_parse_key(key,
			(uint8_t *) ((int32_t)privateKey + offset), (uint32_t) len,
			pwd, (uint32_t) passwordLen);
	if (0 != mbed_err) {

		print_mbedtls_error(__func__, mbed_err);
		/* Free the private key */
		mbedtls_pk_free(key);
		mbedtls_free(key);

		return J_NO_PRIVATE_KEY;
	}

	/* Link the private key in the keycert */
	conf->key_cert->key = key;

	return J_SSL_NO_ERROR;
}

int32_t LLNET_SSL_CONTEXT_IMPL_initChainBuffer(int32_t contextID, int32_t nbChainCerts, int32_t chainCertsTotalSize, uint8_t retry){
	LLNET_SSL_DEBUG_TRACE("%s(context=%d, nbChainCerts=%d, chainCertsTotalSize=%d, retry=%d)\n", __func__, (int)contextID, (int)nbChainCerts, (int)chainCertsTotalSize, retry);

	return J_SSL_NO_ERROR;
}

int32_t LLNET_SSL_CONTEXT_IMPL_addChainCertificate(int32_t contextID, uint8_t* cert, int32_t offset, int32_t len, int32_t format, int32_t chainBufferSize, uint8_t retry){
	LLNET_SSL_DEBUG_TRACE("%s(context=%d, retry=%d)\n", __func__, (int)contextID, retry);
	mbedtls_ssl_config* conf = (mbedtls_ssl_config*)(contextID);
	int ret;

	/* Check parameters */
	if ((NULL == (uint8_t *)((int32_t)cert + offset)) || (0 == len) ||
		((CERT_DER_FORMAT != format) && (CERT_PEM_FORMAT != format))) {
		return J_BAD_FUNC_ARG;
	}

	/* Try to parse the certificate, adding it to the chained list of certificated from keycert */
	if (CERT_DER_FORMAT == format) {
		ret = mbedtls_x509_crt_parse_der(conf->key_cert->cert, (uint8_t *) ((int32_t)cert + offset), (uint32_t) len);
		ret = LLNET_SSL_TranslateReturnCode(ret);
	} else {
		ret = LLNET_SSL_utils_mbedtls_x509_crt_parse(conf->key_cert->cert, (uint8_t *) cert, offset, len);
	}

	/* Check parse result */
	if (J_SSL_NO_ERROR != ret) {
		/* Free the certificate */
		mbedtls_x509_crt_free(conf->key_cert->cert);

		return J_CERT_PARSE_ERROR;
	}

	return J_SSL_NO_ERROR;
}

int32_t LLNET_SSL_CONTEXT_IMPL_closeContext(int32_t contextID, uint8_t retry){
	LLNET_SSL_DEBUG_TRACE("%s(context=%d, retry=%d)\n", __func__, (int)contextID, retry);
	mbedtls_ssl_config* conf = (mbedtls_ssl_config*)(contextID);

	if (NULL != conf)
	{
		if (NULL != conf->ca_chain)
		{
			void* chain_ptr = (void*)conf->ca_chain;
			mbedtls_ssl_conf_ca_chain(conf, NULL, NULL);
			mbedtls_x509_crt_free(chain_ptr);
			mbedtls_free(chain_ptr);
		}

		if (NULL != conf->p_vrfy)
		{
			void* vrfy_ptr = (void*)conf->p_vrfy;
			mbedtls_ssl_conf_verify(conf, NULL, NULL);
			mbedtls_free(vrfy_ptr);
		}

		if (NULL != conf->cert_profile)
		{
			void* profile_ptr = (void*)conf->cert_profile;
			mbedtls_ssl_conf_cert_profile(conf, NULL);
			mbedtls_free(profile_ptr);
		}

		if (NULL != conf->key_cert)
		{
			if (NULL != conf->key_cert->key)
			{
				mbedtls_pk_free(conf->key_cert->key);
				mbedtls_free(conf->key_cert->key);
			}

			if (NULL != conf->key_cert->cert)
			{
				mbedtls_x509_crt_free(conf->key_cert->cert);
				mbedtls_free(conf->key_cert->cert);
			}
		}

		mbedtls_ssl_config_free(conf);
		mbedtls_free((void*)conf);
	}

	return J_SSL_NO_ERROR;
}
#ifdef __cplusplus
	}
#endif
