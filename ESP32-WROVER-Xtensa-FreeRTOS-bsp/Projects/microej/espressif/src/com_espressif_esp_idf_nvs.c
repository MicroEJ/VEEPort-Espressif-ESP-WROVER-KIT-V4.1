/*
 * C
 *
 * Copyright 2020 MicroEJ Corp. All rights reserved.
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

#include "com_espressif_esp_idf_nvs.h"
#include "nvs.h"

void Java_com_espressif_esp_1idf_nvs_nvs_1close(jint handle) {
	nvs_close(handle);
}

void Java_com_espressif_esp_1idf_nvs_nvs_1commit(jint handle) {
	int err = nvs_commit(handle);
	if (err != ESP_OK) {
		SNI_throwNativeIOException(err, NULL);
	}
}

void Java_com_espressif_esp_1idf_nvs_nvs_1erase_1all(jint handle) {
	int err = nvs_erase_all(handle);
	if (err != ESP_OK) {
		SNI_throwNativeIOException(err, NULL);
	}
}

void Java_com_espressif_esp_1idf_nvs_nvs_1erase_1key(jint handle, jbyte* key) {
	int err = nvs_erase_key(handle, (char*) key);
	if (err != ESP_OK) {
		SNI_throwNativeIOException(err, NULL);
	}
}

jint Java_com_espressif_esp_1idf_nvs_nvs_1get_1blob(jint handle, jbyte* key, jbyte* blob, jint length) {
	int err;
	err = nvs_get_blob(handle, (char*) key, (char*) blob, (size_t*) &length);
	if (err != ESP_OK){
		SNI_throwNativeIOException(err, NULL);
	}
	return length;
}

jshort Java_com_espressif_esp_1idf_nvs_nvs_1get_1i16(jint handle, jbyte* key) {
	jshort value;
	int err = nvs_get_i16(handle, (char*) key, &value);
	if (err != ESP_OK) {
		SNI_throwNativeIOException(err, NULL);
	}
	return value;
}

jint Java_com_espressif_esp_1idf_nvs_nvs_1get_1i32(jint handle, jbyte* key) {
	jint value;
	int err = nvs_get_i32(handle, (char*) key, &value);
	if (err != ESP_OK) {
		SNI_throwNativeIOException(err, NULL);
	}
	return value;
}

jlong Java_com_espressif_esp_1idf_nvs_nvs_1get_1i64(jint handle, jbyte* key) {
	jlong value;
	int err = nvs_get_i64(handle, (char*) key, &value);
	if (err != ESP_OK) {
		SNI_throwNativeIOException(err, NULL);
	}
	return value;
}

jbyte Java_com_espressif_esp_1idf_nvs_nvs_1get_1i8(jint handle, jbyte* key) {
	jbyte value;
	int err = nvs_get_i8(handle, (char*) key, &value);
	if (err != ESP_OK) {
		SNI_throwNativeIOException(err, NULL);
	}
	return value;
}

jint Java_com_espressif_esp_1idf_nvs_nvs_1get_1str(jint handle, jbyte* key, jbyte* str, jint length) {
	int err;
	err = nvs_get_str(handle, (char*) key, (char*) str, (size_t*) &length);
	if (err != ESP_OK) {
		SNI_throwNativeIOException(err, NULL);
	}
	return length;
}

jshort Java_com_espressif_esp_1idf_nvs_nvs_1get_1u16(jint handle, jbyte* key) {
	jshort value;
	int err = nvs_get_u16(handle, (char*) key, (uint16_t *) &value);
	if (err != ESP_OK) {
		SNI_throwNativeIOException(err, NULL);
	}
	return value;
}

jint Java_com_espressif_esp_1idf_nvs_nvs_1get_1u32(jint handle, jbyte* key) {
	jint value;
	int err = nvs_get_u32(handle, (char*) key, (uint32_t *) &value);
	if (err != ESP_OK) {
		SNI_throwNativeIOException(err, NULL);
	}
	return value;
}

jlong Java_com_espressif_esp_1idf_nvs_nvs_1get_1u64(jint handle, jbyte* key) {
	jlong value;
	int err = nvs_get_u64(handle, (char*) key, (uint64_t *) &value);
	if (err != ESP_OK) {
		SNI_throwNativeIOException(err, NULL);
	}
	return value;
}

jbyte Java_com_espressif_esp_1idf_nvs_nvs_1get_1u8(jint handle, jbyte* key) {
	jbyte value;
	int err = nvs_get_u8(handle, (char*) key, (uint8_t *) &value);
	if (err != ESP_OK) {
		SNI_throwNativeIOException(err, NULL);
	}
	return value;
}

jlong Java_com_espressif_esp_1idf_nvs_nvs_1get_1used_1entry_1count(jint handle) {
	size_t used_entries;
	int err = nvs_get_used_entry_count(handle, &used_entries);
	if (err != ESP_OK) {
		SNI_throwNativeIOException(err, NULL);
	}
	return used_entries;
}

jint Java_com_espressif_esp_1idf_nvs_nvs_1open(jbyte* name, jint open_mode) {
	nvs_handle handle;
	int err = nvs_open((char*) name, open_mode, &handle);
	if (err != ESP_OK) {
		SNI_throwNativeIOException(err, NULL);
	}
	return handle;
}

jint Java_com_espressif_esp_1idf_nvs_nvs_1open_1from_1partition(jbyte* part_name, jbyte* name, jint open_mode) {
	nvs_handle handle;
	int err = nvs_open_from_partition((char*) part_name, (char*) name, open_mode, &handle);
	if (err != ESP_OK) {
		SNI_throwNativeIOException(err, NULL);
	}
	return handle;
}

void Java_com_espressif_esp_1idf_nvs_nvs_1set_1blob(jint handle, jbyte* key, jbyte* blob, jint length) {
	int err = nvs_set_blob(handle, (char*) key, (char*) blob, (size_t) length);
	if (err != ESP_OK) {
		SNI_throwNativeIOException(err, NULL);
	}
}

void Java_com_espressif_esp_1idf_nvs_nvs_1set_1i16(jint handle, jbyte* key, jshort value) {
	int err = nvs_set_i16(handle, (char*) key, value);
	if (err != ESP_OK) {
		SNI_throwNativeIOException(err, NULL);
	}
}

void Java_com_espressif_esp_1idf_nvs_nvs_1set_1i32(jint handle, jbyte* key, jint value) {
	int err = nvs_set_i32(handle, (char*) key, value);
	if (err != ESP_OK) {
		SNI_throwNativeIOException(err, NULL);
	}
}

void Java_com_espressif_esp_1idf_nvs_nvs_1set_1i64(jint handle, jbyte* key, jlong value) {
	int err = nvs_set_i64(handle, (char*) key, value);
	if (err != ESP_OK) {
		SNI_throwNativeIOException(err, NULL);
	}
}

void Java_com_espressif_esp_1idf_nvs_nvs_1set_1i8(jint handle, jbyte* key, jbyte value) {
	int err = nvs_set_i8(handle, (char*) key, value);
	if (err != ESP_OK) {
		SNI_throwNativeIOException(err, NULL);
	}
}

void Java_com_espressif_esp_1idf_nvs_nvs_1set_1str(jint handle, jbyte* key, jbyte* value) {
	int err = nvs_set_str(handle, (char*) key, (char*) value);
	if (err != ESP_OK) {
		SNI_throwNativeIOException(err, NULL);
	}
}

void Java_com_espressif_esp_1idf_nvs_nvs_1set_1u16(jint handle, jbyte* key, jshort value) {
	int err = nvs_set_u16(handle, (char*) key, value);
	if (err != ESP_OK) {
		SNI_throwNativeIOException(err, NULL);
	}
}

void Java_com_espressif_esp_1idf_nvs_nvs_1set_1u32(jint handle, jbyte* key, jint value) {
	int err = nvs_set_u32(handle, (char*) key, value);
	if (err != ESP_OK) {
		SNI_throwNativeIOException(err, NULL);
	}
}

void Java_com_espressif_esp_1idf_nvs_nvs_1set_1u64(jint handle, jbyte* key, jlong value) {
	int err = nvs_set_u64(handle, (char*) key, value);
	if (err != ESP_OK) {
		SNI_throwNativeIOException(err, NULL);
	}
}

void Java_com_espressif_esp_1idf_nvs_nvs_1set_1u8(jint handle, jbyte* key, jbyte value) {
	int err = nvs_set_u8(handle, (char*) key, value);
	if (err != ESP_OK) {
		SNI_throwNativeIOException(err, NULL);
	}
}
