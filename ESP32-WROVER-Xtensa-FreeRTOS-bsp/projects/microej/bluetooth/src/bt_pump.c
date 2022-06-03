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
#include "freertos/queue.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatts_api.h"
#include "LLBLUETOOTH_defines.h"
#include "sni.h"
#include "bt_helper.h"
#include "bt_manager.h"
#include "bt_pump.h"

#define PUMP_QUEUE_SIZE 10
#define MAX_SCAN_FILTER_DATA_SIZE 32

#define ALLOC_EVENT(type, extra)\
	uint32_t out_size = sizeof(type) + extra;\
	type *out = (type *) pvPortMalloc(out_size);\
	event_info_t out_info = {(LLBLUETOOTH_event_t *) out, out_size};

#define FREE_EVENT(event) vPortFree(event)

typedef struct {
	LLBLUETOOTH_event_t *event;
	uint32_t size;
} event_info_t;

static SemaphoreHandle_t register_lock = NULL;
static bool register_success;

static QueueHandle_t event_queue = NULL;
static int32_t suspended_thread_id = SNI_ERROR;

static uint8_t scan_filter_action = SCAN_FILTER_ACTION_NONE;
static uint8_t scan_filter_type;
static uint8_t scan_filter_data[MAX_SCAN_FILTER_DATA_SIZE];
static uint32_t scan_filter_data_size;

static int32_t poll_event(LLBLUETOOTH_event_t *llevent, uint32_t max_event_size);
static void handle_event_gap(esp_gap_ble_cb_event_t type, esp_ble_gap_cb_param_t *event);
static void handle_event_gatts(esp_gatts_cb_event_t type, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *event);
static void handle_event_gattc(esp_gattc_cb_event_t type, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *event);
static void handle_event_scan_result(const struct ble_scan_result_evt_param *event);
static void handle_event_scan_stop_complete(const struct ble_scan_stop_cmpl_evt_param *event);
static void handle_event_adv_stop_complete(const struct ble_adv_stop_cmpl_evt_param *event);
static void handle_event_auth_complete(const esp_ble_auth_cmpl_t *event);
static void handle_event_security_request(const esp_ble_sec_req_t *event);
static void handle_event_passkey_notification(const esp_ble_sec_key_notif_t *event);
static void handle_event_passkey_request(const esp_ble_sec_req_t *event);
static void handle_event_gatts_register(const struct gatts_reg_evt_param *event, esp_gatt_if_t gattc_if);
static void handle_event_gatts_unregister(void);
static void handle_event_create_service(const struct gatts_create_evt_param *event);
static void handle_event_add_characteristic(const struct gatts_add_char_evt_param *event);
static void handle_event_add_descriptor(const struct gatts_add_char_descr_evt_param *event);
static void handle_event_start_service(const struct gatts_start_evt_param *event);
static void handle_event_read_request(const struct gatts_read_evt_param *event);
static void handle_event_write_request(const struct gatts_write_evt_param *event);
static void handle_event_notification_confirm(const struct gatts_conf_evt_param *event);
static void handle_event_gattc_register(const struct gattc_reg_evt_param *event, esp_gatt_if_t gattc_if);
static void handle_event_gattc_unregister(void);
static void handle_event_connect(const struct gattc_connect_evt_param *event);
static void handle_event_disconnect(const struct gattc_disconnect_evt_param *event);
static void handle_event_search_result(const struct gattc_search_res_evt_param *event);
static void handle_event_search_complete(const struct gattc_search_cmpl_evt_param *event);
static void handle_event_read_completed(const struct gattc_read_char_evt_param *event);
static void handle_event_write_completed(const struct gattc_write_evt_param *event);
static void handle_event_notification_received(const struct gattc_notify_evt_param *event);
static void send_event(event_info_t *event_info);
static bool check_scan_result_filter(const uint8_t *adv_data);

