/*
 * C
 *
 * Copyright 2018-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#include <stdlib.h>

#include "esp_heap_caps.h"

#include "microej_allocator_configuration.h"
#include "microej_allocator.h"

#include "esp_log.h"
void* microej_calloc4tls(size_t nmemb, size_t size) {
	//ESP_LOGI(__func__, "allocate %d, %d\n", nmemb, size);
	return microej_calloc(nmemb, size);
}

void microej_free4tls(void *ptr) {
	//ESP_LOGI(__func__, "free %p\n", ptr);
	microej_free(ptr);;
}

void* microej_malloc(size_t size)
{
#ifdef CONFIG_MICROEJ_ALLOCATION_FROM_SPIRAM_FIRST
    return heap_caps_malloc_prefer(size, 2, MALLOC_CAP_DEFAULT|MALLOC_CAP_SPIRAM, MALLOC_CAP_DEFAULT|MALLOC_CAP_INTERNAL);
#else
    return malloc(size);
#endif
}

void* microej_calloc(size_t nmemb, size_t size)
{
#ifdef CONFIG_MICROEJ_ALLOCATION_FROM_SPIRAM_FIRST
    return heap_caps_calloc_prefer(nmemb, size, 2, MALLOC_CAP_DEFAULT|MALLOC_CAP_SPIRAM, MALLOC_CAP_DEFAULT|MALLOC_CAP_INTERNAL);
#else
    return calloc(nmemb, size);
#endif
}

void microej_free(void *ptr)
{
#ifdef CONFIG_MICROEJ_ALLOCATION_FROM_SPIRAM_FIRST
	return heap_caps_free(ptr);
#else
	return free(ptr);
#endif
}

