/*
 * C
 *
 * Copyright 2019-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatts_api.h"
#include "bt_manager.h"

/*
//relate to esp_gap_ble_api.h, default set device to Just Works mode
#define ESP_IO_CAP_OUT                      0   //DisplayOnly
#define ESP_IO_CAP_IO                       1   //DisplayYesNo
#define ESP_IO_CAP_IN                       2   //KeyboardOnly
#define ESP_IO_CAP_NONE                     3   //NoInputNoOutput
#define ESP_IO_CAP_KBDISP                   4   //Keyboard display
*/
#define BLE_IO_CAP				ESP_IO_CAP_NONE

#define BLE_AUTH_REQ			ESP_LE_AUTH_BOND
#define BLE_SECURITY_ACTION		ESP_BLE_SEC_ENCRYPT_MITM

#define SCAN_TYPE				BLE_SCAN_TYPE_ACTIVE
#define SCAN_INTERVAL			(100*1000/625)
#define SCAN_WINDOW				(50*1000/625)
#define SCAN_FILTER_POLICY		BLE_SCAN_FILTER_ALLOW_ALL
#define SCAN_FILTER_DUPLICATE	BLE_SCAN_DUPLICATE_DISABLE
#define SCAN_OWN_ADDR_TYPE		BLE_ADDR_TYPE_PUBLIC

#define ADV_INTERVAL			(150*1000/625)
#define ADV_TYPE				ADV_TYPE_IND
#define ADV_OWN_ADDR_TYPE		BLE_ADDR_TYPE_PUBLIC
#define ADV_CHANNEL_MAP			ADV_CHNL_ALL
#define ADV_FILTER_POLICY		ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY

static esp_gatt_if_t gatts_if = ESP_GATT_IF_NONE;
static esp_gatt_if_t gattc_if = ESP_GATT_IF_NONE;

static bool is_connecting = false;
static uint8_t connecting_addr[ESP_BD_ADDR_LEN];
static esp_ble_addr_type_t connecting_addr_type;

static bool is_connected = false;
static uint8_t connected_addr[ESP_BD_ADDR_LEN];
static esp_ble_addr_type_t connected_addr_type;
static uint16_t connected_conn_id;

static SemaphoreHandle_t create_attribute_lock = NULL;
static esp_gatt_status_t create_attribute_status;
static uint16_t create_attribute_handle;

static uint16_t attribute_request_handle = ESP_GATT_ILLEGAL_HANDLE;
static uint32_t attribute_request_trans_id;

static uint16_t notification_attr_handle = ESP_GATT_ILLEGAL_HANDLE;

bool BT_MANAGER_enable(void)
{
	// release classic BT memory as it is not used
	esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);

	esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
	int32_t status = esp_bt_controller_init(&bt_cfg);
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_bt_controller_init status=%d\n", status);
		return false;
	}

	status = esp_bt_controller_enable(ESP_BT_MODE_BLE);
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_bt_controller_enable status=%d\n", status);
		return false;
	}

	status = esp_bluedroid_init();
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_bluedroid_init status=%d\n", status);
		return false;
	}

	status = esp_bluedroid_enable();
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_bluedroid_enable status=%d\n", status);
		return false;
	}

	esp_ble_io_cap_t io_cap = BLE_IO_CAP;
	status = esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &io_cap, sizeof(esp_ble_io_cap_t));
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_ble_gap_set_security_param status=%d\n", status);
		return false;
	}

	esp_ble_auth_req_t auth_req = BLE_AUTH_REQ;
	status = esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req, sizeof(esp_ble_auth_req_t));
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_ble_gap_set_security_param status=%d\n", status);
		return false;
	}

	if (!create_attribute_lock) {
		vSemaphoreCreateBinary(create_attribute_lock);
	} else {
		xSemaphoreGive(create_attribute_lock);
	}
	xSemaphoreTake(create_attribute_lock, portMAX_DELAY);

	return true;
}

void BT_MANAGER_disable(void)
{
	gatts_if = ESP_GATT_IF_NONE;
	gattc_if = ESP_GATT_IF_NONE;
	is_connecting = false;
	is_connected = false;
	attribute_request_handle = ESP_GATT_ILLEGAL_HANDLE;
	notification_attr_handle = ESP_GATT_ILLEGAL_HANDLE;

	xQueueReset(create_attribute_lock);

	int32_t status = esp_bluedroid_disable();
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_bluedroid_disable status=%d\n", status);
		return;
	}

	status = esp_bluedroid_deinit();
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_bluedroid_deinit status=%d\n", status);
		return;
	}

	status = esp_bt_controller_disable();
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_bt_controller_disable status=%d\n", status);
		return;
	}

	status = esp_bt_controller_deinit();
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_bt_controller_deinit status=%d\n", status);
		return;
	}
}

