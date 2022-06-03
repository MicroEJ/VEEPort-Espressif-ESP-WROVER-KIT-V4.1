/*
 * C
 *
 * Copyright 2020-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#ifndef ECOM_WIFI_HELPER_H
#define ECOM_WIFI_HELPER_H

/**
 * @file
 * @brief ECOM-WIFI helper implementation.
 * @author MicroEJ Developer Team
 * @version 2.1.0
 * @date 17 June 2021
 */

#include "ecom_wifi_configuration.h"
#include "microej_async_worker.h"

#ifdef __cplusplus
	extern "C" {
#endif

/*
 * IMPORTANT NOTICES
 *
 * This file lists the data structures used by all the LLECOM_WIFI_* functions.
 * If you add a new function and its corresponding data structure, do not forget to add
 * the data structure to the ECOM_WIFI_worker_param_t union.
 *
 * Some structures duplicate the definition of fields specified in generic structures instead of
 * referencing the generic structures (e.g. the <code>path</code> and <code>result</code> fields).
 * This decision has been made to simplify usage and readability at the expense of maintainability.
 * Be careful when modifying the fields of the structures or when defining new structures.
 * See the comment of each structure to identify the ones that are affected by these constraints.
 */

/**
 * @brief Data structure for disable SoftAP, leave and some get operations.
 *
 * This structure is used by <code>LLECOM_WIFI_IMPL_disableSoftAP</code>, <code>LLECOM_WIFI_IMPL_getChannel</code>,
 * <code>LLECOM_WIFI_IMPL_getClientState</code>, <code>LLECOM_WIFI_IMPL_getAccessPointState</code>,
 * <code>LLECOM_WIFI_IMPL_getSecurityMode</code>, <code>LLECOM_WIFI_IMPL_getWPSModes</code> and
 * <code>LLECOM_WIFI_IMPL_leave</code>.
 *
 * Fields defined in this structure correspond to the parameters of these functions and
 * <code>result</code> field corresponds to the value returned by them.
 */
typedef struct {
	int32_t result; /*!< [OUT] Result of the operation. */
	int32_t error_code; /*!< [OUT] Error code returned in case of error. */
	char* error_message; /*!< [OUT] Error message related to the error code. */
} ECOM_WIFI_get_info_t;

/**
 * @brief Data structure for enable SoftAP and join operations.
 *
 * This structure is used by <code>LLECOM_WIFI_IMPL_enableSoftAP</code>.
 *
 * Fields defined in this structure correspond to the parameters of these functions and
 * <code>result</code> field corresponds to the value returned by them.
 */
typedef struct {
	int8_t* ssid; /*!< [IN] The buffer into which the ssid will be stored. */
	int32_t ssidLength; /*!< [IN] The ssid buffer length. */
	int8_t* passphrase; /*!< [IN] The buffer into which the passphrase will be stored. */
	int32_t passphraseLength; /*!< [IN] The passphrase buffer length. */
	int32_t result; /*!< [OUT] Result of the operation. */
	int32_t error_code; /*!< [OUT] Error code returned in case of error. */
	char* error_message; /*!< [OUT] Error message related to the error code. */
	int8_t buffer_ssid[MAX_SSID_SIZE]; /* Internal buffer that store the ssid. */
	int8_t buffer_pass[MAX_PASSPHRASE_SIZE]; /* Internal buffer that store the passphrase. */
} ECOM_WIFI_enable_softap_t;

/**
 * @brief Data structure for join with security mode operations.
 *
 * This structure is used by <code>LLECOM_WIFI_IMPL_join</code>
 * and <code>LLECOM_WIFI_IMPL_joinWithSecurityMode</code>.
 *
 * Fields defined in this structure correspond to the parameters of this function and
 * <code>result</code> field corresponds to the value returned by it.
 */
typedef struct {
	int8_t* ssid; /*!< [IN] The buffer into which the ssid will be stored. */
	int32_t ssidLength; /*!< [IN] The ssid buffer length. */
	int8_t* passphrase; /*!< [IN] The buffer into which the passphrase will be stored. */
	int32_t passphraseLength; /*!< [IN] The passphrase buffer length. */
	int32_t securityModeValue; /*!< [IN] The ssid security mode value. */
	int32_t result; /*!< [OUT] Result of the operation. */
	int32_t error_code; /*!< [OUT] Error code returned in case of error. */
	char* error_message; /*!< [OUT] Error message related to the error code. */
	int8_t buffer_ssid[MAX_SSID_SIZE]; /* Internal buffer that store the ssid. */
	int8_t buffer_pass[MAX_PASSPHRASE_SIZE]; /* Internal buffer that store the passphrase. */
} ECOM_WIFI_join_security_mode_t;

/**
 * @brief Data structure for set name SoftAP operations.
 *
 * This structure is used by <code>LLECOM_WIFI_IMPL_setNameSoftAP</code>
 *
 * Fields defined in this structure correspond to the parameters of this function and
 * <code>result</code> field corresponds to the value returned by it.
 */
typedef struct {
	int8_t* name; /*!< [IN] The buffer into which the name will be stored. */
	int32_t nameLength; /*!< [IN] The name buffer length. */
	int32_t result; /*!< [OUT] Result of the operation. */
	int32_t error_code; /*!< [OUT] Error code returned in case of error. */
	char* error_message; /*!< [OUT] Error message related to the error code. */
	int8_t buffer_name[MAX_NAME_SIZE]; /* Internal buffer that store the name. */
} ECOM_WIFI_set_name_t;

/**
 * @brief Data structure for get BSSID/SSID operations.
 *
 * This structure is used by <code>LLECOM_WIFI_IMPL_getBSSID</code> and
 * <code>LLECOM_WIFI_IMPL_getSSID</code>.
 *
 * Fields defined in this structure correspond to the parameters of this function and
 * <code>result</code> field corresponds to the value returned by it.
 */
typedef struct {
	int8_t* xssid; /*!< [OUT] The buffer into which the bssid/ssid will be stored. */
	int32_t xssidLength; /*!< [IN] The bssid/ssid buffer length. */
	int32_t result; /*!< [OUT] Result of the operation. */
	int32_t error_code; /*!< [OUT] Error code returned in case of error. */
	char* error_message; /*!< [OUT] Error message related to the error code. */
	int8_t buffer_xssid[MAX_SSID_SIZE]; /* Internal buffer that store the bssid/ssid. */
} ECOM_WIFI_get_xssid_t;

/**
 * @brief Data structure for get RSSI operations.
 *
 * This structure is used by <code>LLECOM_WIFI_IMPL_getRSSI</code>
 *
 * Fields defined in this structure correspond to the parameters of this function and
 * <code>result</code> field corresponds to the value returned by it.
 */
typedef struct {
	float* rssi; /*!< [OUT] The buffer into which the rssi will be stored (data stored on 1 byte). */
	int32_t rssiLength; /*!< [IN] The rssi buffer length. */
	int32_t result; /*!< [OUT] Result of the operation. */
	int32_t error_code; /*!< [OUT] Error code returned in case of error. */
	char* error_message; /*!< [OUT] Error message related to the error code. */
	int8_t buffer_rssi[MAX_SSID_SIZE]; /* Internal buffer that store the ssid. */
} ECOM_WIFI_get_rssi_t;

/**
 * @brief Data structure for scan AP count operations.
 *
 * This structure is used by <code>LLECOM_WIFI_IMPL_scanAPCount</code>
 *
 * Fields defined in this structure correspond to the parameters of this function and
 * <code>result</code> field corresponds to the value returned by it.
 */
typedef struct {
	int8_t active; /*!< [IN] True for an active scan, false for a passive scan. */
	int32_t result; /*!< [OUT] Result of the operation. */
	int32_t error_code; /*!< [OUT] Error code returned in case of error. */
	char* error_message; /*!< [OUT] Error message related to the error code. */
} ECOM_WIFI_scanAP_count_t;

/**
 * @brief Data structure for scan AP bssid/ssid available at index operations.
 *
 * This structure is used by <code>LLECOM_WIFI_IMPL_scanAPbssidAt</code> and
 * <code>LLECOM_WIFI_IMPL_scanAPssidAt</code>.
 *
 * Fields defined in this structure correspond to the parameters of this function and
 * <code>result</code> field corresponds to the value returned by it.
 */
typedef struct {
	int8_t* xssid; /*!< [OUT] The buffer into which the bssid/ssid will be stored. */
	int32_t xssidLength; /*!< [IN] The bssid/ssid buffer length. */
	int32_t index; /*!< [IN] The index of the Access Point currently available. */
	int32_t result; /*!< [OUT] Result of the operation. */
	int32_t error_code; /*!< [OUT] Error code returned in case of error. */
	char* error_message; /*!< [OUT] Error message related to the error code. */
	int8_t buffer_xssid[MAX_SSID_SIZE]; /* Internal buffer that store the bssid/ssid. */
} ECOM_WIFI_scanAP_xssid_t;

/**
 * @brief Data structure for scan AP channel, mode and wpsMode available at index operations.
 *
 * This structure is used by <code>LLECOM_WIFI_IMPL_scanAPchannelAt</code>,
 * <code>LLECOM_WIFI_IMPL_scanAPsecurityModeAt</code> and <code>LLECOM_WIFI_IMPL_scanAPwpsModeAt</code>.
 *
 * Fields defined in this structure correspond to the parameters of these functions and
 * <code>result</code> field corresponds to the value returned by them.
 */
typedef struct {
	int32_t index; /*!< [IN] The index of the Access Point currently available. */
	int32_t result; /*!< [OUT] Result of the operation. */
	int32_t error_code; /*!< [OUT] Error code returned in case of error. */
	char* error_message; /*!< [OUT] Error message related to the error code. */
} ECOM_WIFI_scanAP_channel_t;

/**
 * @brief Data structure for scan AP rssi available at index operations.
 *
 * This structure is used by <code>LLECOM_WIFI_IMPL_scanAPrssiAt</code>
 *
 * Fields defined in this structure correspond to the parameters of this function and
 * <code>result</code> field corresponds to the value returned by it.
 */
typedef struct {
	float* rssi; /*!< [OUT] The buffer into which the rssi will be stored. */
	int32_t rssiLength; /*!< [IN] The rssi buffer length. */
	int32_t index; /*!< [IN] The index of the Access Point currently available. */
	int32_t result; /*!< [OUT] Result of the operation. */
	int32_t error_code; /*!< [OUT] Error code returned in case of error. */
	char* error_message; /*!< [OUT] Error message related to the error code. */
	int8_t buffer_rssi[MAX_SSID_SIZE]; /* Internal buffer that store the ssid. */
} ECOM_WIFI_scanAP_rssi_t;

/**
 * @union ECOM_WIFI_worker_param_t
 */
typedef union {
	ECOM_WIFI_get_info_t get_info;
	ECOM_WIFI_enable_softap_t enable_softap;
	ECOM_WIFI_join_security_mode_t join_security_mode;
	ECOM_WIFI_set_name_t set_name;
	ECOM_WIFI_get_xssid_t get_xssid;
	ECOM_WIFI_get_rssi_t get_rssi;
	ECOM_WIFI_scanAP_count_t scanAP_count;
	ECOM_WIFI_scanAP_xssid_t scanAP_xssid;
	ECOM_WIFI_scanAP_channel_t scanAP_channel;
	ECOM_WIFI_scanAP_rssi_t scanAP_rssi;
} ECOM_WIFI_worker_param_t;

/**
 * @brief Initialize the underlying layer which will execute all LLECOM_WIFI_* actions.
 */
void LLECOM_WIFI_helper_initialize(void);

/**
 * @brief Action requested by <code>LLECOM_WIFI_IMPL_disableSoftAP</code> and executed asynchronously via async_worker.
 *
 * @param[in] job the context of the job, containing input/output parameters
 */
void LLECOM_WIFI_IMPL_disableSoftAP_action(MICROEJ_ASYNC_WORKER_job_t* job);

/**
 * @brief Action requested by <code>LLECOM_WIFI_IMPL_enableSoftAP</code> and executed asynchronously via async_worker.
 *
 * @param[in] job the context of the job, containing input/output parameters
 */
void LLECOM_WIFI_IMPL_enableSoftAP_action(MICROEJ_ASYNC_WORKER_job_t* job);

/**
 * @brief Action requested by <code>LLECOM_WIFI_IMPL_setNameSoftAP</code> and executed asynchronously via async_worker.
 *
 * @param[in] job the context of the job, containing input/output parameters
 */
void LLECOM_WIFI_IMPL_setNameSoftAP_action(MICROEJ_ASYNC_WORKER_job_t* job);

/**
 * @brief Action requested by <code>LLECOM_WIFI_IMPL_getBSSID</code> and executed asynchronously via async_worker.
 *
 * @param[in] job the context of the job, containing input/output parameters
 */
void LLECOM_WIFI_IMPL_getBSSID_action(MICROEJ_ASYNC_WORKER_job_t* job);

/**
 * @brief Action requested by <code>LLECOM_WIFI_IMPL_getChannel</code> and executed asynchronously via async_worker.
 *
 * @param[in] job the context of the job, containing input/output parameters
 */
void LLECOM_WIFI_IMPL_getChannel_action(MICROEJ_ASYNC_WORKER_job_t* job);

/**
 * @brief Action requested by <code>LLECOM_WIFI_IMPL_getClientState</code> and executed asynchronously via async_worker.
 *
 * @param[in] job the context of the job, containing input/output parameters
 */
void LLECOM_WIFI_IMPL_getClientState_action(MICROEJ_ASYNC_WORKER_job_t* job);

/**
 * @brief Action requested by <code>LLECOM_WIFI_IMPL_getAccessPointState</code> and executed asynchronously via async_worker.
 *
 * @param[in] job the context of the job, containing input/output parameters
 */
void LLECOM_WIFI_IMPL_getAccessPointState_action(MICROEJ_ASYNC_WORKER_job_t* job);

/**
 * @brief Action requested by <code>LLECOM_WIFI_IMPL_getRSSI</code> and executed asynchronously via async_worker.
 *
 * @param[in] job the context of the job, containing input/output parameters
 */
void LLECOM_WIFI_IMPL_getRSSI_action(MICROEJ_ASYNC_WORKER_job_t* job);

/**
 * @brief Action requested by <code>LLECOM_WIFI_IMPL_getSecurityMode</code> and executed asynchronously via async_worker.
 *
 * @param[in] job the context of the job, containing input/output parameters
 */
void LLECOM_WIFI_IMPL_getSecurityMode_action(MICROEJ_ASYNC_WORKER_job_t* job);

/**
 * @brief Action requested by <code>LLECOM_WIFI_IMPL_getSSID</code> and executed asynchronously via async_worker.
 *
 * @param[in] job the context of the job, containing input/output parameters
 */
void LLECOM_WIFI_IMPL_getSSID_action(MICROEJ_ASYNC_WORKER_job_t* job);

/**
 * @brief Action requested by <code>LLECOM_WIFI_IMPL_getWPSModes</code> and executed asynchronously via async_worker.
 *
 * @param[in] job the context of the job, containing input/output parameters
 */
void LLECOM_WIFI_IMPL_getWPSModes_action(MICROEJ_ASYNC_WORKER_job_t* job);

/**
 * @brief Action requested by <code>LLECOM_WIFI_IMPL_joinWithSecurityMode</code> and executed asynchronously via async_worker.
 *
 * @param[in] job the context of the job, containing input/output parameters
 */
void LLECOM_WIFI_IMPL_joinWithSecurityMode_action(MICROEJ_ASYNC_WORKER_job_t* job);

/**
 * @brief Action requested by <code>LLECOM_WIFI_IMPL_leave</code> and executed asynchronously via async_worker.
 *
 * @param[in] job the context of the job, containing input/output parameters
 */
void LLECOM_WIFI_IMPL_leave_action(MICROEJ_ASYNC_WORKER_job_t* job);

/**
 * @brief Action requested by <code>LLECOM_WIFI_IMPL_scanAPCount</code> and executed asynchronously via async_worker.
 *
 * @param[in] job the context of the job, containing input/output parameters
 */
void LLECOM_WIFI_IMPL_scanAPCount_action(MICROEJ_ASYNC_WORKER_job_t* job);

/**
 * @brief Action requested by <code>LLECOM_WIFI_IMPL_scanAPssidAt</code> and executed asynchronously via async_worker.
 *
 * @param[in] job the context of the job, containing input/output parameters
 */
void LLECOM_WIFI_IMPL_scanAPssidAt_action(MICROEJ_ASYNC_WORKER_job_t* job);

/**
 * @brief Action requested by <code>LLECOM_WIFI_IMPL_scanAPbssidAt</code> and executed asynchronously via async_worker.
 *
 * @param[in] job the context of the job, containing input/output parameters
 */
void LLECOM_WIFI_IMPL_scanAPbssidAt_action(MICROEJ_ASYNC_WORKER_job_t* job);

/**
 * @brief Action requested by <code>LLECOM_WIFI_IMPL_scanAPchannelAt</code> and executed asynchronously via async_worker.
 *
 * @param[in] job the context of the job, containing input/output parameters
 */
void LLECOM_WIFI_IMPL_scanAPchannelAt_action(MICROEJ_ASYNC_WORKER_job_t* job);

/**
 * @brief Action requested by <code>LLECOM_WIFI_IMPL_scanAPrssiAt</code> and executed asynchronously via async_worker.
 *
 * @param[in] job the context of the job, containing input/output parameters
 */
void LLECOM_WIFI_IMPL_scanAPrssiAt_action(MICROEJ_ASYNC_WORKER_job_t* job);

/**
 * @brief Action requested by <code>LLECOM_WIFI_IMPL_scanAPsecurityModeAt</code> and executed asynchronously via async_worker.
 *
 * @param[in] job the context of the job, containing input/output parameters
 */
void LLECOM_WIFI_IMPL_scanAPsecurityModeAt_action(MICROEJ_ASYNC_WORKER_job_t* job);

/**
 * @brief Action requested by <code>LLECOM_WIFI_IMPL_scanAPwpsModeAt</code> and executed asynchronously via async_worker.
 *
 * @param[in] job the context of the job, containing input/output parameters
 */
void LLECOM_WIFI_IMPL_scanAPwpsModeAt_action(MICROEJ_ASYNC_WORKER_job_t* job);

#ifdef __cplusplus
	}
#endif

#endif /* ECOM_WIFI_HELPER_H */
