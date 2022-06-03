/*
 * C
 *
 * Copyright 2018 IS2T. All rights reserved.
 * IS2T PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */


#include "ramTest-conf.h"
#include "esp_timer.h"
#include <stdlib.h>
#include <stdio.h>

#ifndef __RAM_TEST_SPEED_INTERN_H
#define __RAM_TEST_SPEED_INTERN_H

/** to have a test precision with speed you must provide a function
 * that get us since start of application */
#define TEST_TIME_getTime()   (esp_timer_get_time())

/** private function declaration */
#ifdef RAM_TEST_32BIT_WORLD
    #if ((RAM_TEST_32BIT_WORLD) & RAM_TEST_WRITE_SPEED ) != 0
    __attribute__((weak)) void RAM_TEST_SPEED_writeBlock32(volatile uint32_t *address, volatile uint32_t value, uint32_t size);
    #endif
    #if ((RAM_TEST_32BIT_WORLD) & RAM_TEST_READ_SPEED ) != 0
    __attribute__((weak)) void RAM_TEST_SPEED_readBlock32(volatile uint32_t *address, uint32_t size);
    #endif
    #if ((RAM_TEST_32BIT_WORLD) & RAM_TEST_TRANSFERT_SPEED ) != 0
    __attribute__((weak)) void RAM_TEST_SPEED_transfertBlock32(volatile uint32_t *src, volatile uint32_t *dest, uint32_t size);
    #endif
#endif

#ifdef RAM_TEST_16BIT_WORLD

    #if ((RAM_TEST_16BIT_WORLD) & RAM_TEST_WRITE_SPEED ) != 0
    __attribute__((weak)) void RAM_TEST_SPEED_writeBlock16(volatile uint16_t *address, volatile uint16_t value, uint32_t size);
    #endif
    #if ((RAM_TEST_16BIT_WORLD) & RAM_TEST_READ_SPEED ) != 0
    __attribute__((weak)) void RAM_TEST_SPEED_readBlock16(volatile uint16_t *address, uint32_t size);
    #endif
    #if ((RAM_TEST_16BIT_WORLD) & RAM_TEST_TRANSFERT_SPEED ) != 0
    __attribute__((weak)) void RAM_TEST_SPEED_transfertBlock16(volatile uint16_t *src, volatile uint16_t *dest, uint32_t size);
    #endif
#endif

#ifdef RAM_TEST_8BIT_WORLD

    #if ((RAM_TEST_8BIT_WORLD) & RAM_TEST_WRITE_SPEED ) != 0
    __attribute__((weak)) void RAM_TEST_SPEED_writeBlock8(volatile uint8_t *address, volatile uint8_t value, uint32_t size);
    #endif
    #if ((RAM_TEST_8BIT_WORLD) & RAM_TEST_READ_SPEED ) != 0
    __attribute__((weak)) void RAM_TEST_SPEED_readBlock8(volatile uint8_t *address, uint32_t size);
    #endif
    #if ((RAM_TEST_8BIT_WORLD) & RAM_TEST_TRANSFERT_SPEED ) != 0
    __attribute__((weak)) void RAM_TEST_SPEED_transfertBlock8(volatile uint8_t *src, volatile uint8_t *dest, uint32_t size);
    #endif
#endif

#endif
