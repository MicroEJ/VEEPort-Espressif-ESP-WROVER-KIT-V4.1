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
#include <LLSEC_SIG_impl.h>
#include <sni.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "LLSEC_mbedtls.h"
#include "mbedtls/platform.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "mbedtls/error.h"
#include "mbedtls/md.h"
#include "mbedtls/pk.h"

#define LLSEC_SIG_SUCCESS 0
#define LLSEC_SIG_ERROR   1

//#define LLSEC_SIG_DEBUG

#ifdef LLSEC_SIG_DEBUG
#define LLSEC_SIG_DEBUG_TRACE(...) printf(__VA_ARGS__)
#else
#define LLSEC_SIG_DEBUG_TRACE(...) ((void)0)
#endif

typedef struct LLSEC_SIG_algorithm LLSEC_SIG_algorithm;
typedef int (*LLSEC_SIG_verify)(LLSEC_SIG_algorithm* algorithm, uint8_t* signature, int32_t signature_length, LLSEC_pub_key* pub_key, uint8_t* digest, int32_t digest_length);
typedef int (*LLSEC_SIG_sign)(LLSEC_SIG_algorithm* algorithm, uint8_t* signature, int32_t* signature_length, LLSEC_priv_key* priv_key, uint8_t* digest, int32_t digest_length);

struct LLSEC_SIG_algorithm {
    char* name;
    char* digest_name;
    char* digest_native_name;
    char* oid;
    LLSEC_SIG_verify verify;
    LLSEC_SIG_sign sign;
};

static int LLSEC_SIG_mbedtls_verify(LLSEC_SIG_algorithm* algorithm, uint8_t* signature, int32_t signature_length, LLSEC_pub_key* pub_key, uint8_t* digest, int32_t digest_length);
static int LLSEC_SIG_mbedtls_sign(LLSEC_SIG_algorithm* algorithm, uint8_t* signature, int32_t* signature_length, LLSEC_priv_key* priv_key, uint8_t* digest, int32_t digest_length);

static int LLSEC_SIG_mbedtls_ec_verify(LLSEC_SIG_algorithm* algorithm, uint8_t* signature, int32_t signature_length, LLSEC_pub_key* pub_key, uint8_t* digest, int32_t digest_length);
static int LLSEC_SIG_mbedtls_ec_sign(LLSEC_SIG_algorithm* algorithm, uint8_t* signature, int32_t* signature_length, LLSEC_priv_key* priv_key, uint8_t* digest, int32_t digest_length);

static LLSEC_SIG_algorithm available_algorithms[] = {
    {
        .name = "SHA256withRSA",
        .digest_name = "SHA-256",
        .digest_native_name = "SHA256",
        .oid = "1.2.840.113549.1.1.11",
        .verify = LLSEC_SIG_mbedtls_verify,
        .sign = LLSEC_SIG_mbedtls_sign
    },
    {
        .name = "SHA256withECDSA",
        .digest_name = "SHA-256",
        .digest_native_name = "SHA256",
        .oid = "1.2.840.10045.4.3.2",
        .verify = LLSEC_SIG_mbedtls_ec_verify,
        .sign = LLSEC_SIG_mbedtls_ec_sign
    }
};

static int LLSEC_SIG_mbedtls_ec_verify(LLSEC_SIG_algorithm* algorithm, uint8_t* signature, int32_t signature_length, LLSEC_pub_key* pub_key, uint8_t* digest, int32_t digest_length)
{
    LLSEC_SIG_DEBUG_TRACE("%s \n", __func__);

    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    const char* pers = llsec_gen_random_str_internal(8);

    int ret = LLSEC_SIG_ERROR;

    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);

    ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                 (const unsigned char*)pers,
                                 strlen(pers));
    if (ret != 0) {
        mbedtls_ctr_drbg_free(&ctr_drbg);
        mbedtls_entropy_free(&entropy);
        mbedtls_free((void*)pers);
        return LLSEC_SIG_ERROR;
    }

    mbedtls_ecdsa_context* ctx = (mbedtls_ecdsa_context*)pub_key->key;
    ret = mbedtls_ecdsa_read_signature(ctx, digest, (size_t)digest_length,
                                        signature, signature_length);
    if (ret != 0) {
        mbedtls_ctr_drbg_free(&ctr_drbg);
        mbedtls_entropy_free(&entropy);
        mbedtls_free((void*)pers);
        return LLSEC_SIG_ERROR;
    }

    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
    mbedtls_free((void*)pers);

    return LLSEC_SIG_SUCCESS;
}

