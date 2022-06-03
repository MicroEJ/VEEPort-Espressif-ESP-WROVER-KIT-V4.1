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
#include <LLSEC_PRIVATE_KEY_impl.h>
#include <LLSEC_mbedtls.h>
#include <LLSEC_configuration.h>

#include <sni.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mbedtls/platform.h"


//#define LLSEC_PRIVATE_KEY_DEBUG

#ifdef LLSEC_PRIVATE_KEY_DEBUG
#define LLSEC_PRIVATE_KEY_DEBUG_TRACE(...) printf(__VA_ARGS__)
#else
#define LLSEC_PRIVATE_KEY_DEBUG_TRACE(...) ((void)0)
#endif

/**
 * @brief return the max size of the encoded key.
 *
 * @param[in] native_id the C structure pointer holding the key data
 *
 * @return max encoded size for the private key in DER format
 *
 * @note Throws NativeException on error.
 */
int32_t LLSEC_PRIVATE_KEY_IMPL_get_encoded_max_size(int32_t native_id)
{
    LLSEC_PRIVATE_KEY_DEBUG_TRACE("%s \n", __func__);

    LLSEC_priv_key* key = (LLSEC_priv_key*)native_id;
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

    char buf_local[LLSEC_PRIVATE_KEY_LOCAL_BUFFER_SIZE]; 
    int length = mbedtls_pk_write_key_der(&pk, (unsigned char*)(&buf_local), sizeof(buf_local));

    if (length < 0) {
        SNI_throwNativeException(-length, "Encoded max size get failed");
    }

    return length;
}

/**
 * @brief encode the private key into DER format.
 *
 * @param[in] native_id the C structure pointer holding the key data
 * @param[out] output a byte array to hold the encoded key data
 * @pram[in] outputLength the length of the output array
 *
 * @return the reel size of the encoded key.
 *
 * @note Throws NativeException on error.
 */
int32_t LLSEC_PRIVATE_KEY_IMPL_get_encode(int32_t native_id, uint8_t* output, int32_t outputLength)
{
    LLSEC_PRIVATE_KEY_DEBUG_TRACE("%s \n", __func__);

    LLSEC_priv_key* key = (LLSEC_priv_key*)native_id;
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

    //Write a private key to a PKCS#1 or SEC1 DER structure
    int length = mbedtls_pk_write_key_der(&pk, output, outputLength);

    if (length < 0) {
        SNI_throwNativeException(-1, "DER encoding failed");
    }

    return length;
}
