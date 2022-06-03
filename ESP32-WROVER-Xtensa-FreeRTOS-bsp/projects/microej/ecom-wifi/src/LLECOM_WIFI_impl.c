/*
 * C
 *
 * Copyright 2020-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief LLECOM_WIFI implementation with async worker.
 * @author MicroEJ Developer Team
 * @version 2.1.0
 * @date 17 June 2021
 */

/* Includes ------------------------------------------------------------------*/

#include <stdbool.h>
#include <stddef.h>
#include "sni.h"
#include "LLECOM_WIFI_impl.h"
#include "ecom_wifi_configuration.h"
#include "ecom_wifi_helper.h"

#ifdef __cplusplus
	extern "C" {
#endif

/**
 * Sanity check between the expected version of the configuration and the actual version of
 * the configuration.
 * If an error is raised here, it means that a new version of the CCO has been installed and
 * the configuration ecom_wifi_configuration.h must be updated based on the one provided
 * by the new CCO version.
 */
#if ECOM_WIFI_CONFIGURATION_VERSION != 2

	#error "Version of the configuration file ecom_wifi_configuration.h is not compatible with this implementation."

#endif

#ifndef ECOM_WIFI_CUSTOM_WORKER
/* Async worker task declaration ---------------------------------------------*/
MICROEJ_ASYNC_WORKER_worker_declare(ecom_wifi_worker, ECOM_WIFI_WORKER_JOB_COUNT, ECOM_WIFI_worker_param_t, ECOM_WIFI_WAITING_LIST_SIZE);
OSAL_task_stack_declare(ecom_wifi_worker_stack, ECOM_WIFI_WORKER_STACK_SIZE);
#endif

static int32_t LLECOM_WIFI_async_exec_get_info(SNI_callback retry_function, MICROEJ_ASYNC_WORKER_action_t action, SNI_callback on_done);
static int32_t LLECOM_WIFI_async_exec_get_info_on_done(int8_t getResult);
static int32_t LLECOM_WIFI_IMPL_enableSoftAP_on_done(int8_t* ssid, int32_t ssidOffset, int32_t ssidLength, int8_t* passphrase, int32_t passphraseOffset, int32_t passphraseLength, int8_t getResult);
static int32_t LLECOM_WIFI_IMPL_setNameSoftAP_on_done(int8_t* name, int32_t nameOffset, int32_t nameLength, int8_t getResult);
static int32_t LLECOM_WIFI_IMPL_getXSSID(int8_t* xssid, int32_t xssidOffset, int32_t xssidLength, SNI_callback retry_function, MICROEJ_ASYNC_WORKER_action_t action);
static int32_t LLECOM_WIFI_IMPL_getXSSID_on_done(int8_t* xssid, int32_t xssidOffset, int32_t xssidLength, int8_t getResult);
static int32_t LLECOM_WIFI_IMPL_getRSSI_on_done(float* rssi, int32_t rssiOffset, int32_t rssiLength, int8_t getResult);
static int32_t LLECOM_WIFI_joinWithSecurityMode(int8_t* ssid, int32_t ssidOffset, int32_t ssidLength, int8_t* passphrase, int32_t passphraseOffset, int32_t passphraseLength, int32_t securityModeValue, SNI_callback retry_function, SNI_callback on_done);
static int32_t LLECOM_WIFI_IMPL_join_on_done(int8_t* ssid, int32_t ssidOffset, int32_t ssidLength, int8_t* passphrase, int32_t passphraseOffset, int32_t passphraseLength, int8_t getResult);
static int32_t LLECOM_WIFI_IMPL_joinWithSecurityMode_on_done(int8_t* ssid, int32_t ssidOffset, int32_t ssidLength, int8_t* passphrase, int32_t passphraseOffset, int32_t passphraseLength, int32_t securityModeValue, int8_t getResult);
static int32_t LLECOM_WIFI_IMPL_scanAPCount_on_done(int8_t active, int8_t getResult);
static int32_t LLECOM_WIFI_IMPL_scanAPxssidAt(int8_t* xssid, int32_t xssidOffset, int32_t xssidLength, int32_t index, SNI_callback retry_function, MICROEJ_ASYNC_WORKER_action_t action);
static int32_t LLECOM_WIFI_IMPL_scanAPxssidAt_on_done(int8_t* xssid, int32_t xssidOffset, int32_t xssidLength, int32_t index, int8_t getResult);
static int32_t LLECOM_WIFI_async_exec_scanAP(int32_t index, SNI_callback retry_function, MICROEJ_ASYNC_WORKER_action_t action, SNI_callback on_done);
static int32_t LLECOM_WIFI_async_exec_scanAP_on_done(int32_t index, int8_t getResult);
static int32_t LLECOM_WIFI_IMPL_scanAPrssiAt_on_done(float* rssi, int32_t rssiOffset, int32_t rssiLength, int32_t index, int8_t getResult);
static int32_t LLECOM_WIFI_joinWithSecurityMode_on_done(void);

void LLECOM_WIFI_initialize(void) {
#ifndef ECOM_WIFI_CUSTOM_WORKER
	MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_initialize(&ecom_wifi_worker, (uint8_t*)"MicroEJ ECOM-WIFI", ecom_wifi_worker_stack, ECOM_WIFI_WORKER_PRIORITY);
	if (status == MICROEJ_ASYNC_WORKER_INVALID_ARGS) {
		SNI_throwNativeException(status, "Invalid argument for ECOM-WIFI async worker");
	} else if (status == MICROEJ_ASYNC_WORKER_ERROR) {
		SNI_throwNativeException(status, "Error while initializing ECOM-WIFI async worker");
	}
#endif

	llecom_wifi_init();
}

int32_t LLECOM_WIFI_IMPL_disableSoftAP(int8_t getResult) {
	return LLECOM_WIFI_async_exec_get_info((SNI_callback)LLECOM_WIFI_IMPL_disableSoftAP, LLECOM_WIFI_IMPL_disableSoftAP_action, (SNI_callback)LLECOM_WIFI_async_exec_get_info_on_done);
}

int32_t LLECOM_WIFI_IMPL_enableSoftAP(int8_t* ssid, int32_t ssidOffset, int32_t ssidLength, int8_t* passphrase, int32_t passphraseOffset, int32_t passphraseLength, int8_t getResult) {
	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_allocate_job(&ecom_wifi_worker, (SNI_callback)LLECOM_WIFI_IMPL_enableSoftAP);
	if (job == NULL) {
		// No job available, either:
		// - wait for a job to be available and this function to be executed again,
		// - or an exception is pending
		return ECOM_WIFI_ERROR;
	}

	ECOM_WIFI_enable_softap_t* params = (ECOM_WIFI_enable_softap_t*)job->params;

	int32_t result = 0;
	result = SNI_retrieveArrayElements(ssid, ssidOffset, ssidLength, (int8_t*)&params->buffer_ssid,
			sizeof(params->buffer_ssid), (int8_t**)&params->ssid, (uint32_t*)&params->ssidLength, true);
	result |= SNI_retrieveArrayElements(passphrase, passphraseOffset, passphraseLength, (int8_t*)&params->buffer_pass,
			sizeof(params->buffer_pass), (int8_t**)&params->passphrase, (uint32_t*)&params->passphraseLength, true);
	if (result == SNI_OK) {
		MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_async_exec(&ecom_wifi_worker, job, LLECOM_WIFI_IMPL_enableSoftAP_action, (SNI_callback)LLECOM_WIFI_IMPL_enableSoftAP_on_done);
		if (status == MICROEJ_ASYNC_WORKER_OK) {
			// Wait for the action to be done
			return SNI_IGNORED_RETURNED_VALUE;//returned value not used
		} // else an error occurred and MICROEJ_ASYNC_WORKER_async_exec has thrown a SNI exception
	}

	// Else an error occurred and MICROEJ_ASYNC_WORKER_async_exec has thrown a SNI exception
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);
	return ECOM_WIFI_ERROR;
}

