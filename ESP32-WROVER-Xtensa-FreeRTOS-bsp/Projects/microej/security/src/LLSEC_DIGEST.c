/*
 * C
 *
 * Copyright 2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */

#include <LLSEC_DIGEST_impl.h>
#include <LLSEC_ERRORS.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sni.h>
#include <stdint.h>

/**
 * @file
 * @brief MicroEJ Security low level API
 * @author MicroEJ Developer Team
 * @version 1.5.0
 * @date 19 March 2020
 */

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
int32_t LLSEC_DIGEST_IMPL_get_algorithm_description(uint8_t* algorithm_name, LLSEC_DIGEST_algorithm_desc* algorithm_desc) {
	// TODO not implemented yet
	SNI_throwNativeException(J_UNKNOWN_ERROR, "Native not implemented yet");
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
int32_t LLSEC_DIGEST_IMPL_init(int32_t algorithm_id) {
	// TODO not implemented yet
	SNI_throwNativeException(J_UNKNOWN_ERROR, "Native not implemented yet");
	return -1;
}

/**
 * @brief Closes the resource related to the native ID.
 *
 * @param[in] algorithm_id				The algorithm ID.
 * @param[in] native_id					The resource's native ID.
 *
 * @note Throws NativeException on error.
 */
void LLSEC_DIGEST_IMPL_close(int32_t algorithm_id, int32_t native_id) {
	// TODO not implemented yet
	SNI_throwNativeException(J_UNKNOWN_ERROR, "Native not implemented yet");
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
void LLSEC_DIGEST_IMPL_update(int32_t algorithm_id, int32_t native_id, uint8_t* buffer, int32_t buffer_offset, int32_t buffer_length) {
	// TODO not implemented yet
	SNI_throwNativeException(J_UNKNOWN_ERROR, "Native not implemented yet");
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
void LLSEC_DIGEST_IMPL_digest(int32_t algorithm_id, int32_t native_id, uint8_t* out, int32_t out_offset, int32_t out_length) {
	// TODO not implemented yet
	SNI_throwNativeException(J_UNKNOWN_ERROR, "Native not implemented yet");
}
