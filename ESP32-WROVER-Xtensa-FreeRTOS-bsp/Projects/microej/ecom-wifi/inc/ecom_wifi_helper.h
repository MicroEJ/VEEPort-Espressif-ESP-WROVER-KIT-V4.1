/*
 * C
 *
 * Copyright 2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 *
 */

#ifndef ECOM_WIFI_HELPER_H
#define ECOM_WIFI_HELPER_H

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
 * <code>LLECOM_WIFI_IMPL_getClientState</code>, <code>LLECOM_WIFI_IMPL_getSecurityMode</code>,
 * <code>LLECOM_WIFI_IMPL_getWPSModes</code>, <code>LLECOM_WIFI_IMPL_leave</code>
 * and <code>LLECOM_WIFI_IMPL_getCapability</code>.
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
 * This structure is used by <code>LLECOM_WIFI_IMPL_enableSoftAP</code>
 * and <code>LLECOM_WIFI_IMPL_join</code>.
 *
 * Fields defined in this structure correspond to the parameters of these functions and
 * <code>result</code> field corresponds to the value returned by them.
 */
typedef struct {
	int8_t* ssid; /*!< [IN] The buffer into which the ssid will be stored. */
	int32_t ssidOffset; /*!< [IN] The offset where the ssid will be stored in the buffer. */
	int32_t ssidLength; /*!< [IN] The ssid buffer length. */
	int8_t* passphrase; /*!< [IN] The buffer into which the passphrase will be stored. */
	int32_t passphraseOffset; /*!< [IN] The offset where the passphrase will be store in the buffer. */
	int32_t passphraseLength; /*!< [IN] The passphrase buffer length. */
	int32_t result; /*!< [OUT] Result of the operation. */
	int32_t error_code; /*!< [OUT] Error code returned in case of error. */
	char* error_message; /*!< [OUT] Error message related to the error code. */
} ECOM_WIFI_enable_join_t;

/**
 * @brief Data structure for join security mode operations.
 *
 * This structure is used by <code>LLECOM_WIFI_IMPL_joinWithSecurityMode</code>.
 *
 * Fields defined in this structure correspond to the parameters of this function and
 * <code>result</code> field corresponds to the value returned by it.
 */
typedef struct {
	int8_t* ssid; /*!< [IN] The buffer into which the ssid will be stored. */
	int32_t ssidOffset; /*!< [IN] The offset where the ssid will be stored in the buffer. */
	int32_t ssidLength; /*!< [IN] The ssid buffer length. */
	int8_t* passphrase; /*!< [IN] The buffer into which the passphrase will be stored. */
	int32_t passphraseOffset; /*!< [IN] The offset where the passphrase will be store in the buffer. */
	int32_t passphraseLength; /*!< [IN] The passphrase buffer length. */
	int32_t securityModeValue; /*!< [IN] The ssid security mode value. */
	int32_t result; /*!< [OUT] Result of the operation. */
	int32_t error_code; /*!< [OUT] Error code returned in case of error. */
	char* error_message; /*!< [OUT] Error message related to the error code. */
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
	int32_t nameOffset; /*!< [IN] The offset where the name will be stored in the buffer. */
	int32_t nameLength; /*!< [IN] The name buffer length. */
	int32_t result; /*!< [OUT] Result of the operation. */
	int32_t error_code; /*!< [OUT] Error code returned in case of error. */
	char* error_message; /*!< [OUT] Error message related to the error code. */
} ECOM_WIFI_set_name_t;

/**
 * @brief Data structure for get BSSID operations.
 *
 * This structure is used by <code>LLECOM_WIFI_IMPL_getBSSID</code>
 *
 * Fields defined in this structure correspond to the parameters of this function and
 * <code>result</code> field corresponds to the value returned by it.
 */
typedef struct {
	int8_t* bssid; /*!< [IN] The buffer into which the bssid will be stored (bssid is 48-bit long). */
	int32_t bssidOffset; /*!< [IN] The offset where the bssid will be stored in the buffer. */
	int32_t bssidLength; /*!< [IN] The bssid buffer length. */
	int32_t result; /*!< [OUT] Result of the operation. */
	int32_t error_code; /*!< [OUT] Error code returned in case of error. */
	char* error_message; /*!< [OUT] Error message related to the error code. */
} ECOM_WIFI_get_bssid_t;

/**
 * @brief Data structure for get RSSI operations.
 *
 * This structure is used by <code>LLECOM_WIFI_IMPL_getRSSI</code>
 *
 * Fields defined in this structure correspond to the parameters of this function and
 * <code>result</code> field corresponds to the value returned by it.
 */