int32_t LLECOM_WIFI_IMPL_setNameSoftAP(int8_t* name, int32_t nameOffset, int32_t nameLength, int8_t getResult) {
	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_allocate_job(&ecom_wifi_worker, (SNI_callback)LLECOM_WIFI_IMPL_setNameSoftAP);
	if (job == NULL) {
		// No job available, either:
		// - wait for a job to be available and this function to be executed again,
		// - or an exception is pending
		return ECOM_WIFI_ERROR;
	}

	ECOM_WIFI_set_name_t* params = (ECOM_WIFI_set_name_t*)job->params;

	int32_t result = SNI_retrieveArrayElements(name, nameOffset, nameLength, (int8_t*)&params->buffer_name,
			sizeof(params->buffer_name), (int8_t**)&params->name, (uint32_t*)&params->nameLength, true);
	if (result == SNI_OK) {
		MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_async_exec(&ecom_wifi_worker, job, LLECOM_WIFI_IMPL_setNameSoftAP_action, (SNI_callback)LLECOM_WIFI_IMPL_setNameSoftAP_on_done);
		if (status == MICROEJ_ASYNC_WORKER_OK) {
			// Wait for the action to be done
			return SNI_IGNORED_RETURNED_VALUE;//returned value not used
		} // else an error occurred and MICROEJ_ASYNC_WORKER_async_exec has thrown a SNI exception
	}

	// Else an error occurred and MICROEJ_ASYNC_WORKER_async_exec has thrown a SNI exception
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);
	return ECOM_WIFI_ERROR;
}

int32_t LLECOM_WIFI_IMPL_getBSSID(int8_t* bssid, int32_t bssidOffset, int32_t bssidLength, int8_t getResult) {
	return LLECOM_WIFI_IMPL_getXSSID(bssid, bssidOffset, bssidLength, (SNI_callback)LLECOM_WIFI_IMPL_getBSSID, LLECOM_WIFI_IMPL_getBSSID_action);
}

int32_t LLECOM_WIFI_IMPL_getChannel(int8_t getResult) {
	return LLECOM_WIFI_async_exec_get_info((SNI_callback)LLECOM_WIFI_IMPL_getChannel, LLECOM_WIFI_IMPL_getChannel_action, (SNI_callback)LLECOM_WIFI_async_exec_get_info_on_done);
}

int32_t LLECOM_WIFI_IMPL_getClientState(int8_t getResult) {
	return LLECOM_WIFI_async_exec_get_info((SNI_callback)LLECOM_WIFI_IMPL_getClientState, LLECOM_WIFI_IMPL_getClientState_action, (SNI_callback)LLECOM_WIFI_async_exec_get_info_on_done);
}

int32_t LLECOM_WIFI_IMPL_getAccessPointState(int8_t getResult) {
	return LLECOM_WIFI_async_exec_get_info((SNI_callback)LLECOM_WIFI_IMPL_getAccessPointState, LLECOM_WIFI_IMPL_getAccessPointState_action, (SNI_callback)LLECOM_WIFI_async_exec_get_info_on_done);
}

