/*
 * C
 *
 * Copyright 2018-2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */

/* Includes --------------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "LLECOM_WIFI_impl.h"
#include "LLNET_ERRORS.h"
#include "sni.h"
#include "microej_async_worker.h"
#include "ecom_wifi_helper.h"
#include "ecom_wifi_configuration.h"

/**
 * Sanity check between the expected version of the configuration and the actual version of
 * the configuration.
 * If an error is raised here, it means that a new version of the CCO has been installed and
 * the configuration ecom_wifi_configuration.h must be updated based on the one provided
 * by the new CCO version.
 */
#if ECOM_WIFI_CONFIGURATION_VERSION != 1

	#error "Version of the configuration file ecom_wifi_configuration.h is not compatible with this implementation."

#endif

//TODO: init errorcode and error message in caller

MICROEJ_ASYNC_WORKER_worker_declare(ecom_wifi_worker, ECOM_WIFI_WORKER_JOB_COUNT, ECOM_WIFI_worker_param_t, ECOM_WIFI_WAITING_LIST_SIZE);
OSAL_task_stack_declare(ecom_wifi_worker_stack, ECOM_WIFI_WORKER_STACK_SIZE);

static int32_t LLECOM_WIFI_async_exec_get_info(SNI_callback retry_function, MICROEJ_ASYNC_WORKER_action_t action, SNI_callback on_done) {

	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_allocate_job(&ecom_wifi_worker, retry_function);
	if (job == NULL) {
		// No job available, either:
		// - wait for a job to be available and this function to be executed again,
		// - or an exception is pending
		return J_EUNKNOWN; //unused value
	}

	MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_async_exec(&ecom_wifi_worker, job, action, on_done);
	if (status == MICROEJ_ASYNC_WORKER_OK) {
		// Wait for the action to be done
		return 0; //returned value not used
	}

	// Else an error occurred and MICROEJ_ASYNC_WORKER_async_exec has thrown a SNI exception
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);
	return J_EUNKNOWN; //unused value
}

static int32_t LLECOM_WIFI_async_exec_get_info_on_done(void) {

	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_get_job_done();
	ECOM_WIFI_get_info_t* params = (ECOM_WIFI_get_info_t*)job->params;

	int32_t result = params->result;
	if(result < 0){
		// Exception
		SNI_throwNativeIOException(params->error_code, params->error_message);
	}
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);

	return result;
}

static int32_t LLECOM_WIFI_async_exec_scanAP(int32_t index, SNI_callback retry_function, MICROEJ_ASYNC_WORKER_action_t action, SNI_callback on_done) {

	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_allocate_job(&ecom_wifi_worker, retry_function);
	if (job == NULL) {
		// No job available, either:
		// - wait for a job to be available and this function to be executed again,
		// - or an exception is pending
		return J_EUNKNOWN; //unused value
	}

	ECOM_WIFI_scanAP_channel_t* params = (ECOM_WIFI_scanAP_channel_t*)job->params;
	params->index = index;

	MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_async_exec(&ecom_wifi_worker, job, action, on_done);
	if (status == MICROEJ_ASYNC_WORKER_OK) {
		// Wait for the action to be done
		return 0; //returned value not used
	}

	// Else an error occurred and MICROEJ_ASYNC_WORKER_async_exec has thrown a SNI exception
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);
	return J_EUNKNOWN; //unused value
}

static int32_t LLECOM_WIFI_async_exec_scanAP_on_done(int32_t index) {

	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_get_job_done();
	ECOM_WIFI_scanAP_channel_t* params = (ECOM_WIFI_scanAP_channel_t*)job->params;

	int32_t result = params->result;
	if(result < 0){
		// Exception
		SNI_throwNativeIOException(params->error_code, params->error_message);
	}
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);

	return result;
}

