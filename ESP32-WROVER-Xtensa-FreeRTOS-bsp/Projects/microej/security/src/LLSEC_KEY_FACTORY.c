/*
 * C
 *
 * Copyright 2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */

#include <LLSEC_KEY_FACTORY_impl.h>
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
 * @brief Gets for the given algorithm the key factory description.
 *
 * @param[in] algorithm_name			Null terminated string that describes the algorithm.
 *
 * @return The algorithm ID on success or -1 on error.
 */
int32_t LLSEC_KEY_FACTORY_IMPL_get_algorithm_description(uint8_t* algorithm_name) {
	// TODO not implemented yet
	SNI_throwNativeException(J_UNKNOWN_ERROR, "Native not implemented yet");
	return -1;
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
void LLSEC_KEY_FACTORY_IMPL_get_private_key_data(int32_t algorithm_id, uint8_t* format_name, uint8_t* key_data, int32_t key_data_length, uint8_t* encoded_key, int32_t encoded_key_length) {
	// TODO not implemented yet
	SNI_throwNativeException(J_UNKNOWN_ERROR, "Native not implemented yet");
}

/**
 * @brief Gets the native private key size corresponding to the algorithm.
 *
 * @param[in] algorithm_id				The algorithm ID.
 * @param[in] format_name				Null terminated string that describes the key format.
 * @param[in] encoded_key				The private key in its encoded form.
 * @param[in] encoded_key_length		The encoded key length.
 *
 * @return The size of the private key.
 */
int32_t LLSEC_KEY_FACTORY_IMPL_get_private_key_length(int32_t algorithm_id, uint8_t* format_name, uint8_t* encoded_key, int32_t encoded_key_length) {
	// TODO not implemented yet
	SNI_throwNativeException(J_UNKNOWN_ERROR, "Native not implemented yet");
	return -1;
}
