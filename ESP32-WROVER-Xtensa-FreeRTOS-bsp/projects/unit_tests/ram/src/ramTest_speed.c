/*
 * C
 *
 * Copyright 2018 IS2T. All rights reserved.
 * IS2T PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ramTest_speed-intern.h"

/** public variables */

/** private constant definition */

/** private variable definitions */

/** private function declaration */

#ifdef RAM_TEST_32BIT_WORLD
    double RAM_TEST_SPEED_getWriteSpeed32(void);
    double RAM_TEST_SPEED_getReadSpeed32(void);
    double RAM_TEST_SPEED_getTransfertSpeed32(void);
#endif

#ifdef RAM_TEST_16BIT_WORLD
    double RAM_TEST_SPEED_getWriteSpeed16(void);
    double RAM_TEST_SPEED_getReadSpeed16(void);
    double RAM_TEST_SPEED_getTransfertSpeed16(void);
#endif

#ifdef RAM_TEST_8BIT_WORLD
    double RAM_TEST_SPEED_getWriteSpeed8(void);
    double RAM_TEST_SPEED_getReadSpeed8(void);
    double RAM_TEST_SPEED_getTransfertSpeed8(void);
#endif


/** function definitions */
double ramTest_getAverageWriteSpeed(void)
{
    double avrSpeed = 0;
    double counter = 0;

    #if ((RAM_TEST_32BIT_WORLD) & RAM_TEST_WRITE_SPEED ) != 0
    avrSpeed += RAM_TEST_SPEED_getWriteSpeed32();
    counter++;
    #endif

    #if ((RAM_TEST_16BIT_WORLD) & RAM_TEST_WRITE_SPEED ) != 0
    avrSpeed += RAM_TEST_SPEED_getWriteSpeed16();
    counter++;
    #endif

    #if ((RAM_TEST_8BIT_WORLD) & RAM_TEST_WRITE_SPEED ) != 0
    avrSpeed += RAM_TEST_SPEED_getWriteSpeed8();
    counter++;
    #endif

    if (counter != 0)
    {
        return (avrSpeed / counter);
    }
    else
    {
        return 0;
    }
}

double ramTest_getAverageReadSpeed(void)
{
    double avrSpeed = 0;
    double counter = 0;

    #if ((RAM_TEST_32BIT_WORLD) & RAM_TEST_READ_SPEED ) != 0
    avrSpeed += RAM_TEST_SPEED_getReadSpeed32();
    counter++;
    #endif

    #if ((RAM_TEST_16BIT_WORLD) & RAM_TEST_READ_SPEED ) != 0
    avrSpeed += RAM_TEST_SPEED_getReadSpeed16();
    counter++;
    #endif

    #if ((RAM_TEST_8BIT_WORLD) & RAM_TEST_READ_SPEED ) != 0
    avrSpeed += RAM_TEST_SPEED_getReadSpeed8();
    counter++;
    #endif

    if (counter != 0)
    {
        return (avrSpeed / counter);
    }
    else
    {
        return 0;
    }
}

double ramTest_getAverageTransfertSpeed(void)
{
    double avrSpeed = 0;
    double counter = 0;

    #if ((RAM_TEST_32BIT_WORLD) & RAM_TEST_TRANSFERT_SPEED ) != 0
    avrSpeed += RAM_TEST_SPEED_getTransfertSpeed32();
    counter++;
    #endif

    #if ((RAM_TEST_16BIT_WORLD) & RAM_TEST_TRANSFERT_SPEED ) != 0
    avrSpeed += RAM_TEST_SPEED_getTransfertSpeed16();
    counter++;
    #endif

    #if ((RAM_TEST_8BIT_WORLD) & RAM_TEST_TRANSFERT_SPEED ) != 0
    avrSpeed += RAM_TEST_SPEED_getTransfertSpeed8();
    counter++;
    #endif

    if (counter != 0)
    {
        return (avrSpeed / counter);
    }
    else
    {
        return 0;
    }
}

#ifdef RAM_TEST_32BIT_WORLD

#if ((RAM_TEST_32BIT_WORLD) & RAM_TEST_WRITE_SPEED ) != 0
double RAM_TEST_SPEED_getWriteSpeed32(void)
{
    int64_t time;
    double speed;

    time = TEST_TIME_getTime();
    RAM_TEST_SPEED_writeBlock32((volatile uint32_t *) RAM_TEST_SPEED_START_ADDRESS, RAM_TEST_SPEED_PATTERN32, RAM_TEST_SIZE);
    time = TEST_TIME_getTime() - time;

    speed = RAM_TEST_SIZE / time;   /* bytes per us */
    speed *= 1000.0;                /* bytes per ms */
    speed *= 1000.0;                /* bytes per  s */

    TEST_DBG("RAM_TEST_Speed with 32-bit write access : %f MBytes/s \n", (speed / 1024.0) / 1024.0);
    return speed;
}
#endif