void LLECOM_WIFI_initialize(void) {

	MICROEJ_ASYNC_WORKER_status_t res = MICROEJ_ASYNC_WORKER_initialize(&ecom_wifi_worker, (uint8_t *)"MicroEJ ECOM_WIFI", ecom_wifi_worker_stack, ECOM_WIFI_WORKER_PRIORITY);
	if (res == MICROEJ_ASYNC_WORKER_INVALID_ARGS) {
		SNI_throwNativeException(res, "Invalid argument for ECOM_WIFI async worker");
	} else if (res == MICROEJ_ASYNC_WORKER_ERROR) {
		SNI_throwNativeException(res, "Error while initializing ECOM_WIFI async worker");
	}
	// else OK

	LLECOM_WIFI_init();
}

int32_t LLECOM_WIFI_IMPL_disableSoftAP(int8_t getResult) {

	return LLECOM_WIFI_async_exec_get_info((SNI_callback)LLECOM_WIFI_IMPL_disableSoftAP, LLECOM_WIFI_IMPL_disableSoftAP_action, (SNI_callback)LLECOM_WIFI_async_exec_get_info_on_done);
}

static int32_t LLECOM_WIFI_IMPL_enableSoftAP_on_done(int8_t* ssid, int32_t ssidOffset, int32_t ssidLength, int8_t* passphrase, int32_t passphraseOffset, int32_t passphraseLength);

int32_t LLECOM_WIFI_IMPL_enableSoftAP(int8_t* ssid, int32_t ssidOffset, int32_t ssidLength, int8_t* passphrase, int32_t passphraseOffset, int32_t passphraseLength, int8_t getResult) {

	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_allocate_job(&ecom_wifi_worker, (SNI_callback)LLECOM_WIFI_IMPL_enableSoftAP);
	if (job == NULL) {
		// No job available, either:
		// - wait for a job to be available and this function to be executed again,
		// - or an exception is pending
		return J_EUNKNOWN; //unused value
	}

	ECOM_WIFI_enable_join_t* params = (ECOM_WIFI_enable_join_t*)job->params;
	params->ssid = ssid;
	params->ssidOffset = ssidOffset;
	params->ssidLength = ssidLength;
	params->passphrase = passphrase;
	params->passphraseOffset = passphraseOffset;
	params->passphraseLength = passphraseLength;

	MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_async_exec(&ecom_wifi_worker, job, LLECOM_WIFI_IMPL_enableSoftAP_action, (SNI_callback)LLECOM_WIFI_IMPL_enableSoftAP_on_done);
	if (status == MICROEJ_ASYNC_WORKER_OK) {
		// Wait for the action to be done
		return 0; //returned value not used
	}

	// Else an error occurred and MICROEJ_ASYNC_WORKER_async_exec has thrown a SNI exception
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);
	return J_EUNKNOWN; //unused value
}

static int32_t LLECOM_WIFI_IMPL_enableSoftAP_on_done(int8_t* ssid, int32_t ssidOffset, int32_t ssidLength, int8_t* passphrase, int32_t passphraseOffset, int32_t passphraseLength) {

	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_get_job_done();
	ECOM_WIFI_enable_join_t* params = (ECOM_WIFI_enable_join_t*)job->params;

	int32_t result = params->result;
	if(result < 0){
		// Exception
		SNI_throwNativeIOException(params->error_code, params->error_message);
	}
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);

	return result;
}

static int32_t LLECOM_WIFI_IMPL_setNameSoftAP_on_done(int8_t* name, int32_t nameOffset, int32_t nameLength);

