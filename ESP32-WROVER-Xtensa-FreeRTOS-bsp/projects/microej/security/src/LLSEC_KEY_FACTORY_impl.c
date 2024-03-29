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

#include <LLSEC_ERRORS.h>
#include <LLSEC_KEY_FACTORY_impl.h>
#include <LLSEC_mbedtls.h>
#include <sni.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "mbedtls/platform.h"

#define LLSEC_KEY_FACTORY_SUCCESS 0
#define LLSEC_KEY_FACTORY_ERROR   -1

//#define LLSEC_KEY_FACTORY_DEBUG

#ifdef LLSEC_KEY_FACTORY_DEBUG
// cppcheck-suppress misra-c2012-21.6 // Include only in debug
#include <stdio.h>
#define LLSEC_KEY_FACTORY_DEBUG_TRACE(...) (void)printf(__VA_ARGS__)
#else
#define LLSEC_KEY_FACTORY_DEBUG_TRACE(...) ((void)0)
#endif

// cppcheck-suppress misra-c2012-8.9 // Define here for code readability even if it called once in this file.
static const char* pkcs8_format = "PKCS#8";
// cppcheck-suppress misra-c2012-8.9 // Define here for code readability even if it called once in this file.
static const char* x509_format = "X.509";

typedef int32_t (*LLSEC_KEY_FACTORY_get_private_key_data)(LLSEC_priv_key* priv_key, uint8_t* encoded_key, int32_t encoded_key_length);
typedef int32_t (*LLSEC_KEY_FACTORY_get_public_key_data)(LLSEC_pub_key* pub_key, uint8_t* encoded_key, int32_t encoded_key_length);
typedef void (*LLSEC_KEY_FACTORY_key_close)(void* native_id);

typedef struct {
    char* name;
    LLSEC_KEY_FACTORY_get_private_key_data get_private_key_data;
    LLSEC_KEY_FACTORY_get_public_key_data get_public_key_data;
    LLSEC_KEY_FACTORY_key_close private_key_close;
    LLSEC_KEY_FACTORY_key_close public_key_close;
} LLSEC_KEY_FACTORY_algorithm;

static int32_t LLSEC_KEY_FACTORY_RSA_mbedtls_get_private_key_data(LLSEC_priv_key* priv_key, uint8_t* encoded_key, int32_t encoded_key_length);
static int32_t LLSEC_KEY_FACTORY_RSA_mbedtls_get_public_key_data(LLSEC_pub_key* pub_key, uint8_t* encoded_key, int32_t encoded_key_length);
static int32_t LLSEC_KEY_FACTORY_EC_mbedtls_get_private_key_data(LLSEC_priv_key* priv_key, uint8_t* encoded_key, int32_t encoded_key_length);
static int32_t LLSEC_KEY_FACTORY_EC_mbedtls_get_public_key_data(LLSEC_pub_key* pub_key, uint8_t* encoded_key, int32_t encoded_key_length);
static void LLSEC_KEY_FACTORY_mbedtls_private_key_close(void* native_id);
static void LLSEC_KEY_FACTORY_mbedtls_public_key_close(void* native_id);

// cppcheck-suppress misra-c2012-8.9 // Define here for code readability even if it called once in this file.
static LLSEC_KEY_FACTORY_algorithm available_key_algorithms[2] = {
    {
        .name = "RSA",
        .get_private_key_data = LLSEC_KEY_FACTORY_RSA_mbedtls_get_private_key_data,
        .get_public_key_data  = LLSEC_KEY_FACTORY_RSA_mbedtls_get_public_key_data,
        .private_key_close    = LLSEC_KEY_FACTORY_mbedtls_private_key_close,
        .public_key_close     = LLSEC_KEY_FACTORY_mbedtls_public_key_close
    },
    {
        .name = "EC",
        .get_private_key_data = LLSEC_KEY_FACTORY_EC_mbedtls_get_private_key_data,
        .get_public_key_data = LLSEC_KEY_FACTORY_EC_mbedtls_get_public_key_data,
        .private_key_close    = LLSEC_KEY_FACTORY_mbedtls_private_key_close,
        .public_key_close     = LLSEC_KEY_FACTORY_mbedtls_public_key_close
    }
};

