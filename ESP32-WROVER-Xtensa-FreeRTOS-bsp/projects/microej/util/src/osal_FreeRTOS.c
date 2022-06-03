/*
 * C
 *
 * Copyright 2017-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief OS Abstraction Layer FreeRTOS implementation
 * @author MicroEJ Developer Team
 * @version 1.0.2
 * @date 8 December 2020
 */

#include <stdint.h>
#include "osal.h"

/**
 * @brief Convert time from milliseconds to FreeRTOS ticks.
 *
 * @param[in] time in milliseconds
 *
 * @return time in FreeRTOS ticks
 */
static TickType_t OSAL_FreeRTOS_convert_time_to_tick(uint32_t milliseconds);

/**
 * @brief Create an OS task and start it.
 *
 * @param[in] entry_point function called at task startup
 * @param[in] name the task name
 * @param[in] stack task stack declared using OSAL_task_stack_declare() macro
 * @param[in] priority task priority
 * @param[in] parameters task entry parameters. NULL if no entry parameters
 * @param[in,out] handle pointer on a task handle
 *
 * @return operation status (@see OSAL_status_t)
 */
OSAL_status_t OSAL_task_create(OSAL_task_entry_point_t entry_point, uint8_t* name, OSAL_task_stack_t stack, int32_t priority, void* parameters, OSAL_task_handle_t* handle)
{
    uint16_t stack_size = (uint16_t)stack;

    if (handle == NULL)
    {
        return OSAL_WRONG_ARGS;
    }

    if (xTaskCreate(entry_point, (char const *)name,
        (stack_size / (sizeof(portSTACK_TYPE))),
        parameters,
        (unsigned portBASE_TYPE)priority,
        handle) != pdPASS)
    {
        return OSAL_ERROR;
    }

    return OSAL_OK;
}

/**
 * @brief Delete an OS task and start it.
 *
 * @param[in] handle pointer on the task handle
 *
 * @return operation status (@see OSAL_status_t)
 */
OSAL_status_t OSAL_task_delete(OSAL_task_handle_t* handle)
{
    if (handle == NULL)
    {
        return OSAL_WRONG_ARGS;
    }

    vTaskDelete(*handle);

    return OSAL_OK;
}

/**
 * @brief Get the handle of the current OS task.
 *
 * @param[in,out] handle pointer on a task handle
 *
 * @return operation status (@see OSAL_status_t)
 */
OSAL_status_t OSAL_task_get_current(OSAL_task_handle_t* handle)
{
    if (handle == NULL)
    {
        return OSAL_WRONG_ARGS;
    }

    *handle = (OSAL_task_handle_t)xTaskGetCurrentTaskHandle();

    return OSAL_OK;
}

/**
 * @brief Create an OS queue with a predefined queue size.
 *
 * @param[in] name queue name
 * @param[in,out] handle pointer on a queue handle
 * @param[in] queue structure containing address and size declared using OSAL_queue_declare() macro
 *
 * @return operation status (@see OSAL_status_t)
 */
OSAL_status_t OSAL_queue_create(uint8_t* name, OSAL_queue_handle_t* handle, OSAL_queue_t queue)
{
    if (handle == NULL)
    {
        return OSAL_WRONG_ARGS;
    }

    *handle = xQueueCreate(queue, sizeof(void *));
    if ((*handle) == NULL)
    {
        return OSAL_ERROR;
    }

    return OSAL_OK;
}

/**
 * @brief Delete an OS queue.
 *
 * @param[in] handle pointer on the queue handle
 *
 * @return operation status (@see OSAL_status_t)
 */
OSAL_status_t OSAL_queue_delete(OSAL_queue_handle_t* handle)
{
    if (handle == NULL)
    {
        return OSAL_WRONG_ARGS;
    }

    vQueueDelete(*handle);

    return OSAL_OK;
}

/**
 * @brief Post a message in an OS queue.
 *
 * @param[in] handle pointer on the queue handle
 * @param[in] msg message to post in the message queue
 *
 * @return operation status (@see OSAL_status_t)
 */
OSAL_status_t OSAL_queue_post(OSAL_queue_handle_t* handle, void* msg)
{
    if (handle == NULL)
    {
        return OSAL_WRONG_ARGS;
    }

    if (xQueueSend(*handle, &msg, 0) != pdTRUE)
    {
        return OSAL_NOMEM;
    }

    return OSAL_OK;
}

/**
 * @brief Fetch a message from an OS queue. Blocks until a message arrived or a timeout occurred.
 *
 * @param[in] handle pointer on the queue handle
 * @param[in,out] msg message fetched from the OS queue
 * @param[in] timeout maximum time to wait for message arrival, OSAL_INFINITE_TIME for infinite timeout
 *
 * @return operation status (@see OSAL_status_t)
 */
OSAL_status_t OSAL_queue_fetch(OSAL_queue_handle_t* handle, void** msg, uint32_t timeout)
{
    TickType_t timeout_in_tick = OSAL_FreeRTOS_convert_time_to_tick(timeout);

    if (handle == NULL)
    {
        return OSAL_WRONG_ARGS;
    }

    if (xQueueReceive(*handle, msg, timeout_in_tick) != pdTRUE)
    {
        return OSAL_ERROR;
    }

    return OSAL_OK;
}

