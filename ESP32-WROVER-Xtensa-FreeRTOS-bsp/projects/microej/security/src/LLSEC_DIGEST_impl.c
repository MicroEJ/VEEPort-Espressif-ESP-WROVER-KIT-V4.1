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

#include <LLSEC_DIGEST_impl.h>
#include <LLSEC_ERRORS.h>
#include <sni.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mbedtls/platform.h"
#include "mbedtls/md.h"

#define LLSEC_DIGEST_SUCCESS 1
#define LLSEC_DIGEST_ERROR   0

#define SHA256_DIGEST_LENGTH 32
#define SHA512_DIGEST_LENGTH 64

//#define LLSEC_DIGEST_DEBUG

#ifdef LLSEC_DIGEST_DEBUG
#define LLSEC_DIGEST_DEBUG_TRACE(...) printf(__VA_ARGS__)
#else
#define LLSEC_DIGEST_DEBUG_TRACE(...) ((void)0)
#endif

typedef int (*LLSEC_DIGEST_init)(void** native_id);
typedef int (*LLSEC_DIGEST_update)(void* native_id, uint8_t* buffer, int32_t buffer_length);
typedef int (*LLSEC_DIGEST_digest)(void* native_id, uint8_t* out, int32_t* out_length);
typedef void (*LLSEC_DIGEST_close)(void* native_id);

/*
 * LL-API related functions & struct
 */
typedef struct {
    char* name;
    LLSEC_DIGEST_init init;
    LLSEC_DIGEST_update update;
    LLSEC_DIGEST_digest digest;
    LLSEC_DIGEST_close close;
    LLSEC_DIGEST_algorithm_desc description;
} LLSEC_DIGEST_algorithm;

static int mbedtls_digest_update(void* native_id, uint8_t* buffer, int32_t buffer_length);
static int mbedtls_digest_digest(void* native_id, uint8_t* out, int32_t* out_length);
static int LLSEC_DIGEST_SHA256_init(void** native_id);
static int LLSEC_DIGEST_SHA512_init(void** native_id);
static void mbedtls_digest_close(void* native_id);

static LLSEC_DIGEST_algorithm available_algorithms[] = {
    {
        .name   = "SHA-256",
        .init   = LLSEC_DIGEST_SHA256_init,
        .update = mbedtls_digest_update,
        .digest = mbedtls_digest_digest,
        .close  = mbedtls_digest_close,
        {
            .digest_length = SHA256_DIGEST_LENGTH
        }
    },
    {
        .name   = "SHA-512",
        .init   = LLSEC_DIGEST_SHA512_init,
        .update = mbedtls_digest_update,
        .digest = mbedtls_digest_digest,
        .close  = mbedtls_digest_close,
        {
            .digest_length = SHA512_DIGEST_LENGTH
        }
    }
};

/*
 * Generic mbedtls function
 */
static int mbedtls_digest_update(void* native_id, uint8_t* buffer, int32_t buffer_length)
{
    LLSEC_DIGEST_DEBUG_TRACE("%s \n", __func__);

    mbedtls_md_context_t* md_ctx = (mbedtls_md_context_t*)native_id;
    int rc = mbedtls_md_update(md_ctx, buffer, buffer_length);
    if (rc != 0) {
        return rc;
    }

    return LLSEC_DIGEST_SUCCESS;
}

static int mbedtls_digest_digest(void* native_id, uint8_t* out, int32_t* out_length)
{
    LLSEC_DIGEST_DEBUG_TRACE("%s \n", __func__);

    mbedtls_md_context_t* md_ctx = (mbedtls_md_context_t*)native_id;
    int rc = mbedtls_md_finish(md_ctx, out);

    if (rc != 0) {
        return rc;
    }

    *out_length = strlen((char*)out);
    return LLSEC_DIGEST_SUCCESS;
}

