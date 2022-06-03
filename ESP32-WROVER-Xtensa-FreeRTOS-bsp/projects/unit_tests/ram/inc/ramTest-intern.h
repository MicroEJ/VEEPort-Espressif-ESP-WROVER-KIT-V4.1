/*
 * C
 *
 * Copyright 2018 IS2T. All rights reserved.
 * IS2T PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */


#include "ramTest-conf.h"
#include <stdlib.h>
#include <stdio.h>

#ifndef __RAM_TEST_INTERN_H
#define __RAM_TEST_INTERN_H

/** public variables */

/** private constant definition */

#define RAM_TEST_FULL_TEST

#ifndef RAM_TEST_32BIT_WORLD
    #define RAM_TEST_32BIT_WORLD    (0)
#endif

#ifndef RAM_TEST_16BIT_WORLD
    #define RAM_TEST_16BIT_WORLD    (0)
#endif

#ifndef RAM_TEST_8BIT_WORLD
    #define RAM_TEST_8BIT_WORLD     (0)
#endif

#ifdef RAM_TEST_FULL_TEST
    #undef RAM_TEST_32BIT_WORLD
    #undef RAM_TEST_16BIT_WORLD
    #undef RAM_TEST_8BIT_WORLD
    #define RAM_TEST_32BIT_WORLD    (RAM_TEST_ALL_TESTS)
    #define RAM_TEST_16BIT_WORLD    (RAM_TEST_ALL_TESTS)
    #define RAM_TEST_8BIT_WORLD     (RAM_TEST_ALL_TESTS)
#endif

/** private function declaration */
#ifdef RAM_TEST_32BIT_WORLD
uint32_t RAM_TEST_CheckAllPatternMemory32(void);
uint32_t RAM_TEST_CheckAllPatternAllMemory32(void);
uint32_t RAM_TEST_CheckAddress32(void);
#endif
#ifdef RAM_TEST_16BIT_WORLD
uint32_t RAM_TEST_CheckAllPatternMemory16(void);
uint32_t RAM_TEST_CheckAllPatternAllMemory16(void);
uint32_t RAM_TEST_CheckAddress16(void);
#endif
#ifdef RAM_TEST_8BIT_WORLD
uint32_t RAM_TEST_CheckAllPatternMemory8(void);
uint32_t RAM_TEST_CheckAllPatternAllMemory8(void);
uint32_t RAM_TEST_CheckAddress8(void);
#endif
void RAM_TEST_MemoryCleanup(void);

#endif