static int32_t LLSEC_KEY_FACTORY_RSA_mbedtls_get_private_key_data(LLSEC_priv_key* priv_key, uint8_t* encoded_key, int32_t encoded_key_length)
{
    LLSEC_KEY_FACTORY_DEBUG_TRACE("%s \n", __func__);

    int return_code = LLSEC_KEY_FACTORY_SUCCESS;
    priv_key->type = TYPE_RSA;
    mbedtls_pk_context pk;
    mbedtls_pk_init(&pk);
    return_code = mbedtls_pk_parse_key(&pk, encoded_key, encoded_key_length, NULL, 0);

    if(return_code != LLSEC_KEY_FACTORY_SUCCESS) {
        LLSEC_KEY_FACTORY_DEBUG_TRACE("mbedtls private key parsing failed");
        mbedtls_free(priv_key);
    }

    if(return_code == LLSEC_KEY_FACTORY_SUCCESS) {
        priv_key->key = (char*)mbedtls_pk_rsa(pk);

        if (priv_key->key == NULL) {
            SNI_throwNativeException(-1, "RSA context extraction failed");
            mbedtls_free(priv_key);
            return_code = LLSEC_KEY_FACTORY_ERROR;
        }
    }
    
    if(return_code == LLSEC_KEY_FACTORY_SUCCESS) {

        void* native_id = (void*)priv_key;
        if (SNI_registerResource(native_id, (SNI_closeFunction)LLSEC_KEY_FACTORY_mbedtls_private_key_close, NULL) != SNI_OK) {
            SNI_throwNativeException(LLSEC_KEY_FACTORY_ERROR, "Can't register SNI native resource");
            mbedtls_rsa_free((mbedtls_rsa_context*)priv_key->key);
            mbedtls_free(priv_key);
            return_code = LLSEC_KEY_FACTORY_ERROR;
        } else {
            // cppcheck-suppress misra-c2012-11.6 // Abstract data type for SNI usage
            return_code = (int32_t)native_id;
        }
    }

    return return_code;
}

static int32_t LLSEC_KEY_FACTORY_RSA_mbedtls_get_public_key_data(LLSEC_pub_key* pub_key, uint8_t* encoded_key, int32_t encoded_key_length)
{
    LLSEC_KEY_FACTORY_DEBUG_TRACE("%s \n", __func__);

    int return_code = LLSEC_KEY_FACTORY_SUCCESS;
    pub_key->type = TYPE_RSA;
    mbedtls_pk_context pk;
    mbedtls_pk_init(&pk);
    return_code = mbedtls_pk_parse_public_key(&pk, encoded_key, encoded_key_length);

    if(return_code != LLSEC_KEY_FACTORY_SUCCESS) {
        LLSEC_KEY_FACTORY_DEBUG_TRACE("mbedtls public key parsing failed");
        mbedtls_free(pub_key);
    }

    if(return_code == LLSEC_KEY_FACTORY_SUCCESS) {
        pub_key->key = (char*)mbedtls_pk_rsa(pk);

        if (pub_key->key == NULL) {
            SNI_throwNativeException(LLSEC_KEY_FACTORY_ERROR, "RSA public key extraction failed");
            mbedtls_free(pub_key);
            return_code = LLSEC_KEY_FACTORY_ERROR;
        }
    }

    if(return_code == LLSEC_KEY_FACTORY_SUCCESS) {

        void* native_id = (void*)pub_key;
        if (SNI_registerResource(native_id, (SNI_closeFunction)LLSEC_KEY_FACTORY_mbedtls_public_key_close, NULL) != SNI_OK) {
            SNI_throwNativeException(LLSEC_KEY_FACTORY_ERROR, "Can't register SNI native resource");
            mbedtls_rsa_free((mbedtls_rsa_context*)pub_key->key);
            mbedtls_free(pub_key);
            return_code = LLSEC_KEY_FACTORY_ERROR;
        } else {
            // cppcheck-suppress misra-c2012-11.6 // Abstract data type for SNI usage
            return_code = (int32_t)native_id;
        }
    }

    return return_code;
}

