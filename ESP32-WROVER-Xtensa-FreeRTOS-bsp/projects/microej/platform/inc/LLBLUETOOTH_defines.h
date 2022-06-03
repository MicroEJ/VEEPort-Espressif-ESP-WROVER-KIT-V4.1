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

#ifndef _LLBLUETOOTH_DEFINES
#define _LLBLUETOOTH_DEFINES

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Attribute types */
#define ATTRIBUTE_TYPE_CHARACTERISTIC	(0)
#define ATTRIBUTE_TYPE_DESCRIPTOR		(1)

/** Attribute permissions */
#define ATTRIBUTE_PERMISSION_NONE			(0x00)
#define ATTRIBUTE_PERMISSION_READ			(0x01)
#define ATTRIBUTE_PERMISSION_READ_ENCRYPT	(0x02)
#define ATTRIBUTE_PERMISSION_READ_AUTH		(0x04)
#define ATTRIBUTE_PERMISSION_WRITE			(0x08)
#define ATTRIBUTE_PERMISSION_WRITE_ENCRYPT	(0x10)
#define ATTRIBUTE_PERMISSION_WRITE_AUTH		(0x20)

/** Address types */
#define ADDRESS_TYPE_PUBLIC		(0)
#define ADDRESS_TYPE_PRIVATE	(1)

/** Scan filter actions */
#define SCAN_FILTER_ACTION_NONE					(0)
#define SCAN_FILTER_ACTION_FIELD_EXISTS			(1)
#define SCAN_FILTER_ACTION_FIELD_EQUALS			(2)
#define SCAN_FILTER_ACTION_FIELD_STARTS_WITH	(3)

/** Event types */
#define EVENT_GAP_SCAN_RESULT				(1)
#define EVENT_GAP_SCAN_COMPLETED			(2)
#define EVENT_GAP_ADVERTISEMENT_COMPLETED	(3)
#define EVENT_GAP_CONNECT_FAILED			(4)
#define EVENT_GAP_CONNECTED					(5)
#define EVENT_GAP_DISCONNECTED				(6)
#define EVENT_GAP_PAIR_REQUEST				(7)
#define EVENT_GAP_PAIR_COMPLETED			(8)
#define EVENT_GAP_PASSKEY_REQUEST			(9)
#define EVENT_GAP_PASSKEY_GENERATED			(10)
#define EVENT_GATTC_DISCOVERY_RESULT		(11)
#define EVENT_GATTC_DISCOVERY_COMPLETED		(12)
#define EVENT_GATTC_READ_COMPLETED			(13)
#define EVENT_GATTC_WRITE_COMPLETED			(14)
#define EVENT_GATTC_NOTIFICATION_RECEIVED	(15)
#define EVENT_GATTS_READ_REQUEST			(16)
#define EVENT_GATTS_WRITE_REQUEST			(17)
#define EVENT_GATTS_NOTIFICATION_SENT		(18)

/** Address */
typedef struct {
	uint8_t type; // one of ADDRESS_TYPE_* values
	uint8_t addr[6]; // in big-endian
} LLBLUETOOTH_address_t;

/** UUID */
typedef struct {
	uint8_t uuid[16]; // in big-endian
} LLBLUETOOTH_uuid_t;

/** Server attribute */
typedef struct {
	uint8_t type; // one of ATTRIBUTE_TYPE_* values
	uint8_t _padding;
	union {
		struct {
			LLBLUETOOTH_uuid_t uuid;
			uint8_t permissions; // combined ATTRIBUTE_PERMISSION_* values
			uint8_t properties;
		} characteristic;
		struct {
			LLBLUETOOTH_uuid_t uuid;
			uint8_t permissions; // combined ATTRIBUTE_PERMISSION_* values
		} descriptor;
	};
} LLBLUETOOTH_gatts_attribute_t;

/** Server service */
typedef struct {
	LLBLUETOOTH_uuid_t uuid;
	uint8_t num_characteristics;
	uint8_t num_descriptors;
	// followed by: LLBLUETOOTH_gatts_attribute_t attributes[num_char+num_desc];
} LLBLUETOOTH_gatts_service_t;

/** Client attribute */
typedef struct {
	uint8_t type; // one of ATTRIBUTE_TYPE_* values
	uint8_t _padding;
	LLBLUETOOTH_uuid_t uuid;
	union {
		struct {
			uint16_t value_handle;
			uint8_t properties;
			uint8_t _padding;
		} characteristic;
		struct {
			uint16_t handle;
		} descriptor;
	};
} LLBLUETOOTH_gattc_attribute_t;

/** Generic event - can be casted to one of the event structures depending on event type */
typedef struct {
	uint8_t event_type; // one of EVENT_* values
} LLBLUETOOTH_event_t;