/**
 * @brief Create an OS counter semaphore with a semaphore count initial value.
 *
 * @param[in] name counter semaphore name
 * @param[in] initial_count counter semaphore initial count value
 * @param[in] max_count counter semaphore maximum count value
 * @param[in,out] handle pointer on a counter semaphore handle
 *
 * @return operation status (@see OSAL_status_t)
 */
OSAL_status_t OSAL_counter_semaphore_create(uint8_t* name, uint32_t initial_count, uint32_t max_count, OSAL_counter_semaphore_handle_t* handle)
{
    if (handle == NULL)
    {
        return OSAL_WRONG_ARGS;
    }

    *handle = xSemaphoreCreateCounting(max_count, initial_count);
    if ((*handle) == NULL)
    {
        return OSAL_NOMEM;
    }

    return OSAL_OK;
}

/**
 * @brief Delete an OS counter semaphore.
 *
 * @param[in] handle pointer on the counter semaphore handle
 *
 * @return operation status (@see OSAL_status_t)
 */
OSAL_status_t OSAL_counter_semaphore_delete(OSAL_counter_semaphore_handle_t* handle)
{
    if (handle == NULL)
    {
        return OSAL_WRONG_ARGS;
    }

    vSemaphoreDelete(*handle);

    return OSAL_OK;
}

/**
 * @brief Take operation on OS counter semaphore. Block the current task until counter semaphore
 * become available or timeout occurred. Decrease the counter semaphore count value by 1 and
 * block the current task if count value equals to 0.
 *
 * @param[in] handle pointer on the counter semaphore handle
 * @param[in] timeout maximum time to wait until the counter semaphore become available, OSAL_INFINITE_TIME for infinite timeout
 *
 * @return operation status (@see OSAL_status_t)
 */
OSAL_status_t OSAL_counter_semaphore_take(OSAL_counter_semaphore_handle_t* handle, uint32_t timeout)
{
    TickType_t timeout_in_tick = OSAL_FreeRTOS_convert_time_to_tick(timeout);

    if (handle == NULL)
    {
        return OSAL_WRONG_ARGS;
    }

    if (xSemaphoreTake(*handle, timeout_in_tick) != pdTRUE)
    {
        return OSAL_ERROR;
    }

    return OSAL_OK;
}

/**
 * @brief Give operation on OS counter semaphore. Increase the counter semaphore count value by 1 and unblock the current task if count value.
 * equals to 0.
 *
 * @param[in] handle pointer on the counter semaphore handle
 *
 * @return operation status (@see OSAL_status_t)
 */
OSAL_status_t OSAL_counter_semaphore_give(OSAL_counter_semaphore_handle_t* handle)
{
    if (handle == NULL)
    {
        return OSAL_WRONG_ARGS;
    }

    if (xSemaphoreGive(*handle) != pdTRUE)
    {
        return OSAL_ERROR;
    }

    return OSAL_OK;
}

/**
 * @brief Create an OS binary semaphore with a semaphore count initial value (0 or 1).
 *
 * @param[in] name counter semaphore name
 * @param[in] initial_count counter semaphore initial count value
 * @param[in,out] handle pointer on a binary semaphore handle
 *
 * @return operation status (@see OSAL_status_t)
 */
OSAL_status_t OSAL_binary_semaphore_create(uint8_t* name, uint32_t initial_count, OSAL_binary_semaphore_handle_t* handle)
{
    if (handle == NULL)
    {
        return OSAL_WRONG_ARGS;
    }

    *handle = xSemaphoreCreateBinary();
    if ((*handle) == NULL)
    {
        return OSAL_NOMEM;
    }

    return OSAL_OK;
}

/**
 * @brief Delete an OS binary semaphore.
 *
 * @param[in] handle pointer on the binary semaphore handle
 *
 * @return operation status (@see OSAL_status_t)
 */
OSAL_status_t OSAL_binary_semaphore_delete(OSAL_binary_semaphore_handle_t* handle)
{
    if (handle == NULL)
    {
        return OSAL_WRONG_ARGS;
    }

    vSemaphoreDelete(*handle);

    return OSAL_OK;
}

/**
 * @brief Take operation on OS binary semaphore. Block the current task until binary semaphore
 * become available or timeout occurred. Decrease the binary semaphore count value by 1 and
 * block the current task if count value equals to 0.
 *
 * @param[in] handle pointer on the binary semaphore handle
 * @param[in] timeout maximum time to wait until the binary semaphore become available, OSAL_INFINITE_TIME for infinite timeout
 *
 * @return operation status (@see OSAL_status_t)
 */
OSAL_status_t OSAL_binary_semaphore_take(OSAL_binary_semaphore_handle_t* handle, uint32_t timeout)
{
    TickType_t timeout_in_tick = OSAL_FreeRTOS_convert_time_to_tick(timeout);

    if (handle == NULL)
    {
        return OSAL_WRONG_ARGS;
    }

    if (xSemaphoreTake(*handle, timeout_in_tick) != pdTRUE)
    {
        return OSAL_ERROR;
    }

    return OSAL_OK;
}

