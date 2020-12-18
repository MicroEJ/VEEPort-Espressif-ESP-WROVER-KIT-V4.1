/*
 * C
 *
 * Copyright 2019-2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 *
 */

#ifndef MICROEJ_ALLOCATOR_H
#define MICROEJ_ALLOCATOR_H

#include <stdlib.h>
#include "microej_allocator_configuration.h"

/**
 * @brief Allocate a memory area and return the pointer to the allocated memory.
 * @param[in] size Number of bytes to allocate.
 * @return A pointer to the allocated memory. NULL if allocation failed.
 */
void* microej_malloc(size_t size);

/**
 * @brief Allocate a memory area of n elements of size bytes and return the pointer to the allocated memory.
 * @param[in] n Number of elements to allocate.
 * @param[in] size Number of bytes to allocate per elements.
 * @return A pointer to the allocated memory. NULL if allocation failed.
 */
void* microej_calloc(size_t n, size_t size);

/*
* @brief Frees the memory area pointed to by ptr.
* @param[in] ptr Pointer on the memory area which have been returned by a previous call to microej_malloc or microej_calloc.
*/
void microej_free(void *ptr);

#endif // MICROEJ_ALLOCATOR_H