int32_t LLECOM_WIFI_IMPL_getRSSI(float* rssi, int32_t rssiOffset, int32_t rssiLength, int8_t getResult) {
	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_allocate_job(&ecom_wifi_worker, (SNI_callback)LLECOM_WIFI_IMPL_getRSSI);
	if (job == NULL) {
		// No job available, either:
		// - wait for a job to be available and this function to be executed again,
		// - or an exception is pending
		return ECOM_WIFI_ERROR;
	}

	ECOM_WIFI_get_rssi_t* params = (ECOM_WIFI_get_rssi_t*)job->params;

	int32_t result = SNI_retrieveArrayElements((int8_t*)rssi, rssiOffset, rssiLength, (int8_t*)&params->buffer_rssi,
						sizeof(params->buffer_rssi), (int8_t**)&params->rssi, (uint32_t*)&params->rssiLength, false);
	if (result == SNI_OK) {
		MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_async_exec(&ecom_wifi_worker, job, LLECOM_WIFI_IMPL_getRSSI_action, (SNI_callback)LLECOM_WIFI_IMPL_getRSSI_on_done);
		if (status == MICROEJ_ASYNC_WORKER_OK) {
			// Wait for the action to be done
			return SNI_IGNORED_RETURNED_VALUE;//returned value not used
		} // else an error occurred and MICROEJ_ASYNC_WORKER_async_exec has thrown a SNI exception
	}

	// Else an error occurred and MICROEJ_ASYNC_WORKER_async_exec has thrown a SNI exception
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);
	return ECOM_WIFI_ERROR;
}

int32_t LLECOM_WIFI_IMPL_getSecurityMode(int8_t getResult) {
	return LLECOM_WIFI_async_exec_get_info((SNI_callback)LLECOM_WIFI_IMPL_getSecurityMode, LLECOM_WIFI_IMPL_getSecurityMode_action, (SNI_callback)LLECOM_WIFI_async_exec_get_info_on_done);
}

int32_t LLECOM_WIFI_IMPL_getSSID(int8_t* ssid, int32_t ssidOffset, int32_t ssidLength, int8_t getResult) {
	return LLECOM_WIFI_IMPL_getXSSID(ssid, ssidOffset, ssidLength, (SNI_callback)LLECOM_WIFI_IMPL_getSSID, LLECOM_WIFI_IMPL_getSSID_action);
}

int32_t LLECOM_WIFI_IMPL_getWPSModes(int8_t getResult) {
	return LLECOM_WIFI_async_exec_get_info((SNI_callback)LLECOM_WIFI_IMPL_getWPSModes, LLECOM_WIFI_IMPL_getWPSModes_action, (SNI_callback)LLECOM_WIFI_async_exec_get_info_on_done);
}

int32_t LLECOM_WIFI_IMPL_join(int8_t* ssid, int32_t ssidOffset, int32_t ssidLength, int8_t* passphrase, int32_t passphraseOffset, int32_t passphraseLength, int8_t getResult) {
	return LLECOM_WIFI_joinWithSecurityMode(ssid, ssidOffset, ssidLength, passphrase, passphraseOffset, passphraseLength, SECURITY_MODE_WPA2, (SNI_callback)LLECOM_WIFI_IMPL_join, (SNI_callback)LLECOM_WIFI_IMPL_join_on_done);
}

int32_t LLECOM_WIFI_IMPL_joinWithSecurityMode(int8_t* ssid, int32_t ssidOffset, int32_t ssidLength, int8_t* passphrase, int32_t passphraseOffset, int32_t passphraseLength, int32_t securityModeValue, int8_t getResult) {
	return LLECOM_WIFI_joinWithSecurityMode(ssid, ssidOffset, ssidLength, passphrase, passphraseOffset, passphraseLength, securityModeValue, (SNI_callback)LLECOM_WIFI_IMPL_joinWithSecurityMode, (SNI_callback)LLECOM_WIFI_IMPL_joinWithSecurityMode_on_done);
}

int32_t LLECOM_WIFI_IMPL_leave(int8_t getResult) {
	return LLECOM_WIFI_async_exec_get_info((SNI_callback)LLECOM_WIFI_IMPL_leave, LLECOM_WIFI_IMPL_leave_action, (SNI_callback)LLECOM_WIFI_async_exec_get_info_on_done);
}

int32_t LLECOM_WIFI_IMPL_scanAPCount(int8_t active, int8_t getResult) {
	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_allocate_job(&ecom_wifi_worker, (SNI_callback)LLECOM_WIFI_IMPL_scanAPCount);
	if (job == NULL) {
		// No job available, either:
		// - wait for a job to be available and this function to be executed again,
		// - or an exception is pending
		return ECOM_WIFI_ERROR;
	}

	ECOM_WIFI_scanAP_count_t* params = (ECOM_WIFI_scanAP_count_t*)job->params;

	params->active = active;

	MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_async_exec(&ecom_wifi_worker, job, LLECOM_WIFI_IMPL_scanAPCount_action, (SNI_callback)LLECOM_WIFI_IMPL_scanAPCount_on_done);
	if (status == MICROEJ_ASYNC_WORKER_OK) {
		// Wait for the action to be done
		return SNI_IGNORED_RETURNED_VALUE;//returned value not used
	}

	// Else an error occurred and MICROEJ_ASYNC_WORKER_async_exec has thrown a SNI exception
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);
	return ECOM_WIFI_ERROR;
}

int32_t LLECOM_WIFI_IMPL_scanAPssidAt(int8_t* ssid, int32_t ssidOffset, int32_t ssidLength, int32_t index, int8_t getResult) {
	return LLECOM_WIFI_IMPL_scanAPxssidAt(ssid, ssidOffset, ssidLength, index, (SNI_callback)LLECOM_WIFI_IMPL_scanAPssidAt, LLECOM_WIFI_IMPL_scanAPssidAt_action);
}

