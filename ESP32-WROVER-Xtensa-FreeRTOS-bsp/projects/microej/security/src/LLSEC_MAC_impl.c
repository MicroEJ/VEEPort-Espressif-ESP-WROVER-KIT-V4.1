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
#include <LLSEC_MAC_impl.h>
#include <LLSEC_configuration.h>
#include <sni.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mbedtls/platform.h"
#include "mbedtls/md.h"

#define LLSEC_MAC_SUCCESS 1
#define LLSEC_MAC_ERROR   0

//#define LLSEC_MAC_DEBUG

#ifdef LLSEC_MAC_DEBUG
#define LLSEC_MAC_DEBUG_TRACE(...) printf(__VA_ARGS__)
#else
#define LLSEC_MAC_DEBUG_TRACE(...) ((void)0)
#endif

typedef int (*LLSEC_MAC_init)(void** native_id, uint8_t* key, int32_t key_length);
typedef int (*LLSEC_MAC_update)(void* native_id, uint8_t* buffer, int32_t buffer_length);
typedef int (*LLSEC_MAC_do_final)(void* native_id, uint8_t* out, int32_t out_length);
typedef int (*LLSEC_MAC_reset)(void* native_id);
typedef void (*LLSEC_MAC_close)(void* native_id);

typedef struct {
    char* name;
    LLSEC_MAC_init init;
    LLSEC_MAC_update update;
    LLSEC_MAC_do_final do_final;
    LLSEC_MAC_reset reset;
    LLSEC_MAC_close close;
    LLSEC_MAC_algorithm_desc description;
} LLSEC_MAC_algorithm;

static int mbedtls_mac_HmacSha256_init(void** native_id, uint8_t* key, int32_t key_length);
static int mbedtls_mac_update(void* native_id, uint8_t* buffer, int32_t buffer_length);
static int mbedtls_mac_do_final(void* native_id, uint8_t* out, int32_t out_length);
static int mbedtls_mac_reset(void* native_id);
static void mbedtls_mac_close(void* native_id);

static LLSEC_MAC_algorithm available_algorithms[] = {

    {
        .name = "HmacSHA256",
        .init = mbedtls_mac_HmacSha256_init,
        .update = mbedtls_mac_update,
        .do_final = mbedtls_mac_do_final,
        .reset = mbedtls_mac_reset,
        .close = mbedtls_mac_close,
        {
            .mac_length = 32
        }
    }
};

static int mbedtls_mac_HmacSha256_init(void** native_id, uint8_t* key, int32_t key_length)
{
    LLSEC_MAC_DEBUG_TRACE("%s \n", __func__);
    int rc;
    mbedtls_md_context_t* md_ctx = LLSEC_calloc(1, sizeof(mbedtls_md_context_t));
    if (md_ctx == NULL) {
        return LLSEC_MAC_ERROR;
    }
    mbedtls_md_init(md_ctx);
    rc = mbedtls_md_setup(md_ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 1);
    if (rc != 0) {
        LLSEC_free(md_ctx);
        return LLSEC_MAC_ERROR;
    }
    rc = mbedtls_md_hmac_starts(md_ctx, key, key_length);
    if (rc != 0) {
        LLSEC_free(md_ctx);
        return LLSEC_MAC_ERROR;
    }
    *native_id = md_ctx;
    return LLSEC_MAC_SUCCESS;
}

static int mbedtls_mac_update(void* native_id, uint8_t* buffer, int32_t buffer_length)
{
    LLSEC_MAC_DEBUG_TRACE("%s \n", __func__);
    mbedtls_md_context_t* md_ctx = (mbedtls_md_context_t*)native_id;
    if (0 != mbedtls_md_hmac_update(md_ctx, buffer, buffer_length)) {
        return LLSEC_MAC_ERROR;
    }
    return LLSEC_MAC_SUCCESS;
}

