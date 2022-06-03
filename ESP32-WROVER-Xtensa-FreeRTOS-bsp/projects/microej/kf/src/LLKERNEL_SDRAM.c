/*
 * C
 *
 * Copyright 2013-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/* Includes ------------------------------------------------------------------*/

#include "LLKERNEL_impl.h"
#include "BESTFIT_ALLOCATOR.h"
#include <stdio.h>
#include <string.h>
#include "bsp_util.h"

#include "esp_attr.h"

// Define size to allocate for KF working buffer
#define KERNEL_WORKING_BUFFER_SIZE 0x100000

// Declare MicroEJ KF working buffer
static uint8_t EXT_RAM_ATTR kernel_working_buffer[KERNEL_WORKING_BUFFER_SIZE] = {0};

BESTFIT_ALLOCATOR allocatorInstance;
uint32_t KERNEL_allocationInitialized;

/* API -----------------------------------------------------------------------*/

void* LLKERNEL_IMPL_allocate(int32_t size){
	if(KERNEL_allocationInitialized == 0){
		// lazy init
		BESTFIT_ALLOCATOR_new(&allocatorInstance);
		BESTFIT_ALLOCATOR_initialize(&allocatorInstance, (int32_t)(&kernel_working_buffer[0]), (int32_t)(&kernel_working_buffer[KERNEL_WORKING_BUFFER_SIZE]));
		KERNEL_allocationInitialized = 1;
	}

        void* allocationStart = BESTFIT_ALLOCATOR_allocate(&allocatorInstance, size);
#ifdef ALLOCATOR_DEBUG
	printf("LLKERNEL_IMPL_allocate %d %p \n", size, allocationStart);
#endif
        return (void*)allocationStart;
}

void LLKERNEL_IMPL_free(void* block){
    BESTFIT_ALLOCATOR_free(&allocatorInstance, block);
#ifdef ALLOCATOR_DEBUG
    printf("LLKERNEL_IMPL_free %p \n", block);
#endif
}

