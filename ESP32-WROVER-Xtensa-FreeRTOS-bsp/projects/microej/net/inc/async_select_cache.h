/*
 * C
 *
 * Copyright 2017-2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief Socket timeout cache API
 * @author MicroEJ Developer Team
 * @version 2.3.2
 * @date 17 February 2023
 */
 
#ifndef  ASYNC_SELECT_CACHE_H
#define  ASYNC_SELECT_CACHE_H

#include <stdint.h>
#include <sni.h>

#ifdef __cplusplus
	extern "C" {
#endif

/**
 * @brief Initialize the socket timeout cache
 */
void async_select_init_socket_timeout_cache(void);

/**
 * @brief Get a socket timeout from the cache
 *
 * @param[in] fd the socket file descriptor
 *
 * @return socket timeout on success, a negative value on error.
 */
int32_t async_select_get_socket_timeout_from_cache(int32_t fd);

/**
 * @brief Get a socket absolute timeout from the cache (time when the async_select task should unblock the select)
 *
 * @param[in] fd the socket file descriptor
 *
 * @return socket absolute timeout on success, a negative value on error.
 */
int64_t async_select_get_socket_absolute_timeout_from_cache(int32_t fd);

/**
 * @brief Set the given socket's timeout in the cache
 *
 * @param[in] fd the socket file descriptor
 * @param[in] timeout socket timeout
 */
void async_select_set_socket_timeout_in_cache(int32_t fd, int32_t timeout);

/**
 * @brief Set the given socket's absolute timeout in the cache (time when the async_select task should unblock the select)
 *
 * @param[in] fd the socket file descriptor
 * @param[in] absolute_timeout socket absolute timeout
 * @return 0 on success, -1 on failure.
 */
int32_t async_select_set_socket_absolute_timeout_in_cache(int32_t fd, int64_t absolute_timeout);

/**
 * @brief Remove a socket timeout from the cache
 *
 * @param[in] fd the socket file descriptor
 */
void async_select_remove_socket_timeout_from_cache(int32_t fd);

#ifdef __cplusplus
	}
#endif

#endif // ASYNC_SELECT_CACHE_H