static int LLSEC_SIG_mbedtls_ec_sign(LLSEC_SIG_algorithm* algorithm, uint8_t* signature, int32_t* signature_length, LLSEC_priv_key* priv_key, uint8_t* digest, int32_t digest_length)
{
    LLSEC_SIG_DEBUG_TRACE("%s \n", __func__);

    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    const char* pers = llsec_gen_random_str_internal(8);

    int ret = LLSEC_SIG_ERROR;

    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);

    ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                     (const unsigned char*)pers,
                                     strlen(pers));

    if (ret != 0) {
        mbedtls_ctr_drbg_free(&ctr_drbg);
        mbedtls_entropy_free(&entropy);
        mbedtls_free((void*)pers);
        return LLSEC_SIG_ERROR;
    }

    mbedtls_ecdsa_context* ctx = (mbedtls_ecdsa_context*)priv_key->key;
    ret = mbedtls_ecdsa_write_signature(ctx, MBEDTLS_MD_SHA256,
                                         digest, (size_t)digest_length,
                                         signature, (size_t*)signature_length,
                                         mbedtls_ctr_drbg_random, &ctr_drbg);
    if (ret != 0) {
        mbedtls_ctr_drbg_free(&ctr_drbg);
        mbedtls_entropy_free(&entropy);
        mbedtls_free((void*)pers);
        return LLSEC_SIG_ERROR;
    }

    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
    mbedtls_free((void*)pers);

    return LLSEC_SIG_SUCCESS;
}

static int LLSEC_SIG_mbedtls_verify(LLSEC_SIG_algorithm* algorithm, uint8_t* signature, int32_t signature_length, LLSEC_pub_key* pub_key, uint8_t* digest, int32_t digest_length)
{
    LLSEC_SIG_DEBUG_TRACE("%s \n", __func__);

    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    const char* pers = llsec_gen_random_str_internal(8);

    int ret = LLSEC_SIG_ERROR;

    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);

    ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                     (const unsigned char*)pers,
                                     strlen(pers));
    if (ret != 0) {
        mbedtls_ctr_drbg_free(&ctr_drbg);
        mbedtls_entropy_free(&entropy);
        mbedtls_free((void*)pers);
        return LLSEC_SIG_ERROR;
    }

    ret = mbedtls_rsa_pkcs1_verify((mbedtls_rsa_context*)pub_key->key,
                                        mbedtls_ctr_drbg_random, &ctr_drbg,
                                        MBEDTLS_RSA_PUBLIC, MBEDTLS_MD_SHA256,
                                        digest_length, digest, signature);
    if (ret != 0) {
        mbedtls_ctr_drbg_free(&ctr_drbg);
        mbedtls_entropy_free(&entropy);
        mbedtls_free((void*)pers);
        return LLSEC_SIG_ERROR;
    }

    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
    mbedtls_free((void*)pers);

    return LLSEC_SIG_SUCCESS;
}

static int LLSEC_SIG_mbedtls_sign(LLSEC_SIG_algorithm* algorithm, uint8_t* signature, int32_t* signature_length, LLSEC_priv_key* priv_key, uint8_t* digest, int32_t digest_length)
{
    LLSEC_SIG_DEBUG_TRACE("%s \n", __func__);

    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    const char* pers = llsec_gen_random_str_internal(8);

    int ret = LLSEC_SIG_ERROR;

    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);

    ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                         (const unsigned char*)pers,
                                         strlen(pers));
    if (ret != 0) {
        mbedtls_ctr_drbg_free(&ctr_drbg);
        mbedtls_entropy_free(&entropy);
        mbedtls_free((void*)pers);
        return LLSEC_SIG_ERROR;
    }

    ret = mbedtls_rsa_pkcs1_sign((mbedtls_rsa_context*)priv_key->key,
                                          mbedtls_ctr_drbg_random, &ctr_drbg,
                                          MBEDTLS_RSA_PRIVATE, MBEDTLS_MD_SHA256,
                                          digest_length, digest, signature);
    if (ret != 0) {
        mbedtls_ctr_drbg_free(&ctr_drbg);
        mbedtls_entropy_free(&entropy);
        mbedtls_free((void*)pers);
        return LLSEC_SIG_ERROR;
    }

   * signature_length = ((mbedtls_rsa_context*)priv_key->key)->len;

    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
    mbedtls_free((void*)pers);

    return LLSEC_SIG_SUCCESS;
}

/**
 * @brief Gets for the given algorithm the message digest description.
 *
 * @param[in] algorithm_name				Null terminated string that describes the algorithm.
 * @param[out] digest_algorithm_name		Null terminated string that describes the digest algorithm.
 * @param[in] digest_algorithm_name_length	Length of digest_algorithm.
 *
 * @return The algorithm ID on success or -1 on error.
 *
 * @warning <code>algorithm_name</code> must not be used outside of the VM task or saved.
 */
