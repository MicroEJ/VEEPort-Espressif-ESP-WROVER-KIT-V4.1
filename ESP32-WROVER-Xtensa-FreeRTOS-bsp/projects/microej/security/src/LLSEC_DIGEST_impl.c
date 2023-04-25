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

#include <LLSEC_DIGEST_impl.h>
#include <LLSEC_ERRORS.h>
#include <sni.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "mbedtls/platform.h"
#include "mbedtls/md.h"

#define LLSEC_DIGEST_SUCCESS 0
#define LLSEC_DIGEST_ERROR   -1

#define MD5_DIGEST_LENGTH    16
#define SHA1_DIGEST_LENGTH   20
#define SHA256_DIGEST_LENGTH 32
#define SHA512_DIGEST_LENGTH 64

//#define LLSEC_DIGEST_DEBUG

#ifdef LLSEC_DIGEST_DEBUG
// cppcheck-suppress misra-c2012-21.6 // Include only in debug
#include <stdio.h>
#define LLSEC_DIGEST_DEBUG_TRACE(...) (void)printf(__VA_ARGS__)
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
static int LLSEC_DIGEST_MD5_init(void** native_id);
static int LLSEC_DIGEST_SHA1_init(void** native_id);
static int LLSEC_DIGEST_SHA256_init(void** native_id);
static int LLSEC_DIGEST_SHA512_init(void** native_id);
static void mbedtls_digest_close(void* native_id);

// cppcheck-suppress misra-c2012-8.9 // Define here for code readability even if it called once in this file.
static LLSEC_DIGEST_algorithm available_digest_algorithms[4] = {
    {
        .name   = "MD5",
        .init   = LLSEC_DIGEST_MD5_init,
        .update = mbedtls_digest_update,
        .digest = mbedtls_digest_digest,
        .close  = mbedtls_digest_close,
        {
            .digest_length = MD5_DIGEST_LENGTH
        }
    },
    {
        .name   = "SHA-1",
        .init   = LLSEC_DIGEST_SHA1_init,
        .update = mbedtls_digest_update,
        .digest = mbedtls_digest_digest,
        .close  = mbedtls_digest_close,
        {
            .digest_length = SHA1_DIGEST_LENGTH
        }
    },
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
    return rc;
}

static int mbedtls_digest_digest(void* native_id, uint8_t* out, int32_t* out_length)
{
    LLSEC_DIGEST_DEBUG_TRACE("%s \n", __func__);

    mbedtls_md_context_t* md_ctx = (mbedtls_md_context_t*)native_id;
    int rc = mbedtls_md_finish(md_ctx, out);

    if (rc == 0) {
        *out_length = strlen((char*)out);
    }

    return rc;
}

static void mbedtls_digest_close(void* native_id)
{
    LLSEC_DIGEST_DEBUG_TRACE("%s \n", __func__);

    mbedtls_md_context_t* md_ctx = (mbedtls_md_context_t*)native_id;

    /* Memory deallocation */
    mbedtls_md_free(md_ctx);
}

/*
 * Specific md5 function
 */
static int LLSEC_DIGEST_MD5_init(void** native_id)
{
    int return_code = LLSEC_DIGEST_SUCCESS;
    LLSEC_DIGEST_DEBUG_TRACE("%s \n", __func__);

    mbedtls_md_context_t* md_ctx = mbedtls_calloc(1, sizeof(mbedtls_md_context_t));
    if (md_ctx == NULL) {
        return_code = LLSEC_DIGEST_ERROR;
    }

    if (return_code == LLSEC_DIGEST_SUCCESS){
        mbedtls_md_init(md_ctx);

        return_code = mbedtls_md_setup(md_ctx, mbedtls_md_info_from_type(MBEDTLS_MD_MD5), 0);
        if (return_code != LLSEC_DIGEST_SUCCESS) {
            mbedtls_md_free(md_ctx);
        }
    }

    if (return_code == LLSEC_DIGEST_SUCCESS){
        return_code = mbedtls_md_starts(md_ctx);
        if (return_code != LLSEC_DIGEST_SUCCESS) {
            mbedtls_md_free(md_ctx);
        } else {
            *native_id = md_ctx;
        }
    }
    return return_code;
}

