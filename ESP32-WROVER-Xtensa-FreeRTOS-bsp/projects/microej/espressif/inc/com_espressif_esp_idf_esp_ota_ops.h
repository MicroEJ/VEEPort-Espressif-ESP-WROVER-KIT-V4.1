/*
 * C
 *
 * Copyright 2018-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#ifndef _com_espressif_esp_idf_esp_ota_ops
#define _com_espressif_esp_idf_esp_ota_ops 

/**
 * @file
 * @brief esp-idf low level API
 * @author MicroEJ Developer Team
 * @version 1.1.1
 * @date 13 November 2020
 */

#include <stdint.h>
#ifdef __cplusplus
	extern "C" {
#endif
int32_t Java_com_espressif_esp_1idf_esp_1ota_1ops_esp_1ota_1begin(int32_t partition, int32_t image_size);
int32_t Java_com_espressif_esp_1idf_esp_1ota_1ops_esp_1ota_1end();
int32_t Java_com_espressif_esp_1idf_esp_1ota_1ops_esp_1ota_1get_1boot_1partition(void);
int32_t Java_com_espressif_esp_1idf_esp_1ota_1ops_esp_1ota_1get_1next_1update_1partition(int32_t start_from);
int32_t Java_com_espressif_esp_1idf_esp_1ota_1ops_esp_1ota_1get_1running_1partition(void);
int32_t Java_com_espressif_esp_1idf_esp_1ota_1ops_esp_1ota_1set_1boot_1partition(int32_t partition);
int32_t Java_com_espressif_esp_1idf_esp_1ota_1ops_esp_1ota_1write(uint8_t* data, int32_t offset, int32_t size);
int32_t Java_com_espressif_esp_1idf_esp_1ota_1ops_esp_1ota_1handle_1get(void);
#ifdef __cplusplus
	}
#endif
#endif
