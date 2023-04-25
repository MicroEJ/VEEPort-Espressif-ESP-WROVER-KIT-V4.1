/*
 * C
 *
 * Copyright 2021-2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief MicroEJ Security low level API implementation for MbedTLS Library.
 * @author MicroEJ Developer Team
 * @version 1.1.0
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

#define LLSEC_X509_UNKNOWN_FORMAT (int)(-1)

//#define LLSEC_X509_DEBUG

#ifdef LLSEC_X509_DEBUG
// cppcheck-suppress misra-c2012-21.6 // Include only in debug
#include <stdio.h>
#define LLSEC_X509_DEBUG_TRACE(...) (void)printf(__VA_ARGS__)
#else
#define LLSEC_X509_DEBUG_TRACE(...) ((void)0)
#endif

static mbedtls_x509_crt* get_x509_certificate(int8_t* cert_data, int32_t len, int* cert_format);
static int32_t LLSEC_X509_CERT_mbedtls_close_key(int32_t native_id);

static mbedtls_x509_crt* get_x509_certificate(int8_t* cert_data, int32_t len, int* cert_format)
{
    LLSEC_X509_DEBUG_TRACE("%s 00. cert_len:%d\n", __func__, len);

    mbedtls_x509_crt* tmp_cert = NULL;
    int ret = 0;

    /* Allocate a new X509 certificate */
    tmp_cert = (mbedtls_x509_crt*)mbedtls_calloc(1, sizeof(mbedtls_x509_crt));
    if (tmp_cert == NULL) {
        if (cert_format != NULL) {
            *cert_format = J_MEMORY_ERROR;
        }
    }

    if (tmp_cert != NULL){
        /* Initialize the X509 certificate */
        mbedtls_x509_crt_init(tmp_cert);

        /* Parse the X509 DER certificate */
        ret = mbedtls_x509_crt_parse_der(tmp_cert, (const uint8_t*)cert_data, len);
        if (ret == 0) {
            /* Encoded DER certificate */
            if (cert_format != NULL) {
                *cert_format = CERT_DER_FORMAT;
            }
        } else {
            LLSEC_X509_DEBUG_TRACE("%s. mbedtls_x509_crt_parse_der fail, ret: %d\n", __func__, ret);
        }
    }

    if ((tmp_cert != NULL)){
        /*Parse the X509 PEM certificate*/
        mbedtls_x509_crt_init(tmp_cert);

        /*To avoid tmp_cert_data is not a string, which causes mbedtls_x509_crt_parse error*/
        int8_t* tmp_cert_data = (int8_t*)mbedtls_calloc(1, len + 1);
        (void) memcpy(tmp_cert_data, cert_data, len);
        tmp_cert_data[len] = '\0';

        /* Parse the X509 PEM certificate */
        ret = mbedtls_x509_crt_parse(tmp_cert, (const uint8_t*)tmp_cert_data, len + 1);
        mbedtls_free(tmp_cert_data);

        if (ret == 0) {
            /* Encoded PEM certificate */
            if (cert_format != NULL) {
                *cert_format = CERT_PEM_FORMAT;
            }
        } else {
            LLSEC_X509_DEBUG_TRACE("%s. mbedtls_x509_crt_parse(PEM) fail, ret: %d\n", __func__, ret);
        }
    }

    if ((tmp_cert != NULL) && (ret != 0)){
        /*certificate is not PEM/DER*/
        if (cert_format != NULL) {
            *cert_format = J_CERT_PARSE_ERROR;
        }
    }
    return tmp_cert;
}


static int32_t LLSEC_X509_CERT_mbedtls_close_key(int32_t native_id)
{
    LLSEC_X509_DEBUG_TRACE("%s \n", __func__);
    // cppcheck-suppress misra-c2012-11.4 // Abstract data type for SNI usage
    LLSEC_pub_key* key = (LLSEC_pub_key*)native_id;

    if (key->type == TYPE_RSA) {
        mbedtls_rsa_free((mbedtls_rsa_context*)key->key);
    } else {
        mbedtls_ecdsa_free((mbedtls_ecdsa_context*)key->key);
    }

    mbedtls_free(key);
    return LLSEC_X509_SUCCESS;
}

