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

#include <LLSEC_ERRORS.h>
#include <LLSEC_PUBLIC_KEY_impl.h>
#include <LLSEC_mbedtls.h>
#include <LLSEC_configuration.h>

#include <sni.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mbedtls/platform.h"

//#define LLSEC_PUBLIC_KEY_DEBUG

#ifdef LLSEC_PUBLIC_KEY_DEBUG
#define LLSEC_PRIVATE_KEY_DEBUG_TRACE(...) printf(__VA_ARGS__)
#else
#define LLSEC_PRIVATE_KEY_DEBUG_TRACE(...) ((void)0)
#endif

/**
 * @brief get max size for encoded key.
 *
 * @param[in] native_id the C structure pointer holding the key data
 *
 * @return max encoded size for the public key
 *
 * @note Throws NativeException on error.
 */
int32_t LLSEC_PUBLIC_KEY_IMPL_get_encoded_max_size(int32_t native_id)
{
    LLSEC_PRIVATE_KEY_DEBUG_TRACE("%s \n", __func__);

    LLSEC_pub_key* key = (LLSEC_pub_key*)native_id;

    mbedtls_pk_context pk;
    mbedtls_pk_type_t pk_type;

    if (key->type == TYPE_RSA) {
        pk_type = MBEDTLS_PK_RSA;
    } else {
        pk_type = MBEDTLS_PK_ECKEY;
    }

    mbedtls_pk_init(&pk);
    mbedtls_pk_setup(&pk, mbedtls_pk_info_from_type(pk_type));
    pk.pk_ctx = (void*)key->key;

    char buf_local[LLSEC_PUBLIC_KEY_LOCAL_BUFFER_SIZE]; 
    /* Write a public key to a SubjectPublicKeyInfo DER structure */
    int length = mbedtls_pk_write_pubkey_der(&pk, (unsigned char*)(&buf_local), sizeof(buf_local));

    if (length < 0) {
        SNI_throwNativeException(-1, "Encoded key max size get failed");
    }

    return length;
}

/**
 * @brief encode the public key.
 *
 * @param[in] native_id the C structure pointer holding the key data
 * @param[out] output a byte array to hold the encoded key data
 * @param[in] outputLength the length of the output array
 *
 * @return the real size of the encoded key.
 *
 * @note Throws NativeException on error.
 */
int32_t LLSEC_PUBLIC_KEY_IMPL_get_encode(int32_t native_id, uint8_t* output, int32_t outputLength)
{
    LLSEC_PRIVATE_KEY_DEBUG_TRACE("%s \n", __func__);

    LLSEC_pub_key* key = (LLSEC_pub_key*)native_id;
    mbedtls_pk_context pk;
    mbedtls_pk_type_t pk_type;

    if (key->type == TYPE_RSA) {
        pk_type = MBEDTLS_PK_RSA;
    } else {
        pk_type = MBEDTLS_PK_ECKEY;
    }

    mbedtls_pk_init(&pk);
    mbedtls_pk_setup(&pk, mbedtls_pk_info_from_type(pk_type));
    pk.pk_ctx = (void*)key->key;

    /*Write a public key to a SubjectPublicKeyInfo DER structure*/
    int length = mbedtls_pk_write_pubkey_der(&pk, output, outputLength);

    if (length < 0) {
        SNI_throwNativeException(-1, "Public key encoding failed");
    }

    return length;
}