bool BT_PUMP_start(void)
{
	if (!event_queue) {
		event_queue = xQueueCreate(PUMP_QUEUE_SIZE, sizeof(event_info_t));
	} else {
		xQueueReset(event_queue);
	}

	if (!register_lock) {
		vSemaphoreCreateBinary(register_lock);
	} else {
		xSemaphoreGive(register_lock);
	}
	xSemaphoreTake(register_lock, portMAX_DELAY);

	int32_t status = esp_ble_gap_register_callback(handle_event_gap);
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_ble_gap_register_callback status=%d\n", status);
		return false;
	}

	status = esp_ble_gatts_register_callback(handle_event_gatts);
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_ble_gatts_register_callback status=%d\n", status);
		return false;
	}

	status = esp_ble_gattc_register_callback(handle_event_gattc);
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_ble_gattc_register_callback status=%d\n", status);
		return false;
	}

	// register app for GATTS
	status = esp_ble_gatts_app_register(0);
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_ble_gatts_app_register status=%d\n", status);
		return false;
	}

	// wait for register_lock to be given
	xSemaphoreTake(register_lock, portMAX_DELAY);
	if (!register_success) {
		LLBLUETOOTH_DEBUG_TRACE("esp_ble_gatts_app_register failed\n");
		return false;
	}

	// register app for GATTC
	status = esp_ble_gattc_app_register(0);
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_ble_gattc_app_register status=%d\n", status);
		return false;
	}

	// wait for register_lock to be given
	xSemaphoreTake(register_lock, portMAX_DELAY);
	if (!register_success) {
		LLBLUETOOTH_DEBUG_TRACE("esp_ble_gattc_app_register failed\n");
		return false;
	}

	return true;
}

void BT_PUMP_stop() {
	// unregister app for GATTS
	int32_t status = esp_ble_gatts_app_unregister(BT_MANAGER_get_gatts_if());
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_ble_gatts_app_unregister status=%d\n", status);
	}

	// wait for register_lock to be given
	xSemaphoreTake(register_lock, portMAX_DELAY);

	// unregister app for GATTC
	status = esp_ble_gattc_app_unregister(BT_MANAGER_get_gattc_if());
	if (status != ESP_OK) {
		LLBLUETOOTH_DEBUG_TRACE("esp_ble_gattc_app_unregister status=%d\n", status);
	}

	// wait for register_lock to be given
	xSemaphoreTake(register_lock, portMAX_DELAY);

	// resume suspended Java thread
	if (suspended_thread_id != SNI_ERROR) {
		SNI_resumeJavaThread(suspended_thread_id);
		suspended_thread_id = SNI_ERROR;
	}

	// cleanup event queue
	xQueueReset(event_queue);
}

int32_t BT_PUMP_wait_event(LLBLUETOOTH_event_t *event, uint32_t max_event_size)
{
	if (suspended_thread_id == SNI_ERROR) {
		suspended_thread_id = SNI_getCurrentJavaThreadID();
	}

	int32_t event_size = poll_event(event, max_event_size);
	if (event_size > 0) {
		return event_size;
	}

	SNI_suspendCurrentJavaThread(0);
	return 0;
}

static int32_t poll_event(LLBLUETOOTH_event_t *llevent, uint32_t max_event_size)
{
	while (1) {
		// get event
		event_info_t event_info;
		int32_t result = xQueueReceive(event_queue, &event_info, 0);
		if (result != pdTRUE) {
			return 0;
		}

		if (event_info.size > max_event_size) {
			// discard this event and get an other one
			LLBLUETOOTH_DEBUG_TRACE("Warning: buffer it not big enough to hold the event\n");
			FREE_EVENT(event_info.event);
		} else if (event_info.size == 0) {
			// discard this event and get an other one
			FREE_EVENT(event_info.event);
		} else {
			// return this event
			memcpy(llevent, event_info.event, event_info.size);
			FREE_EVENT(event_info.event);
			return event_info.size;
		}
	}
}

void BT_PUMP_set_scan_filter(uint8_t filter_action, uint8_t filter_type, const uint8_t *filter_data,
		uint32_t filter_data_size)
{
	if (filter_data_size > MAX_SCAN_FILTER_DATA_SIZE) {
		filter_data_size = MAX_SCAN_FILTER_DATA_SIZE;
	}

	scan_filter_action = filter_action;
	scan_filter_type = filter_type;
	memcpy(scan_filter_data, filter_data, filter_data_size);
	scan_filter_data_size = filter_data_size;
}

static void handle_event_gap(esp_gap_ble_cb_event_t type, esp_ble_gap_cb_param_t *event)
{
	switch (type) {
	case ESP_GAP_BLE_SCAN_RESULT_EVT:
		return handle_event_scan_result(&event->scan_rst);
	case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT:
		return handle_event_scan_stop_complete(&event->scan_stop_cmpl);
	case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
		return handle_event_adv_stop_complete(&event->adv_stop_cmpl);
	case ESP_GAP_BLE_AUTH_CMPL_EVT:
		return handle_event_auth_complete(&event->ble_security.auth_cmpl);
	case ESP_GAP_BLE_SEC_REQ_EVT:
		return handle_event_security_request(&event->ble_security.ble_req);
	case ESP_GAP_BLE_PASSKEY_NOTIF_EVT:
		return handle_event_passkey_notification(&event->ble_security.key_notif);
	case ESP_GAP_BLE_PASSKEY_REQ_EVT:
		return handle_event_passkey_request(&event->ble_security.ble_req);
	default:
		break; // ignore this event
	}
}

