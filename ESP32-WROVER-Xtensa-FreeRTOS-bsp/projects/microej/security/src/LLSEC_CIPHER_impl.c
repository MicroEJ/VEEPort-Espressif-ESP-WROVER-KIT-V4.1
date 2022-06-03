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

#include <LLSEC_CIPHER_impl.h>
#include <LLSEC_ERRORS.h>
#include <LLSEC_configuration.h>
#include <sni.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 include mbedtls
*/
#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif
#include "mbedtls/aes.h"
#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"

#define LLSEC_CIPHER_SUCCESS 0
#define LLSEC_CIPHER_ERROR   -1
#define AES_CBC_BLOCK_BITS            128u
#define AES_CBC_BLOCK_BYTES           AES_CBC_BLOCK_BITS / 8u

//#define LLSEC_CIPHER_DEBUG

#ifdef LLSEC_CIPHER_DEBUG
#define LLSEC_CIPHER_DEBUG_TRACE(...) printf(__VA_ARGS__)
#else
#define LLSEC_CIPHER_DEBUG_TRACE(...) ((void)0)
#endif

/**
 * Cipher init function type
 */
typedef int (*LLSEC_CIPHER_init)(int32_t transformation_id, void** native_id, uint8_t is_decrypting, uint8_t* key, int32_t key_length, uint8_t* iv, int32_t iv_length);
typedef int (*LLSEC_CIPHER_decrypt)(void* native_id, uint8_t* buffer, int32_t buffer_length, uint8_t* output);
typedef int (*LLSEC_CIPHER_encrypt)(void* native_id, uint8_t* buffer, int32_t buffer_length, uint8_t* output);
typedef void (*LLSEC_CIPHER_close)(void* native_id);

typedef struct {
    char* name; // the name of the transformation
    LLSEC_CIPHER_init init;
    LLSEC_CIPHER_decrypt decrypt;
    LLSEC_CIPHER_encrypt encrypt;
    LLSEC_CIPHER_close close;
    LLSEC_CIPHER_transformation_desc description;
} LLSEC_CIPHER_transformation;

typedef struct {
    LLSEC_CIPHER_transformation* transformation;
    mbedtls_aes_context mbedtls_ctx;
    int32_t iv_length;
    uint8_t iv[1];
} LLSEC_CIPHER_ctx;

static int LLSEC_CIPHER_aescbc_init(int32_t transformation_id, void** native_id, uint8_t is_decrypting, uint8_t* key, int32_t key_length, uint8_t* iv, int32_t iv_length);
static int mbedtls_cipher_decrypt(void* native_id, uint8_t* buffer, int32_t buffer_length, uint8_t* output);
static int mbedtls_cipher_encrypt(void* native_id, uint8_t* buffer, int32_t buffer_length, uint8_t* output);
static void mbedtls_cipher_close(void* native_id);

static LLSEC_CIPHER_transformation available_transformations[] = {
    {
        .name = "AES/CBC/NoPadding",
        .init = LLSEC_CIPHER_aescbc_init,
        .decrypt = mbedtls_cipher_decrypt,
        .encrypt = mbedtls_cipher_encrypt,
        .close = mbedtls_cipher_close,
        {
            .block_size = AES_CBC_BLOCK_BYTES,
            .unit_bytes = AES_CBC_BLOCK_BYTES,
            .cipher_mode = CBC_MODE,
        },
    }
};