typedef struct {
	float* rssi; /*!< [IN] The buffer into which the rssi will be stored (data stored on 1 byte). */
	int32_t rssiOffset; /*!< [IN] The offset where the rssi will be stored in the buffer. */
	int32_t rssiLength; /*!< [IN] The rssi buffer length. */
	int32_t result; /*!< [OUT] Result of the operation. */
	int32_t error_code; /*!< [OUT] Error code returned in case of error. */
	char* error_message; /*!< [OUT] Error message related to the error code. */
} ECOM_WIFI_get_rssi_t;

/**
 * @brief Data structure for get SSID operations.
 *
 * This structure is used by <code>LLECOM_WIFI_IMPL_getSSID</code>
 *
 * Fields defined in this structure correspond to the parameters of this function and
 * <code>result</code> field corresponds to the value returned by it.
 */
typedef struct {
	int8_t* ssid; /*!< [IN] The buffer into which the ssid will be stored. */
	int32_t ssidOffset; /*!< [IN] The offset where the ssid will be stored in the buffer. */
	int32_t ssidLength; /*!< [IN] The ssid buffer length. */
	int32_t result; /*!< [OUT] Result of the operation. */
	int32_t error_code; /*!< [OUT] Error code returned in case of error. */
	char* error_message; /*!< [OUT] Error message related to the error code. */
} ECOM_WIFI_get_ssid_t;

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
 * @brief Data structure for scan AP ssid available at index operations.
 *
 * This structure is used by <code>LLECOM_WIFI_IMPL_scanAPssidAt</code>
 *
 * Fields defined in this structure correspond to the parameters of this function and
 * <code>result</code> field corresponds to the value returned by it.
 */
typedef struct {
	int8_t* ssid; /*!< [IN] The buffer into which the ssid will be stored. */
	int32_t ssidOffset; /*!< [IN] The offset where the ssid will be stored in the buffer. */
	int32_t ssidLength; /*!< [IN] The ssid buffer length. */
	int32_t index; /*!< [IN] The index of the Access Point currently available. */
	int32_t result; /*!< [OUT] Result of the operation. */
	int32_t error_code; /*!< [OUT] Error code returned in case of error. */
	char* error_message; /*!< [OUT] Error message related to the error code. */
} ECOM_WIFI_scanAP_ssid_t;

/**
 * @brief Data structure for scan AP bssid available at index operations.
 *
 * This structure is used by <code>LLECOM_WIFI_IMPL_scanAPbssidAt</code>
 *
 * Fields defined in this structure correspond to the parameters of this function and
 * <code>result</code> field corresponds to the value returned by it.
 */
typedef struct {
	int8_t* bssid; /*!< [IN] The buffer into which the bssid will be stored. */
	int32_t bssidOffset; /*!< [IN] The offset where the bssid will be stored in the buffer. */
	int32_t bssidLength; /*!< [IN] The bssid buffer length. */
	int32_t index; /*!< [IN] The index of the Access Point currently available. */
	int32_t result; /*!< [OUT] Result of the operation. */
	int32_t error_code; /*!< [OUT] Error code returned in case of error. */
	char* error_message; /*!< [OUT] Error message related to the error code. */
} ECOM_WIFI_scanAP_bssid_t;

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
	float* rssi; /*!< [IN] The buffer into which the rssi will be stored. */
	int32_t rssiOffset; /*!< [IN] The offset where the rssi will be stored in the buffer. */
	int32_t rssiLength; /*!< [IN] The rssi buffer length. */
	int32_t index; /*!< [IN] The index of the Access Point currently available. */
	int32_t result; /*!< [OUT] Result of the operation. */
	int32_t error_code; /*!< [OUT] Error code returned in case of error. */
	char* error_message; /*!< [OUT] Error message related to the error code. */
} ECOM_WIFI_scanAP_rssi_t;

/**
 * @union ECOM_WIFI_worker_param_t
 */
typedef union {
	ECOM_WIFI_get_info_t get_information;
	ECOM_WIFI_enable_join_t enable_or_join;
	ECOM_WIFI_join_security_mode_t join_security_mode;
	ECOM_WIFI_set_name_t set_name;
	ECOM_WIFI_get_bssid_t get_bssid;
	ECOM_WIFI_get_rssi_t get_rssi;
	ECOM_WIFI_get_ssid_t get_ssid;
	ECOM_WIFI_scanAP_count_t scanAP_count;
	ECOM_WIFI_scanAP_ssid_t scanAP_ssid;
	ECOM_WIFI_scanAP_bssid_t scanAP_bssid;
	ECOM_WIFI_scanAP_channel_t scanAP_channel;
	ECOM_WIFI_scanAP_rssi_t scanAP_rssi;
} ECOM_WIFI_worker_param_t;

void LLECOM_WIFI_init(void);