int32_t LLECOM_WIFI_IMPL_setNameSoftAP(int8_t* name, int32_t nameOffset, int32_t nameLength, int8_t getResult) {

	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_allocate_job(&ecom_wifi_worker, (SNI_callback)LLECOM_WIFI_IMPL_setNameSoftAP);
	if (job == NULL) {
		// No job available, either:
		// - wait for a job to be available and this function to be executed again,
		// - or an exception is pending
		return J_EUNKNOWN; //unused value
	}

	ECOM_WIFI_set_name_t* params = (ECOM_WIFI_set_name_t*)job->params;
	params->name = name;
	params->nameOffset = nameOffset;
	params->nameLength = nameLength;

	MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_async_exec(&ecom_wifi_worker, job, LLECOM_WIFI_IMPL_setNameSoftAP_action, (SNI_callback)LLECOM_WIFI_IMPL_setNameSoftAP_on_done);
	if (status == MICROEJ_ASYNC_WORKER_OK) {
		// Wait for the action to be done
		return 0; //returned value not used
	}

	// Else an error occurred and MICROEJ_ASYNC_WORKER_async_exec has thrown a SNI exception
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);
	return J_EUNKNOWN; //unused value
}

static int32_t LLECOM_WIFI_IMPL_setNameSoftAP_on_done(int8_t* name, int32_t nameOffset, int32_t nameLength) {

	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_get_job_done();
	ECOM_WIFI_set_name_t* params = (ECOM_WIFI_set_name_t*)job->params;

	int32_t result = params->result;
	if(result < 0){
		// Exception
		SNI_throwNativeIOException(params->error_code, params->error_message);
	}
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);

	return result;
}

static int32_t LLECOM_WIFI_IMPL_getBSSID_on_done(int8_t* bssid, int32_t bssidOffset, int32_t bssidLength);

int32_t LLECOM_WIFI_IMPL_getBSSID(int8_t* bssid, int32_t bssidOffset, int32_t bssidLength, int8_t getResult) {

	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_allocate_job(&ecom_wifi_worker, (SNI_callback)LLECOM_WIFI_IMPL_getBSSID);
	if (job == NULL) {
		// No job available, either:
		// - wait for a job to be available and this function to be executed again,
		// - or an exception is pending
		return J_EUNKNOWN; //unused value
	}

	ECOM_WIFI_get_bssid_t* params = (ECOM_WIFI_get_bssid_t*)job->params;
	params->bssid = bssid;
	params->bssidOffset = bssidOffset;
	params->bssidLength = bssidLength;

	MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_async_exec(&ecom_wifi_worker, job, LLECOM_WIFI_IMPL_getBSSID_action, (SNI_callback)LLECOM_WIFI_IMPL_getBSSID_on_done);
	if (status == MICROEJ_ASYNC_WORKER_OK) {
		// Wait for the action to be done
		return 0; //returned value not used
	}

	// Else an error occurred and MICROEJ_ASYNC_WORKER_async_exec has thrown a SNI exception
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);
	return J_EUNKNOWN; //unused value
}

static int32_t LLECOM_WIFI_IMPL_getBSSID_on_done(int8_t* bssid, int32_t bssidOffset, int32_t bssidLength) {

	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_get_job_done();
	ECOM_WIFI_get_bssid_t* params = (ECOM_WIFI_get_bssid_t*)job->params;

	int32_t result = params->result;
	if(result < 0){
		// Exception
		SNI_throwNativeIOException(params->error_code, params->error_message);
	}
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);

	return result;
}

int32_t LLECOM_WIFI_IMPL_getChannel(int8_t getResult) {

	return LLECOM_WIFI_async_exec_get_info((SNI_callback)LLECOM_WIFI_IMPL_getChannel, LLECOM_WIFI_IMPL_getChannel_action, (SNI_callback)LLECOM_WIFI_async_exec_get_info_on_done);
}

int32_t LLECOM_WIFI_IMPL_getClientState(int8_t getResult) {

	return LLECOM_WIFI_async_exec_get_info((SNI_callback)LLECOM_WIFI_IMPL_getClientState, LLECOM_WIFI_IMPL_getClientState_action, (SNI_callback)LLECOM_WIFI_async_exec_get_info_on_done);
}

static int32_t LLECOM_WIFI_IMPL_getRSSI_on_done(float* rssi, int32_t rssiOffset, int32_t rssiLength);

