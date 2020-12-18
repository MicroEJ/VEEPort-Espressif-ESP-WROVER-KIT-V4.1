/*
 * C
 *
 * Copyright 2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */

#ifndef _com_espressif_esp_idf_nvs
#define _com_espressif_esp_idf_nvs 

/**
 * @file
 * @brief esp-idf low level API
 * @author MicroEJ Developer Team
 * @version 1.1.1
 * @date 13 November 2020
 */

#include <sni.h>
#ifdef __cplusplus
	extern "C" {
#endif
void Java_com_espressif_esp_1idf_nvs_nvs_1close(jint arg0);
void Java_com_espressif_esp_1idf_nvs_nvs_1commit(jint arg0);
void Java_com_espressif_esp_1idf_nvs_nvs_1erase_1all(jint arg0);
void Java_com_espressif_esp_1idf_nvs_nvs_1erase_1key(jint arg0, jbyte* arg1);
jint Java_com_espressif_esp_1idf_nvs_nvs_1get_1blob(jint arg0, jbyte* arg1, jbyte* arg2, jint arg3);
jshort Java_com_espressif_esp_1idf_nvs_nvs_1get_1i16(jint arg0, jbyte* arg1);
jint Java_com_espressif_esp_1idf_nvs_nvs_1get_1i32(jint arg0, jbyte* arg1);
jlong Java_com_espressif_esp_1idf_nvs_nvs_1get_1i64(jint arg0, jbyte* arg1);
jbyte Java_com_espressif_esp_1idf_nvs_nvs_1get_1i8(jint arg0, jbyte* arg1);
jint Java_com_espressif_esp_1idf_nvs_nvs_1get_1str(jint arg0, jbyte* arg1, jbyte* arg2, jint arg3);
jshort Java_com_espressif_esp_1idf_nvs_nvs_1get_1u16(jint arg0, jbyte* arg1);
jint Java_com_espressif_esp_1idf_nvs_nvs_1get_1u32(jint arg0, jbyte* arg1);
jlong Java_com_espressif_esp_1idf_nvs_nvs_1get_1u64(jint arg0, jbyte* arg1);
jbyte Java_com_espressif_esp_1idf_nvs_nvs_1get_1u8(jint arg0, jbyte* arg1);
jlong Java_com_espressif_esp_1idf_nvs_nvs_1get_1used_1entry_1count(jint arg0);
jint Java_com_espressif_esp_1idf_nvs_nvs_1open(jbyte* arg0, jint arg1);
jint Java_com_espressif_esp_1idf_nvs_nvs_1open_1from_1partition(jbyte* arg0, jbyte* arg1, jint arg2);
void Java_com_espressif_esp_1idf_nvs_nvs_1set_1blob(jint arg0, jbyte* arg1, jbyte* arg2, jint arg3);
void Java_com_espressif_esp_1idf_nvs_nvs_1set_1i16(jint arg0, jbyte* arg1, jshort arg2);
void Java_com_espressif_esp_1idf_nvs_nvs_1set_1i32(jint arg0, jbyte* arg1, jint arg2);
void Java_com_espressif_esp_1idf_nvs_nvs_1set_1i64(jint arg0, jbyte* arg1, jlong arg2);
void Java_com_espressif_esp_1idf_nvs_nvs_1set_1i8(jint arg0, jbyte* arg1, jbyte arg2);
void Java_com_espressif_esp_1idf_nvs_nvs_1set_1str(jint arg0, jbyte* arg1, jbyte* arg2);
void Java_com_espressif_esp_1idf_nvs_nvs_1set_1u16(jint arg0, jbyte* arg1, jshort arg2);
void Java_com_espressif_esp_1idf_nvs_nvs_1set_1u32(jint arg0, jbyte* arg1, jint arg2);
void Java_com_espressif_esp_1idf_nvs_nvs_1set_1u64(jint arg0, jbyte* arg1, jlong arg2);
void Java_com_espressif_esp_1idf_nvs_nvs_1set_1u8(jint arg0, jbyte* arg1, jbyte arg2);
#ifdef __cplusplus
	}
#endif
#endif