static void handle_event_gatts(esp_gatts_cb_event_t type, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *event)
{
	switch (type) {
	case ESP_GATTS_REG_EVT:
		return handle_event_gatts_register(&event->reg, gatts_if);
	case ESP_GATTS_UNREG_EVT:
		return handle_event_gatts_unregister();
	case ESP_GATTS_CREATE_EVT:
		return handle_event_create_service(&event->create);
	case ESP_GATTS_ADD_CHAR_EVT:
		return handle_event_add_characteristic(&event->add_char);
	case ESP_GATTS_ADD_CHAR_DESCR_EVT:
		return handle_event_add_descriptor(&event->add_char_descr);
	case ESP_GATTS_START_EVT:
		return handle_event_start_service(&event->start);
	case ESP_GATTS_READ_EVT:
		return handle_event_read_request(&event->read);
	case ESP_GATTS_WRITE_EVT:
		return handle_event_write_request(&event->write);
	case ESP_GATTS_CONF_EVT:
		return handle_event_notification_confirm(&event->conf);
	default:
		break; // ignore this event
	}
}

static void handle_event_gattc(esp_gattc_cb_event_t type, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *event)
{
	switch (type) {
	case ESP_GATTC_REG_EVT:
		return handle_event_gattc_register(&event->reg, gattc_if);
	case ESP_GATTC_UNREG_EVT:
		return handle_event_gattc_unregister();
	case ESP_GATTC_CONNECT_EVT:
		return handle_event_connect(&event->connect);
	case ESP_GATTC_DISCONNECT_EVT:
		return handle_event_disconnect(&event->disconnect);
	case ESP_GATTC_SEARCH_RES_EVT:
		return handle_event_search_result(&event->search_res);
	case ESP_GATTC_SEARCH_CMPL_EVT:
		return handle_event_search_complete(&event->search_cmpl);
	case ESP_GATTC_READ_CHAR_EVT:
	case ESP_GATTC_READ_DESCR_EVT:
		return handle_event_read_completed(&event->read);
	case ESP_GATTC_WRITE_CHAR_EVT:
	case ESP_GATTC_WRITE_DESCR_EVT:
		return handle_event_write_completed(&event->write);
	case ESP_GATTC_NOTIFY_EVT:
		return handle_event_notification_received(&event->notify);
	default:
		break; // ignore this event
	}
}

static void handle_event_scan_result(const struct ble_scan_result_evt_param *event)
{
	if (event->search_evt == ESP_GAP_SEARCH_INQ_RES_EVT && check_scan_result_filter(event->ble_adv)) {
		uint8_t adv_data_size = event->adv_data_len + event->scan_rsp_len;

		ALLOC_EVENT(LLBLUETOOTH_event_gap_scan_result_t, adv_data_size);
		out->event_type = EVENT_GAP_SCAN_RESULT;
		out->addr = BT_HELPER_write_addr(event->bda, event->ble_addr_type);
		out->rssi = event->rssi;
		out->adv_data_size = adv_data_size;
		memcpy(out+1, event->ble_adv, adv_data_size);
		send_event(&out_info);
	}
}

static void handle_event_scan_stop_complete(const struct ble_scan_stop_cmpl_evt_param *event)
{
	if (event->status == ESP_BT_STATUS_SUCCESS) {
		ALLOC_EVENT(LLBLUETOOTH_event_gap_scan_completed_t, 0);
		out->event_type = EVENT_GAP_SCAN_COMPLETED;
		send_event(&out_info);
	}
}

static void handle_event_adv_stop_complete(const struct ble_adv_stop_cmpl_evt_param *event)
{
	if (event->status == ESP_BT_STATUS_SUCCESS) {
		ALLOC_EVENT(LLBLUETOOTH_event_gap_advertisement_completed_t, 0);
		out->event_type = EVENT_GAP_ADVERTISEMENT_COMPLETED;
		send_event(&out_info);
	}
}

