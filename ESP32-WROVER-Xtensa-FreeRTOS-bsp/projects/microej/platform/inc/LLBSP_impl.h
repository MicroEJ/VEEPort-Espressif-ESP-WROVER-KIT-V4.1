/* 
 * C
 * 
 * Copyright 2011-2021 MicroEJ Corp. All rights reserved.
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
#include <intern/LLBSP_impl.h>
// --------------------------------------------------------------------------------
// -                      Functions that must be implemented                      -
// --------------------------------------------------------------------------------

/**
 * @return 1 if the given pointer is in a read only memory (eg. flash), 0 otherwise.
 */
uint8_t LLBSP_IMPL_isInReadOnlyMemory(void* ptr);

/**
 * <p>
 * Writes the character <code>c</code>, cast to an unsigned char, to stdout stream.
 * </p>
 * <p>
 * This function is used by the default implementation of the Java <code>System.out</code>.
 * </p>
 */
void LLBSP_IMPL_putchar(int32_t c);