int32_t LLECOM_WIFI_IMPL_scanAPbssidAt(int8_t* bssid, int32_t bssidOffset, int32_t bssidLength, int32_t index, int8_t getResult) {
	return LLECOM_WIFI_IMPL_scanAPxssidAt(bssid, bssidOffset, bssidLength, index, (SNI_callback)LLECOM_WIFI_IMPL_scanAPbssidAt, LLECOM_WIFI_IMPL_scanAPbssidAt_action);
}

int32_t LLECOM_WIFI_IMPL_scanAPchannelAt(int32_t index, int8_t getResult) {
	return LLECOM_WIFI_async_exec_scanAP(index, (SNI_callback)LLECOM_WIFI_IMPL_scanAPchannelAt, LLECOM_WIFI_IMPL_scanAPchannelAt_action, (SNI_callback)LLECOM_WIFI_async_exec_scanAP_on_done);
}

int32_t LLECOM_WIFI_IMPL_scanAPrssiAt(float* rssi, int32_t rssiOffset, int32_t rssiLength, int32_t index, int8_t getResult) {
	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_allocate_job(&ecom_wifi_worker, (SNI_callback)LLECOM_WIFI_IMPL_scanAPrssiAt);
	if (job == NULL) {
		// No job available, either:
		// - wait for a job to be available and this function to be executed again,
		// - or an exception is pending
		return ECOM_WIFI_ERROR;
	}

	ECOM_WIFI_scanAP_rssi_t* params = (ECOM_WIFI_scanAP_rssi_t*)job->params;

	params->index = index;

	int32_t result = SNI_retrieveArrayElements((int8_t*)rssi, rssiOffset, rssiLength, (int8_t*)&params->buffer_rssi,
						sizeof(params->buffer_rssi), (int8_t**)&params->rssi, (uint32_t*)&params->rssiLength, false);
	if (result == SNI_OK) {
		MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_async_exec(&ecom_wifi_worker, job, LLECOM_WIFI_IMPL_scanAPrssiAt_action, (SNI_callback)LLECOM_WIFI_IMPL_scanAPrssiAt_on_done);
		if (status == MICROEJ_ASYNC_WORKER_OK) {
			// Wait for the action to be done
			return SNI_IGNORED_RETURNED_VALUE;//returned value not used
		} // else an error occurred and MICROEJ_ASYNC_WORKER_async_exec has thrown a SNI exception
	}

	// Else an error occurred and MICROEJ_ASYNC_WORKER_async_exec has thrown a SNI exception
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);
	return ECOM_WIFI_ERROR;
}

int32_t LLECOM_WIFI_IMPL_scanAPsecurityModeAt(int32_t index, int8_t getResult) {
	return LLECOM_WIFI_async_exec_scanAP(index, (SNI_callback)LLECOM_WIFI_IMPL_scanAPsecurityModeAt, LLECOM_WIFI_IMPL_scanAPsecurityModeAt_action, (SNI_callback)LLECOM_WIFI_async_exec_scanAP_on_done);
}

int32_t LLECOM_WIFI_IMPL_scanAPwpsModeAt(int32_t index, int8_t getResult) {
	return LLECOM_WIFI_async_exec_scanAP(index, (SNI_callback)LLECOM_WIFI_IMPL_scanAPwpsModeAt, LLECOM_WIFI_IMPL_scanAPwpsModeAt_action, (SNI_callback)LLECOM_WIFI_async_exec_scanAP_on_done);
}

int32_t LLECOM_WIFI_IMPL_getCapability(int8_t getResult) {
	return WIFI_CAPABILITY_BOTH_EXCLUSIVE; // FIXME link to the hardware part
}

/**
 * @brief Prepare and send an execution job to async_worker, called either from
 * <code>LLECOM_WIFI_IMPL_disableSoftAP</code>, <code>LLECOM_WIFI_IMPL_getChannel</code>,
 * <code>LLECOM_WIFI_IMPL_getClientState</code>, <code>LLECOM_WIFI_IMPL_getSecurityMode</code>,
 * <code>LLECOM_WIFI_IMPL_getWPSModes</code> and <code>LLECOM_WIFI_IMPL_leave</code>.
 *
 * @param[in] retry_function if the current Java thread has been suspended, this function is called when it is resumed.
 * @param[in] action the function to execute asynchronously.
 * @param[in] on_done the <code>SNI_callback</code> called when the job is done.
 *
 * @return <code>SNI_IGNORED_RETURNED_VALUE</code> on success, else a negative error code.
 */
static int32_t LLECOM_WIFI_async_exec_get_info(SNI_callback retry_function, MICROEJ_ASYNC_WORKER_action_t action, SNI_callback on_done) {
	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_allocate_job(&ecom_wifi_worker, retry_function);
	if (job == NULL) {
		// No job available, either:
		// - wait for a job to be available and this function to be executed again,
		// - or an exception is pending
		return ECOM_WIFI_ERROR;
	}

	MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_async_exec(&ecom_wifi_worker, job, action, on_done);
	if (status == MICROEJ_ASYNC_WORKER_OK) {
		// Wait for the action to be done
		return SNI_IGNORED_RETURNED_VALUE;//returned value not used
	}

	// Else an error occurred and MICROEJ_ASYNC_WORKER_async_exec has thrown a SNI exception
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);
	return ECOM_WIFI_ERROR;
}