static int mbedtls_mac_do_final(void* native_id, uint8_t* out, int32_t out_length)
{
    LLSEC_MAC_DEBUG_TRACE("%s \n", __func__);
    mbedtls_md_context_t* md_ctx = (mbedtls_md_context_t*)native_id;
    if (0 != mbedtls_md_hmac_finish(md_ctx, out)) {
        return LLSEC_MAC_ERROR;
    }
    return LLSEC_MAC_SUCCESS;
}

static int mbedtls_mac_reset(void* native_id)
{
    LLSEC_MAC_DEBUG_TRACE("%s \n", __func__);
    mbedtls_md_context_t* md_ctx = (mbedtls_md_context_t*)native_id;
    if (0 != mbedtls_md_hmac_reset(md_ctx)) {
        return LLSEC_MAC_ERROR;
    }
    return LLSEC_MAC_SUCCESS;
}

static void mbedtls_mac_close(void* native_id)
{
    LLSEC_MAC_DEBUG_TRACE("%s native_id:%x\n", __func__, (uint32_t)native_id);
    LLSEC_free(native_id);
}

/**
 * @brief Gets for the given algorithm the message digest description.
 *
 * @param[in] algorithm_name			Null terminated string that describes the algorithm.
 * @param[out] algorithm_desc			Description of the MAC algorithm.
 *
 * @return The algorithm ID on success or -1 on error.
 *
 * @warning <code>algorithm_name</code> must not be used outside of the VM task or saved.
 */
int32_t LLSEC_MAC_IMPL_get_algorithm_description(uint8_t* algorithm_name, LLSEC_MAC_algorithm_desc* algorithm_desc)
{
    LLSEC_MAC_DEBUG_TRACE("%s \n", __func__);

    int32_t nb_algorithms = sizeof(available_algorithms) / sizeof(LLSEC_MAC_algorithm);
    LLSEC_MAC_algorithm* algorithm = &available_algorithms[0];

    while (--nb_algorithms >= 0) {
        if (strcmp((const char*)algorithm_name, algorithm->name) == 0) {
            memcpy(algorithm_desc, &(algorithm->description), sizeof(LLSEC_MAC_algorithm_desc));
            return (int32_t)algorithm;
        }
        algorithm++;
    }

    // Algorithm not found.
    return -1;
}

/**
 * @brief Initializes a Mac resource.
 *
 * @param[in] algorithm_id				The algorithm ID.
 * @param[in] key						The MAC key.
 * @param[in] key_length				The key length.
 *
 * @return The native ID of the resource.
 *
 * @note Throws NativeException on error.
 *
 * @warning <code>key</code> must not be used outside of the VM task or saved.
 */
int32_t LLSEC_MAC_IMPL_init(int32_t algorithm_id, uint8_t* key, int32_t key_length)
{

    LLSEC_MAC_DEBUG_TRACE("%s \n", __func__);

    void* native_id = NULL;
    LLSEC_MAC_algorithm* algorithm = (LLSEC_MAC_algorithm*)algorithm_id;

    int returnCode = algorithm->init(&native_id, key, key_length);

    if (returnCode != LLSEC_MAC_SUCCESS) {
        SNI_throwNativeException(returnCode, "LLSEC_MAC_IMPL_init failed\n");
        return -1;
    }

    // register SNI native resource
    if (SNI_registerResource(native_id, algorithm->close, NULL) != SNI_OK) {
        SNI_throwNativeException(-1, "Can't register SNI native resource");
        algorithm->close((void*)native_id);
        return -1;
    }

    return (int32_t)native_id;
}

/**
 * @brief Processes the provided data to update the MAC resource.
 *
 * @param[in] algorithm_id				The algorithm ID.
 * @param[in] native_id					The native ID.
 * @param[in] buffer					The buffer containing the data to be processed.
 * @param[in] buffer_offset				The buffer offset.
 * @param[in] buffer_length				The buffer length.
 *
 * @note Throws NativeException on error.
 *
 * @warning <code>buffer</code> must not be used outside of the VM task or saved.
 */