esp_gatt_if_t BT_MANAGER_get_gatts_if(void)
{
	return gatts_if;
}

esp_gatt_if_t BT_MANAGER_get_gattc_if(void)
{
	return gattc_if;
}

void BT_MANAGER_set_gatts_if(esp_gatt_if_t gatts_if_param)
{
	gatts_if = gatts_if_param;
}

void BT_MANAGER_set_gattc_if(esp_gatt_if_t gattc_if_param)
{
	gattc_if = gattc_if_param;
}

bool BT_MANAGER_get_connecting_device(uint8_t *addr, esp_ble_addr_type_t *addr_type)
{
	memcpy(addr, connecting_addr, ESP_BD_ADDR_LEN);
	*addr_type = connecting_addr_type;
	return is_connecting;
}

bool BT_MANAGER_get_connected_device(uint8_t *addr, esp_ble_addr_type_t *addr_type)
{
	memcpy(addr, connected_addr, ESP_BD_ADDR_LEN);
	*addr_type = connected_addr_type;
	return is_connected;
}

bool BT_MANAGER_get_connected_conn_id(uint16_t *conn_id)
{
	*conn_id = connected_conn_id;
	return is_connected;
}

bool BT_MANAGER_get_notification_attr_handle(uint16_t *attr_handle)
{
	if (notification_attr_handle == ESP_GATT_ILLEGAL_HANDLE) {
		return false;
	}

	*attr_handle = notification_attr_handle;
	notification_attr_handle = ESP_GATT_ILLEGAL_HANDLE;
	return true;
}

void BT_MANAGER_on_connected(uint16_t conn_id, const uint8_t *addr, esp_ble_addr_type_t *addr_type)
{
	esp_ble_gap_stop_advertising(); // make sure that the advertising completed event is generated

	if (is_connecting && memcmp(addr, connecting_addr, ESP_BD_ADDR_LEN) == 0) {
		*addr_type = connecting_addr_type;
		is_connecting = false;
	} else {
		LLBLUETOOTH_DEBUG_TRACE("Warning: could not guess addr type of connected device, defaulting to BLE_ADDR_TYPE_PUBLIC\n");
		*addr_type = BLE_ADDR_TYPE_PUBLIC;

		int32_t status = esp_ble_gattc_open(gattc_if, (uint8_t *) addr, *addr_type, true);
		if (status != ESP_OK) {
			LLBLUETOOTH_DEBUG_TRACE("esp_ble_gattc_open status=%d\n", status);
		}
	}

	connected_conn_id = conn_id;
	memcpy(connected_addr, addr, ESP_BD_ADDR_LEN);
	connected_addr_type = *addr_type;
	is_connected = true;
}

void BT_MANAGER_on_connect_failed(void)
{
	is_connecting = false;
}

void BT_MANAGER_on_disconnected(void)
{
	is_connected = false;
}

void BT_MANAGER_on_attribute_created(esp_gatt_status_t status, uint16_t handle)
{
	create_attribute_status = status;
	create_attribute_handle = handle;
	xSemaphoreGive(create_attribute_lock);
}

void BT_MANAGER_on_attribute_request(uint16_t handle, uint32_t trans_id)
{
	attribute_request_trans_id = trans_id;
	attribute_request_handle = handle;
}

void BT_MANAGER_on_characteristic_discovered(uint16_t handle)
{
	int32_t status = esp_ble_gattc_register_for_notify(gattc_if, connected_addr, handle);
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_ble_gattc_register_for_notify status=%d\n", status);
	}
}

bool BT_MANAGER_add_service(BT_DATA_service_t *s)
{
	esp_gatt_srvc_id_t service_id = {
		.id = {
			.uuid = s->uuid,
			.inst_id = 0
		},
		.is_primary = true
	};

	uint16_t num_attr = 1 + s->num_char*2 + s->num_desc;

	int32_t status = esp_ble_gatts_create_service(gatts_if, &service_id, num_attr);
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_ble_gatts_create_service status=%d\n", status);
		return false;
	}

	xSemaphoreTake(create_attribute_lock, portMAX_DELAY);
	if (create_attribute_status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("Error adding service status=%d\n", create_attribute_status);
		return false;
	}

	s->handle = create_attribute_handle;
	return true;
}

