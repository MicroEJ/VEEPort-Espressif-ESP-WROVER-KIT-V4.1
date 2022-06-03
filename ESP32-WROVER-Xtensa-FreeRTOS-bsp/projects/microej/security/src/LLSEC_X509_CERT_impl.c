/*
 * C
 *
 * Copyright 2021-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief MicroEJ Security low level API implementation for MbedTLS Library.
 * @author MicroEJ Developer Team
 * @version 0.10.0
 */

#include <LLSEC_X509_CERT_impl.h>
#include <sni.h>
#include <stdlib.h>
#include <string.h>

#include "LLSEC_mbedtls.h"
#include "mbedtls/platform.h"
#include "mbedtls/ssl.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LLSEC_X509_SUCCESS 0
#define LLSEC_X509_ERROR   -1

#define LLSEC_X509_DER_FORMAT     (int)(1)
#define LLSEC_X509_PEM_FORMAT     (int)(0)
#define LLSEC_X509_UNKNOWN_FORMAT (int)(-1)

//#define LLSEC_X509_DEBUG

#ifdef LLSEC_X509_DEBUG
#define LLSEC_X509_DEBUG_TRACE(...) printf(__VA_ARGS__)
#else
#define LLSEC_X509_DEBUG_TRACE(...) ((void)0)
#endif

static mbedtls_x509_crt* get_x509_certificate(int8_t* cert_data, int32_t len, int* cert_format);
static int32_t LLSEC_X509_CERT_mbedtls_close_key(int32_t native_id);

static mbedtls_x509_crt* get_x509_certificate(int8_t* cert_data, int32_t len, int* cert_format)
{
    LLSEC_X509_DEBUG_TRACE("%s 00. cert_len:%d\n", __func__, len);

    mbedtls_x509_crt* tmp_cert = NULL;
    int ret;

    /* Allocate a new X509 certificate */
    tmp_cert = (mbedtls_x509_crt*)mbedtls_calloc(1, sizeof(mbedtls_x509_crt));
    if (tmp_cert == NULL) {
        if (cert_format != NULL) {
            *cert_format = J_MEMORY_ERROR;
        }
        return NULL;
    }

    /* Initialize the X509 certificate */
    mbedtls_x509_crt_init(tmp_cert);

    /* Parse the X509 DER certificate */
    ret = mbedtls_x509_crt_parse_der(tmp_cert, (const uint8_t*)cert_data, len);
    if (ret == 0) {
        /* Encoded DER certificate */
        if (cert_format != NULL) {
            *cert_format = CERT_DER_FORMAT;
        }
        return tmp_cert;
    } else {
        LLSEC_X509_DEBUG_TRACE("%s. mbedtls_x509_crt_parse_der fail, ret: %d\n", __func__, ret);
    }

    /*Parse the X509 PEM certificate*/
    mbedtls_x509_crt_init(tmp_cert);

    /*To avoid tmp_cert_data is not a string, which causes mbedtls_x509_crt_parse error*/
    int8_t* tmp_cert_data = (int8_t*)mbedtls_calloc(1, len + 1);
    memcpy(tmp_cert_data, cert_data, len);
    tmp_cert_data[len] = '\0';

    /* Parse the X509 PEM certificate */
    ret = mbedtls_x509_crt_parse(tmp_cert, (const uint8_t*)tmp_cert_data, len + 1);
    mbedtls_free(tmp_cert_data);

    if (ret == 0) {
        /* Encoded PEM certificate */
        if (cert_format != NULL) {
            *cert_format = CERT_PEM_FORMAT;
        }
        return tmp_cert;
    } else {
        LLSEC_X509_DEBUG_TRACE("%s. mbedtls_x509_crt_parse(PEM) fail, ret: %d\n", __func__, ret);
    }

    /*certificate is not PEM/DER*/
    if (cert_format != NULL) {
        *cert_format = J_CERT_PARSE_ERROR;
    }
    return NULL;
}

static int32_t LLSEC_X509_CERT_mbedtls_close_key(int32_t native_id)
{
    LLSEC_X509_DEBUG_TRACE("%s \n", __func__);
    LLSEC_pub_key* key = (LLSEC_pub_key*)native_id;

    if (key->type == TYPE_RSA) {
        mbedtls_rsa_free((mbedtls_rsa_context*)key->key);
    } else {
        mbedtls_ecdsa_free((mbedtls_ecdsa_context*)key->key);
    }

    mbedtls_free(key);
    return 1;
}

