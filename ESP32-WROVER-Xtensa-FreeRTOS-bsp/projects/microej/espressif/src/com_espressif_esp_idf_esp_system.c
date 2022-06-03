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

#include "com_espressif_esp_idf_esp_system.h"
#include "esp_system.h"

void Java_com_espressif_esp_1idf_esp_1system_esp_1restart(void) {
	esp_restart();
}