#if ((RAM_TEST_32BIT_WORLD) & RAM_TEST_READ_SPEED ) != 0
double RAM_TEST_SPEED_getReadSpeed32(void)
{
    int64_t time;
    double speed;

    time = TEST_TIME_getTime();
    RAM_TEST_SPEED_readBlock32((volatile uint32_t *) RAM_TEST_SPEED_START_ADDRESS, RAM_TEST_SIZE);
    time = TEST_TIME_getTime() - time;

    speed = RAM_TEST_SIZE / time;   /* bytes per us */
    speed *= 1000.0;                /* bytes per ms */
    speed *= 1000.0;                /* bytes per  s */

    TEST_DBG("RAM_TEST_Speed with 32-bit read access : %f MBytes/s \n", (speed / 1024.0) / 1024.0);
    return speed;
}
#endif

#if ((RAM_TEST_32BIT_WORLD) & RAM_TEST_TRANSFERT_SPEED ) != 0
double RAM_TEST_SPEED_getTransfertSpeed32(void)
{
    int64_t time;
    double speed;

    time = TEST_TIME_getTime();
    RAM_TEST_SPEED_transfertBlock32((volatile uint32_t *) RAM_TEST_SPEED_TRANSFERT_SRC, (volatile uint32_t *) RAM_TEST_SPEED_TRANSFERT_DEST, RAM_TEST_SIZE / 2);
    time = TEST_TIME_getTime() - time;

    speed = (RAM_TEST_SIZE / 2) / time;   /* bytes per us */
    speed *= 1000.0;                      /* bytes per ms */
    speed *= 1000.0;                      /* bytes per  s */

    TEST_DBG("RAM_TEST_Speed with 32-bit transfert access : %f MBytes/s \n", (speed / 1024.0) / 1024.0);
    return speed;
}
#endif

#endif

#ifdef RAM_TEST_16BIT_WORLD

#if ((RAM_TEST_16BIT_WORLD) & RAM_TEST_WRITE_SPEED ) != 0
double RAM_TEST_SPEED_getWriteSpeed16(void)
{
    int64_t time;
    double speed;

    time = TEST_TIME_getTime();
    RAM_TEST_SPEED_writeBlock16((volatile uint16_t *) RAM_TEST_SPEED_START_ADDRESS, RAM_TEST_SPEED_PATTERN16, RAM_TEST_SIZE);
    time = TEST_TIME_getTime() - time;

    speed = RAM_TEST_SIZE / time;   /* bytes per us */
    speed *= 1000.0;                /* bytes per ms */
    speed *= 1000.0;                /* bytes per  s */

    TEST_DBG("RAM_TEST_Speed with 16-bit write access : %f MBytes/s \n", (speed / 1024.0) / 1024.0);
    return speed;
}
#endif

#if ((RAM_TEST_16BIT_WORLD) & RAM_TEST_READ_SPEED ) != 0
double RAM_TEST_SPEED_getReadSpeed16(void)
{
    int64_t time;
    double speed;

    time = TEST_TIME_getTime();
    RAM_TEST_SPEED_readBlock16((volatile uint16_t *) RAM_TEST_SPEED_START_ADDRESS, RAM_TEST_SIZE);
    time = TEST_TIME_getTime() - time;

    speed = RAM_TEST_SIZE / time;   /* bytes per us */
    speed *= 1000.0;                /* bytes per ms */
    speed *= 1000.0;                /* bytes per  s */

    TEST_DBG("RAM_TEST_Speed with 16-bit read access : %f MBytes/s \n", (speed / 1024.0) / 1024.0);
    return speed;
}
#endif

#if ((RAM_TEST_16BIT_WORLD) & RAM_TEST_TRANSFERT_SPEED ) != 0
double RAM_TEST_SPEED_getTransfertSpeed16(void)
{
    int64_t time;
    double speed;

    time = TEST_TIME_getTime();
    RAM_TEST_SPEED_transfertBlock16((volatile uint16_t *) RAM_TEST_SPEED_TRANSFERT_SRC, (volatile uint16_t *) RAM_TEST_SPEED_TRANSFERT_DEST, RAM_TEST_SIZE / 2);
    time = TEST_TIME_getTime() - time;

    speed = (RAM_TEST_SIZE / 2) / time;   /* bytes per us */
    speed *= 1000.0;                      /* bytes per ms */
    speed *= 1000.0;                      /* bytes per  s */

    TEST_DBG("RAM_TEST_Speed with 16-bit transfert access : %f MBytes/s \n", (speed / 1024.0) / 1024.0);
    return speed;
}
#endif

#endif

#ifdef RAM_TEST_8BIT_WORLD

#if ((RAM_TEST_8BIT_WORLD) & RAM_TEST_WRITE_SPEED ) != 0
double RAM_TEST_SPEED_getWriteSpeed8(void)
{
    int64_t time;
    double speed;

    time = TEST_TIME_getTime();
    RAM_TEST_SPEED_writeBlock8((volatile uint8_t *) RAM_TEST_SPEED_START_ADDRESS, RAM_TEST_SPEED_PATTERN8, RAM_TEST_SIZE);
    time = TEST_TIME_getTime() - time;

    speed = RAM_TEST_SIZE / time;   /* bytes per us */
    speed *= 1000.0;                /* bytes per ms */
    speed *= 1000.0;                /* bytes per  s */

    TEST_DBG("RAM_TEST_Speed with 8-bit write access : %f MBytes/s \n", (speed / 1024.0) / 1024.0);
    return speed;
}
#endif

