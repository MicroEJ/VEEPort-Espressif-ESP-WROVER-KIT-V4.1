/*
 * C
 *
 * Copyright 2021-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#include "fs_configuration.h"

#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"

// Mount point for the partition
const char *mount_point = "/sdcard";

void LLFS_ESP32_init_sdcard(void) {
	sdmmc_host_t host_config = SDMMC_HOST_DEFAULT();
	host_config.flags = SDMMC_HOST_FLAG_1BIT; // Use 1-line mode, otherwise it will overlap with the second UART GPIO 4.
	host_config.max_freq_khz = SDMMC_FREQ_HIGHSPEED;

	sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
	slot_config.width = 1;
	
	esp_vfs_fat_mount_config_t mount_config = {
		.format_if_mount_failed = true,
		.max_files = 5,
		.allocation_unit_size = 16 * 1024
	};

	ESP_ERROR_CHECK(esp_vfs_fat_sdmmc_mount(mount_point, &host_config, &slot_config, &mount_config, NULL));
}