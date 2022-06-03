/*
 * C
 *
 * Copyright 2019-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#include <stdio.h>
#include <string.h>
#include "LLBLUETOOTH_impl.h"
#include "bt_helper.h"
#include "bt_manager.h"
#include "bt_pump.h"

static bool enable_done = false;
static bool enable_result;

uint8_t LLBLUETOOTH_IMPL_enable(void)
{
	if (enable_done) {
		return enable_result;
	}

	enable_result = (BT_MANAGER_enable() && BT_PUMP_start());
	enable_done = true;
	return enable_result;
}

void LLBLUETOOTH_IMPL_disable(void)
{
	if (enable_done) {
		BT_PUMP_stop();
		BT_MANAGER_disable();

		enable_done = false;
	}
}

uint32_t LLBLUETOOTH_IMPL_waitEvent(LLBLUETOOTH_event_t *event, uint32_t max_event_size)
{
	return BT_PUMP_wait_event(event, max_event_size);
}

uint8_t LLBLUETOOTH_IMPL_startScanning(uint8_t filter_action, uint8_t filter_type, const uint8_t *filter_data,
		uint32_t filter_data_size)
{
	BT_PUMP_set_scan_filter(filter_action, filter_type, filter_data, filter_data_size);
	return BT_MANAGER_start_scan();
}

uint8_t LLBLUETOOTH_IMPL_stopScanning(void)
{
	return BT_MANAGER_stop_scan();
}

uint8_t LLBLUETOOTH_IMPL_startAdvertising(const uint8_t *adv_data, uint32_t adv_data_size)
{
	return BT_MANAGER_start_adv(adv_data, adv_data_size);
}

uint8_t LLBLUETOOTH_IMPL_stopAdvertising(void)
{
	return BT_MANAGER_stop_adv();
}

uint8_t LLBLUETOOTH_IMPL_connect(const LLBLUETOOTH_address_t *addr)
{
	uint8_t peer_addr[ESP_BD_ADDR_LEN];
	esp_ble_addr_type_t addr_type;
	BT_HELPER_read_addr(addr, peer_addr, &addr_type);

	return BT_MANAGER_connect(peer_addr, addr_type);
}

uint8_t LLBLUETOOTH_IMPL_disconnect(uint16_t conn_handle)
{
	return BT_MANAGER_disconnect(conn_handle);
}

uint8_t LLBLUETOOTH_IMPL_sendPairRequest(uint16_t conn_handle)
{
	return BT_MANAGER_send_pair_request(conn_handle);
}

uint8_t LLBLUETOOTH_IMPL_sendPairResponse(uint16_t conn_handle, uint8_t accept)
{
	return BT_MANAGER_send_pair_response(conn_handle, accept);
}

uint8_t LLBLUETOOTH_IMPL_sendPasskeyResponse(uint16_t conn_handle, uint8_t accept, uint32_t passkey)
{
	return BT_MANAGER_send_passkey_response(conn_handle, accept, passkey);
}

uint8_t LLBLUETOOTH_IMPL_discoverServices(uint16_t conn_handle, const LLBLUETOOTH_uuid_t *uuid)
{
	if (uuid == NULL) {
		return BT_MANAGER_discover_services(conn_handle, NULL);
	} else {
		esp_bt_uuid_t esp_uuid = BT_HELPER_read_uuid(uuid);
		return BT_MANAGER_discover_services(conn_handle, &esp_uuid);
	}
}

uint8_t LLBLUETOOTH_IMPL_addService(const LLBLUETOOTH_gatts_service_t *llservice, uint16_t *handles)
{
	BT_DATA_service_t service = BT_HELPER_read_service(llservice);
	if (!BT_MANAGER_add_service(&service)) {
		return false;
	}

	int h = 0;
	handles[h++] = service.handle;

	int num_attributes = service.num_char + service.num_desc;
	const LLBLUETOOTH_gatts_attribute_t *llattributes = (LLBLUETOOTH_gatts_attribute_t *) (llservice+1);
	for (int i = 0; i < num_attributes; i++) {
		const LLBLUETOOTH_gatts_attribute_t *llattribute = &llattributes[i];
		if (llattribute->type == ATTRIBUTE_TYPE_CHARACTERISTIC) {
			BT_DATA_characteristic_t characteristic = BT_HELPER_read_characteristic(llattribute);
			if (!BT_MANAGER_add_characteristic(service.handle, &characteristic)) {
				return false;
			}
			handles[h++] = characteristic.value_handle;
		} else if (llattribute->type == ATTRIBUTE_TYPE_DESCRIPTOR) {
			BT_DATA_descriptor_t descriptor = BT_HELPER_read_descriptor(llattribute);
			if (!BT_MANAGER_add_descriptor(service.handle, &descriptor)) {
				return false;
			}
			handles[h++] = descriptor.handle;
		}
	}

	return BT_MANAGER_start_service(service.handle);
}

uint8_t LLBLUETOOTH_IMPL_sendReadRequest(uint16_t conn_handle, uint16_t attr_handle)
{
	return BT_MANAGER_send_read_request(conn_handle, attr_handle);
}

uint8_t LLBLUETOOTH_IMPL_sendWriteRequest(uint16_t conn_handle, uint16_t attr_handle, const uint8_t *value,
	uint32_t value_size, uint8_t no_response)
{
	return BT_MANAGER_send_write_request(conn_handle, attr_handle, value_size, value, no_response);
}

uint8_t LLBLUETOOTH_IMPL_sendReadResponse(uint16_t conn_handle, uint16_t attr_handle, uint8_t status,
	const uint8_t *value, uint32_t value_size)
{
	return BT_MANAGER_send_read_response(conn_handle, attr_handle, status, value_size, value);
}

uint8_t LLBLUETOOTH_IMPL_sendWriteResponse(uint16_t conn_handle, uint16_t attr_handle, uint8_t status)
{
	return BT_MANAGER_send_write_response(conn_handle, attr_handle, status);
}

uint8_t LLBLUETOOTH_IMPL_sendNotification(uint16_t conn_handle, uint16_t attr_handle, const uint8_t *value,
	uint32_t value_size, uint8_t confirm)
{
	return BT_MANAGER_send_notification(conn_handle, attr_handle, value_size, value, confirm);
}
