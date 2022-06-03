/*
 * C
 *
 * Copyright 2019-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#ifndef BLUETOOTH_MANAGER_H_
#define BLUETOOTH_MANAGER_H_

#include <stdbool.h>
#include <stdint.h>
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "bt_data.h"

#define LLBLUETOOTH_DEBUG

#ifdef LLBLUETOOTH_DEBUG
	#include <stdio.h>
	#define LLBLUETOOTH_DEBUG_TRACE			printf
#else
	#define LLBLUETOOTH_DEBUG_TRACE(...)	((void) 0)
#endif

/**
 * Enables the Bluetooth stack.
 */
bool BT_MANAGER_enable(void);

/**
 * Disables the Bluetooth stack.
 */
void BT_MANAGER_disable(void);

/**
 * Gets the GATTS interface handle.
 */
esp_gatt_if_t BT_MANAGER_get_gatts_if(void);

/**
 * Gets the GATTC interface handle.
 */
esp_gatt_if_t BT_MANAGER_get_gattc_if(void);

/**
 * Sets the GATTS interface handle.
 */
void BT_MANAGER_set_gatts_if(esp_gatt_if_t gatts_if);

/**
 * Sets the GATTC interface handle.
 */
void BT_MANAGER_set_gattc_if(esp_gatt_if_t gattc_if);

/**
 * Gets the address of the device with which a connection has been initiated.
 */
bool BT_MANAGER_get_connecting_device(uint8_t *addr, esp_ble_addr_type_t *addr_type);

/**
 * Gets the address of the device with which a connection has been established.
 */
bool BT_MANAGER_get_connected_device(uint8_t *addr, esp_ble_addr_type_t *addr_type);

/**
 * Gets the connection handle of the connection which has been established.
 */
bool BT_MANAGER_get_connected_conn_id(uint16_t *conn_id);

/**
 * Gets the attribute handle of the characteristic to which a notification or indication has been sent.
 */
bool BT_MANAGER_get_notification_attr_handle(uint16_t *attr_handle);

/**
 * Called when a connection has been established.
 */
void BT_MANAGER_on_connected(uint16_t conn_id, const uint8_t *addr, esp_ble_addr_type_t *addr_type);

/**
 * Called when a connection procedure has been failed.
 */
void BT_MANAGER_on_connect_failed(void);

/**
 * Called when a connection has been terminated.
 */
void BT_MANAGER_on_disconnected(void);

/**
 * Called when an attribute or service has been created.
 */
void BT_MANAGER_on_attribute_created(esp_gatt_status_t status, uint16_t handle);

/**
 * Called when an attribute read/write request has been received.
 */
void BT_MANAGER_on_attribute_request(uint16_t handle, uint32_t trans_id);

/**
 * Called when a characteristic has been discovered.
 */
void BT_MANAGER_on_characteristic_discovered(uint16_t handle);

/**
 * Adds the given GATT service.
 */
bool BT_MANAGER_add_service(BT_DATA_service_t *s);

/**
 * Adds the given GATT characteristic.
 */
bool BT_MANAGER_add_characteristic(uint16_t service_handle, BT_DATA_characteristic_t *c);

/**
 * Adds the given GATT descriptor.
 */
bool BT_MANAGER_add_descriptor(uint16_t service_handle, BT_DATA_descriptor_t *d);

/**
 * Starts/registers the GATT service with the given handle.
 */
bool BT_MANAGER_start_service(uint16_t service_handle);

/**
 * Starts scanning.
 */
bool BT_MANAGER_start_scan(void);

/**
 * Stops scanning.
 */
bool BT_MANAGER_stop_scan(void);

/**
 * Starts advertising using the given advertising data.
 */
bool BT_MANAGER_start_adv(const uint8_t *adv_data, uint8_t adv_data_size);

/**
 * Stops advertising.
 */
bool BT_MANAGER_stop_adv(void);

/**
 * Connects to the device with the given address.
 */
bool BT_MANAGER_connect(const uint8_t *addr, esp_ble_addr_type_t addr_type);

/**
 * Disconnects from the device with the given connection handle.
 */
bool BT_MANAGER_disconnect(uint16_t conn_id);

/**
 * Sends a pair request to the device with the given connection handle.
 */
bool BT_MANAGER_send_pair_request(uint16_t conn_id);

/**
 * Sends a pair response to the device with the given connection handle.
 */
bool BT_MANAGER_send_pair_response(uint16_t conn_id, bool accept);

/**
 * Sends a passkey response to the device with the given connection handle.
 */
bool BT_MANAGER_send_passkey_response(uint16_t conn_id, bool accept, uint32_t passkey);

/**
 * Discovers the services provided by the device with the given connection handle. If the given UUID
 * is NULL, all the services of the device are discovered, otherwise only the services with the given
 * UUID are discovered.
 */
bool BT_MANAGER_discover_services(uint16_t conn_id, const esp_bt_uuid_t *uuid);

/**
 * Sends an read request to the device with the given connection handle on the attribute with the
 * given attribute handle.
 */
bool BT_MANAGER_send_read_request(uint16_t conn_id, uint16_t attr_handle);

/**
 * Sends a write request with the given value to the device with the given connection handle on the
 * attribute with the given attribute handle.
 */
bool BT_MANAGER_send_write_request(uint16_t conn_id, uint16_t attr_handle, uint16_t value_size,
	const uint8_t *value, bool no_response);

/**
 * Sends a read response with the given status and value to the device with the given connection
 * handle on the attribute with the given attribute handle.
 */
bool BT_MANAGER_send_read_response(uint16_t conn_id, uint16_t attr_handle, esp_gatt_status_t status,
	uint16_t value_size, const uint8_t *value);

/**
 * Sends a write response with the given status to the device with the given connection handle on the
 * attribute with the given attribute handle.
 */
bool BT_MANAGER_send_write_response(uint16_t conn_id, uint16_t attr_handle, esp_gatt_status_t status);

/**
 * Sends a notification or indication with the given status and value to the device with the given
 * connection handle on the characteristic with the given attribute handle.
 */
bool BT_MANAGER_send_notification(uint16_t conn_id, uint16_t attr_handle, uint16_t value_size,
	const uint8_t *value, bool confirm);

/**
 * Gets the number of attributes within the given handle range.
 */
bool BT_MANAGER_get_num_attributes(uint16_t conn_id, uint16_t start_handle, uint16_t end_handle,
		uint8_t *num_attributes);

/**
 * Gets the next characteristic within the given handle range.
 */
bool BT_MANAGER_get_characteristic(uint16_t conn_id, uint16_t start_handle, uint16_t end_handle,
		uint16_t offset, esp_gattc_char_elem_t *characteristic);

/**
 * Gets the next descriptor within the given handle range.
 */
bool BT_MANAGER_get_descriptor(uint16_t conn_id, uint16_t char_handle, uint16_t offset,
		esp_gattc_descr_elem_t *descriptor);

/**
 * Gets whether the attribute with the given handle provided by the device with the given connection
 * handle is a characteristic or a descriptor.
 */
bool BT_MANAGER_get_attribute_type(uint16_t conn_id, uint16_t attr_handle, bool *is_characteristic);

#endif
