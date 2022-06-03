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

#include <LLSEC_mbedtls.h>

#include <LLSEC_ERRORS.h>
#include <LLSEC_KEY_PAIR_GENERATOR_impl.h>
#include <sni.h>
#include <stdio.h>
#include <string.h>

#include "mbedtls/platform.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/dhm.h"
#include "mbedtls/ecdh.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/entropy.h"
#include "mbedtls/rsa.h"

#define LLSEC_KEY_PAIR_GENERATOR_SUCCESS 1
#define LLSEC_KEY_PAIR_GENERATOR_ERROR   0

//#define LLSEC_KEY_PAIR_GENERATOR_DEBUG

#ifdef LLSEC_KEY_PAIR_GENERATOR_DEBUG
#define LLSEC_KEY_PAIR_GENERATOR_DEBUG_TRACE(...) printf(__VA_ARGS__)
#else
#define LLSEC_KEY_PAIR_GENERATOR_DEBUG_TRACE(...) ((void)0)
#endif

typedef void (*LLSEC_KEY_PAIR_GENERATOR_close)(void* native_id);

//RSA
static int32_t LLSEC_KEY_PAIR_GENERATOR_RSA_mbedtls_generateKeyPair(int32_t rsa_Key_size, int32_t rsa_public_exponent);

//EC
static int32_t LLSEC_KEY_PAIR_GENERATOR_EC_mbedtls_generateKeyPair(uint8_t* ec_curve_stdname);

//common
static void LLSEC_KEY_PAIR_GENERATOR_mbedtls_close(void* native_id);

typedef struct {
    char* name;
    LLSEC_KEY_PAIR_GENERATOR_close close;
} LLSEC_KEY_PAIR_GENERATOR_algorithm;

static LLSEC_KEY_PAIR_GENERATOR_algorithm supportedAlgorithms[] = {
    {
        .name  = "RSA",
        .close = LLSEC_KEY_PAIR_GENERATOR_mbedtls_close
    },
    {
        .name  = "EC",
        .close = LLSEC_KEY_PAIR_GENERATOR_mbedtls_close
    }

};

static int32_t LLSEC_KEY_PAIR_GENERATOR_RSA_mbedtls_generateKeyPair(int32_t rsa_Key_size, int32_t rsa_public_exponent)
{
    int ret;
    mbedtls_rsa_context* ctx = mbedtls_calloc(1, sizeof(mbedtls_rsa_context)); //RSA key structure
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    const char* pers = llsec_gen_random_str_internal(8);

    mbedtls_entropy_init(&entropy);   //init entropy structure
    mbedtls_ctr_drbg_init(&ctr_drbg); //Initial random structure

    /* init rsa structure */
    mbedtls_rsa_init(ctx, MBEDTLS_RSA_PKCS_V21, //padding OAEP
                     MBEDTLS_MD_SHA256);        //SHA256

    /* update seed according personal string */
    ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                (const uint8_t*)pers, strlen(pers));
    if (0 != ret) {
        LLSEC_KEY_PAIR_GENERATOR_DEBUG_TRACE("%s \n", __func__);
        mbedtls_ctr_drbg_free(&ctr_drbg);
        mbedtls_entropy_free(&entropy);
        mbedtls_rsa_free(ctx);
        mbedtls_free((void*)pers);
        return LLSEC_KEY_PAIR_GENERATOR_ERROR;
    }

    /*Generate ras key pair*/
    ret = mbedtls_rsa_gen_key(ctx, mbedtls_ctr_drbg_random, //API of generating random
                              &ctr_drbg,                    //random structure
                              rsa_Key_size,                 //the size of public key
                              rsa_public_exponent);         //publick key exponent 0x01001

    LLSEC_priv_key* key = (LLSEC_priv_key*)mbedtls_calloc(1, sizeof(LLSEC_priv_key));
    if (key == NULL) {
        LLSEC_KEY_PAIR_GENERATOR_DEBUG_TRACE("%s \n", __func__);
        mbedtls_ctr_drbg_free(&ctr_drbg);
        mbedtls_entropy_free(&entropy);
        mbedtls_rsa_free(ctx);
        mbedtls_free((void*)pers);
        return LLSEC_KEY_PAIR_GENERATOR_ERROR;
    }

    key->key = (char*)ctx;
    key->type = TYPE_RSA;

    // Register the key to be managed by SNI as a native resource.
    // the close callback when be called when the key is collected by the GC
    // The key is freed in the close callback
    int32_t native_id = (int32_t)key;
    if (SNI_registerResource((void*)native_id, LLSEC_KEY_PAIR_GENERATOR_mbedtls_close, NULL) != SNI_OK) {
        SNI_throwNativeException(-1, "Can't register SNI native resource");

        mbedtls_ctr_drbg_free(&ctr_drbg);
        mbedtls_entropy_free(&entropy);
        mbedtls_rsa_free(ctx);
        mbedtls_free(key);
        mbedtls_free((void*)pers);
        return LLSEC_KEY_PAIR_GENERATOR_ERROR;
    }

    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
    mbedtls_free((void*)pers);

    return native_id;
}