static void mbedtls_digest_close(void* native_id)
{
    LLSEC_DIGEST_DEBUG_TRACE("%s \n", __func__);

    mbedtls_md_context_t* md_ctx = (mbedtls_md_context_t*)native_id;

    /* Memory deallocation */
    mbedtls_md_free(md_ctx);
}

/*
 * Specific sha-256 function
 */
static int LLSEC_DIGEST_SHA256_init(void** native_id)
{
    LLSEC_DIGEST_DEBUG_TRACE("%s \n", __func__);

    mbedtls_md_context_t* md_ctx = mbedtls_calloc(1, sizeof(mbedtls_md_context_t));
    if (md_ctx == NULL) {
        return LLSEC_DIGEST_ERROR;
    }

    mbedtls_md_init(md_ctx);

    int rc = mbedtls_md_setup(md_ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 0);
    if (rc != 0) {
        mbedtls_md_free(md_ctx);
        return rc;
    }

    rc = mbedtls_md_starts(md_ctx);
    if (rc != 0) {
        mbedtls_md_free(md_ctx);
        return rc;
    } else {
        *native_id = md_ctx;
        return LLSEC_DIGEST_SUCCESS;
    }
}

/*
 * Specific sha-512 function
 */
static int LLSEC_DIGEST_SHA512_init(void** native_id)
{
    LLSEC_DIGEST_DEBUG_TRACE("%s \n", __func__);

    mbedtls_md_context_t* md_ctx = mbedtls_calloc(1, sizeof(mbedtls_md_context_t));
    if (md_ctx == NULL) {
        return LLSEC_DIGEST_ERROR;
    }

    mbedtls_md_init(md_ctx);

    int rc = mbedtls_md_setup(md_ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA512), 0);
    if (rc != 0) {
        mbedtls_md_free(md_ctx);
        return rc;
    }

    rc = mbedtls_md_starts(md_ctx);
    if (rc != 0) {
        mbedtls_md_free(md_ctx);
        return rc;
    } else {
        *native_id = md_ctx;
        return LLSEC_DIGEST_SUCCESS;
    }
}

/**
 * @brief Gets for the given algorithm the message digest description.
 *
 * @param[in] algorithm_name			Null terminated string that describes the algorithm.
 * @param[out] algorithm_desc			Description of the digest.
 *
 * @return The algorithm ID on success or -1 on error.
 *
 * @warning <code>algorithm_name</code> must not be used outside of the VM task or saved.
 */
int32_t LLSEC_DIGEST_IMPL_get_algorithm_description(uint8_t* algorithm_name, LLSEC_DIGEST_algorithm_desc* algorithm_desc)
{
    LLSEC_DIGEST_DEBUG_TRACE("%s \n", __func__);
    int32_t nb_algorithms = sizeof(available_algorithms) / sizeof(LLSEC_DIGEST_algorithm);
    LLSEC_DIGEST_algorithm* algorithm = &available_algorithms[0];

    while (--nb_algorithms >= 0) {
        if (strcmp((char*)algorithm_name, (algorithm->name)) == 0) {
            memcpy(algorithm_desc, &(algorithm->description), sizeof(LLSEC_DIGEST_algorithm_desc));
            return (int32_t)algorithm;
        }
        algorithm++;
    }

    // Algorithm not found.
    return -1;
}

/**
 * @brief Initializes a Digest resource.
 *
 * @param[in] algorithm_id				The algorithm ID.
 *
 * @return The nativeId of the newly initialized resource.
 *
 * @note Throws NativeException on error.
 */
int32_t LLSEC_DIGEST_IMPL_init(int32_t algorithm_id)
{
    LLSEC_DIGEST_DEBUG_TRACE("%s \n", __func__);
    void* native_id = NULL;
    LLSEC_DIGEST_algorithm* algorithm = (LLSEC_DIGEST_algorithm*)algorithm_id;

    int returnCode = algorithm->init((void**)&native_id);

    if (returnCode != LLSEC_DIGEST_SUCCESS) {
        SNI_throwNativeException(returnCode, "LLSEC_DIGEST_IMPL_init failed");
        return -1;
    }

    /* register SNI native resource */
    if (SNI_registerResource(native_id, algorithm->close, NULL) != SNI_OK) {
        SNI_throwNativeException(-1, "Can't register SNI native resource");
        algorithm->close((void*)native_id);
        return -1;
    }

    return (int32_t)native_id;
}