bool BT_MANAGER_add_characteristic(uint16_t service_handle, BT_DATA_characteristic_t *c)
{
	esp_attr_value_t attr_value = {
		.attr_max_len = 0,
		.attr_len = 0,
		.attr_value = NULL,
	};

	esp_attr_control_t attr_control = {
		.auto_rsp = ESP_GATT_RSP_BY_APP
	};

	int32_t status = esp_ble_gatts_add_char(service_handle, &c->uuid, c->permissions, c->properties,
			&attr_value, &attr_control);
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_ble_gatts_add_char status=%d\n", status);
		return false;
	}

	xSemaphoreTake(create_attribute_lock, portMAX_DELAY);
	if (create_attribute_status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("Error adding characteristic status=%d\n", create_attribute_status);
		return false;
	}

	c->value_handle = create_attribute_handle;
	return true;
}

bool BT_MANAGER_add_descriptor(uint16_t service_handle, BT_DATA_descriptor_t *d)
{
	esp_attr_value_t attr_value = {
		.attr_max_len = 0,
		.attr_len = 0,
		.attr_value = NULL,
	};

	esp_attr_control_t attr_control = {
		.auto_rsp = ESP_GATT_RSP_BY_APP
	};

	int32_t status = esp_ble_gatts_add_char_descr(service_handle, &d->uuid, d->permissions,
			&attr_value, &attr_control);
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_ble_gatts_add_char_descr status=%d\n", status);
		return false;
	}

	xSemaphoreTake(create_attribute_lock, portMAX_DELAY);
	if (create_attribute_status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("Error adding descriptor status=%d\n", create_attribute_status);
		return false;
	}

	d->handle = create_attribute_handle;
	return true;
}

bool BT_MANAGER_start_service(uint16_t service_handle)
{
	int32_t status = esp_ble_gatts_start_service(service_handle);
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_ble_gatts_start_service status=%d\n", status);
		return false;
	}

	xSemaphoreTake(create_attribute_lock, portMAX_DELAY);
	if (create_attribute_status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("Error starting service status=%d\n", create_attribute_status);
		return false;
	}

	return true;
}

bool BT_MANAGER_start_scan(void)
{
	esp_ble_scan_params_t scan_params = {
		.scan_type			= SCAN_TYPE,
		.own_addr_type		= SCAN_OWN_ADDR_TYPE,
		.scan_filter_policy	= SCAN_FILTER_POLICY,
		.scan_interval		= SCAN_INTERVAL,
		.scan_window		= SCAN_WINDOW,
		.scan_duplicate		= SCAN_FILTER_DUPLICATE,
	};

	int32_t status = esp_ble_gap_set_scan_params(&scan_params);
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_ble_gap_set_scan_params status=%d\n", status);
		return false;
	}

	status = esp_ble_gap_start_scanning(0);
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_ble_gap_start_scanning status=%d\n", status);
		return false;
	}

	return true;
}

bool BT_MANAGER_stop_scan(void)
{
	int32_t status = esp_ble_gap_stop_scanning();
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_ble_gap_stop_scanning status=%d\n", status);
		return false;
	}

	return true;
}

bool BT_MANAGER_start_adv(const uint8_t *adv_data, uint8_t adv_data_size)
{
	esp_ble_adv_params_t adv_params = {
		.adv_int_min		= ADV_INTERVAL,
		.adv_int_max		= ADV_INTERVAL,
		.adv_type			= ADV_TYPE,
		.own_addr_type		= ADV_OWN_ADDR_TYPE,
		.channel_map		= ADV_CHANNEL_MAP,
		.adv_filter_policy	= ADV_FILTER_POLICY
	};

	int32_t status = esp_ble_gap_config_adv_data_raw((uint8_t *) adv_data, adv_data_size);
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_ble_gap_config_adv_data_raw status=%d\n", status);
		return false;
	}

	status = esp_ble_gap_start_advertising(&adv_params);
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_ble_gap_start_advertising status=%d\n", status);
		return false;
	}

	return true;
}

bool BT_MANAGER_stop_adv(void)
{
	int32_t status = esp_ble_gap_stop_advertising();
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_ble_gap_stop_advertising status=%d\n", status);
		return false;
	}

	return true;
}