int32_t LLECOM_WIFI_IMPL_getRSSI(float* rssi, int32_t rssiOffset, int32_t rssiLength, int8_t getResult) {

	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_allocate_job(&ecom_wifi_worker, (SNI_callback)LLECOM_WIFI_IMPL_getRSSI);
	if (job == NULL) {
		// No job available, either:
		// - wait for a job to be available and this function to be executed again,
		// - or an exception is pending
		return J_EUNKNOWN; //unused value
	}

	ECOM_WIFI_get_rssi_t* params = (ECOM_WIFI_get_rssi_t*)job->params;
	params->rssi = rssi;
	params->rssiOffset = rssiOffset;
	params->rssiLength = rssiLength;

	MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_async_exec(&ecom_wifi_worker, job, LLECOM_WIFI_IMPL_getRSSI_action, (SNI_callback)LLECOM_WIFI_IMPL_getRSSI_on_done);
	if (status == MICROEJ_ASYNC_WORKER_OK) {
		// Wait for the action to be done
		return 0; //returned value not used
	}

	// Else an error occurred and MICROEJ_ASYNC_WORKER_async_exec has thrown a SNI exception
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);
	return J_EUNKNOWN; //unused value
}

static int32_t LLECOM_WIFI_IMPL_getRSSI_on_done(float* rssi, int32_t rssiOffset, int32_t rssiLength) {

	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_get_job_done();
	ECOM_WIFI_get_rssi_t* params = (ECOM_WIFI_get_rssi_t*)job->params;

	int32_t result = params->result;
	if(result < 0){
		// Exception
		SNI_throwNativeIOException(params->error_code, params->error_message);
	}
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);

	return result;
}

int32_t LLECOM_WIFI_IMPL_getSecurityMode(int8_t getResult) {

	return LLECOM_WIFI_async_exec_get_info((SNI_callback)LLECOM_WIFI_IMPL_getSecurityMode, LLECOM_WIFI_IMPL_getSecurityMode_action, (SNI_callback)LLECOM_WIFI_async_exec_get_info_on_done);
}

static int32_t LLECOM_WIFI_IMPL_getSSID_on_done(int8_t* ssid, int32_t ssidOffset, int32_t ssidLength);

int32_t LLECOM_WIFI_IMPL_getSSID(int8_t* ssid, int32_t ssidOffset, int32_t ssidLength, int8_t getResult) {

	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_allocate_job(&ecom_wifi_worker, (SNI_callback)LLECOM_WIFI_IMPL_getSSID);
	if (job == NULL) {
		// No job available, either:
		// - wait for a job to be available and this function to be executed again,
		// - or an exception is pending
		return J_EUNKNOWN; //unused value
	}

	ECOM_WIFI_get_ssid_t* params = (ECOM_WIFI_get_ssid_t*)job->params;
	params->ssid = ssid;
	params->ssidOffset = ssidOffset;
	params->ssidLength = ssidLength;

	MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_async_exec(&ecom_wifi_worker, job, LLECOM_WIFI_IMPL_getSSID_action, (SNI_callback)LLECOM_WIFI_IMPL_getSSID_on_done);
	if (status == MICROEJ_ASYNC_WORKER_OK) {
		// Wait for the action to be done
		return 0; //returned value not used
	}

	// Else an error occurred and MICROEJ_ASYNC_WORKER_async_exec has thrown a SNI exception
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);
	return J_EUNKNOWN; //unused value
}

static int32_t LLECOM_WIFI_IMPL_getSSID_on_done(int8_t* ssid, int32_t ssidOffset, int32_t ssidLength) {

	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_get_job_done();
	ECOM_WIFI_get_ssid_t* params = (ECOM_WIFI_get_ssid_t*)job->params;

	int32_t result = params->result;
	if(result < 0){
		// Exception
		SNI_throwNativeIOException(params->error_code, params->error_message);
	}
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);

	return result;
}

