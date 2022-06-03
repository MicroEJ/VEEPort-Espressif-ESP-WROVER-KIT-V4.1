/*
 * C
 *
 * Copyright 2018-2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */

/**
 * @file
 * @brief MicroEJ BLUETOOTH low level API
 * @author MicroEJ Developer Team
 * @version 2.0.2
 * @date 8 July 2021
 */

#ifndef _LLBLUETOOTH_IMPL
#define _LLBLUETOOTH_IMPL

#include <stdint.h>
#include <intern/LLBLUETOOTH_impl.h>
#include <LLBLUETOOTH_defines.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Enables Bluetooth.
 *
 * If Bluetooth is already enabled, this function returns true without performing any operation.
 *
 * @return true if Bluetooth was enabled successfully, false otherwise.
 */
uint8_t LLBLUETOOTH_IMPL_enable(void);

/**
 * Disables Bluetooth.
 *
 * This function frees up all resources allocated during Bluetooth operations and resumes the event thread.
 * If Bluetooth is not enabled, this function returns without performing any operation.
 * After this function has been called, the enable function may be called again.
 */
void LLBLUETOOTH_IMPL_disable(void);

/**
 * Waits for a Bluetooth event.
 *
 * If an event is available, this function writes the event data in the structure and returns the number of bytes
 * written. If no event is available, this function returns zero immediately and suspends the calling Java thread until
 * an event is available.
 *
 * @param event
 *            the event structure to fill.
 * @param max_event_size
 *            the maximum size of the event, in bytes.
 * @return the number of bytes written if an event is available, zero if no event is available.
 */
uint32_t LLBLUETOOTH_IMPL_waitEvent(LLBLUETOOTH_event_t *event, uint32_t max_event_size);

/**
 * Starts scanning. If a connection is initiated while scanning, the scan is stopped. It may be started again once the
 * connection is established.
 *
 * @param filter_action
 *            the filter action.
 * @param filter_type
 *            the filter type.
 * @param filter_data
 *            the filter data.
 * @param filter_data_size
 *            the size of the filter data.
 * @return true if scanning was started successfully, false otherwise.
 */
uint8_t LLBLUETOOTH_IMPL_startScanning(uint8_t filter_action, uint8_t filter_type, const uint8_t *filter_data,
		uint32_t filter_data_size);

/**
 * Stops scanning.
 *
 * @return true if scanning was stopped successfully, false otherwise.
 */
uint8_t LLBLUETOOTH_IMPL_stopScanning(void);

/**
 * Starts advertising. If a device connects while advertising, the advertisement is stopped. It may be started again
 * once the connection is established.
 *
 * @param adv_data
 *            the advertisement data.
 * @param adv_data_size
 *            the size of the advertisement data.
 * @return true if advertising was started successfully, false otherwise.
 */
uint8_t LLBLUETOOTH_IMPL_startAdvertising(const uint8_t *adv_data, uint32_t adv_data_size);

/**
 * Stops advertising.
 *
 * @return true if advertising was stopped successfully, false otherwise.
 */
uint8_t LLBLUETOOTH_IMPL_stopAdvertising(void);

/**
 * Initiates connection with a device.
 *
 * @param addr
 *            the device address.
 * @return true if connection was initiated successfully, false otherwise.
 */
uint8_t LLBLUETOOTH_IMPL_connect(const LLBLUETOOTH_address_t *addr);

/**
 * Initiates disconnection with a device.
 *
 * @param conn_handle
 *            the connection handle of the device.
 * @return true if disconnection was initiated successfully, false otherwise.
 */
uint8_t LLBLUETOOTH_IMPL_disconnect(uint16_t conn_handle);

/**
 * Sends a pair request to a device.
 *
 * @param conn_handle
 *            the connection handle of the device.
 * @return true if the pair request was sent successfully, false otherwise.
 */
uint8_t LLBLUETOOTH_IMPL_sendPairRequest(uint16_t conn_handle);

/**
 * Replies to the pair request of a device.
 *
 * @param conn_handle
 *            the connection handle of the device.
 * @param accept
 *            true if the pairing request should be accepted, false otherwise.
 * @return true if the reply was sent successfully, false otherwise.
 */