bool BT_MANAGER_connect(const uint8_t *addr, esp_ble_addr_type_t addr_type)
{
	memcpy(connecting_addr, addr, ESP_BD_ADDR_LEN);
	connecting_addr_type = addr_type;
	is_connecting = true;

	esp_ble_gap_stop_scanning(); // scanning has to be stopped before we can connect to a device

	int32_t status = esp_ble_gattc_open(gattc_if, (uint8_t *) addr, addr_type, true);
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_ble_gattc_open status=%d\n", status);
		return false;
	}

	return true;
}

bool BT_MANAGER_disconnect(uint16_t conn_id)
{
	int32_t status = esp_ble_gattc_close(gattc_if, conn_id);
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_ble_gattc_close status=%d\n", status);
		return false;
	}

	return true;
}

bool BT_MANAGER_send_pair_request(uint16_t conn_id)
{
	int32_t status = esp_ble_set_encryption(connected_addr, BLE_SECURITY_ACTION);
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_ble_set_encryption status=%d\n", status);
		return false;
	}

	return true;
}

bool BT_MANAGER_send_pair_response(uint16_t conn_id, bool accept)
{
	int32_t status = esp_ble_gap_security_rsp(connected_addr, accept);
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_ble_set_encryption status=%d\n", status);
		return false;
	}

	return true;
}

bool BT_MANAGER_send_passkey_response(uint16_t conn_id, bool accept, uint32_t passkey)
{
	int32_t status = esp_ble_passkey_reply(connected_addr, accept, passkey);
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_ble_passkey_reply status=%d\n", status);
		return false;
	}

	return true;
}

bool BT_MANAGER_discover_services(uint16_t conn_id, const esp_bt_uuid_t *uuid)
{
	int32_t status = esp_ble_gattc_search_service(gattc_if, conn_id, (esp_bt_uuid_t *) uuid);
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_ble_gattc_search_service status=%d\n", status);
		return false;
	}

	return true;
}

bool BT_MANAGER_send_read_request(uint16_t conn_id, uint16_t attr_handle)
{
	bool is_characteristic;
	if (!BT_MANAGER_get_attribute_type(conn_id, attr_handle, &is_characteristic)) {
		return false;
	}

	if (is_characteristic) {
		int32_t status = esp_ble_gattc_read_char(gattc_if, conn_id, attr_handle, ESP_GATT_AUTH_REQ_NONE);
		if (status != ESP_OK) {
			LLBLUETOOTH_DEBUG_TRACE("esp_ble_gattc_read_char status=%d\n", status);
			return false;
		}
	} else {
		int32_t status = esp_ble_gattc_read_char_descr(gattc_if, conn_id, attr_handle, ESP_GATT_AUTH_REQ_NONE);
		if (status != ESP_OK) {
			LLBLUETOOTH_DEBUG_TRACE("esp_ble_gattc_read_char_descr status=%d\n", status);
			return false;
		}
	}

	return true;
}

bool BT_MANAGER_send_write_request(uint16_t conn_id, uint16_t attr_handle, uint16_t value_size,
	const uint8_t *value, bool no_response)
{
	bool is_characteristic;
	if (!BT_MANAGER_get_attribute_type(conn_id, attr_handle, &is_characteristic)) {
		return false;
	}

	esp_gatt_write_type_t write_type = (no_response ? ESP_GATT_WRITE_TYPE_NO_RSP : ESP_GATT_WRITE_TYPE_RSP);

	if (is_characteristic) {
		int32_t status = esp_ble_gattc_write_char(gattc_if, conn_id, attr_handle, value_size,
				(uint8_t *) value, write_type, ESP_GATT_AUTH_REQ_NONE);
		if (status != ESP_OK) {
			LLBLUETOOTH_DEBUG_TRACE("esp_ble_gattc_write_char status=%d\n", status);
			return false;
		}
	} else {
		int32_t status = esp_ble_gattc_write_char_descr(gattc_if, conn_id, attr_handle, value_size,
				(uint8_t *) value, write_type, ESP_GATT_AUTH_REQ_NONE);
		if (status != ESP_OK) {
			LLBLUETOOTH_DEBUG_TRACE("esp_ble_gattc_write_char_descr status=%d\n", status);
			return false;
		}
	}

	return true;
}

