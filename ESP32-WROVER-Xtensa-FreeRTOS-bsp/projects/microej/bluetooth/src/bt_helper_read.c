/*
 * C
 *
 * Copyright 2019-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#include <stdio.h>
#include <string.h>
#include "bt_helper.h"

void BT_HELPER_read_addr(const LLBLUETOOTH_address_t *lladdr, uint8_t *addr, esp_ble_addr_type_t *addr_type)
{
	*addr_type = (lladdr->type == ADDRESS_TYPE_PUBLIC ? BLE_ADDR_TYPE_PUBLIC : BLE_ADDR_TYPE_RANDOM);
	memcpy(addr, lladdr->addr, ESP_BD_ADDR_LEN);
}

esp_bt_uuid_t BT_HELPER_read_uuid(const LLBLUETOOTH_uuid_t *lluuid)
{
	esp_bt_uuid_t uuid;
	uuid.len = ESP_UUID_LEN_128;
	for (int i = 0; i < 16; i++) {
		uuid.uuid.uuid128[i] = lluuid->uuid[16-1-i];
	}
	return uuid;
}

esp_gatt_perm_t BT_HELPER_read_permissions(uint8_t llperm)
{
	esp_gatt_perm_t perm = 0;
	if (llperm & ATTRIBUTE_PERMISSION_READ) perm |= ESP_GATT_PERM_READ;
	if (llperm & ATTRIBUTE_PERMISSION_READ_ENCRYPT) perm |= ESP_GATT_PERM_READ_ENCRYPTED;
	if (llperm & ATTRIBUTE_PERMISSION_READ_AUTH) perm |= ESP_GATT_PERM_READ_ENC_MITM;
	if (llperm & ATTRIBUTE_PERMISSION_WRITE) perm |= ESP_GATT_PERM_WRITE;
	if (llperm & ATTRIBUTE_PERMISSION_WRITE_ENCRYPT) perm |= ESP_GATT_PERM_WRITE_ENCRYPTED;
	if (llperm & ATTRIBUTE_PERMISSION_WRITE_AUTH) perm |= ESP_GATT_PERM_WRITE_ENC_MITM;
	return perm;
}

BT_DATA_service_t BT_HELPER_read_service(const LLBLUETOOTH_gatts_service_t *llservice)
{
	BT_DATA_service_t service;
	service.uuid = BT_HELPER_read_uuid(&llservice->uuid);
	service.num_char = llservice->num_characteristics;
	service.num_desc = llservice->num_descriptors;
	return service;
}

BT_DATA_characteristic_t BT_HELPER_read_characteristic(const LLBLUETOOTH_gatts_attribute_t *llattribute)
{
	BT_DATA_characteristic_t characteristic;
	characteristic.uuid = BT_HELPER_read_uuid(&llattribute->characteristic.uuid);
	characteristic.permissions = BT_HELPER_read_permissions(llattribute->characteristic.permissions);
	characteristic.properties = llattribute->characteristic.properties;
	return characteristic;
}

BT_DATA_descriptor_t BT_HELPER_read_descriptor(const LLBLUETOOTH_gatts_attribute_t *llattribute)
{
	BT_DATA_descriptor_t descriptor;
	descriptor.uuid = BT_HELPER_read_uuid(&llattribute->descriptor.uuid);
	descriptor.permissions = BT_HELPER_read_permissions(llattribute->descriptor.permissions);
	return descriptor;
}