int32_t LLSEC_X509_CERT_IMPL_parse(int8_t* cert, int32_t off, int32_t len)
{
    LLSEC_X509_DEBUG_TRACE("%s(cert=%d, off=%d, len=%d)\n", __func__, (int)cert, (int)off, (int)len);

    int32_t format = LLSEC_X509_UNKNOWN_FORMAT;
    int8_t* cert_data = cert + off;
    mbedtls_x509_crt* tmp_cert = get_x509_certificate(cert_data, len, &format);

    /* Free the X509 certificate */
    if (tmp_cert != NULL) {
        mbedtls_x509_crt_free(tmp_cert);
        mbedtls_free((void*)tmp_cert);
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
int32_t LLSEC_X509_CERT_IMPL_get_x500_principal_data(int8_t* cert_data, int32_t cert_data_length, uint8_t* principal_data, int32_t principal_data_length, uint8_t get_issuer)
{
    LLSEC_X509_DEBUG_TRACE("%s(cert=%d, Cert_len=%d,prin_len=%d,get_issuer=%d)\n", __func__, (int)cert_data, (int)cert_data_length, (int)principal_data_length, (int)get_issuer);

    mbedtls_x509_crt* x509 = get_x509_certificate(cert_data, cert_data_length, NULL);
    if (x509 == NULL) {
        SNI_throwNativeException(-1, "Bad x509 certificate");
        return LLSEC_X509_ERROR;
    }

    int len;
    char buf[256];
    if (get_issuer) {
        len = mbedtls_x509_dn_gets(buf, sizeof(buf), &(x509->issuer));
    } else {
        len = mbedtls_x509_dn_gets(buf, sizeof(buf), &(x509->subject));
    }

    if (len > principal_data_length) {
        SNI_throwNativeException(-1, "Principal data buffer is too small");
        return LLSEC_X509_ERROR;
    } else {
        memcpy(principal_data, &buf[0], len);

        /* Free the X509 certificate */
        mbedtls_x509_crt_free(x509);

        return len;
    }
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
int32_t LLSEC_X509_CERT_IMPL_get_key(int8_t* cert_data, int32_t cert_data_length)
{
    LLSEC_X509_DEBUG_TRACE("%s(cert=%d, len=%d)\n", __func__, (int)cert_data, (int)cert_data_length);
    LLSEC_pub_key* pub_key = (LLSEC_pub_key*)mbedtls_calloc(1, sizeof(LLSEC_pub_key));
    if (pub_key == NULL) {
        SNI_throwNativeException(-1, "Can't allocate LLSEC_pub_key structure");
        return LLSEC_X509_ERROR;
    }

    mbedtls_x509_crt* x509 = get_x509_certificate(cert_data, cert_data_length, NULL);

    if (x509 == NULL) {
        mbedtls_free(pub_key);
        SNI_throwNativeException(-1, "Bad x509 certificate");

        return LLSEC_X509_ERROR;
    }

    //Note:key TYPE: mbedtls_rsa_context or mbedtls_ecdsa_context
    if (mbedtls_pk_get_type(&x509->pk) == MBEDTLS_PK_RSA) {
        pub_key->type = TYPE_RSA;
        pub_key->key = (char*)mbedtls_pk_rsa(x509->pk);
    } else {
        pub_key->type = TYPE_ECDSA;
        pub_key->key = (char*)mbedtls_pk_ec(x509->pk);
    }

    if (pub_key->key == NULL) {
        mbedtls_x509_crt_free(x509);
        mbedtls_free(pub_key);

        SNI_throwNativeException(-1, "Invalid public key from x509 certificate");
        return LLSEC_X509_ERROR;
    }

    int32_t native_id = (int32_t)pub_key;
    if (SNI_registerResource((void*)native_id, (SNI_closeFunction)LLSEC_X509_CERT_mbedtls_close_key, NULL) != SNI_OK) {
        SNI_throwNativeException(-1, "Can't register SNI native resource");

        if (pub_key->type == TYPE_RSA) {
            mbedtls_rsa_free((mbedtls_rsa_context*)pub_key->key);
        } else {
            mbedtls_ecdsa_free((mbedtls_ecdsa_context*)pub_key->key);
        }

        mbedtls_x509_crt_free(x509);
        mbedtls_free(pub_key);
        return LLSEC_X509_ERROR;
    }

    return native_id;
}

int32_t LLSEC_X509_CERT_IMPL_verify(int8_t* cert_data, int32_t cert_data_length, int32_t public_key_id)
{
    LLSEC_X509_DEBUG_TRACE("%s \n", __func__);
    int rc = LLSEC_X509_ERROR;
    uint32_t flags;

    mbedtls_x509_crt* x509 = get_x509_certificate(cert_data, cert_data_length, NULL);
    if (x509 == NULL) {
        SNI_throwNativeException(-1, "Bad x509 certificate");
        return LLSEC_X509_ERROR;
    }

    rc = mbedtls_x509_crt_verify(x509, NULL, NULL, NULL, &flags, NULL, NULL);
    if (rc != LLSEC_X509_SUCCESS) {
        // Error
        mbedtls_x509_crt_free(x509);
        LLSEC_X509_DEBUG_TRACE("LLSEC_X509 > verify error");
        SNI_throwNativeException(-1, "Error x509 verify failed");
        return LLSEC_X509_ERROR;
    }

    mbedtls_x509_crt_free(x509);
    return LLSEC_X509_SUCCESS;
}

/**
 * @brief Get the pointer for the close key  method to be used as a close resource callback with SNI.
 *
 * @return the pointer for the close method.
 *
 * @note Throws NativeException on error.
 */
int32_t LLSEC_X509_CERT_IMPL_get_close_key()
{
    LLSEC_X509_DEBUG_TRACE("%s \n", __func__);
    return (int32_t)LLSEC_X509_CERT_mbedtls_close_key;
}