void LLSEC_MAC_IMPL_update(int32_t algorithm_id, int32_t native_id, uint8_t* buffer, int32_t buffer_offset, int32_t buffer_length)
{
    LLSEC_MAC_DEBUG_TRACE("%s \n", __func__);

    LLSEC_MAC_algorithm* algorithm = (LLSEC_MAC_algorithm*)algorithm_id;

    int returnCode = algorithm->update((void*)native_id, buffer + buffer_offset, buffer_length);

    if (returnCode != LLSEC_MAC_SUCCESS) {
        SNI_throwNativeException(returnCode, "LLSEC_MAC_IMPL_update failed");
    }
}

/**
 * @brief Finishes the MAC operation.
 *
 * @param[in] algorithm_id				The algorithm ID.
 * @param[in] native_id					The native ID.
 * @param[out] out						The MAC result.
 * @param[in] out_offset				The offset of the out buffer.
 * @param[in] out_length				The length of the out buffer.
 *
 * @note Throws NativeException on error.
 *
 * @warning <code>out</code> must not be used outside of the VM task or saved.
 */
void LLSEC_MAC_IMPL_do_final(int32_t algorithm_id, int32_t native_id, uint8_t* out, int32_t out_offset, int32_t out_length)
{
    LLSEC_MAC_DEBUG_TRACE("%s \n", __func__);

    LLSEC_MAC_algorithm* algorithm = (LLSEC_MAC_algorithm*)algorithm_id;

    int returnCode = algorithm->do_final((void*)native_id, out + out_offset, out_length);

    if (returnCode != LLSEC_MAC_SUCCESS) {
        SNI_throwNativeException(returnCode, "LLSEC_MAC_IMPL_do_final failed");
    }
}

/**
 * @brief Resets the MAC resource.
 *
 * @param[in] algorithm_id				The algorithm ID.
 * @param[in] native_id					The native ID.
 *
 * @note Throws NativeException on error.
 */
void LLSEC_MAC_IMPL_reset(int32_t algorithm_id, int32_t native_id)
{
    LLSEC_MAC_DEBUG_TRACE("%s \n", __func__);

    LLSEC_MAC_algorithm* algorithm = (LLSEC_MAC_algorithm*)algorithm_id;

    int returnCode = algorithm->reset((void*)native_id);

    if (returnCode != LLSEC_MAC_SUCCESS) {
        SNI_throwNativeException(returnCode, "LLSEC_MAC_IMPL_reset failed");
    }
}

/**
 * @brief Closes the resources related to the native id.
 *
 * @param[in] algorithm_id				The algorithm ID.
 * @param[in] native_id					The native ID.
 *
 * @note Throws NativeException on error.
 */
void LLSEC_MAC_IMPL_close(int32_t algorithm_id, int32_t native_id)
{
    LLSEC_MAC_DEBUG_TRACE("%s \n", __func__);

    LLSEC_MAC_algorithm* algorithm = (LLSEC_MAC_algorithm*)algorithm_id;

    algorithm->close((void*)native_id);
    if (SNI_unregisterResource((void*)native_id, (SNI_closeFunction)algorithm->close) != SNI_OK) {
        SNI_throwNativeException(-1, "Can't unregister SNI native resource\n");
    }
}
/**
 * @brief Gets the id of the native close function.
 *
 * @param[in] algorithm_id				The algorithm ID.
 *
 * @return the id of the static native close function.
 *
 * @note Throws NativeException on error.
 */
int32_t LLSEC_MAC_IMPL_get_close_id(int32_t algorithm_id)
{
    LLSEC_MAC_DEBUG_TRACE("%s \n", __func__);

    LLSEC_MAC_algorithm* algorithm = (LLSEC_MAC_algorithm*)algorithm_id;
    return (int32_t)algorithm->close;
}