static int32_t LLSEC_KEY_FACTORY_EC_mbedtls_get_private_key_data(LLSEC_priv_key* priv_key, uint8_t* encoded_key, int32_t encoded_key_length)
{
    LLSEC_KEY_FACTORY_DEBUG_TRACE("%s \n", __func__);
    LLSEC_KEY_FACTORY_DEBUG_TRACE("encLen %d, strlen: %zu, enc:%s  \n", encoded_key_length, strlen((char*)encoded_key), encoded_key);

    int return_code = LLSEC_KEY_FACTORY_SUCCESS;
    priv_key->type = TYPE_ECDSA;
    mbedtls_pk_context pk;
    mbedtls_pk_init(&pk);
    return_code  = mbedtls_pk_parse_key(&pk, encoded_key, encoded_key_length, NULL, 0);

    if (return_code != LLSEC_KEY_FACTORY_SUCCESS) {
        LLSEC_KEY_FACTORY_DEBUG_TRACE("parse private key error: %d\n", return_code);
        mbedtls_free(priv_key);
    }

    if (return_code == LLSEC_KEY_FACTORY_SUCCESS) {

        LLSEC_KEY_FACTORY_DEBUG_TRACE("%s 11.\n", __func__);
        priv_key->key = (char*)mbedtls_pk_ec(pk);

        if (priv_key->key == NULL) {
            SNI_throwNativeException(-1, "EC private key extraction failed");
            mbedtls_free(priv_key);
            return_code = LLSEC_KEY_FACTORY_ERROR;
        }
    }


    if (return_code == LLSEC_KEY_FACTORY_SUCCESS) {

        void* native_id = (void*)priv_key;
        if (SNI_registerResource(native_id, (SNI_closeFunction)LLSEC_KEY_FACTORY_mbedtls_private_key_close, NULL) != SNI_OK) {
            SNI_throwNativeException(-1, "Can't register SNI native resource");
            mbedtls_ecdsa_free((mbedtls_ecdsa_context*)priv_key->key);
            mbedtls_free(priv_key);
            return_code = LLSEC_KEY_FACTORY_ERROR;
        } else {
            // cppcheck-suppress misra-c2012-11.6 // Abstract data type for SNI usage
            return_code = (int32_t)native_id;
        }
    }

    return return_code;
}

static int32_t LLSEC_KEY_FACTORY_EC_mbedtls_get_public_key_data(LLSEC_pub_key* pub_key, uint8_t* encoded_key, int32_t encoded_key_length)
{
    LLSEC_KEY_FACTORY_DEBUG_TRACE("%s \n", __func__);

    int return_code = LLSEC_KEY_FACTORY_SUCCESS;

    pub_key->type = TYPE_ECDSA;

    mbedtls_pk_context pk;
    mbedtls_pk_init(&pk);
    return_code = mbedtls_pk_parse_public_key(&pk, encoded_key, encoded_key_length);

    if(return_code != LLSEC_KEY_FACTORY_SUCCESS) {
        LLSEC_KEY_FACTORY_DEBUG_TRACE("parse public key error: %d\n", return_code);
        mbedtls_free(pub_key);
    }

    if(return_code == LLSEC_KEY_FACTORY_SUCCESS) {
        pub_key->key = (char*)mbedtls_pk_ec(pk);

        if (pub_key->key == NULL) {
            SNI_throwNativeException(-1, "EC context extraction failed");
            mbedtls_free(pub_key);
            return_code = LLSEC_KEY_FACTORY_ERROR;
        }
    }

    if(return_code == LLSEC_KEY_FACTORY_SUCCESS) {
        void* native_id = (void*)pub_key;
        if (SNI_registerResource(native_id, (SNI_closeFunction)LLSEC_KEY_FACTORY_mbedtls_public_key_close, NULL) != SNI_OK) {
            SNI_throwNativeException(-1, "Can't register SNI native resource");
            mbedtls_ecdsa_free((mbedtls_ecdsa_context*)pub_key->key);
            mbedtls_free(pub_key);
            return_code = LLSEC_KEY_FACTORY_ERROR;
        } else {
            // cppcheck-suppress misra-c2012-11.6 // Abstract data type for SNI usage
            return_code = (int32_t)native_id;
        }
    }

    return return_code;

}