static int LLSEC_CIPHER_aescbc_init(int32_t transformation_id, void** native_id, uint8_t is_decrypting, uint8_t* key, int32_t key_length, uint8_t* iv, int32_t iv_length)
{
    LLSEC_CIPHER_ctx* p_cipher_ctx;
    LLSEC_CIPHER_DEBUG_TRACE("%s %d\n", __func__, is_decrypting);

    int return_code = 0;
    p_cipher_ctx = LLSEC_calloc(1, sizeof(LLSEC_CIPHER_ctx) - 1 + iv_length);
    p_cipher_ctx->transformation = (LLSEC_CIPHER_transformation*)transformation_id;
    mbedtls_aes_init(&p_cipher_ctx->mbedtls_ctx);

    if (is_decrypting) {
        return_code = mbedtls_aes_setkey_dec(&p_cipher_ctx->mbedtls_ctx, key, key_length * 8);
    } else {
        return_code = mbedtls_aes_setkey_enc(&p_cipher_ctx->mbedtls_ctx, key, key_length * 8);
    }

    if(0 != return_code) {
        return  LLSEC_CIPHER_ERROR;
    }

    p_cipher_ctx->iv_length = iv_length;
    memcpy(p_cipher_ctx->iv, iv, iv_length);

    *native_id = p_cipher_ctx;
    return LLSEC_CIPHER_SUCCESS;
}

static int mbedtls_cipher_decrypt(void* native_id, uint8_t* buffer, int32_t buffer_length, uint8_t* output)
{
    int ret = 0;
    LLSEC_CIPHER_ctx* p_cipher_ctx = (LLSEC_CIPHER_ctx*)native_id;
    LLSEC_CIPHER_DEBUG_TRACE("%s \n", __func__);
    ret = mbedtls_aes_crypt_cbc(&p_cipher_ctx->mbedtls_ctx, MBEDTLS_AES_DECRYPT, buffer_length,
                                p_cipher_ctx->iv, buffer, output);
    if (ret == 0) {
        return LLSEC_CIPHER_SUCCESS;
    } else {
        return ret;
    }
}

static int mbedtls_cipher_encrypt(void* native_id, uint8_t* buffer, int32_t buffer_length, uint8_t* output)
{
    int ret = 0;
    LLSEC_CIPHER_ctx* p_cipher_ctx = (LLSEC_CIPHER_ctx*)native_id;
    LLSEC_CIPHER_DEBUG_TRACE("%s \n", __func__);
    ret = mbedtls_aes_crypt_cbc(&p_cipher_ctx->mbedtls_ctx, MBEDTLS_AES_ENCRYPT, buffer_length,
                                p_cipher_ctx->iv, buffer, output);
    if (ret == 0) {
        return LLSEC_CIPHER_SUCCESS;
    } else {
        return ret;
    }
}

static void mbedtls_cipher_close(void* native_id)
{
    LLSEC_CIPHER_DEBUG_TRACE("%s native_id:%x\n", __func__, (uint32_t)native_id);
    LLSEC_free(native_id);
}

/**
 * @brief Gets for the given transformation the cipher description.
 * <p>
 * <code>transformation_desc</code> must be filled-in with:
 * <ul>
 * 	<li>[0-3]: native transformation ID</li>
 * 	<li>[4-7]: blockSize: the block size (in bytes), or 0 if the underlying algorithm is not a block cipher</li>
 * 	<li>[8-11]: unitBytes: unit size (number of input bytes that can be processed at a time)</li>
 * 	<li>[12-15]: cipherMode: one of the <code>*_MODE</code> values defined in {@link AbstractCipher}</li>
 * </ul>
 *
 * @param transformation_name[in]		Null terminated string that describes the transformation.
 * @param transformation_desc[out]		Description of the cipher.
 *
 * @return The transformation ID on success or -1 on error.
 *
 * @warning <code>transformation_name</code> must not be used outside of the VM task or saved.
 */
int32_t LLSEC_CIPHER_IMPL_get_transformation_description(uint8_t* transformation_name, LLSEC_CIPHER_transformation_desc* transformation_desc)
{
    LLSEC_CIPHER_DEBUG_TRACE("%s transformation_name %s\n", __func__, transformation_name);
    int32_t nb_transformations = sizeof(available_transformations) / sizeof(LLSEC_CIPHER_transformation);
    LLSEC_CIPHER_transformation* transformation = &available_transformations[0];

    while (--nb_transformations >= 0) {
        if (strcmp((const char*)transformation_name, transformation->name) == 0) {
            memcpy(transformation_desc, &(transformation->description), sizeof(LLSEC_CIPHER_transformation_desc));
            return (int32_t)transformation;
        }
        transformation++;
    }

    // Transformation not found.
    return -1;
}