/**
 * @brief The <code>SNI_callback</code> called when the async_worker job requested by
 * <code>LLECOM_WIFI_IMPL_disableSoftAP</code>, <code>LLECOM_WIFI_IMPL_getChannel</code>,
 * <code>LLECOM_WIFI_IMPL_getClientState</code>, <code>LLECOM_WIFI_IMPL_getSecurityMode</code>,
 * <code>LLECOM_WIFI_IMPL_getWPSModes</code> or <code>LLECOM_WIFI_IMPL_leave</code> is done.
 *
 * @param[in] getResult unused parameter
 *
 * @return <code>LLECOM_WIFI_IMPL_disableSoftAP_action</code>, <code>LLECOM_WIFI_IMPL_getChannel_action</code>,
 * <code>LLECOM_WIFI_IMPL_getClientState_action</code>, <code>LLECOM_WIFI_IMPL_getSecurityMode_action</code>,
 * <code>LLECOM_WIFI_IMPL_getWPSModes_action</code> or <code>LLECOM_WIFI_IMPL_leave_action</code> function return code.
 */
static int32_t LLECOM_WIFI_async_exec_get_info_on_done(int8_t getResult) {
	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_get_job_done();
	ECOM_WIFI_get_info_t* params = (ECOM_WIFI_get_info_t*)job->params;

	int32_t result = params->result;

	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);

	return result;
}

/**
 * @brief The <code>SNI_callback</code> called when the async_worker job requested by
 * <code>LLECOM_WIFI_IMPL_enableSoftAP</code> is done.
 *
 * @param[in] ssid the buffer into which the ssid will be stored
 * @param[in] ssidOffset the offset where the ssid will be stored in the buffer
 * @param[in] ssidLength the ssid buffer length
 * @param[in] passphrase passphrase
 * @param[in] passphraseOffset the offset where the passphrase will be store in the buffer
 * @param[in] passphraseLength the passphrase buffer length
 * @param[in] getResult unused parameter
 *
 * @return <code>LLECOM_WIFI_IMPL_enableSoftAP_action</code> function return code.
 */
static int32_t LLECOM_WIFI_IMPL_enableSoftAP_on_done(int8_t* ssid, int32_t ssidOffset, int32_t ssidLength, int8_t* passphrase, int32_t passphraseOffset, int32_t passphraseLength, int8_t getResult) {
	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_get_job_done();
	ECOM_WIFI_enable_softap_t* params = (ECOM_WIFI_enable_softap_t*)job->params;

	int32_t result = params->result;

	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);

	return result;
}

/**
 * @brief The <code>SNI_callback</code> called when the async_worker job requested by
 * <code>LLECOM_WIFI_IMPL_setNameSoftAP</code> is done.
 *
 * @param[in] name the buffer into which the name will be stored
 * @param[in] nameOffset the offset where the name will be stored in the buffer
 * @param[in] nameLength the name buffer length
 * @param[in] getResult unused parameter
 *
 * @return <code>LLECOM_WIFI_IMPL_setNameSoftAP_action</code> function return code.
 */
static int32_t LLECOM_WIFI_IMPL_setNameSoftAP_on_done(int8_t* name, int32_t nameOffset, int32_t nameLength, int8_t getResult) {
	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_get_job_done();
	ECOM_WIFI_set_name_t* params = (ECOM_WIFI_set_name_t*)job->params;

	int32_t result = params->result;

	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);

	return result;
}

/**
 * @brief Prepare and send an execution job to async_worker, called either from
 * <code>LLECOM_WIFI_IMPL_getBSSID</code> and <code>LLECOM_WIFI_IMPL_getSSID</code>.
 *
 * @param[in] xssid the buffer into which the bssid/ssid will be stored
 * @param[in] xssidOffset the offset where the bssid/ssid will be stored in the buffer
 * @param[in] xssidLength the bssid/ssid buffer length
 * @param[in] retry_function if the current Java thread has been suspended, this function is called when it is resumed.
 * @param[in] action the function to execute asynchronously.
 *
 * @return <code>SNI_IGNORED_RETURNED_VALUE</code> on success, else a negative error code.
 */
static int32_t LLECOM_WIFI_IMPL_getXSSID(int8_t* xssid, int32_t xssidOffset, int32_t xssidLength, SNI_callback retry_function, MICROEJ_ASYNC_WORKER_action_t action) {
	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_allocate_job(&ecom_wifi_worker, retry_function);
	if (job == NULL) {
		// No job available, either:
		// - wait for a job to be available and this function to be executed again,
		// - or an exception is pending
		return ECOM_WIFI_ERROR;
	}

	ECOM_WIFI_get_xssid_t* params = (ECOM_WIFI_get_xssid_t*)job->params;

	int32_t result = SNI_retrieveArrayElements(xssid, xssidOffset, xssidLength, (int8_t*)&params->buffer_xssid,
						sizeof(params->buffer_xssid), (int8_t**)&params->xssid, (uint32_t*)&params->xssidLength, false);
	if (result == SNI_OK) {
		MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_async_exec(&ecom_wifi_worker, job, action, (SNI_callback)LLECOM_WIFI_IMPL_getXSSID_on_done);
		if (status == MICROEJ_ASYNC_WORKER_OK) {
			// Wait for the action to be done
			return SNI_IGNORED_RETURNED_VALUE;//returned value not used
		} // else an error occurred and MICROEJ_ASYNC_WORKER_async_exec has thrown a SNI exception
	}

	// Else an error occurred and MICROEJ_ASYNC_WORKER_async_exec has thrown a SNI exception
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);
	return ECOM_WIFI_ERROR;
}