/**
 * @brief Give operation on OS binary semaphore. Increase the binary semaphore count value by 1 and unblock the current task if count value.
 * equals to 0.
 *
 * @param[in] handle pointer on the binary semaphore handle
 *
 * @return operation status (@see OSAL_status_t)
 */
OSAL_status_t OSAL_binary_semaphore_give(OSAL_binary_semaphore_handle_t* handle)
{
    if (handle == NULL)
    {
        return OSAL_WRONG_ARGS;
    }

    if (xSemaphoreGive(*handle) != pdTRUE)
    {
        return OSAL_ERROR;
    }

    return OSAL_OK;
}

/**
 * @brief Create an OS mutex.
 *
 * @param[in] name mutex name
 * @param[in,out] handle pointer on a mutex handle
 *
 * @return operation status (@see OSAL_status_t)
 */
OSAL_status_t OSAL_mutex_create(uint8_t* name, OSAL_mutex_handle_t* handle)
{
    if (handle == NULL)
    {
        return OSAL_WRONG_ARGS;
    }

    *handle = xSemaphoreCreateMutex();
    if ((*handle) == NULL)
    {
        return OSAL_NOMEM;
    }

    return OSAL_OK;
}

/**
 * @brief Delete an OS mutex.
 *
 * @param[in] handle pointer on the mutex handle
 *
 * @return operation status (@see OSAL_status_t)
 */
OSAL_status_t OSAL_mutex_delete(OSAL_mutex_handle_t* handle)
{
    if (handle == NULL)
    {
        return OSAL_WRONG_ARGS;
    }

    vSemaphoreDelete(*handle);

    return OSAL_OK;
}

/**
 * @brief Take operation on OS mutex.
 *
 * @param[in] handle pointer on the mutex handle
 * @param[in] timeout maximum time to wait until the mutex become available, OSAL_INFINITE_TIME for infinite timeout
 *
 * @return operation status (@see OSAL_status_t)
 */
OSAL_status_t OSAL_mutex_take(OSAL_mutex_handle_t* handle, uint32_t timeout)
{
    TickType_t timeout_in_tick = OSAL_FreeRTOS_convert_time_to_tick(timeout);

    if (handle == NULL)
    {
        return OSAL_WRONG_ARGS;
    }

    if (xSemaphoreTake(*handle, timeout_in_tick) != pdTRUE)
    {
        return OSAL_ERROR;
    }

    return OSAL_OK;
}

/**
 * @brief Give operation on OS mutex.
 *
 * @param[in] handle pointer on the mutex handle
 *
 * @return operation status (@see OSAL_status_t)
 */
OSAL_status_t OSAL_mutex_give(OSAL_mutex_handle_t* handle)
{
    if (handle == NULL)
    {
        return OSAL_WRONG_ARGS;
    }

    if (xSemaphoreGive(*handle) != pdTRUE)
    {
        return OSAL_ERROR;
    }

    return OSAL_OK;
}

/**
 * @brief Disable the OS scheduler context switching. Prevent the OS from
 * scheduling the current thread calling #OSAL_disable_context_switching while
 * the OS scheduling is already disable has an undefined behavior. This method
 * may be called from an interrupt.
 *
 * @return operation status (@see OSAL_status_t)
 */
OSAL_status_t OSAL_disable_context_switching(void)
{
    vTaskSuspendAll();

    return OSAL_OK;
}

/**
 * @brief Reenable the OS scheduling that was disabled by #OSAL_disable_context_switching.
 * This method may be called from an interrupt.
 *
 * @return operation status (@see OSAL_status_t)
 */
OSAL_status_t OSAL_enable_context_switching(void)
{
    xTaskResumeAll();

    return OSAL_OK;
}

/**
 * @brief Asleep the current task during specified number of milliseconds.
 *
 * @param[in] milliseconds number of milliseconds
 *
 * @return operation status (@see OSAL_status_t)
 */
OSAL_status_t OSAL_sleep(uint32_t milliseconds)
{
    TickType_t delay_in_ticks = OSAL_FreeRTOS_convert_time_to_tick(milliseconds);

    vTaskDelay(delay_in_ticks);

    return OSAL_OK;
}

static TickType_t OSAL_FreeRTOS_convert_time_to_tick(uint32_t milliseconds)
{
    TickType_t time_to_tick_timeout;

    if (milliseconds == OSAL_INFINITE_TIME)
    {
        time_to_tick_timeout = portMAX_DELAY;
    }
    else
    {
        if (milliseconds == 0)
        {
            time_to_tick_timeout = 0;
        }
        else
        {
            time_to_tick_timeout = milliseconds / portTICK_PERIOD_MS;
            if (time_to_tick_timeout == 0)
            {
                time_to_tick_timeout = 1;
            }
        }
    }

    return time_to_tick_timeout;
}