/*
 * Specific sha-1 function
 */
static int LLSEC_DIGEST_SHA1_init(void** native_id)
{
    int return_code = LLSEC_DIGEST_SUCCESS;
    LLSEC_DIGEST_DEBUG_TRACE("%s \n", __func__);

    mbedtls_md_context_t* md_ctx = mbedtls_calloc(1, sizeof(mbedtls_md_context_t));
    if (md_ctx == NULL) {
        return_code = LLSEC_DIGEST_ERROR;
    }

    if (return_code == LLSEC_DIGEST_SUCCESS){
        mbedtls_md_init(md_ctx);

        return_code = mbedtls_md_setup(md_ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA1), 0);
        if (return_code != LLSEC_DIGEST_SUCCESS) {
            mbedtls_md_free(md_ctx);
        }
    }

    if (return_code == LLSEC_DIGEST_SUCCESS){
        return_code = mbedtls_md_starts(md_ctx);
        if (return_code != LLSEC_DIGEST_SUCCESS) {
            mbedtls_md_free(md_ctx);
        } else {
            *native_id = md_ctx;
        }
    }
    return return_code;
}

/*
 * Specific sha-256 function
 */
static int LLSEC_DIGEST_SHA256_init(void** native_id)
{
    int return_code = LLSEC_DIGEST_SUCCESS;
    LLSEC_DIGEST_DEBUG_TRACE("%s \n", __func__);

    mbedtls_md_context_t* md_ctx = mbedtls_calloc(1, sizeof(mbedtls_md_context_t));
    if (md_ctx == NULL) {
        return_code = LLSEC_DIGEST_ERROR;
    }

    if (return_code == LLSEC_DIGEST_SUCCESS){
        mbedtls_md_init(md_ctx);

        return_code = mbedtls_md_setup(md_ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 0);
        if (return_code != LLSEC_DIGEST_SUCCESS) {
            mbedtls_md_free(md_ctx);
        }
    }

    if (return_code == LLSEC_DIGEST_SUCCESS){
        return_code = mbedtls_md_starts(md_ctx);
        if (return_code != LLSEC_DIGEST_SUCCESS) {
            mbedtls_md_free(md_ctx);
        } else {
            *native_id = md_ctx;
        }
    }
    return return_code;
}

/*
 * Specific sha-512 function
 */
