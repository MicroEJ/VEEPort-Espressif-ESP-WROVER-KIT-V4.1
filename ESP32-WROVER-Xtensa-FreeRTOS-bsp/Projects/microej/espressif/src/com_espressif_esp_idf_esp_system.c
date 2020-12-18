/*
 * C
 *
 * Copyright 2018-2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
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