/**
 * @brief Returns the number of bytes that are buffered internally inside the given cipher.
 *
 * @param[in] nativeTransformationId	The transformation ID.
 * @param[in] nativeId					The resource's native ID.
 *
 * @return The length of the buffer.
 */
int32_t LLSEC_CIPHER_IMPL_get_buffered_length(int32_t nativeTransformationId, int32_t nativeId)
{
    return 0;
}

/**
 * @brief Gets the initialization vector.
 *
 * @param[in] transformation_id			The transformation ID.
 * @param[in] native_id					The resource's native ID.
 * @param[out] iv						The initialization vector of the cipher.
 * @param[out] iv_length				The initialization vector size.
 *
 * @note Throws NativeException on error.
 *
 * @warning <code>iv</code> must not be used outside of the VM task or saved.
 */
void LLSEC_CIPHER_IMPL_get_IV(int32_t transformation_id, int32_t native_id, uint8_t* iv, int32_t iv_length)
{
    LLSEC_CIPHER_ctx* p_cipher_ctx = (LLSEC_CIPHER_ctx*)native_id;
    LLSEC_CIPHER_DEBUG_TRACE("%s \n", __func__);
    memcpy(iv, p_cipher_ctx->iv, iv_length);
}

/**
 * @brief Gets the length of the initialized vector.
 *
 * @param[in] transformation_id			The transformation ID.
 * @param[in] native_id					The resource's native ID.
 *
 * @return The length of the IV, -1 if there is no IV.
 */
int32_t LLSEC_CIPHER_IMPL_get_IV_length(int32_t transformation_id, int32_t native_id)
{
    LLSEC_CIPHER_ctx* p_cipher_ctx = (LLSEC_CIPHER_ctx*)native_id;
    LLSEC_CIPHER_DEBUG_TRACE("%s \n", __func__);
    return p_cipher_ctx->iv_length;
}

/**
 * @brief Initializes a Cipher resource.
 *
 * @param[in] tranformation_id			The transformation ID.
 * @param[in] is_decrypting				'1' for decrypting, '0' for encryting.
 * @param[in] key						The key.
 * @param[in] key_length				The key size.
 * @param[in] iv						The initialization vector.
 * @param[in] iv_length					The initialization vector size.
 *
 * @return The nativeId of the newly initialized resource.
 *
 * @note Throws NativeException on error.
 *
 * @warning <code>key</code> must not be used outside of the VM task or saved.
 * @warning <code>iv</code> must not be used outside of the VM task or saved.
 */
int32_t LLSEC_CIPHER_IMPL_init(int32_t transformation_id, uint8_t is_decrypting, uint8_t* key, int32_t key_length, uint8_t* iv, int32_t iv_length)
{
    LLSEC_CIPHER_DEBUG_TRACE("%s \n", __func__);
    void* native_id = NULL;
    LLSEC_CIPHER_transformation* transformation = (LLSEC_CIPHER_transformation*)transformation_id;

    if (iv_length <= 0) {
        SNI_throwNativeException(iv_length, "LLSEC_CIPHER_IMPL_init invalid iv length");
        return -1;
    }

    int returnCode = transformation->init(transformation_id, (void**)&native_id, is_decrypting, key, key_length, iv, iv_length);

    if (returnCode != LLSEC_CIPHER_SUCCESS) {
        SNI_throwNativeException(returnCode, "LLSEC_CIPHER_IMPL_init failed");
        return -1;
    }

    // register SNI native resource
    if (SNI_registerResource(native_id, transformation->close, NULL) != SNI_OK) {
        SNI_throwNativeException(-1, "Can't register SNI native resource");
        transformation->close((void*)native_id);
        return -1;
    }

    return (int32_t)(native_id);
}