/**
 * @brief The <code>SNI_callback</code> called when the async_worker job requested by
 * <code>LLECOM_WIFI_IMPL_getBSSID</code> and <code>LLECOM_WIFI_IMPL_getSSID</code> is done.
 *
 * @param[out] xssid the buffer into which the bssid/ssid will be stored
 * @param[in] xssidOffset the offset where the bssid/ssid will be stored in the buffer
 * @param[in] xssidLength the bssid/ssid buffer length
 * @param[in] getResult unused parameter
 *
 * @return <code>LLECOM_WIFI_IMPL_getBSSID_action</code> or
 * <code>LLECOM_WIFI_IMPL_getSSID_action</code> function return code.
 */
static int32_t LLECOM_WIFI_IMPL_getXSSID_on_done(int8_t* xssid, int32_t xssidOffset, int32_t xssidLength, int8_t getResult) {
	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_get_job_done();
	ECOM_WIFI_get_xssid_t* params = (ECOM_WIFI_get_xssid_t*)job->params;

	int32_t result = params->result;
	if (result >= 0) {
		int32_t release_result = SNI_flushArrayElements(xssid, xssidOffset, xssidLength, (int8_t*)params->xssid, params->xssidLength);
		if (release_result != SNI_OK) {
			result = ECOM_WIFI_ERROR;
		}
	}
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);

	return result;
}

/**
 * @brief The <code>SNI_callback</code> called when the async_worker job requested by
 * <code>LLECOM_WIFI_IMPL_getRSSI</code> is done.
 *
 * @param[out] rssi the buffer into which the rssi will be stored (data stored on 1 byte)
 * @param[in] rssiOffset the offset where the rssi will be stored in the buffer
 * @param[in] rssiLength the rssi buffer length
 * @param[in] getResult unused parameter
 *
 * @return <code>LLECOM_WIFI_IMPL_getRSSI_action</code> function return code.
 */
static int32_t LLECOM_WIFI_IMPL_getRSSI_on_done(float* rssi, int32_t rssiOffset, int32_t rssiLength, int8_t getResult) {
	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_get_job_done();
	ECOM_WIFI_get_rssi_t* params = (ECOM_WIFI_get_rssi_t*)job->params;

	int32_t result = params->result;
	if (result >= 0) {
		int32_t release_result = SNI_flushArrayElements((int8_t*)rssi, rssiOffset, rssiLength, (int8_t*)params->rssi, params->rssiLength);
		if (release_result != SNI_OK) {
			result = ECOM_WIFI_ERROR;
		}
	}
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);

	return result;
}

/**
 * @brief Prepare and send an execution job to async_worker, called either from
 * <code>LLECOM_WIFI_IMPL_join</code> and <code>LLECOM_WIFI_IMPL_joinWithSecurityMode</code>.
 *
 * @param[in] ssid the buffer into which the ssid will be stored
 * @param[in] ssidOffset the offset where the ssid will be stored in the buffer
 * @param[in] ssidLength the ssid buffer length
 * @param[in] passphrase the buffer into which the passphrase will be stored
 * @param[in] passphraseOffset the offset where the passphrase will be store in the buffer
 * @param[in] passphraseLength the passphrase buffer length
 * @param[in] securityModeValue the ssid security mode value
 * @param[in] retry_function if the current Java thread has been suspended, this function is called when it is resumed.
 * @param[in] on_done the <code>SNI_callback</code> called when the job is done.
 *
 * @return <code>SNI_IGNORED_RETURNED_VALUE</code> on success, else a negative error code.
 */
static int32_t LLECOM_WIFI_joinWithSecurityMode(int8_t* ssid, int32_t ssidOffset, int32_t ssidLength, int8_t* passphrase, int32_t passphraseOffset, int32_t passphraseLength, int32_t securityModeValue, SNI_callback retry_function, SNI_callback on_done) {
	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_allocate_job(&ecom_wifi_worker, retry_function);
	if (job == NULL) {
		// No job available, either:
		// - wait for a job to be available and this function to be executed again,
		// - or an exception is pending
		return ECOM_WIFI_ERROR;
	}

	ECOM_WIFI_join_security_mode_t* params = (ECOM_WIFI_join_security_mode_t*)job->params;

	params->securityModeValue = securityModeValue;

	int32_t result = 0;
	result = SNI_retrieveArrayElements(ssid, ssidOffset, ssidLength, (int8_t*)&params->buffer_ssid,
			sizeof(params->buffer_ssid), (int8_t**)&params->ssid, (uint32_t*)&params->ssidLength, true);
	result |= SNI_retrieveArrayElements(passphrase, passphraseOffset, passphraseLength, (int8_t*)&params->buffer_pass,
			sizeof(params->buffer_pass), (int8_t**)&params->passphrase, (uint32_t*)&params->passphraseLength, true);
	if (result == SNI_OK) {
		MICROEJ_ASYNC_WORKER_status_t status;
		status = MICROEJ_ASYNC_WORKER_async_exec(&ecom_wifi_worker, job, LLECOM_WIFI_IMPL_joinWithSecurityMode_action, on_done);
		if (status == MICROEJ_ASYNC_WORKER_OK) {
			// Wait for the action to be done
			return SNI_IGNORED_RETURNED_VALUE;//returned value not used
		} // else an error occurred and MICROEJ_ASYNC_WORKER_async_exec has thrown a SNI exception
	}

	// Else an error occurred and MICROEJ_ASYNC_WORKER_async_exec has thrown a SNI exception
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);
	return ECOM_WIFI_ERROR;
}