static void handle_event_auth_complete(const esp_ble_auth_cmpl_t *event)
{
	uint16_t conn_id;
	if (BT_MANAGER_get_connected_conn_id(&conn_id)) {
		ALLOC_EVENT(LLBLUETOOTH_event_gap_pair_completed_t, 0);
		out->event_type = EVENT_GAP_PAIR_COMPLETED;
		out->success = (event->success ? 1 : 0);
		out->conn_handle = conn_id;
		send_event(&out_info);
	}
}

static void handle_event_security_request(const esp_ble_sec_req_t *event)
{
	uint16_t conn_id;
	if (BT_MANAGER_get_connected_conn_id(&conn_id)) {
		ALLOC_EVENT(LLBLUETOOTH_event_gap_pair_request_t, 0);
		out->event_type = EVENT_GAP_PAIR_REQUEST;
		out->conn_handle = conn_id;
		send_event(&out_info);
	}
}

static void handle_event_passkey_notification(const esp_ble_sec_key_notif_t *event)
{
	uint16_t conn_id;
	if (BT_MANAGER_get_connected_conn_id(&conn_id)) {
		ALLOC_EVENT(LLBLUETOOTH_event_gap_passkey_generated_t, 0);
		out->event_type = EVENT_GAP_PASSKEY_GENERATED;
		out->conn_handle = conn_id;
		out->passkey = event->passkey;
		send_event(&out_info);
	}
}

static void handle_event_passkey_request(const esp_ble_sec_req_t *event)
{
	uint16_t conn_id;
	if (BT_MANAGER_get_connected_conn_id(&conn_id)) {
		ALLOC_EVENT(LLBLUETOOTH_event_gap_passkey_request_t, 0);
		out->event_type = EVENT_GAP_PASSKEY_REQUEST;
		out->conn_handle = conn_id;
		send_event(&out_info);
	}
}

static void handle_event_gatts_register(const struct gatts_reg_evt_param *event, esp_gatt_if_t gatts_if)
{
	BT_MANAGER_set_gatts_if(gatts_if);
	register_success = (event->status == ESP_GATT_OK);
	xSemaphoreGive(register_lock);
}

static void handle_event_gatts_unregister(void)
{
	BT_MANAGER_set_gatts_if(ESP_GATT_IF_NONE);
	xSemaphoreGive(register_lock);
}

static void handle_event_create_service(const struct gatts_create_evt_param *event)
{
	BT_MANAGER_on_attribute_created(event->status, event->service_handle);
}

static void handle_event_add_characteristic(const struct gatts_add_char_evt_param *event)
{
	BT_MANAGER_on_attribute_created(event->status, event->attr_handle);
}

static void handle_event_add_descriptor(const struct gatts_add_char_descr_evt_param *event)
{
	BT_MANAGER_on_attribute_created(event->status, event->attr_handle);
}

static void handle_event_start_service(const struct gatts_start_evt_param *event)
{
	BT_MANAGER_on_attribute_created(event->status, event->service_handle);
}

static void handle_event_read_request(const struct gatts_read_evt_param *event)
{
	BT_MANAGER_on_attribute_request(event->handle, event->trans_id);

	ALLOC_EVENT(LLBLUETOOTH_event_gatts_read_request_t, 0);
	out->event_type = EVENT_GATTS_READ_REQUEST;
	out->conn_handle = event->conn_id;
	out->attr_handle = event->handle;
	send_event(&out_info);
}

static void handle_event_write_request(const struct gatts_write_evt_param *event)
{
	BT_MANAGER_on_attribute_request(event->handle, event->trans_id);

	ALLOC_EVENT(LLBLUETOOTH_event_gatts_write_request_t, event->len);
	out->event_type = EVENT_GATTS_WRITE_REQUEST;
	out->conn_handle = event->conn_id;
	out->attr_handle = event->handle;
	out->value_size = event->len;
	memcpy(out+1, event->value, event->len);
	send_event(&out_info);
}

static void handle_event_notification_confirm(const struct gatts_conf_evt_param *event)
{
	uint16_t handle;
	if (!BT_MANAGER_get_notification_attr_handle(&handle)) {
		LLBLUETOOTH_DEBUG_TRACE("Notification confirm received but no notification has been sent\n");
		return;
	}

	ALLOC_EVENT(LLBLUETOOTH_event_gatts_notification_sent_t, 0);
	out->event_type = EVENT_GATTS_NOTIFICATION_SENT;
	out->success = (event->status == ESP_GATT_OK ? 1 : 0);
	out->conn_handle = event->conn_id;
	out->attr_handle = handle;
	send_event(&out_info);
}