int32_t LLSEC_X509_CERT_IMPL_parse(int8_t* cert, int32_t off, int32_t len)
{
    LLSEC_X509_DEBUG_TRACE("%s(cert=%p, off=%d, len=%d)\n", __func__, cert, (int)off, (int)len);

    int32_t format = LLSEC_X509_UNKNOWN_FORMAT;
    int8_t* cert_data = &cert[off];
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
    int32_t return_code = LLSEC_X509_SUCCESS;
    LLSEC_X509_DEBUG_TRACE("%s(cert=%p, Cert_len=%d,prin_len=%d,get_issuer=%d)\n", __func__, cert_data, (int)cert_data_length, (int)principal_data_length, (int)get_issuer);

    mbedtls_x509_crt* x509 = get_x509_certificate(cert_data, cert_data_length, NULL);
    if (x509 == NULL) {
        SNI_throwNativeException(LLSEC_X509_ERROR, "Bad x509 certificate");
        return_code = LLSEC_X509_ERROR;
    }

    if( return_code == LLSEC_X509_SUCCESS) {
        int len;
        char buf[256];
        if (get_issuer!= (uint8_t) 0) {
            len = mbedtls_x509_dn_gets(buf, sizeof(buf), &(x509->issuer));
        } else {
            len = mbedtls_x509_dn_gets(buf, sizeof(buf), &(x509->subject));
        }

        if (len > principal_data_length) {
            SNI_throwNativeException(LLSEC_X509_ERROR, "Principal data buffer is too small");
            return_code = LLSEC_X509_ERROR;
        } else {
            (void) memcpy(principal_data, &buf[0], len);

            /* Free the X509 certificate */
            mbedtls_x509_crt_free(x509);

            return_code = len;
        }
    }

    return return_code;
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
    int32_t return_code = LLSEC_X509_SUCCESS;
    LLSEC_X509_DEBUG_TRACE("%s(cert=%p, len=%d)\n", __func__, cert_data, (int)cert_data_length);
    LLSEC_pub_key* pub_key = (LLSEC_pub_key*)mbedtls_calloc(1, sizeof(LLSEC_pub_key));
    mbedtls_x509_crt* x509 = NULL;
    void* native_id = NULL;

    if (pub_key == NULL) {
        SNI_throwNativeException(LLSEC_X509_ERROR, "Can't allocate LLSEC_pub_key structure");
        return_code = LLSEC_X509_ERROR;
    }

    if( return_code == LLSEC_X509_SUCCESS) {
        x509 = get_x509_certificate(cert_data, cert_data_length, NULL);

        if (x509 == NULL) {
            mbedtls_free(pub_key);
            SNI_throwNativeException(LLSEC_X509_ERROR, "Bad x509 certificate");

            return_code = LLSEC_X509_ERROR;
        }
    }

    if( return_code == LLSEC_X509_SUCCESS) {
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

            SNI_throwNativeException(LLSEC_X509_ERROR, "Invalid public key from x509 certificate");
            return_code = LLSEC_X509_ERROR;
        }
    }

    if( return_code == LLSEC_X509_SUCCESS) {
        native_id = (void*)pub_key;
        if (SNI_registerResource(native_id, (SNI_closeFunction)LLSEC_X509_CERT_mbedtls_close_key, NULL) != SNI_OK) {
            SNI_throwNativeException(LLSEC_X509_ERROR, "Can't register SNI native resource");

            if (pub_key->type == TYPE_RSA) {
                mbedtls_rsa_free((mbedtls_rsa_context*)pub_key->key);
            } else {
                mbedtls_ecdsa_free((mbedtls_ecdsa_context*)pub_key->key);
            }

            mbedtls_x509_crt_free(x509);
            mbedtls_free(pub_key);
            return_code = LLSEC_X509_ERROR;
        }
    }

    if( return_code == LLSEC_X509_SUCCESS) {
        // cppcheck-suppress misra-c2012-11.6 // Abstract data type for SNI usage
        return_code = (uint32_t)native_id;
    }

    return return_code;
}

int32_t LLSEC_X509_CERT_IMPL_verify(int8_t* cert_data, int32_t cert_data_length, int32_t public_key_id)
{
    (void) public_key_id; // Unused input parameter

    LLSEC_X509_DEBUG_TRACE("%s \n", __func__);
    int return_code = LLSEC_X509_ERROR;
    uint32_t flags;

    mbedtls_x509_crt* x509 = get_x509_certificate(cert_data, cert_data_length, NULL);
    if (x509 == NULL) {
        SNI_throwNativeException(LLSEC_X509_ERROR, "Bad x509 certificate");
        return_code = LLSEC_X509_ERROR;
    }

    if (return_code == LLSEC_X509_SUCCESS){
        return_code = mbedtls_x509_crt_verify(x509, NULL, NULL, NULL, &flags, NULL, NULL);
        if (return_code != LLSEC_X509_SUCCESS) {
            // Error
            mbedtls_x509_crt_free(x509);
            LLSEC_X509_DEBUG_TRACE("LLSEC_X509 > verify error");
            SNI_throwNativeException(LLSEC_X509_ERROR, "Error x509 verify failed");
            return_code = LLSEC_X509_ERROR;
        }
    }

    if (return_code == LLSEC_X509_SUCCESS){
        mbedtls_x509_crt_free(x509);
    }
    return return_code;
}

/**
 * @brief Get the pointer for the close key  method to be used as a close resource callback with SNI.
 *
 * @return the pointer for the close method.
 *
 * @note Throws NativeException on error.
 */
int32_t LLSEC_X509_CERT_IMPL_get_close_key(void)
{
    LLSEC_X509_DEBUG_TRACE("%s \n", __func__);
    return (int32_t)LLSEC_X509_CERT_mbedtls_close_key;
}
