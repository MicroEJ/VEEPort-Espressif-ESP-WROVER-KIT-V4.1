/*
 * C
 *
 * Copyright 2018 IS2T. All rights reserved.
 * IS2T PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "ramTest-intern.h"

#define RAM_TEST_PATTERN_32_0    (uint32_t)0x00000000
#define RAM_TEST_PATTERN_32_1    (uint32_t)0x11111111
#define RAM_TEST_PATTERN_32_2    (uint32_t)0xAAAAAAAA
#define RAM_TEST_PATTERN_32_3    (uint32_t)0x55555555
#define RAM_TEST_PATTERN_32_4    (uint32_t)0xFFFF0000
#define RAM_TEST_PATTERN_32_5    (uint32_t)0x0000FFFF
#define RAM_TEST_PATTERN_32_6    (uint32_t)0x12345678
#define RAM_TEST_PATTERN_32_7    (uint32_t)0x87654321

#define RAM_TEST_PATTERN_16_0    (uint16_t)0x0000
#define RAM_TEST_PATTERN_16_1    (uint16_t)0x1111
#define RAM_TEST_PATTERN_16_2    (uint16_t)0xAAAA
#define RAM_TEST_PATTERN_16_3    (uint16_t)0x5555
#define RAM_TEST_PATTERN_16_4    (uint16_t)0xFF00
#define RAM_TEST_PATTERN_16_5    (uint16_t)0x00FF
#define RAM_TEST_PATTERN_16_6    (uint16_t)0x1234
#define RAM_TEST_PATTERN_16_7    (uint16_t)0x5678
#define RAM_TEST_PATTERN_16_8    (uint16_t)0x8765
#define RAM_TEST_PATTERN_16_9    (uint16_t)0x4321

#define RAM_TEST_PATTERN_8_0    (uint8_t)0x00
#define RAM_TEST_PATTERN_8_1    (uint8_t)0x11
#define RAM_TEST_PATTERN_8_2    (uint8_t)0xAA
#define RAM_TEST_PATTERN_8_3    (uint8_t)0x55
#define RAM_TEST_PATTERN_8_4    (uint8_t)0xF0
#define RAM_TEST_PATTERN_8_5    (uint8_t)0x0F
#define RAM_TEST_PATTERN_8_6    (uint8_t)0x12
#define RAM_TEST_PATTERN_8_7    (uint8_t)0x34
#define RAM_TEST_PATTERN_8_8    (uint8_t)0x56
#define RAM_TEST_PATTERN_8_9    (uint8_t)0x78
#define RAM_TEST_PATTERN_8_10   (uint8_t)0x87
#define RAM_TEST_PATTERN_8_11   (uint8_t)0x65
#define RAM_TEST_PATTERN_8_12   (uint8_t)0x43
#define RAM_TEST_PATTERN_8_13   (uint8_t)0x21

#if (((RAM_TEST_32BIT_WORLD) & (RAM_TEST_ALL_FUNCTIONAL)) != 0)
const uint32_t ram_patterns32[]= {
    #if ((RAM_TEST_32BIT_WORLD) & RAM_TEST_ALL_LOW_DATA ) != 0
                                RAM_TEST_PATTERN_32_0,
    #endif
    #if ((RAM_TEST_32BIT_WORLD) & RAM_TEST_ALL_HIGH_DATA ) != 0
                                RAM_TEST_PATTERN_32_1,
    #endif
    #if ((RAM_TEST_32BIT_WORLD) & RAM_TEST_ODD_BIT_DATA ) != 0
                                RAM_TEST_PATTERN_32_2,
    #endif
    #if ((RAM_TEST_32BIT_WORLD) & RAM_TEST_EVEN_BIT_DATA ) != 0
                                RAM_TEST_PATTERN_32_3,
    #endif
    #if ((RAM_TEST_32BIT_WORLD) & RAM_TEST_HALF_HIGH_DATA ) != 0
                                RAM_TEST_PATTERN_32_4,
                                RAM_TEST_PATTERN_32_5,
    #endif
    #if ((RAM_TEST_32BIT_WORLD) & RAM_TEST_GENERIC_DATA ) != 0
                                RAM_TEST_PATTERN_32_6,
                                RAM_TEST_PATTERN_32_7
    #endif
};
#endif

#if (((RAM_TEST_16BIT_WORLD) & (RAM_TEST_ALL_FUNCTIONAL)) != 0)
const uint16_t ram_patterns16[]= {
    #if ((RAM_TEST_16BIT_WORLD) & RAM_TEST_ALL_LOW_DATA ) != 0
                                RAM_TEST_PATTERN_16_0,
    #endif
    #if ((RAM_TEST_16BIT_WORLD) & RAM_TEST_ALL_HIGH_DATA ) != 0
                                RAM_TEST_PATTERN_16_1,
    #endif
    #if ((RAM_TEST_16BIT_WORLD) & RAM_TEST_ODD_BIT_DATA ) != 0
                                RAM_TEST_PATTERN_16_2,
    #endif
    #if ((RAM_TEST_16BIT_WORLD) & RAM_TEST_EVEN_BIT_DATA ) != 0
                                RAM_TEST_PATTERN_16_3,
    #endif
    #if ((RAM_TEST_16BIT_WORLD) & RAM_TEST_HALF_HIGH_DATA ) != 0
                                RAM_TEST_PATTERN_16_4,
                                RAM_TEST_PATTERN_16_5,
    #endif
    #if ((RAM_TEST_16BIT_WORLD) & RAM_TEST_GENERIC_DATA ) != 0
                                RAM_TEST_PATTERN_16_6,
                                RAM_TEST_PATTERN_16_7,
                                RAM_TEST_PATTERN_16_8,
                                RAM_TEST_PATTERN_16_9
    #endif
};
#endif

#if (((RAM_TEST_8BIT_WORLD) & (RAM_TEST_ALL_FUNCTIONAL)) != 0)
const uint8_t ram_patterns8[]= {
    #if ((RAM_TEST_8BIT_WORLD) & RAM_TEST_ALL_LOW_DATA ) != 0
                                RAM_TEST_PATTERN_8_0,
    #endif
    #if ((RAM_TEST_8BIT_WORLD) & RAM_TEST_ALL_HIGH_DATA ) != 0
                                RAM_TEST_PATTERN_8_1,
    #endif
    #if ((RAM_TEST_8BIT_WORLD) & RAM_TEST_ODD_BIT_DATA ) != 0
                                RAM_TEST_PATTERN_8_2,
    #endif
    #if ((RAM_TEST_8BIT_WORLD) & RAM_TEST_EVEN_BIT_DATA ) != 0
                                RAM_TEST_PATTERN_8_3,
    #endif
    #if ((RAM_TEST_8BIT_WORLD) & RAM_TEST_HALF_HIGH_DATA ) != 0
                                RAM_TEST_PATTERN_8_4,
                                RAM_TEST_PATTERN_8_5,
    #endif
    #if ((RAM_TEST_8BIT_WORLD) & RAM_TEST_GENERIC_DATA ) != 0
                                RAM_TEST_PATTERN_8_6,
                                RAM_TEST_PATTERN_8_7,
                                RAM_TEST_PATTERN_8_8,
                                RAM_TEST_PATTERN_8_9,
                                RAM_TEST_PATTERN_8_10,
                                RAM_TEST_PATTERN_8_11,
                                RAM_TEST_PATTERN_8_12,
                                RAM_TEST_PATTERN_8_13
    #endif
};
#endif

#if (((RAM_TEST_32BIT_WORLD) & (RAM_TEST_ALL_FUNCTIONAL)) != 0)
static uint32_t checkPatternAllMemory32(uint32_t pattern)
{
    uint32_t *pointer= (uint32_t *) RAM_TEST_ADDRESS_BEGIN;
    uint32_t i;
    uint32_t error_count = 0;

    for (i = 0; i < (RAM_TEST_SIZE / (sizeof(uint32_t))); i++)
    {
        *(pointer + i) = pattern;
    }

    for (i = 0 ; i <(RAM_TEST_SIZE / (sizeof(uint32_t))); i++)
    {
        if (*(pointer + i) != pattern)
        {
            error_count++;
        }
    }

    if (error_count != 0)
    {
        TEST_DBG("RAM_TEST_CheckPatternAllMemory with 32-bit access : data bus problems - pattern 0x%x ( %i errors )\n", (unsigned int)pattern, (int)error_count);
    }
    else
    {
        TEST_DBG("RAM_TEST_CheckPatternAllMemory with 32-bit access : data bus pattern 0x%x - no error \n", (unsigned int)pattern);
    }
    RAM_TEST_MemoryCleanup();

    return error_count;
}

static uint32_t checkPatternMemory32(uint32_t pattern)
{
    uint32_t *pointer= (uint32_t *) RAM_TEST_ADDRESS_BEGIN;
    uint32_t i;
    uint32_t error_count = 0;

    for (i = 0; i < (RAM_TEST_SIZE / (sizeof(uint32_t))); i++)
    {
        *(pointer + i) = pattern;
        if (*(pointer + i) != pattern)
        {
            error_count++;
        }
    }

    if (error_count != 0)
    {
        TEST_DBG("RAM_TEST_CheckPatternMemory with 32-bit access: data bus problems - pattern 0x%x ( %i errors )\n", (unsigned int)pattern, (int)error_count);
    }
    else
    {
        TEST_DBG("RAM_TEST_CheckPatternMemory with 32-bit access: data bus pattern 0x%x - no error \n", (unsigned int)pattern);
    }
    RAM_TEST_MemoryCleanup();

    return error_count;
}

uint32_t RAM_TEST_CheckAllPatternAllMemory32(void)
{
    uint32_t i;
    uint32_t error_count = 0;

    for (i = 0; i < (sizeof(ram_patterns32) / sizeof(uint32_t)); i++)
    {
        error_count += checkPatternAllMemory32(ram_patterns32[i]);
    }

    return error_count;
}

uint32_t RAM_TEST_CheckAllPatternMemory32(void)
{
    uint32_t i;
    uint32_t error_count = 0;

    for (i = 0; i < (sizeof(ram_patterns32) / sizeof(uint32_t)); i++)
    {
        error_count += checkPatternMemory32(ram_patterns32[i]);
    }

    return error_count;
}

uint32_t RAM_TEST_CheckAddress32(void)
{
    uint32_t *pointer = (uint32_t *) RAM_TEST_ADDRESS_BEGIN;
    uint32_t i;
    uint32_t error_count = 0;

    for (i = 0; i < (RAM_TEST_SIZE / (sizeof(uint32_t))); i++)
    {
        *(pointer + i) = i;
    }

    for (i = 0; i < (RAM_TEST_SIZE / (sizeof(uint32_t))); i++)
    {
        if (*(pointer + i) != i)
        {
            error_count++;
        }
    }

    if (error_count != 0)
    {
        TEST_DBG("RAM_TEST: address bus problems : %i errors \n", (int)error_count);
    }
    else
    {
        TEST_DBG("RAM_TEST: address bus  no error \n");
    }

    return error_count;
}
#endif

#if (((RAM_TEST_16BIT_WORLD) & (RAM_TEST_ALL_FUNCTIONAL)) != 0)
static uint32_t checkPatternAllMemory16(uint16_t pattern)
{
    uint16_t *pointer = (uint16_t *) RAM_TEST_ADDRESS_BEGIN;
    uint32_t i;
    uint32_t error_count = 0;

    for (i = 0; i < (RAM_TEST_SIZE / (sizeof(uint16_t))); i++)
    {
        *(pointer + i) = pattern;
    }

    for (i = 0; i < (RAM_TEST_SIZE / (sizeof(uint16_t))); i++)
    {
        if (*(pointer + i) != pattern)
        {
            error_count++;
        }
    }

    if (error_count != 0)
    {
        TEST_DBG("RAM_TEST_CheckPatternAllMemory with 16-bit access: data bus problems - pattern 0x%x ( %i errors )\n", (unsigned int)pattern, (int)error_count);
    }
    else
    {
        TEST_DBG("RAM_TEST_CheckPatternAllMemory with 16-bit access: data bus pattern 0x%x - no error \n", (unsigned int)pattern);
    }
    RAM_TEST_MemoryCleanup();
    return error_count;
}

static uint32_t checkPatternMemory16(uint16_t pattern)
{
    uint16_t *pointer = (uint16_t *) RAM_TEST_ADDRESS_BEGIN;
    uint32_t i;
    uint32_t error_count = 0;

    for (i = 0; i < (RAM_TEST_SIZE / (sizeof(uint16_t))); i++)
    {
        *(pointer + i) = pattern;
        if (*(pointer + i) != pattern)
        {
            error_count++;
        }
    }

    if (error_count != 0)
    {
        TEST_DBG("RAM_TEST_CheckPatternMemory with 16-bit access: data bus problems - pattern 0x%x ( %i errors )\n", (unsigned int)pattern, (int)error_count);
    }
    else
    {
        TEST_DBG("RAM_TEST_CheckPatternMemorywith 16-bit access: data bus pattern 0x%x - no error \n", (unsigned int)pattern);
    }
    RAM_TEST_MemoryCleanup();

    return error_count;
}

uint32_t RAM_TEST_CheckAllPatternAllMemory16(void)
{
    uint32_t i;
    uint32_t error_count = 0;

    for (i = 0; i < (sizeof(ram_patterns16) / sizeof(uint16_t)); i++)
    {
        error_count += checkPatternAllMemory16(ram_patterns16[i]);
    }

    return error_count;
}

uint32_t RAM_TEST_CheckAllPatternMemory16(void)
{
    uint32_t i;
    uint32_t error_count = 0;

    for (i = 0; i < (sizeof(ram_patterns16) / sizeof(uint16_t)); i++)
    {
        error_count += checkPatternMemory16(ram_patterns16[i]);
    }

    return error_count;
}

uint32_t RAM_TEST_CheckAddress16(void)
{
    uint16_t *pointer = (uint16_t *) RAM_TEST_ADDRESS_BEGIN;
    uint32_t i, p = 0;
    uint32_t error_count = 0;

    while (p < ((RAM_TEST_SIZE ) / (sizeof(uint16_t) * (UINT16_MAX + 1))))
    {
        for (i = 0; i < UINT16_MAX; i++)
        {
            *(pointer + (p * UINT16_MAX) + i) = i;
        }

        for (i = 0; i < UINT16_MAX; i++)
        {
            if (*(pointer + (p * UINT16_MAX) + i) != i)
            {
                error_count++;
            }
        }

        p++;
    }

    if (error_count != 0)
    {
        TEST_DBG("RAM_TEST: address bus problems : %i errors \n", (int)error_count);
    }
    else
    {
        TEST_DBG("RAM_TEST: address bus  no error \n");
    }

    return error_count;
}
#endif

#if (((RAM_TEST_8BIT_WORLD) & (RAM_TEST_ALL_FUNCTIONAL)) != 0)
static uint32_t checkPatternAllMemory8(uint8_t pattern)
{
    uint8_t *pointer = (uint8_t *) RAM_TEST_ADDRESS_BEGIN;
    uint32_t i;
    uint32_t error_count = 0;

    for (i = 0; i < (RAM_TEST_SIZE / (sizeof(uint8_t))); i++)
    {
        *(pointer + i) = pattern;
    }

    for (i = 0; i < (RAM_TEST_SIZE / (sizeof(uint8_t))); i++)
    {
        if (*(pointer + i) != pattern)
        {
            error_count++;
        }
    }

    if (error_count != 0)
    {
        TEST_DBG("RAM_TEST_CheckPatternAllMemory with 8-bit access: data bus problems - pattern 0x%x ( %i errors )\n", (unsigned int)pattern, (int)error_count);
    }
    else
    {
        TEST_DBG("RAM_TEST_CheckPatternAllMemory with 8-bit access: data bus pattern 0x%x - no error \n", (unsigned int)pattern);
    }
    RAM_TEST_MemoryCleanup();

    return error_count;
}

static uint32_t checkPatternMemory8(uint8_t pattern)
{
    uint8_t *pointer = (uint8_t *) RAM_TEST_ADDRESS_BEGIN;
    uint32_t i;
    uint32_t error_count = 0;

    for (i = 0; i < (RAM_TEST_SIZE / (sizeof(uint8_t))); i++)
    {
        *(pointer + i) = pattern;
        if (*(pointer + i) != pattern)
        {
            error_count++;
        }
    }

    if (error_count != 0)
    {
        TEST_DBG("RAM_TEST_CheckPatternMemory with 8-bit access: data bus problems - pattern 0x%x ( %i errors )\n", (unsigned int)pattern, (int)error_count);
    }
    else
    {
        TEST_DBG("RAM_TEST_CheckPatternMemory with 8-bit access: data bus pattern 0x%x - no error \n", (unsigned int)pattern);
    }
    RAM_TEST_MemoryCleanup();

    return error_count;
}

uint32_t RAM_TEST_CheckAllPatternAllMemory8(void)
{
    uint32_t i;
    uint32_t error_count = 0;

    for (i = 0; i < (sizeof(ram_patterns8) / sizeof(uint8_t)); i++)
    {
        error_count += checkPatternAllMemory8(ram_patterns8[i]);
    }

    return error_count;
}

uint32_t RAM_TEST_CheckAllPatternMemory8(void)
{
    uint32_t i;
    uint32_t error_count = 0;

    for (i = 0; i < (sizeof(ram_patterns8) / sizeof(uint8_t)); i++)
    {
        error_count += checkPatternMemory8(ram_patterns8[i]);
    }

    return error_count;
}

uint32_t RAM_TEST_CheckAddress8(void )
{
    uint8_t *pointer = (uint8_t *) RAM_TEST_ADDRESS_BEGIN;
    uint32_t i, p = 0;
    uint32_t error_count = 0;

    while (p < (RAM_TEST_SIZE / (sizeof(uint8_t) * (UINT8_MAX + 1))))
    {
        for (i = 0; i < UINT8_MAX; i++)
        {
            *(pointer + (p * UINT8_MAX) + i) = i;
        }

        for (i = 0; i < UINT8_MAX; i++)
        {
            if (*(pointer + (p * UINT8_MAX) + i) != i)
            {
                error_count++;
            }
        }

        p++;
    }

    if (error_count != 0)
    {
        TEST_DBG("RAM_TEST: address bus problems : %i errors \n", (int)error_count);
    }
    else
    {
        TEST_DBG("RAM_TEST: address bus  no error \n");
    }

    return error_count;
}
#endif

void RAM_TEST_MemoryCleanup(void)
{
    uint32_t *pointer = (uint32_t*) RAM_TEST_ADDRESS_BEGIN;
    uint32_t i;

    for (i = 0; i < (RAM_TEST_SIZE / (sizeof(uint32_t))); i++)
    {
        *(pointer + i) = 0x00;
    }
}
