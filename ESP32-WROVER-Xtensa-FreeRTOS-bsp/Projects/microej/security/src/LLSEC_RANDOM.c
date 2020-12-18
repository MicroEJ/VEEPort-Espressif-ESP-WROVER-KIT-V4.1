/*
 * C
 *
 * Copyright 2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */

#include <LLSEC_RANDOM_impl.h>
#include <LLSEC_ERRORS.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sni.h>

/**
 * @file
 * @brief MicroEJ Security low level API
 * @author MicroEJ Developer Team
 * @version 1.5.0
 * @date 13 March 2020
 */

/**
 * @brief Initializes a Random resource.
 *
 * @return The native ID of the resource.
 *
 * @note Throws NativeException on error.
 */
int32_t LLSEC_RANDOM_IMPL_init() {
	// TODO not implemented yet
	return 0;
}

/**
 * @brief Closes the resource related to the nativeId.
 *
 * @param[in] native_id					The native ID.
 *
 * @note Throws NativeException on error.
 */
void LLSEC_RANDOM_IMPL_close(int32_t native_id) {
	// TODO not implemented yet
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
void LLSEC_RANDOM_IMPL_next_bytes(int32_t native_id, uint8_t* rnd, int32_t size) {
	// TODO not implemented yet
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
void LLSEC_RANDOM_IMPL_set_seed(int32_t native_id, uint8_t* seed, int32_t size) {
	// TODO not implemented yet
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
void LLSEC_RANDOM_IMPL_generate_seed(int32_t native_id, uint8_t* seed, int32_t size) {
	// TODO not implemented yet
}