int32_t LLSEC_SIG_IMPL_get_algorithm_description(uint8_t* algorithm_name, uint8_t* digest_algorithm_name, int32_t digest_algorithm_name_length)
{
    LLSEC_SIG_DEBUG_TRACE("%s \n", __func__);

    int32_t nb_algorithms = sizeof(available_algorithms) / sizeof(LLSEC_SIG_algorithm);
    LLSEC_SIG_algorithm* algorithm = &available_algorithms[0];

    while (--nb_algorithms >= 0) {
        if (strcmp((char*)algorithm_name, algorithm->name) == 0) {
            strncpy((char*)digest_algorithm_name, algorithm->digest_name, digest_algorithm_name_length);
            /* strncpy result may not be null-terminated. */
            digest_algorithm_name[digest_algorithm_name_length - 1] = '\0';
            return (int32_t)algorithm;
        }
        algorithm++;
    }

    /* Algorithm not found. */
    return -1;
}

void LLSEC_SIG_IMPL_get_algorithm_oid(uint8_t* algorithm_name, uint8_t* oid, int32_t oid_length)
{
    LLSEC_SIG_DEBUG_TRACE("%s \n", __func__);

    int32_t nb_algorithms = sizeof(available_algorithms) / sizeof(LLSEC_SIG_algorithm);
    LLSEC_SIG_algorithm* algorithm = &available_algorithms[0];

    while (--nb_algorithms >= 0) {
        if (strcmp((char*)algorithm_name, algorithm->name) == 0) {
            int32_t length = strlen(algorithm->oid);
            if (length + 1 > oid_length) {
                SNI_throwNativeException(-1, "Native oid length is bigger that the output byte array");
            }
            strncpy((char*)oid, algorithm->oid, length);
            /* strncpy result may not be null-terminated. */
            oid[length + 1] = '\0';
            return;
        }
        algorithm++;
    }
    /* Algorithm not found. */
    SNI_throwNativeException(-1, "Algorithm not found");
}

/**
 * @brief Verifies a message.
 *
 * @param[in] algorithm_id					The algorithm ID.
 * @param[in] signature						The buffer containing the signature.
 * @param[in] signature_length				The signature length.
 * @param[in] key							The public key.
 * @param[in] key_length					The key length.
 * @param[in] digest						The digest of the message to verify.
 * @param[in] digest_length					The digest length.
 *
 * @return JTRUE if the signature is valid, JFALSE otherwise.
 *
 * @note Throws NativeException on error.
 *
 * @warning <code>signature</code> must not be used outside of the VM task or saved.
 * @warning <code>key</code> must not be used outside of the VM task or saved.
 * @warning <code>digest</code> must not be used outside of the VM task or saved.
 *
 */
uint8_t LLSEC_SIG_IMPL_verify(int32_t algorithm_id, uint8_t* signature, int32_t signature_length, int32_t public_key_id, uint8_t* digest, int32_t digest_length)
{
    LLSEC_SIG_DEBUG_TRACE("%s \n", __func__);

    LLSEC_SIG_algorithm* algorithm = (LLSEC_SIG_algorithm*)algorithm_id;
    int returnCode = algorithm->verify(algorithm, signature, signature_length, (LLSEC_pub_key*)public_key_id, digest, digest_length);

    if (returnCode == LLSEC_SIG_SUCCESS) {
        return JTRUE;
    }
    SNI_throwNativeException(returnCode, "LLSEC_SIG_IMPL_verify failed");
    return JFALSE;
}

/**
 * @brief Signs a message.
 *
 * @param[in] algorithm_id					The algorithm ID.
 * @param[out] signature					The buffer containing the signature.
 * @param[in] signature_length				The signature length.
 * @param[in] key							The private key.
 * @param[in] key_length					The key length.
 * @param[in] digest						The digest of the message to sign.
 * @param[in] digest_length					The digest length.
 *
 * @return The length of the signature.
 *
 * @note Throws NativeException on error.
 *
 * @warning <code>signature</code> must not be used outside of the VM task or saved.
 * @warning <code>key</code> must not be used outside of the VM task or saved.
 * @warning <code>digest</code> must not be used outside of the VM task or saved.
 *
 */
int32_t LLSEC_SIG_IMPL_sign(int32_t algorithm_id, uint8_t* signature, int32_t signature_length, int32_t private_key_id, uint8_t* digest, int32_t digest_length)
{
    LLSEC_SIG_DEBUG_TRACE("%s \n", __func__);

    LLSEC_SIG_algorithm* algorithm = (LLSEC_SIG_algorithm*)algorithm_id;
    int returnCode = algorithm->sign(algorithm, signature, &signature_length, (LLSEC_priv_key*)private_key_id, digest, digest_length);

    if (returnCode != LLSEC_SIG_SUCCESS) {
        SNI_throwNativeException(returnCode, "LLSEC_SIG_IMPL_sign failed");
        return JFALSE;
    }

    return signature_length;
}