static void handle_event_gattc_register(const struct gattc_reg_evt_param *event, esp_gatt_if_t gattc_if)
{
	BT_MANAGER_set_gattc_if(gattc_if);
	register_success = (event->status == ESP_GATT_OK);
	xSemaphoreGive(register_lock);
}

static void handle_event_gattc_unregister(void)
{
	BT_MANAGER_set_gattc_if(ESP_GATT_IF_NONE);
	xSemaphoreGive(register_lock);
}

static void handle_event_connect(const struct gattc_connect_evt_param *event)
{
	esp_ble_addr_type_t addr_type;
	BT_MANAGER_on_connected(event->conn_id, event->remote_bda, &addr_type);

	ALLOC_EVENT(LLBLUETOOTH_event_gap_connected_t, 0);
	out->event_type = EVENT_GAP_CONNECTED;
	out->addr = BT_HELPER_write_addr(event->remote_bda, addr_type);
	out->conn_handle = event->conn_id;
	send_event(&out_info);
}

static void handle_event_disconnect(const struct gattc_disconnect_evt_param *event)
{
	uint8_t connected_addr[ESP_BD_ADDR_LEN];
	esp_ble_addr_type_t connected_addr_type;
	bool connected = BT_MANAGER_get_connected_device(connected_addr, &connected_addr_type);

	if (connected && memcmp(connected_addr, event->remote_bda, ESP_BD_ADDR_LEN) == 0) {
		BT_MANAGER_on_disconnected();

		ALLOC_EVENT(LLBLUETOOTH_event_gap_disconnected_t, 0);
		out->event_type = EVENT_GAP_DISCONNECTED;
		out->conn_handle = event->conn_id;
		send_event(&out_info);
	}

	uint8_t connecting_addr[ESP_BD_ADDR_LEN];
	esp_ble_addr_type_t connecting_addr_type;
	bool connecting = BT_MANAGER_get_connecting_device(connecting_addr, &connecting_addr_type);

	if (connecting && memcmp(connecting_addr, event->remote_bda, ESP_BD_ADDR_LEN) == 0) {
		BT_MANAGER_on_connect_failed();

		ALLOC_EVENT(LLBLUETOOTH_event_gap_connect_failed_t, 0);
		out->event_type = EVENT_GAP_CONNECT_FAILED;
		out->addr = BT_HELPER_write_addr(connecting_addr, connecting_addr_type);
		send_event(&out_info);
	}
}

static void handle_event_search_result(const struct gattc_search_res_evt_param *event)
{
	// get the number of attributes in the service
	uint8_t num_attributes;
	if (!BT_MANAGER_get_num_attributes(event->conn_id, event->start_handle, event->end_handle, &num_attributes)) {
		return;
	}

	// create the event and fill the service info
	ALLOC_EVENT(LLBLUETOOTH_event_gattc_discovery_result_t, num_attributes * sizeof(LLBLUETOOTH_gattc_attribute_t));
	out->event_type = EVENT_GATTC_DISCOVERY_RESULT;
	out->conn_handle = event->conn_id;
	out->service_handle = event->start_handle;
	out->service_uuid = BT_HELPER_write_uuid(&event->srvc_id.uuid);

	// get pointer on the attributes array of the event
	LLBLUETOOTH_gattc_attribute_t *attributes = (LLBLUETOOTH_gattc_attribute_t *) (out+1);
	uint16_t attribute_index = 0;

	uint16_t characteristic_offset = 0;
	while (1) {
		// get next characteristic
		esp_gattc_char_elem_t characteristic;
		if (!BT_MANAGER_get_characteristic(event->conn_id, event->start_handle, event->end_handle,
				characteristic_offset++, &characteristic)) {
			break;
		}

		// sanity check
		if (attribute_index >= num_attributes) {
			LLBLUETOOTH_DEBUG_TRACE("Error serializing characteristic\n");
			break;
		}

		// signal to bt manager that a characteristic has been discovered
		BT_MANAGER_on_characteristic_discovered(characteristic.char_handle);

		// fill characteristic info
		LLBLUETOOTH_gattc_attribute_t *attribute = &attributes[attribute_index++];
		attribute->type = ATTRIBUTE_TYPE_CHARACTERISTIC;
		attribute->uuid = BT_HELPER_write_uuid(&characteristic.uuid);
		attribute->characteristic.value_handle = characteristic.char_handle;
		attribute->characteristic.properties = characteristic.properties;

		uint16_t descriptor_offset = 0;
		while (1) {
			// get next descriptor
			esp_gattc_descr_elem_t descriptor;
			if (!BT_MANAGER_get_descriptor(event->conn_id, characteristic.char_handle, descriptor_offset++,
					&descriptor)) {
				break;
			}

			// sanity check
			if (attribute_index >= num_attributes) {
				LLBLUETOOTH_DEBUG_TRACE("Error serializing descriptor\n");
				break;
			}

			// fill descriptor info
			attribute = &attributes[attribute_index++];
			attribute->type = ATTRIBUTE_TYPE_DESCRIPTOR;
			attribute->uuid = BT_HELPER_write_uuid(&descriptor.uuid);
			attribute->descriptor.handle = descriptor.handle;
		}
	}

	// write effective number of attributes
	out->num_attributes = attribute_index;

	// push event to queue
	send_event(&out_info);
}