static int LLSEC_DIGEST_SHA512_init(void** native_id)
{
    int return_code = LLSEC_DIGEST_SUCCESS;
    LLSEC_DIGEST_DEBUG_TRACE("%s \n", __func__);

    mbedtls_md_context_t* md_ctx = mbedtls_calloc(1, sizeof(mbedtls_md_context_t));
    if (md_ctx == NULL) {
        return_code = LLSEC_DIGEST_ERROR;
    }

    if (return_code == LLSEC_DIGEST_SUCCESS){
        mbedtls_md_init(md_ctx);

        return_code = mbedtls_md_setup(md_ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA512), 0);
        if (return_code != LLSEC_DIGEST_SUCCESS) {
            mbedtls_md_free(md_ctx);
        }
    }

    if (return_code == LLSEC_DIGEST_SUCCESS){
        return_code = mbedtls_md_starts(md_ctx);
        if (return_code != LLSEC_DIGEST_SUCCESS) {
            mbedtls_md_free(md_ctx);
        } else {
            *native_id = md_ctx;
        }
    }
    return return_code;
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
    int32_t return_code = LLSEC_DIGEST_SUCCESS;
    LLSEC_DIGEST_DEBUG_TRACE("%s \n", __func__);
    int32_t nb_algorithms = sizeof(available_digest_algorithms) / sizeof(LLSEC_DIGEST_algorithm);
    LLSEC_DIGEST_algorithm* algorithm = &available_digest_algorithms[0];

    while (--nb_algorithms >= 0) {
        if (strcmp((char*)algorithm_name, (algorithm->name)) == 0) {
            (void) memcpy(algorithm_desc, &(algorithm->description), sizeof(LLSEC_DIGEST_algorithm_desc));
            break;
        }
        algorithm++;
    }

    if (nb_algorithms >= 0)
    {
        // cppcheck-suppress misra-c2012-11.4 // Abstract data type for SNI usage
        return_code = (int32_t)algorithm;
    } else {
        return_code = LLSEC_DIGEST_ERROR;
    }
    return return_code;
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
    int32_t return_code = 0;
    LLSEC_DIGEST_DEBUG_TRACE("%s \n", __func__);
    void* native_id = NULL;
    // cppcheck-suppress misra-c2012-11.4 // Abstract data type for SNI usage
    LLSEC_DIGEST_algorithm* algorithm = (LLSEC_DIGEST_algorithm*)algorithm_id;

    return_code = algorithm->init((void**)&native_id);

    if (return_code != LLSEC_DIGEST_SUCCESS) {
        SNI_throwNativeException(return_code, "LLSEC_DIGEST_IMPL_init failed");
    } else {
        /* register SNI native resource */
        if (SNI_registerResource(native_id, algorithm->close, NULL) != SNI_OK) {
            SNI_throwNativeException(LLSEC_DIGEST_ERROR, "Can't register SNI native resource");
            algorithm->close((void*)native_id);
            return_code = LLSEC_DIGEST_ERROR;
        } else {
            // cppcheck-suppress misra-c2012-11.6 // Abstract data type for SNI usage
            return_code = (int32_t)native_id;
        }
    }
    return return_code;
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
    // cppcheck-suppress misra-c2012-11.4 // Abstract data type for SNI usage
    LLSEC_DIGEST_algorithm* algorithm = (LLSEC_DIGEST_algorithm*)algorithm_id;

    // cppcheck-suppress misra-c2012-11.6 // Abstract data type for SNI usage
    algorithm->close((void*)native_id);

    // cppcheck-suppress misra-c2012-11.6 // Abstract data type for SNI usage
    // cppcheck-suppress misra-c2012-11.1 // Abstract data type for SNI usage
    if (SNI_unregisterResource((void*)native_id, (SNI_closeFunction)algorithm->close) != SNI_OK) {
        SNI_throwNativeException(LLSEC_DIGEST_ERROR, "Can't unregister SNI native resource");
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
    // cppcheck-suppress misra-c2012-11.4 // Abstract data type for SNI usage
    LLSEC_DIGEST_algorithm* algorithm = (LLSEC_DIGEST_algorithm*)algorithm_id;
    // cppcheck-suppress misra-c2012-11.6 // Abstract data type for SNI usage
    int returnCode = algorithm->update((void*)native_id, &buffer[buffer_offset], buffer_length);

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
    // cppcheck-suppress misra-c2012-11.4 // Abstract data type for SNI usage
    LLSEC_DIGEST_algorithm* algorithm = (LLSEC_DIGEST_algorithm*)algorithm_id;
    // cppcheck-suppress misra-c2012-11.6 // Abstract data type for SNI usage
    int returnCode = algorithm->digest((void*)native_id, &out[out_offset], &out_length);

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
    // cppcheck-suppress misra-c2012-11.4 // Abstract data type for SNI usage
    LLSEC_DIGEST_algorithm* algorithm = (LLSEC_DIGEST_algorithm*)algorithm_id;
    // cppcheck-suppress misra-c2012-11.1 // Abstract data type for SNI usage
    return (int32_t)algorithm->close;
}