static int32_t LLSEC_KEY_PAIR_GENERATOR_EC_mbedtls_generateKeyPair(uint8_t* ec_curve_stdname)
{
    int ret;
    mbedtls_ecdsa_context* ctx = mbedtls_calloc(1, sizeof(mbedtls_ecdsa_context));
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    const char* pers = llsec_gen_random_str_internal(8);

    mbedtls_ecdsa_init(ctx);
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);

    ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                (const uint8_t*)pers, strlen(pers));
    if (0 != ret) {
        LLSEC_KEY_PAIR_GENERATOR_DEBUG_TRACE("%s \n", __func__);
        mbedtls_ecdsa_free(ctx);
        mbedtls_ctr_drbg_free(&ctr_drbg);
        mbedtls_entropy_free(&entropy);
        mbedtls_free((void*)pers);
        return LLSEC_KEY_PAIR_GENERATOR_ERROR;
    }

    /* Generate ecdsa Key pair */
    ret = mbedtls_ecdsa_genkey(ctx,
                               MBEDTLS_ECP_DP_SECP256R1,
                               mbedtls_ctr_drbg_random, &ctr_drbg);
    if (0 != ret) {
        LLSEC_KEY_PAIR_GENERATOR_DEBUG_TRACE("%s \n", __func__);
        mbedtls_ecdsa_free(ctx);
        mbedtls_ctr_drbg_free(&ctr_drbg);
        mbedtls_entropy_free(&entropy);
        mbedtls_free((void*)pers);
        return LLSEC_KEY_PAIR_GENERATOR_ERROR;
    }

    LLSEC_priv_key* key = (LLSEC_priv_key*)mbedtls_calloc(1, sizeof(LLSEC_priv_key));
    if (key == NULL) {
        LLSEC_KEY_PAIR_GENERATOR_DEBUG_TRACE("%s \n", __func__);
        mbedtls_ecdsa_free(ctx);
        mbedtls_ctr_drbg_free(&ctr_drbg);
        mbedtls_entropy_free(&entropy);
        mbedtls_free((void*)pers);
        return LLSEC_KEY_PAIR_GENERATOR_ERROR;
    }

    key->key = (char*)ctx;
    key->type = TYPE_ECDSA;

    // Register the key to be managed by SNI as a native resource.
    // the close callback when be called when the key is collected by the GC
    // The key is freed in the close callback
    int32_t native_id = (int32_t)key;
    if (SNI_registerResource((void*)native_id, LLSEC_KEY_PAIR_GENERATOR_mbedtls_close, NULL) != SNI_OK) {
        SNI_throwNativeException(-1, "Can't register SNI native resource");

        mbedtls_ecdsa_free(ctx);
        mbedtls_ctr_drbg_free(&ctr_drbg);
        mbedtls_entropy_free(&entropy);
        mbedtls_free(key);
        mbedtls_free((void*)pers);
        return LLSEC_KEY_PAIR_GENERATOR_ERROR;
    }

    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
    mbedtls_free((void*)pers);

    return native_id;
}

static void LLSEC_KEY_PAIR_GENERATOR_mbedtls_close(void* native_id)
{
    LLSEC_KEY_PAIR_GENERATOR_DEBUG_TRACE("%s \n", __func__);

    LLSEC_priv_key* key = (LLSEC_priv_key*)native_id;

    if (key->type == TYPE_RSA) {
        mbedtls_rsa_free((mbedtls_rsa_context*)key->key);
    } else {
        mbedtls_ecdsa_free((mbedtls_ecdsa_context*)key->key);
    }
    mbedtls_free(key);
}

int32_t LLSEC_KEY_PAIR_GENERATOR_IMPL_get_algorithm(uint8_t* algorithm_name)
{
    LLSEC_KEY_PAIR_GENERATOR_DEBUG_TRACE("%s \n", __func__);

    int32_t nb_algorithms = sizeof(supportedAlgorithms) / sizeof(LLSEC_KEY_PAIR_GENERATOR_algorithm);
    LLSEC_KEY_PAIR_GENERATOR_algorithm* algorithm = &supportedAlgorithms[0];

    while (--nb_algorithms >= 0) {
        if (strcmp((char*)algorithm_name, algorithm->name) == 0) {
            return (int32_t)algorithm;
        }
        algorithm++;
    }

    // Algorithm not found.
    return -1;
}

int32_t LLSEC_KEY_PAIR_GENERATOR_IMPL_generateKeyPair(int32_t algorithm_id, int32_t rsa_key_size, int32_t rsa_public_exponent, uint8_t* ec_curve_stdname)
{
    LLSEC_KEY_PAIR_GENERATOR_DEBUG_TRACE("%s \n", __func__);

    LLSEC_KEY_PAIR_GENERATOR_algorithm* algorithm = (LLSEC_KEY_PAIR_GENERATOR_algorithm*)algorithm_id;
    if ((strcmp(algorithm->name, "RSA") == 0)) {
        return LLSEC_KEY_PAIR_GENERATOR_RSA_mbedtls_generateKeyPair(rsa_key_size, rsa_public_exponent);
    } else if ((strcmp(algorithm->name, "EC") == 0)) {
        return LLSEC_KEY_PAIR_GENERATOR_EC_mbedtls_generateKeyPair(ec_curve_stdname);
    }

    // Algorithm not found error.
    // this should never happen because the algorithm_id is a valid algorithm at this level.
    SNI_throwNativeException(-1, "Unsupported algorithm");
    return -1;
}

int32_t LLSEC_KEY_PAIR_GENERATOR_IMPL_get_close_id(int32_t algorithm_id)
{
    LLSEC_KEY_PAIR_GENERATOR_algorithm* algorithm = (LLSEC_KEY_PAIR_GENERATOR_algorithm*)algorithm_id;
    return (int32_t)algorithm->close;
}