static void LLSEC_KEY_FACTORY_mbedtls_private_key_close(void* native_id)
{
    // cppcheck-suppress misra-c2012-11.5 // Abstract data type for SNI usage
    LLSEC_priv_key* key = (LLSEC_priv_key*)native_id;

    if (key->type == TYPE_RSA) {
        mbedtls_rsa_free((mbedtls_rsa_context*)key->key);
    } else {
        mbedtls_ecdsa_free((mbedtls_ecdsa_context*)key->key);
    }
    mbedtls_free(key);
}

static void LLSEC_KEY_FACTORY_mbedtls_public_key_close(void* native_id)
{
    // cppcheck-suppress misra-c2012-11.5 // Abstract data type for SNI usage
    LLSEC_pub_key* key = (LLSEC_pub_key*)native_id;
    if (key->type == TYPE_RSA) {
        mbedtls_rsa_free((mbedtls_rsa_context*)key->key);
    } else {
        mbedtls_ecdsa_free((mbedtls_ecdsa_context*)key->key);
    }
    mbedtls_free(key);
}
/*------------------------------------------Above is internal implementation--------------------------------------------------------*/

/**
 * @brief Gets for the given algorithm the key factory description.
 *
 * @param[in] algorithm_name			Null terminated string that describes the algorithm.
 *
 * @return The algorithm ID on success or -1 on error.
 */
int32_t LLSEC_KEY_FACTORY_IMPL_get_algorithm_description(uint8_t* algorithm_name)
{
    int32_t return_code = LLSEC_KEY_FACTORY_SUCCESS;
    LLSEC_KEY_FACTORY_DEBUG_TRACE("%s \n", __func__);
    int32_t nb_algorithms = sizeof(available_key_algorithms) / sizeof(LLSEC_KEY_FACTORY_algorithm);
    LLSEC_KEY_FACTORY_algorithm* algorithm = &available_key_algorithms[0];

    while (--nb_algorithms >= 0) {
        if (strcmp((char*)algorithm_name, algorithm->name) == 0) {
            break;
        }
        algorithm++;
    }

    if (nb_algorithms >= 0)
    {
        // cppcheck-suppress misra-c2012-11.4 // Abstract data type for SNI usage
        return_code = (int32_t)algorithm;
    } else {
        return_code = LLSEC_KEY_FACTORY_ERROR;
    }
    return return_code;
}

/**
 * @brief Gets the native public key corresponding to the encoded data.
 *
 * @param[in] algorithm_id				The algorithm ID.
 * @param[in] format_name				Null terminated string that describes the key format.
 * @param[in] encoded_key				The public key in its encoded form.
 * @param[in] encoded_key_length		The encoded key length.
 *
 * @return pointer on the C structure holding the key information.
 *
 * @note Throws NativeException on error.
 */
int32_t LLSEC_KEY_FACTORY_IMPL_get_public_key_data(int32_t algorithm_id, uint8_t* format_name, uint8_t* encoded_key, int32_t encoded_key_length)
{
    int32_t return_code = LLSEC_KEY_FACTORY_SUCCESS;
    LLSEC_KEY_FACTORY_DEBUG_TRACE("%s \n", __func__);
    LLSEC_pub_key* public_key = NULL;


    if (strcmp((char*)format_name, x509_format) != 0) {
        SNI_throwNativeException(LLSEC_KEY_FACTORY_ERROR, "Invalid format name");
        return_code = LLSEC_KEY_FACTORY_ERROR;
    }

    if (return_code == LLSEC_KEY_FACTORY_SUCCESS) {
        public_key = (LLSEC_pub_key*)mbedtls_calloc(1, sizeof(LLSEC_pub_key));
        if (public_key == NULL) {
            SNI_throwNativeException(LLSEC_KEY_FACTORY_ERROR, "Can't allocate LLSEC_pub_key structure");
            return_code = LLSEC_KEY_FACTORY_ERROR;
        }
    }

    if (return_code == LLSEC_KEY_FACTORY_SUCCESS) {
        // cppcheck-suppress misra-c2012-11.4 // Abstract data type for SNI usage
        LLSEC_KEY_FACTORY_algorithm* algorithm = (LLSEC_KEY_FACTORY_algorithm*)algorithm_id;
        return_code = algorithm->get_public_key_data(public_key, encoded_key, encoded_key_length);
    }

    return return_code;
}