/**
 * @brief The <code>SNI_callback</code> called when the async_worker job requested by
 * <code>LLECOM_WIFI_IMPL_join</code> is done.
 *
 * @param[in] ssid the buffer into which the ssid will be stored
 * @param[in] ssidOffset the offset where the ssid will be stored in the buffer
 * @param[in] ssidLength the ssid buffer length
 * @param[in] passphrase the buffer into which the passphrase will be stored
 * @param[in] passphraseOffset the offset where the passphrase will be store in the buffer
 * @param[in] passphraseLength the passphrase buffer length
 * @param[in] getResult unused parameter
 *
 * @return <code>LLECOM_WIFI_IMPL_joinWithSecurityMode_action</code> function return code.
 */
static int32_t LLECOM_WIFI_IMPL_join_on_done(int8_t* ssid, int32_t ssidOffset, int32_t ssidLength, int8_t* passphrase, int32_t passphraseOffset, int32_t passphraseLength, int8_t getResult) {
	return LLECOM_WIFI_joinWithSecurityMode_on_done();
}

/**
 * @brief The <code>SNI_callback</code> called when the async_worker job requested by
 * <code>LLECOM_WIFI_IMPL_joinWithSecurityMode</code> is done.
 *
 * @param[in] ssid the buffer into which the ssid will be stored
 * @param[in] ssidOffset the offset where the ssid will be stored in the buffer
 * @param[in] ssidLength the ssid buffer length
 * @param[in] passphrase the buffer into which the passphrase will be stored
 * @param[in] passphraseOffset the offset where the passphrase will be store in the buffer
 * @param[in] passphraseLength the passphrase buffer length
 * @param[in] securityModeValue the ssid security mode value
 * @param[in] getResult unused parameter
 *
 * @return <code>LLECOM_WIFI_IMPL_joinWithSecurityMode_action</code> function return code.
 */
static int32_t LLECOM_WIFI_IMPL_joinWithSecurityMode_on_done(int8_t* ssid, int32_t ssidOffset, int32_t ssidLength, int8_t* passphrase, int32_t passphraseOffset, int32_t passphraseLength, int32_t securityModeValue, int8_t getResult) {
	return LLECOM_WIFI_joinWithSecurityMode_on_done();
}

/**
 * @brief The <code>SNI_callback</code> called when the async_worker job requested by
 * <code>LLECOM_WIFI_IMPL_scanAPCount</code> is done.
 *
 * @param[in] active true for an active scan, false for a passive scan
 * @param[in] getResult unused parameter
 *
 * @return <code>LLECOM_WIFI_IMPL_scanAPCount_action</code> function return code.
 */
static int32_t LLECOM_WIFI_IMPL_scanAPCount_on_done(int8_t active, int8_t getResult) {
	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_get_job_done();
	ECOM_WIFI_scanAP_count_t* params = (ECOM_WIFI_scanAP_count_t*)job->params;

	int32_t result = params->result;

	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);

	return result;
}

/**
 * @brief Prepare and send an execution job to async_worker, called either from
 * <code>LLECOM_WIFI_IMPL_scanAPbssidAt</code> and <code>LLECOM_WIFI_IMPL_scanAPssidAt</code>.
 *
 * @param[in] xssid the buffer into which the bssid/ssid will be stored
 * @param[in] xssidOffset the offset where the bssid/ssid will be stored in the buffer
 * @param[in] xssidLength the bssid/ssid buffer length
 * @param[in] index index of the Access Point currently available
 * @param[in] retry_function if the current Java thread has been suspended, this function is called when it is resumed.
 * @param[in] action the function to execute asynchronously.
 *
 * @return <code>SNI_IGNORED_RETURNED_VALUE</code> on success, else a negative error code.
 */
static int32_t LLECOM_WIFI_IMPL_scanAPxssidAt(int8_t* xssid, int32_t xssidOffset, int32_t xssidLength, int32_t index, SNI_callback retry_function, MICROEJ_ASYNC_WORKER_action_t action) {
	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_allocate_job(&ecom_wifi_worker, retry_function);
	if (job == NULL) {
		// No job available, either:
		// - wait for a job to be available and this function to be executed again,
		// - or an exception is pending
		return ECOM_WIFI_ERROR;
	}

	ECOM_WIFI_scanAP_xssid_t* params = (ECOM_WIFI_scanAP_xssid_t*)job->params;

	params->index = index;

	int32_t result = SNI_retrieveArrayElements(xssid, xssidOffset, xssidLength, (int8_t*)&params->buffer_xssid,
						sizeof(params->buffer_xssid), (int8_t**)&params->xssid, (uint32_t*)&params->xssidLength, false);
	if (result == SNI_OK) {
		MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_async_exec(&ecom_wifi_worker, job, action, (SNI_callback)LLECOM_WIFI_IMPL_scanAPxssidAt_on_done);
		if (status == MICROEJ_ASYNC_WORKER_OK) {
			// Wait for the action to be done
			return SNI_IGNORED_RETURNED_VALUE;//returned value not used
		} // else an error occurred and MICROEJ_ASYNC_WORKER_async_exec has thrown a SNI exception
	}

	// Else an error occurred and MICROEJ_ASYNC_WORKER_async_exec has thrown a SNI exception
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);
	return ECOM_WIFI_ERROR;
}

/**
 * @brief The <code>SNI_callback</code> called when the async_worker job requested by
 * <code>LLECOM_WIFI_IMPL_scanAPssidAt</code> and <code>LLECOM_WIFI_IMPL_scanAPbssidAt</code> is done.
 *
 * @param[out] xssid the buffer into which the bssid/ssid will be stored
 * @param[in] xssidOffset the offset where the bssid/ssid will be stored in the buffer
 * @param[in] xssidLength the bssid/ssid buffer length
 * @param[in] index index of the Access Point currently available
 * @param[in] getResult unused parameter
 *
 * @return <code>LLECOM_WIFI_IMPL_scanAPssidAt_action</code> or
 * <code>LLECOM_WIFI_IMPL_scanAPbssidAt_action</code> function return code.
 */