#if ((RAM_TEST_8BIT_WORLD) & RAM_TEST_READ_SPEED ) != 0
double RAM_TEST_SPEED_getReadSpeed8(void)
{
    int64_t time;
    double speed;

    time = TEST_TIME_getTime();
    RAM_TEST_SPEED_readBlock8((volatile uint8_t *) RAM_TEST_SPEED_START_ADDRESS, RAM_TEST_SIZE);
    time = TEST_TIME_getTime() - time;

    speed = RAM_TEST_SIZE / time;   /* bytes per us */
    speed *= 1000.0;                /* bytes per ms */
    speed *= 1000.0;                /* bytes per  s */

    TEST_DBG("RAM_TEST_Speed with 8-bit read access : %f MBytes/s \n", (speed / 1024.0) / 1024.0);
    return speed;
}
#endif

#if ((RAM_TEST_8BIT_WORLD) & RAM_TEST_TRANSFERT_SPEED ) != 0
double RAM_TEST_SPEED_getTransfertSpeed8(void)
{
    int64_t time;
    double speed;

    time = TEST_TIME_getTime();
    RAM_TEST_SPEED_transfertBlock8((volatile uint8_t *) RAM_TEST_SPEED_TRANSFERT_SRC, (volatile uint8_t *) RAM_TEST_SPEED_TRANSFERT_DEST, RAM_TEST_SIZE / 2);
    time = TEST_TIME_getTime() - time;

    speed = (RAM_TEST_SIZE / 2) / time;   /* bytes per us */
    speed *= 1000.0;                      /* bytes per ms */
    speed *= 1000.0;                      /* bytes per  s */

    TEST_DBG("RAM_TEST_Speed with 8-bit transfert access : %f MBytes/s \n", (speed / 1024.0) / 1024.0);
    return speed;
}
#endif

#endif


__attribute__((weak)) void RAM_TEST_SPEED_writeBlock32(volatile uint32_t *address, volatile uint32_t value, uint32_t size)
{
    uint32_t i;

    for (i = 0; i < (size / sizeof(uint32_t)); i++)
    {
        *address++ = value;
    }
}

__attribute__((weak)) void RAM_TEST_SPEED_readBlock32(volatile uint32_t *address, uint32_t size)
{
    volatile uint32_t value;
    uint32_t i;

    for (i = 0; i < (size / sizeof(uint32_t)); i++)
    {
        value = *address++;
    }
    (void)value;
}

__attribute__((weak)) void RAM_TEST_SPEED_transfertBlock32(volatile uint32_t *src, volatile uint32_t *dest, uint32_t size)
{
    uint32_t i;

    for (i = 0; i < (size / sizeof(uint32_t)); i++)
    {
        *dest++ = *src++;
    }
}

__attribute__((weak)) void RAM_TEST_SPEED_writeBlock16(volatile uint16_t *address, volatile uint16_t value, uint32_t size)
{
    uint32_t i;

    for (i = 0; i < (size / sizeof(uint16_t)); i++)
    {
        *address++ = value;
    }
}

__attribute__((weak)) void RAM_TEST_SPEED_readBlock16(volatile uint16_t * address, uint32_t size)
{
    volatile uint16_t value;
    uint32_t i;

    for (i = 0; i < (size / sizeof(uint16_t)); i++)
    {
        value = *address++;
    }
    (void)value;
}

__attribute__((weak)) void RAM_TEST_SPEED_transfertBlock16(volatile uint16_t *src, volatile uint16_t *dest, uint32_t size)
{
    uint32_t i;

    for (i = 0; i < (size / sizeof(uint16_t)); i++)
    {
        *dest++ = *src++;
    }
}

__attribute__((weak)) void RAM_TEST_SPEED_writeBlock8(volatile uint8_t *address, volatile uint8_t value, uint32_t size)
{
    uint32_t i;

    for (i = 0; i < (size / sizeof(uint8_t)); i++)
    {
        *address++ = value;
    }
}

__attribute__((weak)) void RAM_TEST_SPEED_readBlock8(volatile uint8_t *address, uint32_t size)
{
    volatile uint8_t value;
    uint32_t i;

    for (i = 0; i < (size / sizeof(uint8_t)); i++)
    {
        value = *address++;
    }
    (void)value;
}

__attribute__((weak)) void RAM_TEST_SPEED_transfertBlock8(volatile uint8_t *src, volatile uint8_t *dest, uint32_t size)
{
    uint32_t i;

    for (i = 0; i < (size / sizeof(uint8_t)); i++)
    {
        *dest++ = *src++;
    }
}