uint8_t LLBLUETOOTH_IMPL_sendPairResponse(uint16_t conn_handle, uint8_t accept);

/**
 * Replies to the passkey request of a device.
 *
 * @param conn_handle
 *            the connection handle of the device.
 * @param accept
 *            true if the pairing request should be accepted, false otherwise.
 * @param passkey
 *            the passkey value (6-digits PIN code).
 * @return true if the reply was sent successfully, false otherwise.
 */
uint8_t LLBLUETOOTH_IMPL_sendPasskeyResponse(uint16_t conn_handle, uint8_t accept, uint32_t passkey);

/**
 * Discovers the services which are provided by this device.
 *
 * @param conn_handle
 *            the connection handle of the device.
 * @param uuid
 *            the UUID of the service to discover, or NULL to discover all services.
 * @return true if services discovery was initiated successfully, false otherwise.
 */
uint8_t LLBLUETOOTH_IMPL_discoverServices(uint16_t conn_handle, const LLBLUETOOTH_uuid_t *uuid);

/**
 * Adds a service to the GATT server.
 *
 * The handles buffer is filled with the handle of the service, the value handle of each characteristic and the handle
 * of each descriptor.
 *
 * @param service
 *            the service.
 * @param handles
 *            the handles buffer.
 * @return true if the service was added successfully, false otherwise.
 */
uint8_t LLBLUETOOTH_IMPL_addService(const LLBLUETOOTH_gatts_service_t *service, uint16_t *handles);

/**
 * Sends a read request on one of the attributes provided by a device.
 *
 * @param conn_handle
 *            the connection handle of the device.
 * @param attr_handle
 *            the attribute handle.
 * @return true if the read request was sent successfully.
 */
uint8_t LLBLUETOOTH_IMPL_sendReadRequest(uint16_t conn_handle, uint16_t attr_handle);

/**
 * Sends a write request on one of the attributes provided by a device.
 *
 * @param conn_handle
 *            the connection handle of the device.
 * @param attr_handle
 *            the attribute handle.
 * @param value
 *            the value to write.
 * @param value_size
 *            the size of the value.
 * @param no_response
 *            true to use the write without response procedure, false to use the write procedure.
 * @return true if the write request was sent successfully.
 */
uint8_t LLBLUETOOTH_IMPL_sendWriteRequest(uint16_t conn_handle, uint16_t attr_handle, const uint8_t *value,
	uint32_t value_size, uint8_t no_response);

/**
 * Sends the response to a read request sent by the given device on an attribute.
 *
 * @param conn_handle
 *            the connection handle of the device.
 * @param attr_handle
 *            the attribute handle.
 * @param status
 *            the status to send back.
 * @param value
 *            the value to send back.
 * @param value_size
 *            the size of the value.
 * @return true if the read response was sent successfully.
 */
uint8_t LLBLUETOOTH_IMPL_sendReadResponse(uint16_t conn_handle, uint16_t attr_handle, uint8_t status,
	const uint8_t *value, uint32_t value_size);

/**
 * Sends the response to a write request sent by the given device on an attribute.
 *
 * @param conn_handle
 *            the connection handle of the device.
 * @param attr_handle
 *            the attribute handle.
 * @param status
 *            the status to send back.
 * @return true if the write response was sent successfully.
 */
uint8_t LLBLUETOOTH_IMPL_sendWriteResponse(uint16_t conn_handle, uint16_t attr_handle, uint8_t status);

/**
 * Sends a notification or indication on a characteristic to the given device.
 *
 * @param conn_handle
 *            the connection handle of the device.
 * @param attr_handle
 *            the attribute handle.
 * @param value
 *            the value to send.
 * @param value_size
 *            the size of the value.
 * @param confirm
 *            true to request a confirmation (GATT indication), false otherwise (GATT notification).
 * @return true if the notification was sent successfully.
 */
uint8_t LLBLUETOOTH_IMPL_sendNotification(uint16_t conn_handle, uint16_t attr_handle, const uint8_t *value,
	uint32_t value_size, uint8_t confirm);

#ifdef __cplusplus
}
#endif

#endif // _LLBLUETOOTH_IMPL