bool BT_MANAGER_send_read_response(uint16_t conn_id, uint16_t attr_handle, esp_gatt_status_t status,
	uint16_t value_size, const uint8_t *value)
{
	if (attr_handle != attribute_request_handle) {
		LLBLUETOOTH_DEBUG_TRACE("Could not send response on this attribute\n");
		return false;
	}

	uint32_t trans_id = attribute_request_trans_id;
	attribute_request_handle = ESP_GATT_ILLEGAL_HANDLE;

	esp_gatt_rsp_t response = {
		.attr_value = {
				.handle = attr_handle,
				.offset = 0,
				.len = value_size,
				.auth_req = ESP_GATT_AUTH_REQ_NONE
		}
	};
	memcpy(&response.attr_value.value, value, value_size);

	int32_t error = esp_ble_gatts_send_response(gatts_if, conn_id, trans_id, status, &response);
	if (error != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_ble_gatts_send_response error=%d\n", error);
		return false;
	}

	return true;
}

bool BT_MANAGER_send_write_response(uint16_t conn_id, uint16_t attr_handle, esp_gatt_status_t status)
{
	if (attr_handle != attribute_request_handle) {
		LLBLUETOOTH_DEBUG_TRACE("Could not send response on this attribute\n");
		return false;
	}

	uint32_t trans_id = attribute_request_trans_id;
	attribute_request_handle = ESP_GATT_ILLEGAL_HANDLE;

	esp_gatt_rsp_t response = {
		.handle = attr_handle
	};

	int32_t error = esp_ble_gatts_send_response(gatts_if, conn_id, trans_id, status, &response);
	if (error != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_ble_gatts_send_response error=%d\n", error);
		return false;
	}

	return true;
}

bool BT_MANAGER_send_notification(uint16_t conn_id, uint16_t attr_handle, uint16_t value_size,
	const uint8_t *value, bool confirm)
{
	notification_attr_handle = attr_handle;

	int32_t status = esp_ble_gatts_send_indicate(gatts_if, conn_id, attr_handle, value_size,
			(uint8_t *) value, confirm);
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_ble_gatts_send_indicate status=%d\n", status);
		notification_attr_handle = ESP_GATT_ILLEGAL_HANDLE;
		return false;
	}

	return true;
}

bool BT_MANAGER_get_num_attributes(uint16_t conn_id, uint16_t start_handle, uint16_t end_handle,
		uint8_t *num_attributes)
{
	uint16_t count;
	int32_t status = esp_ble_gattc_get_attr_count(gattc_if, conn_id, ESP_GATT_DB_ALL,
			start_handle, end_handle, ESP_GATT_ILLEGAL_HANDLE, &count);
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_ble_gattc_get_attr_count status=%d\n", status);
		return false;
	}

	if (count > 0) {
		count--; // ignore service attribute
	}

	*num_attributes = (count > 0xFF ? 0xFF : (uint8_t) count);
	return true;
}

bool BT_MANAGER_get_characteristic(uint16_t conn_id, uint16_t start_handle, uint16_t end_handle,
		uint16_t offset, esp_gattc_char_elem_t *characteristic)
{
	uint16_t count = 1;
	int32_t status = esp_ble_gattc_get_all_char(gattc_if, conn_id, start_handle, end_handle,
			characteristic, &count, offset);
	if (status == ESP_GATT_INVALID_OFFSET || status == ESP_GATT_NOT_FOUND) {
		return false;
	} else if (status != ESP_GATT_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_ble_gattc_get_all_char status=%d\n", status);
		return false;
	}

	return true;
}

bool BT_MANAGER_get_descriptor(uint16_t conn_id, uint16_t char_handle, uint16_t offset,
		esp_gattc_descr_elem_t *descriptor)
{
	uint16_t count = 1;
	int32_t status = esp_ble_gattc_get_all_descr(gattc_if, conn_id, char_handle, descriptor,
			&count, offset);
	if (status == ESP_GATT_INVALID_OFFSET || status == ESP_GATT_NOT_FOUND) {
		return false;
	} else if (status != ESP_GATT_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_ble_gattc_get_all_descr status=%d\n", status);
		return false;
	}

	return true;
}

bool BT_MANAGER_get_attribute_type(uint16_t conn_id, uint16_t attr_handle, bool *is_characteristic)
{
	uint16_t count = 1;
	int32_t status = esp_ble_gattc_get_attr_count(gattc_if, conn_id, ESP_GATT_DB_CHARACTERISTIC,
			attr_handle, attr_handle, ESP_GATT_ILLEGAL_HANDLE, &count);
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_ble_gattc_get_attr_count status=%d\n", status);
		return false;
	}

	*is_characteristic = (count == 1);
	return true;
}