static int32_t LLECOM_WIFI_IMPL_scanAPxssidAt_on_done(int8_t* xssid, int32_t xssidOffset, int32_t xssidLength, int32_t index, int8_t getResult) {
	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_get_job_done();
	ECOM_WIFI_scanAP_xssid_t* params = (ECOM_WIFI_scanAP_xssid_t*)job->params;

	int32_t result = params->result;
	if (result >= 0) {
		int32_t release_result = SNI_flushArrayElements(xssid, xssidOffset, xssidLength, (int8_t*)params->xssid, params->xssidLength);
		if (release_result != SNI_OK) {
			result = ECOM_WIFI_ERROR;
		}
	}
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);

	return result;
}

/**
 * @brief Prepare and send an execution job to async_worker, called either from
 * <code>LLECOM_WIFI_IMPL_scanAPchannelAt</code>, <code>LLECOM_WIFI_IMPL_scanAPsecurityModeAt</code>
 * and <code>LLECOM_WIFI_IMPL_scanAPwpsModeAt</code>.
 *
 * @param[in] index index of the Access Point currently available
 * @param[in] retry_function if the current Java thread has been suspended, this function is called when it is resumed.
 * @param[in] action the function to execute asynchronously.
 * @param[in] on_done the <code>SNI_callback</code> called when the job is done.
 *
 * @return <code>SNI_IGNORED_RETURNED_VALUE</code> on success, else a negative error code.
 */
static int32_t LLECOM_WIFI_async_exec_scanAP(int32_t index, SNI_callback retry_function, MICROEJ_ASYNC_WORKER_action_t action, SNI_callback on_done) {
	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_allocate_job(&ecom_wifi_worker, retry_function);
	if (job == NULL) {
		// No job available, either:
		// - wait for a job to be available and this function to be executed again,
		// - or an exception is pending
		return ECOM_WIFI_ERROR;
	}

	ECOM_WIFI_scanAP_channel_t* params = (ECOM_WIFI_scanAP_channel_t*)job->params;

	params->index = index;

	MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_async_exec(&ecom_wifi_worker, job, action, on_done);
	if (status == MICROEJ_ASYNC_WORKER_OK) {
		// Wait for the action to be done
		return SNI_IGNORED_RETURNED_VALUE;//returned value not used
	}

	// Else an error occurred and MICROEJ_ASYNC_WORKER_async_exec has thrown a SNI exception
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);
	return ECOM_WIFI_ERROR;
}

/**
 * @brief The <code>SNI_callback</code> called when the async_worker job requested by
 * <code>LLECOM_WIFI_IMPL_scanAPchannelAt</code>, <code>LLECOM_WIFI_IMPL_scanAPsecurityModeAt</code>
 * and <code>LLECOM_WIFI_IMPL_scanAPwpsModeAt</code> is done.
 *
 * @param[in] index index of the Access Point currently available
 * @param[in] getResult unused parameter
 *
 * @return <code>LLECOM_WIFI_IMPL_scanAPchannelAt_action</code>,
 * <code>LLECOM_WIFI_IMPL_scanAPsecurityModeAt_action</code> or <code>LLECOM_WIFI_IMPL_scanAPwpsModeAt_action</code>
 * function return code.
 */
static int32_t LLECOM_WIFI_async_exec_scanAP_on_done(int32_t index, int8_t getResult) {
	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_get_job_done();
	ECOM_WIFI_scanAP_channel_t* params = (ECOM_WIFI_scanAP_channel_t*)job->params;

	int32_t result = params->result;

	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);

	return result;
}

/**
 * @brief The <code>SNI_callback</code> called when the async_worker job requested by
 * <code>LLECOM_WIFI_IMPL_scanAPrssiAt</code> is done.
 *
 * @param[out] rssi the buffer into which the rssi will be stored
 * @param[in] rssiOffset the offset where the rssi will be stored in the buffer
 * @param[in] rssiLength the rssi buffer length
 * @param[in] index index of the Access Point currently available
 * @param[in] getResult unused parameter
 *
 * @return <code>LLECOM_WIFI_IMPL_scanAPrssiAt_action</code> function return code.
 */
static int32_t LLECOM_WIFI_IMPL_scanAPrssiAt_on_done(float* rssi, int32_t rssiOffset, int32_t rssiLength, int32_t index, int8_t getResult) {
	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_get_job_done();
	ECOM_WIFI_scanAP_rssi_t* params = (ECOM_WIFI_scanAP_rssi_t*)job->params;

	int32_t result = params->result;
	if (result >= 0) {
		int32_t release_result = SNI_flushArrayElements((int8_t *)rssi, rssiOffset, rssiLength, (int8_t*)params->rssi, params->rssiLength);
		if (release_result != SNI_OK) {
			result = ECOM_WIFI_ERROR;
		}
	}
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);

	return result;
}

/**
 * @brief Unified handling for all <code>SNI_callback</code> of LLECOM_WIFI_IMPL_join*_on_done functions.
 *
 * @return <code>LLECOM_WIFI_IMPL_join*_on_done</code> function return code.
 */
static int32_t LLECOM_WIFI_joinWithSecurityMode_on_done(void) {
	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_get_job_done();
	ECOM_WIFI_join_security_mode_t* params = (ECOM_WIFI_join_security_mode_t*)job->params;

	int32_t result = params->result;

	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);

	return result;
}

#ifdef __cplusplus
	}
#endif
