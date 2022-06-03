/*
 * C
 *
 * Copyright 2018-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#ifndef OSAL_PORTMACRO_H
#define OSAL_PORTMACRO_H

/**
 * @file
 * @brief OS Abstraction Layer FreeRTOS port macro
 * @author MicroEJ Developer Team
 * @version 1.0.2
 * @date 8 December 2020
 */

#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/** @brief Custom OS type definitions */
#define OSAL_CUSTOM_TYPEDEF

/** @brief task function entry point */
typedef TaskFunction_t OSAL_task_entry_point_t;

/** @brief OS task handle */
typedef TaskHandle_t OSAL_task_handle_t;

/** @brief OS queue handle */
typedef QueueHandle_t OSAL_queue_handle_t;

/** @brief OS counter semaphore handle */
typedef SemaphoreHandle_t OSAL_counter_semaphore_handle_t;

/** @brief OS binary semaphore handle */
typedef SemaphoreHandle_t OSAL_binary_semaphore_handle_t;

/** @brief OS mutex handle */
typedef SemaphoreHandle_t OSAL_mutex_handle_t;

/** @brief OS task stack */
typedef int32_t OSAL_task_stack_t;

/** @brief OS queue */
typedef int32_t OSAL_queue_t;

/*
 * @brief Declare a task stack.
 *
 * @param[in] _name name of the variable that defines the stack.
 * @param[in] _size size of the stack in bytes. _size must be compile time constant value.
 */
#define OSAL_task_stack_declare(_name, _size) OSAL_task_stack_t _name = _size

/*
 * @brief Declare a queue.
 *
 * @param[in] _name name of the variable that defines the queue address.
 * @param[in] _size number of items that can be stored in the queue. _size must be compile time constant value.
 */
#define OSAL_queue_declare(_name, _size) OSAL_queue_t _name = _size

#endif // OSAL_PORTMACRO_H