int32_t LLECOM_WIFI_IMPL_getWPSModes(int8_t getResult) {

	return LLECOM_WIFI_async_exec_get_info((SNI_callback)LLECOM_WIFI_IMPL_getWPSModes, LLECOM_WIFI_IMPL_getWPSModes_action, (SNI_callback)LLECOM_WIFI_async_exec_get_info_on_done);
}

int32_t LLECOM_WIFI_IMPL_join(int8_t* ssid, int32_t ssidOffset, int32_t ssidLength, int8_t* passphrase, int32_t passphraseOffset, int32_t passphraseLength, int8_t getResult) {

	return LLECOM_WIFI_IMPL_joinWithSecurityMode(ssid, ssidOffset, ssidLength, passphrase, passphraseOffset, passphraseLength, SECURITY_MODE_WPA2, getResult);
}

static int32_t LLECOM_WIFI_IMPL_joinWithSecurityMode_on_done(int8_t* ssid, int32_t ssidOffset, int32_t ssidLength, int8_t* passphrase, int32_t passphraseOffset, int32_t passphraseLength, int32_t securityModeValue);

int32_t LLECOM_WIFI_IMPL_joinWithSecurityMode(int8_t* ssid, int32_t ssidOffset, int32_t ssidLength, int8_t* passphrase, int32_t passphraseOffset, int32_t passphraseLength, int32_t securityModeValue, int8_t getResult) {

	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_allocate_job(&ecom_wifi_worker, (SNI_callback)LLECOM_WIFI_IMPL_joinWithSecurityMode);
	if (job == NULL) {
		// No job available, either:
		// - wait for a job to be available and this function to be executed again,
		// - or an exception is pending
		return J_EUNKNOWN; //unused value
	}

	ECOM_WIFI_join_security_mode_t* params = (ECOM_WIFI_join_security_mode_t*)job->params;
	params->ssid = ssid;
	params->ssidOffset = ssidOffset;
	params->ssidLength = ssidLength;
	params->passphrase = passphrase;
	params->passphraseOffset = passphraseOffset;
	params->passphraseLength = passphraseLength;
	params->securityModeValue = securityModeValue;

	MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_async_exec(&ecom_wifi_worker, job, LLECOM_WIFI_IMPL_joinWithSecurityMode_action, (SNI_callback)LLECOM_WIFI_IMPL_joinWithSecurityMode_on_done);
	if (status == MICROEJ_ASYNC_WORKER_OK) {
		// Wait for the action to be done
		return 0; //returned value not used
	}

	// Else an error occurred and MICROEJ_ASYNC_WORKER_async_exec has thrown a SNI exception
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);
	return J_EUNKNOWN; //unused value
}

static int32_t LLECOM_WIFI_IMPL_joinWithSecurityMode_on_done(int8_t* ssid, int32_t ssidOffset, int32_t ssidLength, int8_t* passphrase, int32_t passphraseOffset, int32_t passphraseLength, int32_t securityModeValue) {

	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_get_job_done();
	ECOM_WIFI_join_security_mode_t* params = (ECOM_WIFI_join_security_mode_t*)job->params;

	int32_t result = params->result;
	if(result < 0){
		// Exception
		SNI_throwNativeIOException(params->error_code, params->error_message);
	}
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);

	return result;
}

int32_t LLECOM_WIFI_IMPL_leave(int8_t getResult) {

	return LLECOM_WIFI_async_exec_get_info((SNI_callback)LLECOM_WIFI_IMPL_leave, LLECOM_WIFI_IMPL_leave_action, (SNI_callback)LLECOM_WIFI_async_exec_get_info_on_done);
}

static int32_t LLECOM_WIFI_IMPL_scanAPCount_on_done(int8_t active);

