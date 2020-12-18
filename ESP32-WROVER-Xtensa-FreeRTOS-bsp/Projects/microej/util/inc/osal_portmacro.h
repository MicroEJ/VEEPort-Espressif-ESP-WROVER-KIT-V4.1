/*
 * C
 *
 * Copyright 2017-2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */

#ifndef OSAL_PORTMACRO_H
#define OSAL_PORTMACRO_H

/**
 * @file
 * @brief OS Abstraction Layer FreeRTOS port macro
 * @author MicroEJ Developer Team
 * @version 0.2.2
 * @date 8 December 2020
 */
#include <stdint.h>

/** @brief OS task stack */
typedef int32_t OSAL_task_stack_t;

/*
 * @brief Declare a task stack.
 *
 * @param[in] _name name of the variable that defines the stack.
 * @param[in] _size size of the stack in bytes. _size must be compile time constant value.
 */
#define OSAL_task_stack_declare(_name, _size) OSAL_task_stack_t _name = _size


#endif // OSAL_PORTMACRO_H
