/*
 * C
 *
 * Copyright 2017-2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#ifndef  ASYNC_SELECT_H
#define  ASYNC_SELECT_H

/**
 * @file
 * @brief Asynchronous network select API
 * @author MicroEJ Developer Team
 * @version 2.3.2
 * @date 17 February 2023
 */

#include <stdint.h>
#include <sni.h>

#ifdef __cplusplus
	extern "C" {
#endif

/** @brief Select operations list. */
typedef enum
{
  SELECT_READ,
  SELECT_WRITE
}SELECT_Operation;

/**
 * @brief Execute a select() for the given file descriptor and operation without blocking.
 *
 * @param[in] fd the file descriptor.
 * @param[in] operation the operation (read or write) we want to monitor with the select().
 *
 * @return select() result.
 */
int32_t non_blocking_select(int32_t fd, SELECT_Operation operation);

/**
 * @brief Executes asynchronously a select() operation for the given file descriptor.
 * This function will suspend the execution of the current Java thread using
 * SNI_suspendCurrentJavaThreadWithCallback(). Once the select() succeeds the Java
 * thread is resumed and the given SNI callback is called.
 *
 * @param[in] fd the file descriptor.
 * @param[in] operation the operation (read or write) we want to monitor with the select().
 * @param[in] timeout_ms timeout in millisecond
 * @param[in] the SNI callback to call when the Java thread is resumed or timeout occurs.
 *
 * @return 0 on success, -1 on failure.
 */
int32_t async_select(int32_t fd, SELECT_Operation operation, int64_t timeout_ms, SNI_callback callback);

/**
 * @brief Initialize the async_select component. This function must be called prior to any call of
 * async_select().
 *
 * @return 0 on success, -1 on failure.
 */
int32_t async_select_init(void);

/**
 * @brief Notifies the async_select task that a file descriptor has been closed.
 * On some systems the close of a file descriptor does not unblock the select that's
 * why we need to notify the async_select task.
 */
void async_select_notify_closed_fd(int32_t fd);

#ifdef __cplusplus
	}
#endif

#endif // ASYNC_SELECT_H
