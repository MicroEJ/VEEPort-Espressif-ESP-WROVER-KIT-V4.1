/*
 * C
 *
 * Copyright 2016-2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */

#ifndef  LLNET_COMMON_H
#define  LLNET_COMMON_H

/**
 * @file
 * @brief Common LLNET macro and functions.
 * @author MicroEJ Developer Team
 * @version 0.2.0
 * @date 21 December 2017
 */

#include <stdint.h>
#include <stdbool.h>
#include <sni.h>
#include "LLNET_configuration.h"
#include "async_select.h"
#include "async_select_configuration.h"
#include <sys/socket.h>


/**
 * Sanity check between the expected version of the configuration and the actual version of
 * the configuration.
 * If an error is raised here, it means that a new version of the CCO has been installed and
 * the configuration LLNET_configuration.h must be updated based on the one provided
 * by the new CCO version.
 */
#if LLNET_CONFIGURATION_VERSION != 1

	#error "Version of the configuration file LLNET_configuration.h is not compatible with this implementation."

#endif


// Define LLNET_DEBUG in LLNET_configuration.h to enable network debug trace
#ifdef LLNET_DEBUG
	#define LLNET_DEBUG_TRACE 	printf
#else
	#define LLNET_DEBUG_TRACE(...) ((void) 0)
#endif

/**
 * @brief Add an async_select operation
 *
 * @param[in] fd socket file descriptor
 * @param[in] operation select operation to add
 * @param[in] retry if it's a repeating call
 *
 * @return 0 on success, a negative value on error.
 */
int32_t asyncOperation(int32_t fd, SELECT_Operation operation, uint8_t retry);

/**
 * @brief Fills-in the given timeval struct with the given time in milliseconds.
 *
 * @param[in] time_ms time in milliseconds.
 * @param[in] time_timeval pointer to the timeval struct to fill-in.
 */
void time_ms_to_timeval(int64_t time_ms, struct timeval* time_timeval);

/**
 * @brief Sets the given socket in non-blocking mode or not.
 *
 * @param[in] fd socket file descriptor
 * @param[in] non_blocking true to enable non-blocking, false to enable blocking.
 *
 * @return 0 on success, a negative value on error.
 */
int32_t set_socket_non_blocking(int32_t fd, bool non_blocking);

/**
 * @brief Checks if the given socket is in non-blocking mode or not.
 *
 * @param[in] fd socket file descriptor
 *
 * @return true if the socket is in non_blocking, false otherwise.
 */
bool is_socket_non_blocking(int32_t fd);

/**
 * @brief Convert a network error code into a java error code.
 *
 * @param[in] err the error code returned by a BSD-like function.
 *
 * @return an error code defined in LLNET_ERRORS.h.
 */
int32_t map_to_java_exception(int32_t err);

#endif // LLNET_COMMON_H