int32_t LLECOM_WIFI_IMPL_scanAPCount(int8_t active, int8_t getResult) {

	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_allocate_job(&ecom_wifi_worker, (SNI_callback)LLECOM_WIFI_IMPL_scanAPCount);
	if (job == NULL) {
		// No job available, either:
		// - wait for a job to be available and this function to be executed again,
		// - or an exception is pending
		return J_EUNKNOWN; //unused value
	}

	ECOM_WIFI_scanAP_count_t* params = (ECOM_WIFI_scanAP_count_t*)job->params;
	params->active = active;

	MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_async_exec(&ecom_wifi_worker, job, LLECOM_WIFI_IMPL_scanAPCount_action, (SNI_callback)LLECOM_WIFI_IMPL_scanAPCount_on_done);
	if (status == MICROEJ_ASYNC_WORKER_OK) {
		// Wait for the action to be done
		return 0; //returned value not used
	}

	// Else an error occurred and MICROEJ_ASYNC_WORKER_async_exec has thrown a SNI exception
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);
	return J_EUNKNOWN; //unused value
}

static int32_t LLECOM_WIFI_IMPL_scanAPCount_on_done(int8_t active) {

	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_get_job_done();
	ECOM_WIFI_scanAP_count_t* params = (ECOM_WIFI_scanAP_count_t*)job->params;

	int32_t result = params->result;
	if(result < 0){
		// Exception
		SNI_throwNativeIOException(params->error_code, params->error_message);
	}
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);

	return result;
}

static int32_t LLECOM_WIFI_IMPL_scanAPssidAt_on_done(int8_t* ssid, int32_t ssidOffset,	int32_t ssidLength, int32_t index);

int32_t LLECOM_WIFI_IMPL_scanAPssidAt(int8_t* ssid, int32_t ssidOffset,	int32_t ssidLength, int32_t index, int8_t getResult) {

	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_allocate_job(&ecom_wifi_worker, (SNI_callback)LLECOM_WIFI_IMPL_scanAPssidAt);
	if (job == NULL) {
		// No job available, either:
		// - wait for a job to be available and this function to be executed again,
		// - or an exception is pending
		return J_EUNKNOWN; //unused value
	}

	ECOM_WIFI_scanAP_ssid_t* params = (ECOM_WIFI_scanAP_ssid_t*)job->params;
	params->ssid = ssid;
	params->ssidOffset = ssidOffset;
	params->ssidLength = ssidLength;
	params->index = index;

	MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_async_exec(&ecom_wifi_worker, job, LLECOM_WIFI_IMPL_scanAPssidAt_action, (SNI_callback)LLECOM_WIFI_IMPL_scanAPssidAt_on_done);
	if (status == MICROEJ_ASYNC_WORKER_OK) {
		// Wait for the action to be done
		return 0; //returned value not used
	}

	// Else an error occurred and MICROEJ_ASYNC_WORKER_async_exec has thrown a SNI exception
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);
	return J_EUNKNOWN; //unused value
}

static int32_t LLECOM_WIFI_IMPL_scanAPssidAt_on_done(int8_t* ssid, int32_t ssidOffset,	int32_t ssidLength, int32_t index) {

	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_get_job_done();
	ECOM_WIFI_scanAP_ssid_t* params = (ECOM_WIFI_scanAP_ssid_t*)job->params;

	int32_t result = params->result;
	if(result < 0){
		// Exception
		SNI_throwNativeIOException(params->error_code, params->error_message);
	}
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);

	return result;
}

static int32_t LLECOM_WIFI_IMPL_scanAPbssidAt_on_done(int8_t* bssid, int32_t bssidOffset, int32_t bssidLength, int32_t index);