void LLECOM_WIFI_IMPL_enableSoftAP_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_WIFI_IMPL_disableSoftAP_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_WIFI_IMPL_setNameSoftAP_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_WIFI_IMPL_getBSSID_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_WIFI_IMPL_getChannel_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_WIFI_IMPL_getClientState_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_WIFI_IMPL_getRSSI_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_WIFI_IMPL_getSecurityMode_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_WIFI_IMPL_getSSID_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_WIFI_IMPL_getWPSModes_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_WIFI_IMPL_joinWithSecurityMode_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_WIFI_IMPL_leave_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_WIFI_IMPL_scanAPCount_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_WIFI_IMPL_scanAPssidAt_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_WIFI_IMPL_scanAPbssidAt_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_WIFI_IMPL_scanAPchannelAt_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_WIFI_IMPL_scanAPrssiAt_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_WIFI_IMPL_scanAPsecurityModeAt_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_WIFI_IMPL_scanAPwpsModeAt_action(MICROEJ_ASYNC_WORKER_job_t* job);

// TODO Remove these defines once SNI 1.4 is available (see M0074FS-134)
// Function succeeded
#define SNI_OK			(0)
// An error was detected
#define SNI_ERROR 		(-1)
// An illegal argument has been given
#define SNI_ILLEGAL_ARGUMENT (-2)

// TODO Remove this function once SNI 1.4 is available (see M0074FS-134)
/**
 * Gets the body of a Java byte array so it can be used outside of the scope of the current native function.
 * <p>
 * If the given Java array is an immortal array, then <code>out_buffer</code> is set to reference directly the
 * element at the offset <code>java_start</code> in the given Java array and <code>out_length</code> is set to
 * <code>java_length</code>.
 * If the given Java array is not an immortal array, then <code>out_buffer</code> is set to reference
 * <code>buffer</code>, <code>*out_length</code> is set to the smaller value of <code>java_length</code>
 * and <code>buffer_length</code>, and then, if <code>refresh_content</code> is true, <code>*out_length</code>
 * bytes are copied from the content of the Java array into <code>buffer</code>.
 * <p>
 * Since the returned array may be a copy of the Java array, changes made to the returned array will not necessarily
 * be reflected in the original Java array until <code>SNI_releaseArrayElements()</code> is called.
 * <p>
 * The function <code>SNI_releaseArrayElements()</code> doesn't have to be called if changes made to the returned
 * array don't need to be reflected in the original Java array.
 *
 * @param java_array the Java array.
 *
 * @param java_start the start offset in array <code>java_array</code> of the first element to get.
 *
 * @param java_length the maximum number of bytes to get.
 *
 * @param buffer the buffer to use if the given Java array is not immortal.
 *
 * @param buffer_length length in bytes of <code>buffer</code>.
 *
 * @param out_buffer output pointer to the retrieved elements.
 *
 * @param out_length the number of elements retrieved.
 *
 * @param refresh_content specifies whether or not the retrieved array will be initialized with the elements of the given Java array.
 *
 * @return <code>SNI_OK</code> on success. Returns <code>SNI_ILLEGAL_ARGUMENT</code> if <code>java_start</code> is
 * negative, <code>java_length</code> is negative, <code>java_length</code> is greater than
 * <code>SNI_getArrayLength(java_array) - java_start</code>, or one of the given pointers is null.
 */
int32_t SNI_getArrayElements(int8_t* java_array, int32_t java_start, int32_t java_length, int8_t* buffer, uint32_t buffer_length, int8_t** out_buffer, uint32_t* out_length, bool refresh_content);

// TODO Remove this function once SNI 1.4 is available (see M0074FS-134)
/**
 * If necessary, this function copies back all the content of <code>buffer</code> to the Java array.
 * <p>
 * If the given Java array is an immortal array, this function does nothing.
 * If the given Java array is not an immortal array, then this function copies back all the content of
 * <code>buffer</code> to the Java array at the offset <code>java_start</code>.
 *
 * @param java_array the Java array.
 *
 * @param java_start the start offset in array <code>java_array</code> of the first element to release.
 *
 * @param java_length the maximum number of bytes to release.
 *
 * @param buffer the buffer to use if the given Java array is not immortal.
 *
 * @param buffer_length length in bytes of <code>buffer</code>.
 *
 * @return <code>SNI_OK</code> on success. Returns <code>SNI_ILLEGAL_ARGUMENT</code> if <code>java_start</code> is
 * negative, <code>java_length</code> is negative, <code>java_length</code> is greater than
 * <code>SNI_getArrayLength(java_array) - java_start</code>, <code>buffer_length</code> is greater than
 * <code>java_length</code>, or one of the given pointers is null.
 */
int32_t SNI_releaseArrayElements(int8_t* java_array, int32_t java_start, int32_t java_length, int8_t* buffer, uint32_t buffer_length);

#ifdef __cplusplus
	}
#endif

#endif /* ECOM_WIFI_HELPER_H */
