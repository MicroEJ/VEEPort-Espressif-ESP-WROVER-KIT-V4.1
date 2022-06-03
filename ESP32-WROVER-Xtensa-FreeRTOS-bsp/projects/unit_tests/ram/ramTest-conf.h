/*
 * C
 *
 * Copyright 2018 IS2T. All rights reserved.
 * IS2T PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */

/* Prevent recursive inclusion */

#ifndef __TEST_RAM_CONF_H
#define __TEST_RAM_CONF_H
#include <stdio.h>
#include "ramTest.h"

#define CONFIG_TEST_PRINTF_ON

/** Debug macro */
#ifndef TEST_DBG
#ifdef CONFIG_TEST_PRINTF_ON
    #define TEST_DBG      printf
#else
    #define TEST_DBG(...) (void)0
#endif
#endif

/** RAM_TEST parameters definition */

/*
 * RAM test activation : 3 constants can be defined to activate tests for a given word width for RAM accesses :
 *  - RAM_TEST_32BIT_WORLD : 32 bits access tests
 *  - RAM_TEST_16BIT_WORLD : 16 bits access tests
 *  - RAM_TEST_8BIT_WORLD  : 8 bits access tests.
 *
 * for each RAM access word width, the following constants can be used to active
 * tests:
 * - RAM_TEST_ALL_LOW_DATA : this constant enables RAM_TEST_CheckPatternMemoryXX
 * and RAM_TEST_CheckPatternAllMemoryXX tests with the 0x00 pattern value.
 *
 * - RAM_TEST_ALL_HIGH_DATA : this constant enables RAM_TEST_CheckPatternMemoryXX
 * and RAM_TEST_CheckPatternAllMemoryXX tests with the 0xFFFFFFFF, 0xFFFF or 0xFF pattern values.
 *
 * - RAM_TEST_HALF_HIGH_DATA : this constant enables RAM_TEST_CheckPatternMemoryXX
 * and RAM_TEST_CheckPatternAllMemoryXX tests with the 0x0000FFFF and 0xFFFF0000 pattern values
 * (16bit-patterns are 0x00FF and 0xFF00; 8bit-patterns are 0x0F and 0xF0).
 *
 * - RAM_TEST_ODD_BIT_DATA : this constant enables RAM_TEST_CheckPatternMemoryXX
 * and RAM_TEST_CheckPatternAllMemoryXX tests with the 0x55555555 pattern value
 * (16bit-pattern is 0x5555; 8bit-pattern is 0x55).
 *
 * - RAM_TEST_EVEN_BIT_DATA : this constant enables RAM_TEST_CheckPatternMemoryXX
 * and RAM_TEST_CheckPatternAllMemoryXX tests with the 0xAAAAAAAA pattern value
 * (16bit-pattern is 0xAAAA; 8bit-pattern is 0xAA).
 *
 * - RAM_TEST_GENERIC_DATA : this constant enables RAM_TEST_CheckPatternMemoryXX
 * and RAM_TEST_CheckPatternAllMemoryXX tests with the 0x12345678 and 0x87654321 pattern values
 * (16bit-patterns are 0x1234, 0x5678, 0x8765 and 0x4321; 8bit-patterns are 0x12, 0x34, 0x56,
 * 0x78, 0x87, 0x65, 0x43 and 0x21).
 *
 * - RAM_TEST_ADDRESS_BIT : this constant enables RAM_TEST_CheckAddressXX test.
 *
 *
 * - RAM_TEST_READ_SPEED :
 *
 *
 * - RAM_TEST_WRITE_SPEED :
 *
 *
 * - RAM_TEST_TRANSFERT_SPEED :
 *
 *
 * - RAM_TEST_ALL_TESTS : this constant enables all tests.
*/

#define RAM_TEST_32BIT_WORLD     (RAM_TEST_READ_SPEED | RAM_TEST_WRITE_SPEED | RAM_TEST_TRANSFERT_SPEED)
#define RAM_TEST_16BIT_WORLD     (RAM_TEST_READ_SPEED | RAM_TEST_WRITE_SPEED | RAM_TEST_TRANSFERT_SPEED)
#define RAM_TEST_8BIT_WORLD      (RAM_TEST_READ_SPEED | RAM_TEST_WRITE_SPEED | RAM_TEST_TRANSFERT_SPEED)

/** Board specific constants */
#define RAM_TEST_ADDRESS_BEGIN        0x3F800000
#define RAM_TEST_SIZE                 0x00400000

/** RAM_SPEED_TEST parameters definition */
#define RAM_TEST_SPEED_START_ADDRESS                (RAM_TEST_ADDRESS_BEGIN)
#define RAM_TEST_SPEED_TRANSFERT_SRC                (RAM_TEST_ADDRESS_BEGIN + RAM_TEST_SIZE / 2)
#define RAM_TEST_SPEED_TRANSFERT_DEST               (RAM_TEST_ADDRESS_BEGIN)

#define RAM_TEST_SPEED_PATTERN32                    0xAAAAAAAA
#define RAM_TEST_SPEED_PATTERN16                    0xAAAA
#define RAM_TEST_SPEED_PATTERN8                     0xAA

#endif
