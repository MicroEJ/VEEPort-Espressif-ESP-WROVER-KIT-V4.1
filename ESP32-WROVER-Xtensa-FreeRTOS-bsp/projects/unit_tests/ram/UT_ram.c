/*
 * C
 *
 * Copyright 2018 IS2T. All rights reserved.
 * IS2T PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */

#include <embUnit/embUnit.h>
#include "ramTest-intern.h"
#include "ramTest_speed-intern.h"

static void memory_cleanup_f(void)
{
    uint32_t *pointer = (uint32_t*) RAM_TEST_ADDRESS_BEGIN;
    uint32_t i;

    for (i = 0; i < (RAM_TEST_SIZE / (sizeof(uint32_t))); i++)
    {
        *(pointer + i) = 0x00;
    }
}

static void setUp(void)
{
    memory_cleanup_f();
}

static void tearDown(void)
{
    memory_cleanup_f();
}

#if (((RAM_TEST_32BIT_WORLD) & (RAM_TEST_ALL_FUNCTIONAL)) != 0)
static void ram_test_32bits_world_f(void)
{
    TEST_ASSERT_EQUAL_INT(RAM_TEST_CheckAllPatternAllMemory32(), 0);
    TEST_ASSERT_EQUAL_INT(RAM_TEST_CheckAllPatternAllMemory32(), 0);

    #if ((RAM_TEST_ADDRESS_BIT & RAM_TEST_32BIT_WORLD) != 0)
    TEST_ASSERT_EQUAL_INT(RAM_TEST_CheckAddress32(), 0);
    #endif
}
#endif

#if (((RAM_TEST_16BIT_WORLD) & (RAM_TEST_ALL_FUNCTIONAL)) != 0)
static void ram_test_16bits_world_f(void)
{
    TEST_ASSERT_EQUAL_INT(RAM_TEST_CheckAllPatternAllMemory16(), 0);
    TEST_ASSERT_EQUAL_INT(RAM_TEST_CheckAllPatternAllMemory16(), 0);

    #if ((RAM_TEST_ADDRESS_BIT & RAM_TEST_16BIT_WORLD) != 0)
    TEST_ASSERT_EQUAL_INT(RAM_TEST_CheckAddress16(), 0);
    #endif
}
#endif

#if (((RAM_TEST_8BIT_WORLD) & (RAM_TEST_ALL_FUNCTIONAL)) != 0)
static void ram_test_8bits_world_f(void)
{
    TEST_ASSERT_EQUAL_INT(RAM_TEST_CheckAllPatternAllMemory8(), 0);
    TEST_ASSERT_EQUAL_INT(RAM_TEST_CheckAllPatternAllMemory8(), 0);

    #if ((RAM_TEST_ADDRESS_BIT & RAM_TEST_8BIT_WORLD) != 0)
    TEST_ASSERT_EQUAL_INT(RAM_TEST_CheckAddress8(), 0);
	#endif
}
#endif

static void ram_test_read_speed_f(void)
{
    printf("RAM_TEST_Speed average read access (according to your configuration file 8/16/32 bits) : %f MBytes/s \n" , (ramTest_getAverageReadSpeed() / 1024.0) / 1024.0);
}

static void ram_test_write_speed_f(void)
{
    printf("RAM_TEST_Speed average write access (according to your configuration file 8/16/32 bits) : %f MBytes/s \n" , (ramTest_getAverageWriteSpeed() / 1024.0) / 1024.0);
}

static void ram_test_transfert_speed_f(void)
{
    printf("RAM_TEST_Speed average transfert access (according to your configuration file 8/16/32 bits) : %f MBytes/s \n" , (ramTest_getAverageTransfertSpeed() / 1024.0) / 1024.0);
}

TestRef ram_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        #if (((RAM_TEST_32BIT_WORLD) & (RAM_TEST_ALL_FUNCTIONAL)) != 0)
        new_TestFixture("ram_test_32bits_world_f", ram_test_32bits_world_f),
        #endif
        #if (((RAM_TEST_16BIT_WORLD) & (RAM_TEST_ALL_FUNCTIONAL)) != 0)
        new_TestFixture("ram_test_16bits_world_f", ram_test_16bits_world_f),
        #endif
        #if (((RAM_TEST_8BIT_WORLD) & (RAM_TEST_ALL_FUNCTIONAL)) != 0)
        new_TestFixture("ram_test_8bits_world_f", ram_test_8bits_world_f),
        #endif
        new_TestFixture("ram_test_read_speed_f", ram_test_read_speed_f),
        new_TestFixture("ram_test_write_speed_f", ram_test_write_speed_f),
        new_TestFixture("ram_test_transfert_speed_f", ram_test_transfert_speed_f),
    };

    EMB_UNIT_TESTCALLER(ramTest, "ramTest", setUp, tearDown, fixtures);

    return (TestRef)&ramTest;
}