/** EVENT_GAP_SCAN_RESULT */
typedef struct {
	uint8_t event_type;
	LLBLUETOOTH_address_t addr;
	uint8_t rssi;
	uint8_t adv_data_size;
	// followed by: uint8_t adv_data[adv_data_size];
} LLBLUETOOTH_event_gap_scan_result_t;

/** EVENT_GAP_SCAN_COMPLETED */
typedef struct {
	uint8_t event_type;
} LLBLUETOOTH_event_gap_scan_completed_t;

/** EVENT_GAP_ADVERTISEMENT_COMPLETED */
typedef struct {
	uint8_t event_type;
} LLBLUETOOTH_event_gap_advertisement_completed_t;

/** EVENT_GAP_CONNECT_FAILED */
typedef struct {
	uint8_t event_type;
	LLBLUETOOTH_address_t addr;
} LLBLUETOOTH_event_gap_connect_failed_t;

/** EVENT_GAP_CONNECTED */
typedef struct {
	uint8_t event_type;
	LLBLUETOOTH_address_t addr;
	uint16_t conn_handle;
} LLBLUETOOTH_event_gap_connected_t;

/** EVENT_GAP_DISCONNECTED */
typedef struct {
	uint8_t event_type;
	uint8_t _padding;
	uint16_t conn_handle;
} LLBLUETOOTH_event_gap_disconnected_t;

/** EVENT_GAP_PAIR_REQUEST */
typedef struct {
	uint8_t event_type;
	uint8_t _padding;
	uint16_t conn_handle;
} LLBLUETOOTH_event_gap_pair_request_t;

/** EVENT_GAP_PAIR_COMPLETED */
typedef struct {
	uint8_t event_type;
	uint8_t success;
	uint16_t conn_handle;
} LLBLUETOOTH_event_gap_pair_completed_t;

/** EVENT_GAP_PASSKEY_REQUEST */
typedef struct {
	uint8_t event_type;
	uint8_t _padding;
	uint16_t conn_handle;
} LLBLUETOOTH_event_gap_passkey_request_t;

/** EVENT_GAP_PASSKEY_GENERATED */
typedef struct {
	uint8_t event_type;
	uint8_t _padding;
	uint16_t conn_handle;
	uint32_t passkey;
} LLBLUETOOTH_event_gap_passkey_generated_t;

/** EVENT_GATTC_DISCOVERY_RESULT */
typedef struct {
	uint8_t event_type;
	uint8_t num_attributes;
	uint16_t conn_handle;
	uint16_t service_handle;
	LLBLUETOOTH_uuid_t service_uuid;
	// followed by: LLBLUETOOTH_gattc_attribute_t attributes[num_attributes];
} LLBLUETOOTH_event_gattc_discovery_result_t;

/** EVENT_GATTC_DISCOVERY_COMPLETED */
typedef struct {
	uint8_t event_type;
	uint8_t _padding;
	uint16_t conn_handle;
} LLBLUETOOTH_event_gattc_discovery_completed_t;

/** EVENT_GATTC_READ_COMPLETED */
typedef struct {
	uint8_t event_type;
	uint8_t status;
	uint16_t conn_handle;
	uint16_t attr_handle;
	uint16_t value_size;
	// followed by: uint8_t value[value_size];
} LLBLUETOOTH_event_gattc_read_completed_t;

/** EVENT_GATTC_WRITE_COMPLETED */
typedef struct {
	uint8_t event_type;
	uint8_t status;
	uint16_t conn_handle;
	uint16_t attr_handle;
} LLBLUETOOTH_event_gattc_write_completed_t;

/** EVENT_GATTC_NOTIFICATION_RECEIVED */
typedef struct {
	uint8_t event_type;
	uint8_t _padding;
	uint16_t conn_handle;
	uint16_t attr_handle;
	uint16_t value_size;
	// followed by: uint8_t value[value_size];
} LLBLUETOOTH_event_gattc_notification_received_t;

/** EVENT_GATTS_READ_REQUEST */
typedef struct {
	uint8_t event_type;
	uint8_t _padding;
	uint16_t conn_handle;
	uint16_t attr_handle;
} LLBLUETOOTH_event_gatts_read_request_t;

/** EVENT_GATTS_WRITE_REQUEST */
typedef struct {
	uint8_t event_type;
	uint8_t _padding;
	uint16_t conn_handle;
	uint16_t attr_handle;
	uint16_t value_size;
	// followed by: uint8_t value[value_size];
} LLBLUETOOTH_event_gatts_write_request_t;

/** EVENT_GATTS_NOTIFICATION_SENT */
typedef struct {
	uint8_t event_type;
	uint8_t success;
	uint16_t conn_handle;
	uint16_t attr_handle;
} LLBLUETOOTH_event_gatts_notification_sent_t;

#ifdef __cplusplus
}
#endif

#endif // _LLBLUETOOTH_DEFINES
