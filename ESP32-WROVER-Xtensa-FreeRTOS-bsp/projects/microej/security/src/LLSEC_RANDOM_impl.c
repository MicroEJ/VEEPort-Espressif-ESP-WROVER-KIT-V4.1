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
#include <LLSEC_RANDOM_impl.h>
#include <LLSEC_mbedtls.h>

#include <sni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mbedtls/platform.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"

//#define LLSEC_RANDOM_DEBUG

#ifdef LLSEC_RANDOM_DEBUG
#define LLSEC_RANDOM_DEBUG_TRACE(...) printf(__VA_ARGS__)
#else
#define LLSEC_RANDOM_DEBUG_TRACE(...) ((void)0)
#endif

static mbedtls_entropy_context entropy;
static mbedtls_ctr_drbg_context ctr_drbg;
static int initialized = 0;
static int32_t native_ids = 1;

/**
 * @brief Initializes a Random resource.
 *
 * @return The native ID of the resource.
 *
 * @note Throws NativeException on error.
 */
int32_t LLSEC_RANDOM_IMPL_init()
{
    LLSEC_RANDOM_DEBUG_TRACE("%s\n", __func__);
    const char* pers = llsec_gen_random_str_internal(8);

    if (!initialized) {
        int32_t ret;
        mbedtls_ctr_drbg_init(&ctr_drbg);
        mbedtls_entropy_init(&entropy);
        ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                          (const uint8_t*)pers, 0);
        if (ret != 0) {
            SNI_throwNativeException(ret, "mbedtls_ctr_drbg_seed failed");
            LLSEC_RANDOM_IMPL_close(0);
            mbedtls_free((void*)pers);
            return -1;
        }
        initialized = 1;
    }

    int32_t native_id = native_ids++;
    if (SNI_registerResource((void*)native_id, (SNI_closeFunction)LLSEC_RANDOM_IMPL_close, NULL) != SNI_OK) {
        SNI_throwNativeException(-1, "Can't register SNI native resource");
        LLSEC_RANDOM_IMPL_close(native_id);
        mbedtls_free((void*)pers);
        return -1;
    }

    mbedtls_free((void*)pers);
    return native_id;
}

/**
 * @brief Closes the resource related to the nativeId.
 *
 * @param[in] native_id					The native ID.
 *
 * @note Throws NativeException on error.
 */
void LLSEC_RANDOM_IMPL_close(int32_t native_id)
{
    LLSEC_RANDOM_DEBUG_TRACE("%s native_id:%d\n", __func__, native_id);
    /*To suport multi-instance, do not free the ctr_drbg & entropy here!*/
    //mbedtls_ctr_drbg_free(&ctr_drbg);
    //mbedtls_entropy_free(&entropy);
}

/**
 * @brief Generates random bytes.
 *
 * @param[in] native_id					The native ID.
 * @param[out] rnd						The buffer to fill with random bytes.
 * @param[in] size						The size of rnd.
 *
 * @note Throws NativeException on error.
 */
void LLSEC_RANDOM_IMPL_next_bytes(int32_t native_id, uint8_t* rnd, int32_t size)
{
    LLSEC_RANDOM_DEBUG_TRACE("%s rdn:0x%X, %d\n", __func__, (uint32_t)rnd, size);
    uint32_t bytes_left = size;
    int32_t result;
    while (bytes_left > 0) {
        if (bytes_left > MBEDTLS_CTR_DRBG_MAX_REQUEST) {
            result = mbedtls_ctr_drbg_random(&ctr_drbg, rnd, MBEDTLS_CTR_DRBG_MAX_REQUEST);
            bytes_left -= MBEDTLS_CTR_DRBG_MAX_REQUEST;
            rnd += MBEDTLS_CTR_DRBG_MAX_REQUEST;
        } else {
            result = mbedtls_ctr_drbg_random(&ctr_drbg, rnd, bytes_left);
            bytes_left = 0;
        }
        if (0 != result) {
            SNI_throwNativeException(result, "mbedtls_ctr_drbg_random failed");
        }
    }
}

/**
 * @brief Sets the seed of the PRNG.
 *
 * @param[in] native_id					The native ID.
 * @param[in] seed						The array of bytes used as a seed.
 * @param[in] size						The size of seed.
 *
 * @note Throws NativeException on error.
 */
void LLSEC_RANDOM_IMPL_set_seed(int32_t native_id, uint8_t* seed, int32_t size)
{
    LLSEC_RANDOM_DEBUG_TRACE("%s\n", __func__);
    LLSEC_RANDOM_DEBUG_TRACE("LLSEC_RANDOM_IMPL_set_seed, Seeding the random number generator\n");
    int32_t ret;
    ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                     (const unsigned char*)seed, size);
    if (ret != 0) {
        SNI_throwNativeException(ret, "mbedtls_ctr_drbg_seed failed");
    }
}

/**
 * @brief Generates a new seed.
 *
 * @param[in] native_id					The native ID.
 * @param[out] seed						The array to fill with the seed.
 * @param[in] size						The size of seed.
 *
 * @note Throws NativeException on error.
 */
void LLSEC_RANDOM_IMPL_generate_seed(int32_t native_id, uint8_t* seed, int32_t size)
{
    LLSEC_RANDOM_DEBUG_TRACE("%s\n", __func__);
    LLSEC_RANDOM_IMPL_next_bytes(native_id, seed, size);
}

/**
 * @brief Gets the id of the native close function.
 *
 * @return the id of the static native close function.
 *
 * @note Throws NativeException on error.
 */
int32_t LLSEC_RANDOM_IMPL_get_close_id()
{
    LLSEC_RANDOM_DEBUG_TRACE("%s\n", __func__);
    return (int32_t) LLSEC_RANDOM_IMPL_close;
}

/**
 * @brief generate random string function.
 * @param[in] length the string length
 *
 * @return pointer of random string.
 *
 * @note This function should only be used to provide a personalisation string
 * 		 when calling mbedtls_ctr_drbg_seed.
 */
char* llsec_gen_random_str_internal(int length)
{
    int flag;
    int idx;
    char* strRan;
    strRan = (char*)mbedtls_calloc(1, length);
    if (NULL == strRan) {
        LLSEC_RANDOM_DEBUG_TRACE("Random string malloc failed");
        return NULL;
    }

    srand((unsigned)time(NULL));

    for (idx = 0; idx < length - 1; idx++) {
        flag = rand() % 3;
        switch (flag) {
            case 0:
                strRan[idx] = 'A' + rand() % 26;
                break;
            case 1:
                strRan[idx] = 'a' + rand() % 26;
                break;
            case 2:
                strRan[idx] = '0' + rand() % 10;
                break;
            default:
                strRan[idx] = 'x';
                break;
        }
    }
    strRan[length - 1] = '\0';
    return strRan;
}
