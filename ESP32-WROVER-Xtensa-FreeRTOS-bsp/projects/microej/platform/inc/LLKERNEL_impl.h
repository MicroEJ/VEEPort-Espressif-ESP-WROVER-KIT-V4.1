/* 
 * C
 * 
 * Copyright 2014-2016 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */
/* 
 * This file is auto-generated - DO NOT EDIT IT
 */
/* 
 * Implementation header file. Shall only be included by client implementation C files.
 */
#include <stdint.h>
#include <intern/LLKERNEL_impl.h>
// --------------------------------------------------------------------------------
// -                      Functions that must be implemented                      -
// --------------------------------------------------------------------------------

/**
 * Allocate a chunk of memory of the given size in bytes.
 * @return a 32bits aligned address, or null on error 
 */
void* LLKERNEL_IMPL_allocate(int32_t size);

/**
 * Release a chunk of memory previously allocated using {@link LLKERNEL_allocate}. 
 */
void LLKERNEL_IMPL_free(void* block);