static void handle_event_search_complete(const struct gattc_search_cmpl_evt_param *event)
{
	ALLOC_EVENT(LLBLUETOOTH_event_gattc_discovery_completed_t, 0);
	out->event_type = EVENT_GATTC_DISCOVERY_COMPLETED;
	out->conn_handle = event->conn_id;
	send_event(&out_info);
}

static void handle_event_read_completed(const struct gattc_read_char_evt_param *event)
{
	ALLOC_EVENT(LLBLUETOOTH_event_gattc_read_completed_t, event->value_len);
	out->event_type = EVENT_GATTC_READ_COMPLETED;
	out->status = (uint8_t) event->status;
	out->conn_handle = event->conn_id;
	out->attr_handle = event->handle;
	out->value_size = event->value_len;
	memcpy(out+1, event->value, event->value_len);
	send_event(&out_info);
}

static void handle_event_write_completed(const struct gattc_write_evt_param *event)
{
	ALLOC_EVENT(LLBLUETOOTH_event_gattc_write_completed_t, 0);
	out->event_type = EVENT_GATTC_WRITE_COMPLETED;
	out->status = (uint8_t) event->status;
	out->conn_handle = event->conn_id;
	out->attr_handle = event->handle;
	send_event(&out_info);
}

static void handle_event_notification_received(const struct gattc_notify_evt_param *event)
{
	ALLOC_EVENT(LLBLUETOOTH_event_gattc_notification_received_t, event->value_len);
	out->event_type = EVENT_GATTC_NOTIFICATION_RECEIVED;
	out->conn_handle = event->conn_id;
	out->attr_handle = event->handle;
	out->value_size = event->value_len;
	memcpy(out+1, event->value, event->value_len);
	send_event(&out_info);
}

static void send_event(event_info_t *event_info)
{
	int32_t result = xQueueSend(event_queue, event_info, 0);
	if (result != pdTRUE) {
		LLBLUETOOTH_DEBUG_TRACE("Warning: event dropped due to queue being full\n");
		FREE_EVENT(event_info->event);
		return;
	}

	if (suspended_thread_id != SNI_ERROR) {
		SNI_resumeJavaThread(suspended_thread_id);
	} else {
		LLBLUETOOTH_DEBUG_TRACE("Warning: event received while no suspended thread\n");
	}
}

static bool check_scan_result_filter(const uint8_t *adv_data)
{
	if (scan_filter_action == SCAN_FILTER_ACTION_NONE) {
		return true;
	}

	uint8_t value_size;
	const uint8_t *value = esp_ble_resolve_adv_data((uint8_t *) adv_data, scan_filter_type, &value_size);

	if (value == NULL) {
		return false;
	}

	if (scan_filter_action == SCAN_FILTER_ACTION_FIELD_EXISTS) {
		return true;
	} else if (scan_filter_action == SCAN_FILTER_ACTION_FIELD_EQUALS) {
		return (value_size == scan_filter_data_size && memcmp(value, scan_filter_data, scan_filter_data_size) == 0);
	} else if (scan_filter_action == SCAN_FILTER_ACTION_FIELD_STARTS_WITH) {
		return (value_size >= scan_filter_data_size && memcmp(value, scan_filter_data, scan_filter_data_size) == 0);
	} else {
		LLBLUETOOTH_DEBUG_TRACE("Warning: unknown scan filter type\n");
		return false;
	}
}
