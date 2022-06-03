/*
 * C
 *
 * Copyright 2019-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#ifndef BLUETOOTH_HELPER_H_
#define BLUETOOTH_HELPER_H_

#include "esp_gap_ble_api.h"
#include "esp_bt_defs.h"
#include "esp_gatt_defs.h"
#include "LLBLUETOOTH_defines.h"
#include "bt_data.h"

/*
 * Converts the given Bluetooth address from ESP32 format to LLBLUETOOTH format.
 */
LLBLUETOOTH_address_t BT_HELPER_write_addr(const uint8_t *addr, esp_ble_addr_type_t addr_type);

/*
 * Converts the given Bluetooth UUID from ESP32 format to LLBLUETOOTH format.
 */
LLBLUETOOTH_uuid_t BT_HELPER_write_uuid(const esp_bt_uuid_t *uuid);

/*
 * Converts the given Bluetooth address from LLBLUETOOTH format to ESP32 format.
 */
void BT_HELPER_read_addr(const LLBLUETOOTH_address_t *lladdr, uint8_t *addr, esp_ble_addr_type_t *addr_type);

/*
 * Converts the given Bluetooth UUID from LLBLUETOOTH format to ESP32 format.
 */
esp_bt_uuid_t BT_HELPER_read_uuid(const LLBLUETOOTH_uuid_t *lluuid);

/*
 * Converts the given attribute permissions from LLBLUETOOTH format to ESP32 format.
 */
esp_gatt_perm_t BT_HELPER_read_permissions(uint8_t llperm);

/*
 * Converts the given service from LLBLUETOOTH format to ESP32 format.
 */
BT_DATA_service_t BT_HELPER_read_service(const LLBLUETOOTH_gatts_service_t *llservice);

/*
 * Converts the given characteristic from LLBLUETOOTH format to ESP32 format.
 */
BT_DATA_characteristic_t BT_HELPER_read_characteristic(const LLBLUETOOTH_gatts_attribute_t *llattribute);

/*
 * Converts the given descriptor from LLBLUETOOTH format to ESP32 format.
 */
BT_DATA_descriptor_t BT_HELPER_read_descriptor(const LLBLUETOOTH_gatts_attribute_t *llattribute);

#endif
