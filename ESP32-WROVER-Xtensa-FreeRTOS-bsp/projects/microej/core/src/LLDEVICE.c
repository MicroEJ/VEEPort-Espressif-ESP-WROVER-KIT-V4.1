/*
 * C
 *
 * Copyright 2018-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include <string.h>

#include "device.h"
#include "LLDEVICE_impl.h"
#include "esp_system.h"

uint8_t LLDEVICE_IMPL_getArchitecture(uint8_t* buffer, int32_t length)
{
	if (length >= 6) {
		buffer[0]= 'E';
		buffer[1]= 'S';
		buffer[2]= 'P';
		buffer[3]= '3';
		buffer[4]= '2';
		buffer[5]= 0;
		return 1;
	} 
	else {
		return 0;
	}
}

uint32_t LLDEVICE_IMPL_getId(uint8_t* buffer, int32_t length)
{
	if((length >= UNIQUE_DEVICE_ID_SIZE) && (ESP_OK == esp_efuse_mac_get_default(buffer)))
	{
		return UNIQUE_DEVICE_ID_SIZE;
	}
	return 0;
}

void LLDEVICE_IMPL_reboot(void)
{
	esp_restart();
}
