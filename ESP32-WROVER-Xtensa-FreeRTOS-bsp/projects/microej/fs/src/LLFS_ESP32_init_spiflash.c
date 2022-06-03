/*
 * C
 *
 * Copyright 2021-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#include "fs_configuration.h"

#include "esp_vfs_fat.h"
#include "esp_partition.h"
#include "wear_levelling.h"
#include "diskio_wl.h"
#include "diskio_impl.h"

// Mount path for the partition
const char *base_path = "/spiflash";

void LLFS_ESP32_init_spiflash(void) {
	wl_handle_t wl_handle;

	esp_vfs_fat_mount_config_t mount_config;
	mount_config.format_if_mount_failed = true;
	mount_config.max_files = 30;

	ESP_ERROR_CHECK(esp_vfs_fat_spiflash_mount(base_path, NULL, &mount_config, &wl_handle));
}