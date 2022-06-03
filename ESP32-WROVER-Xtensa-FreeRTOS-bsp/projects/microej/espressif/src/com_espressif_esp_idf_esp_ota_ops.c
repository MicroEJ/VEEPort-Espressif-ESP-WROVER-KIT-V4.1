/*
 * C
 *
 * Copyright 2018-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief esp-idf low level API implementation
 * @author MicroEJ Developer Team
 * @version 1.1.1
 * @date 13 November 2020
 */

#include <stdint.h>
#include <stddef.h>
#include "com_espressif_esp_idf_esp_ota_ops.h"
#include "esp_err.h"
#include "esp_ota_ops.h"

static esp_ota_handle_t handle;

int32_t Java_com_espressif_esp_1idf_esp_1ota_1ops_esp_1ota_1begin(int32_t partition, int32_t image_size) {
	return esp_ota_begin((const esp_partition_t *)partition, (size_t)image_size, &handle);
}

int32_t Java_com_espressif_esp_1idf_esp_1ota_1ops_esp_1ota_1end() {
	return esp_ota_end(handle);
}

int32_t Java_com_espressif_esp_1idf_esp_1ota_1ops_esp_1ota_1get_1boot_1partition(void) {
	return (int32_t)esp_ota_get_boot_partition();
}

int32_t Java_com_espressif_esp_1idf_esp_1ota_1ops_esp_1ota_1get_1next_1update_1partition(int32_t start_from) {
	return (int32_t)esp_ota_get_next_update_partition((const esp_partition_t *)start_from);
}

int32_t Java_com_espressif_esp_1idf_esp_1ota_1ops_esp_1ota_1get_1running_1partition(void) {
	return (int32_t)esp_ota_get_running_partition();
}

int32_t Java_com_espressif_esp_1idf_esp_1ota_1ops_esp_1ota_1set_1boot_1partition(int32_t partition) {
	return esp_ota_set_boot_partition((const esp_partition_t*)partition);
}

int32_t Java_com_espressif_esp_1idf_esp_1ota_1ops_esp_1ota_1write(uint8_t* data, int32_t offset, int32_t size) {
	return esp_ota_write(handle, (const void*)(data+offset), (size_t)size);
}