/**
 * @brief Closes the resource related to the native ID.
 *
 * @param[in] algorithm_id				The algorithm ID.
 * @param[in] native_id					The resource's native ID.
 *
 * @note Throws NativeException on error.
 */
void LLSEC_DIGEST_IMPL_close(int32_t algorithm_id, int32_t native_id)
{
    LLSEC_DIGEST_DEBUG_TRACE("%s \n", __func__);
    LLSEC_DIGEST_algorithm* algorithm = (LLSEC_DIGEST_algorithm*)algorithm_id;

    algorithm->close((void*)native_id);

    if (SNI_unregisterResource((void*)native_id, (SNI_closeFunction)algorithm->close) != SNI_OK) {
        SNI_throwNativeException(-1, "Can't unregister SNI native resource");
    }
}

/**
 * @brief Updates the input data.
 *
 * @param[in] algorithm_id				The algorithm ID.
 * @param[in] native_id					The resource's native ID.
 * @param[in] buffer					The buffer containing the input data to add.
 * @param[in] buffer_offset				The buffer offset.
 * @param[in] buffer_length				The buffer length.
 *
 * @note Throws NativeException on error.
 *
 * @warning <code>buffer</code> must not be used outside of the VM task or saved.
 */
void LLSEC_DIGEST_IMPL_update(int32_t algorithm_id, int32_t native_id, uint8_t* buffer, int32_t buffer_offset, int32_t buffer_length)
{
    LLSEC_DIGEST_DEBUG_TRACE("%s \n", __func__);
    LLSEC_DIGEST_algorithm* algorithm = (LLSEC_DIGEST_algorithm*)algorithm_id;
    int returnCode = algorithm->update((void*)native_id, buffer + buffer_offset, buffer_length);

    if (returnCode != LLSEC_DIGEST_SUCCESS) {
        SNI_throwNativeException(returnCode, "LLSEC_DIGEST_IMPL_update failed");
    }
}

/**
 * @brief Calculates the hash.
 *
 * @param[in] algorithm_id				The algorithm ID.
 * @param[in] native_id					The resource's native ID.
 * @param[out] out						The output buffer containing the digest value.
 * @param[in] out_offset				The output offset.
 * @param[in] out_length				The output length.
 *
 * @note Throws NativeException on error.
 *
 * @warning <code>out</code> must not be used outside of the VM task or saved.
 */
void LLSEC_DIGEST_IMPL_digest(int32_t algorithm_id, int32_t native_id, uint8_t* out, int32_t out_offset, int32_t out_length)
{
    LLSEC_DIGEST_DEBUG_TRACE("%s \n", __func__);
    LLSEC_DIGEST_algorithm* algorithm = (LLSEC_DIGEST_algorithm*)algorithm_id;
    int returnCode = algorithm->digest((void*)native_id, out + out_offset, &out_length);

    if (returnCode != LLSEC_DIGEST_SUCCESS) {
        SNI_throwNativeException(returnCode, "LLSEC_DIGEST_IMPL_digest failed");
    }
}

/**
 * @brief Gets the id of the native close function.
 * @param[in] algorithm_id				The algorithm ID.
 *
 * @return the id of the static native close function.
 *
 * @note Throws NativeException on error.
 */
int32_t LLSEC_DIGEST_IMPL_get_close_id(int32_t algorithm_id)
{
    LLSEC_DIGEST_DEBUG_TRACE("%s \n", __func__);
    LLSEC_DIGEST_algorithm* algorithm = (LLSEC_DIGEST_algorithm*)algorithm_id;
    return (int32_t)algorithm->close;
}