int32_t LLECOM_WIFI_IMPL_scanAPbssidAt(int8_t* bssid, int32_t bssidOffset, int32_t bssidLength, int32_t index, int8_t getResult) {

	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_allocate_job(&ecom_wifi_worker, (SNI_callback)LLECOM_WIFI_IMPL_scanAPbssidAt);
	if (job == NULL) {
		// No job available, either:
		// - wait for a job to be available and this function to be executed again,
		// - or an exception is pending
		return J_EUNKNOWN; //unused value
	}

	ECOM_WIFI_scanAP_bssid_t* params = (ECOM_WIFI_scanAP_bssid_t*)job->params;
	params->bssid = bssid;
	params->bssidOffset = bssidOffset;
	params->bssidLength = bssidLength;
	params->index = index;

	MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_async_exec(&ecom_wifi_worker, job, LLECOM_WIFI_IMPL_scanAPbssidAt_action, (SNI_callback)LLECOM_WIFI_IMPL_scanAPbssidAt_on_done);
	if (status == MICROEJ_ASYNC_WORKER_OK) {
		// Wait for the action to be done
		return 0; //returned value not used
	}

	// Else an error occurred and MICROEJ_ASYNC_WORKER_async_exec has thrown a SNI exception
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);
	return J_EUNKNOWN; //unused value
}

static int32_t LLECOM_WIFI_IMPL_scanAPbssidAt_on_done(int8_t* bssid, int32_t bssidOffset, int32_t bssidLength, int32_t index) {

	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_get_job_done();
	ECOM_WIFI_scanAP_bssid_t* params = (ECOM_WIFI_scanAP_bssid_t*)job->params;

	int32_t result = params->result;
	if(result < 0){
		// Exception
		SNI_throwNativeIOException(params->error_code, params->error_message);
	}
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);

	return result;
}

int32_t LLECOM_WIFI_IMPL_scanAPchannelAt(int32_t index, int8_t getResult) {

	return LLECOM_WIFI_async_exec_scanAP(index, (SNI_callback)LLECOM_WIFI_IMPL_scanAPchannelAt, LLECOM_WIFI_IMPL_scanAPchannelAt_action, (SNI_callback)LLECOM_WIFI_async_exec_scanAP_on_done);
}

static int32_t LLECOM_WIFI_IMPL_scanAPrssiAt_on_done(float* rssi, int32_t rssiOffset, int32_t rssiLength, int32_t index);

int32_t LLECOM_WIFI_IMPL_scanAPrssiAt(float* rssi, int32_t rssiOffset, int32_t rssiLength, int32_t index, int8_t getResult) {

	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_allocate_job(&ecom_wifi_worker, (SNI_callback)LLECOM_WIFI_IMPL_scanAPrssiAt);
	if (job == NULL) {
		// No job available, either:
		// - wait for a job to be available and this function to be executed again,
		// - or an exception is pending
		return J_EUNKNOWN; //unused value
	}

	ECOM_WIFI_scanAP_rssi_t* params = (ECOM_WIFI_scanAP_rssi_t*)job->params;
	params->rssi = rssi;
	params->rssiOffset = rssiOffset;
	params->rssiLength = rssiLength;
	params->index = index;

	MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_async_exec(&ecom_wifi_worker, job, LLECOM_WIFI_IMPL_scanAPrssiAt_action, (SNI_callback)LLECOM_WIFI_IMPL_scanAPrssiAt_on_done);
	if (status == MICROEJ_ASYNC_WORKER_OK) {
		// Wait for the action to be done
		return 0; //returned value not used
	}

	// Else an error occurred and MICROEJ_ASYNC_WORKER_async_exec has thrown a SNI exception
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);
	return J_EUNKNOWN; //unused value
}

static int32_t LLECOM_WIFI_IMPL_scanAPrssiAt_on_done(float* rssi, int32_t rssiOffset, int32_t rssiLength, int32_t index) {

	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_get_job_done();
	ECOM_WIFI_scanAP_rssi_t* params = (ECOM_WIFI_scanAP_rssi_t*)job->params;

	int32_t result = params->result;
	if(result < 0){
		// Exception
		SNI_throwNativeIOException(params->error_code, params->error_message);
	}
	MICROEJ_ASYNC_WORKER_free_job(&ecom_wifi_worker, job);

	return result;
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