/**
 * @brief Gets the native private key corresponding to the encoded data.
 *
 * @param[in] algorithm_id				The algorithm ID.
 * @param[in] format_name				Null terminated string that describes the key format.
 * @param[out] key_data					The private key in the native format.
 * @param[out] key_data_length			The private key length.
 * @param[in] encoded_key				The private key in its encoded form.
 * @param[in] encoded_key_length		The encoded key length.
 *
 * @note Throws NativeException on error.
 */
int32_t LLSEC_KEY_FACTORY_IMPL_get_private_key_data(int32_t algorithm_id, uint8_t* format_name, uint8_t* encoded_key, int32_t encoded_key_length)
{
    int32_t return_code = LLSEC_KEY_FACTORY_SUCCESS;
    LLSEC_KEY_FACTORY_DEBUG_TRACE("%s \n", __func__);
    LLSEC_priv_key* private_key = NULL;

    if (strcmp((char*)format_name, pkcs8_format) != 0) {
        SNI_throwNativeException(-1, "Invalid format name");
        return_code = LLSEC_KEY_FACTORY_ERROR;
    }

    if (return_code == LLSEC_KEY_FACTORY_SUCCESS) {
        private_key = (LLSEC_priv_key*)mbedtls_calloc(1, sizeof(LLSEC_priv_key));
        if (private_key == NULL) {
            SNI_throwNativeException(-1, "Can't allocate LLSEC_priv_key structure");
            return_code = LLSEC_KEY_FACTORY_ERROR;
        }
    }

    if (return_code == LLSEC_KEY_FACTORY_SUCCESS) {
        // cppcheck-suppress misra-c2012-11.4 // Abstract data type for SNI usage
        LLSEC_KEY_FACTORY_algorithm* algorithm = (LLSEC_KEY_FACTORY_algorithm*)algorithm_id;
        return_code = algorithm->get_private_key_data(private_key, encoded_key, encoded_key_length);
    }

    return return_code;
}

/**
 * @brief Get the pointer for the close private key method to be used as a close resource callback with SNI.
 *
 * @param[in] algorithm_id algorithm pointer
 *
 * @return the pointer for the close method.
 *
 * @note Throws NativeException on error.
 */
int32_t LLSEC_KEY_FACTORY_IMPL_get_private_key_close_id(int32_t algorithm_id)
{
    LLSEC_KEY_FACTORY_DEBUG_TRACE("%s \n", __func__);

    // cppcheck-suppress misra-c2012-11.4 // Abstract data type for SNI usage
    LLSEC_KEY_FACTORY_algorithm* algorithm = (LLSEC_KEY_FACTORY_algorithm*)algorithm_id;
    // cppcheck-suppress misra-c2012-11.1 // Abstract data type for SNI usage
    return (int32_t)algorithm->private_key_close;
}

/**
 * @brief Get the pointer for the close public key method to be used as a close resource callback with SNI.
 *
 * @param[in] algorithm_id algorithm pointer
 *
 * @return the pointer for the close method.
 *
 * @note Throws NativeException on error.
 */
int32_t LLSEC_KEY_FACTORY_IMPL_get_public_key_close_id(int32_t algorithm_id)
{
    LLSEC_KEY_FACTORY_DEBUG_TRACE("%s \n", __func__);

    // cppcheck-suppress misra-c2012-11.4 // Abstract data type for SNI usage
    LLSEC_KEY_FACTORY_algorithm* algorithm = (LLSEC_KEY_FACTORY_algorithm*)algorithm_id;
    // cppcheck-suppress misra-c2012-11.1 // Abstract data type for SNI usage
    return (int32_t)algorithm->public_key_close;
}