/**
 * @brief Decrypts the message contained in <code>buffer</code>.
 *
 * @param[in] transformation_id			The transformation ID.
 * @param[in] native_id					The resource's native ID.
 * @param[in] buffer					The buffer containing the message to decrypt.
 * @param[in] buffer_offset				The buffer offset.
 * @param[in] buffer_length				The buffer length.
 * @param[out] output					The output buffer containing the plaintext message.
 * @param[out] output_offset			The output offset.
 *
 * @return The length of the buffer.
 *
 * @note Throws NativeException on error.
 *
 * @warning <code>buffer</code> must not be used outside of the VM task or saved.
 * @warning <code>output</code> must not be used outside of the VM task or saved.
 */
int32_t LLSEC_CIPHER_IMPL_decrypt(int32_t transformation_id, int32_t native_id, uint8_t* buffer, int32_t buffer_offset, int32_t buffer_length, uint8_t* output, int32_t output_offset)
{
    LLSEC_CIPHER_DEBUG_TRACE("%s \n", __func__);
    LLSEC_CIPHER_transformation* transformation = (LLSEC_CIPHER_transformation*)transformation_id;
    int returnCode = transformation->decrypt((void*)native_id, buffer + buffer_offset, buffer_length, output + output_offset);
    if (returnCode != LLSEC_CIPHER_SUCCESS) {
        SNI_throwNativeException(returnCode, "LLSEC_CIPHER_IMPL_decrypt failed");
        return -1;
    }
    return buffer_length;
}

/**
 * @brief Encrypts the message contained in <code>buffer</code>.
 *
 * @param[in] transformation_id			The transformation ID.
 * @param[in] native_id					The resource's native ID.
 * @param[in] buffer					The buffer containing the plaintext message to encrypt.
 * @param[in] buffer_offset				The buffer offset.
 * @param[in] buffer_length				The buffer length.
 * @param[out] output					The output buffer containing the encrypted message.
 * @param[in] output_offset				The output offset.
 *
 * @return The length of the buffer.
 *
 * @note Throws NativeException on error.
 *
 * @warning <code>buffer</code> must not be used outside of the VM task or saved.
 * @warning <code>output</code> must not be used outside of the VM task or saved.
 */
int32_t LLSEC_CIPHER_IMPL_encrypt(int32_t transformation_id, int32_t native_id, uint8_t* buffer, int32_t buffer_offset, int32_t buffer_length, uint8_t* output, int32_t output_offset)
{
    LLSEC_CIPHER_DEBUG_TRACE("%s \n", __func__);
    LLSEC_CIPHER_transformation* transformation = (LLSEC_CIPHER_transformation*)transformation_id;
    int returnCode = transformation->encrypt((void*)native_id, buffer + buffer_offset, buffer_length, output + output_offset);
    if (returnCode != LLSEC_CIPHER_SUCCESS) {
        SNI_throwNativeException(returnCode, "LLSEC_CIPHER_IMPL_encrypt failed");
        return -1;
    }
    return buffer_length;
}

/**
 * @brief Closes the resource related to the native ID.
 *
 * @param[in] transformation_id			The transformation ID.
 * @param[in] native_id					The resource's native ID.
 *
 * @note Throws NativeException on error.
 */
void LLSEC_CIPHER_IMPL_close(int32_t transformation_id, int32_t native_id)
{
    LLSEC_CIPHER_DEBUG_TRACE("%s \n", __func__);
    LLSEC_CIPHER_transformation* transformation = (LLSEC_CIPHER_transformation*)transformation_id;

    transformation->close((void*)native_id);
    if (SNI_unregisterResource((void*)native_id, (SNI_closeFunction)transformation->close) != SNI_OK) {
        SNI_throwNativeException(-1, "Can't unregister SNI native resource");
    }
    LLSEC_CIPHER_DEBUG_TRACE("%s \n", __func__);
}

/**
 * @brief Gets the id of the native close function.
 * @param[in] transformation_id			The transformation ID.
 *
 * @return the id of the static native close function.
 *
 * @note Throws NativeException on error.
 */
int32_t LLSEC_CIPHER_IMPL_get_close_id(int32_t transformation_id)
{
    LLSEC_CIPHER_DEBUG_TRACE("%s \n", __func__);
    LLSEC_CIPHER_transformation* transformation = (LLSEC_CIPHER_transformation*)transformation_id;
    return (int32_t)transformation->close;
}
