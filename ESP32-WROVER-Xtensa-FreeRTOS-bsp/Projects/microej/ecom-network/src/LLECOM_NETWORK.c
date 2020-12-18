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
#include "LLECOM_NETWORK_impl.h"
#include "LLNET_ERRORS.h"
#include "sni.h"
#include "microej_async_worker.h"
#include "ecom_network_helper.h"
#include "ecom_network_configuration.h"

/**
 * Sanity check between the expected version of the configuration and the actual version of
 * the configuration.
 * If an error is raised here, it means that a new version of the CCO has been installed and
 * the configuration ecom_network_configuration.h must be updated based on the one provided
 * by the new CCO version.
 */
#if ECOM_NETWORK_CONFIGURATION_VERSION != 1

	#error "Version of the configuration file ecom_network_configuration.h is not compatible with this implementation."

#endif

//TODO: init errorcode and error message in caller

MICROEJ_ASYNC_WORKER_worker_declare(ecom_network_worker, ECOM_NETWORK_WORKER_JOB_COUNT, ECOM_NETWORK_worker_param_t, ECOM_NETWORK_WAITING_LIST_SIZE);
OSAL_task_stack_declare(ecom_network_worker_stack, ECOM_NETWORK_WORKER_STACK_SIZE);

static int32_t LLECOM_NETWORK_async_exec_netif_context(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, SNI_callback retry_function, MICROEJ_ASYNC_WORKER_action_t action, SNI_callback on_done) {

	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_allocate_job(&ecom_network_worker, retry_function);
	if (job == NULL) {
		// No job available, either:
		// - wait for a job to be available and this function to be executed again,
		// - or an exception is pending
		return J_EUNKNOWN; //unused value
	}

	ECOM_NETWORK_netif_context_t* params = (ECOM_NETWORK_netif_context_t*)job->params;
	params->netifName = netifName;
	params->netifNameOffset = netifNameOffset;
	params->netifNameLength = netifNameLength;

	MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_async_exec(&ecom_network_worker, job, action, on_done);
	if (status == MICROEJ_ASYNC_WORKER_OK) {
		// Wait for the action to be done
		return 0; //returned value not used
	}

	// Else an error occurred and MICROEJ_ASYNC_WORKER_async_exec has thrown a SNI exception
	MICROEJ_ASYNC_WORKER_free_job(&ecom_network_worker, job);
	return J_EUNKNOWN; //unused value
}

static int32_t LLECOM_NETWORK_async_exec_netif_context_on_done(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength) {

	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_get_job_done();
	ECOM_NETWORK_netif_context_t* params = (ECOM_NETWORK_netif_context_t*)job->params;

	int32_t result = params->result;
	if(result < 0){
		// Exception
		SNI_throwNativeIOException(params->error_code, params->error_message);
	}
	MICROEJ_ASYNC_WORKER_free_job(&ecom_network_worker, job);

	return result;
}

static int32_t LLECOM_NETWORK_async_exec_dns_context(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t* address, int32_t addressOffset, int32_t addressLength, int32_t index, SNI_callback retry_function, MICROEJ_ASYNC_WORKER_action_t action, SNI_callback on_done) {

	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_allocate_job(&ecom_network_worker, retry_function);
	if (job == NULL) {
		// No job available, either:
		// - wait for a job to be available and this function to be executed again,
		// - or an exception is pending
		return J_EUNKNOWN; //unused value
	}

	ECOM_NETWORK_dns_context_t* params = (ECOM_NETWORK_dns_context_t*)job->params;
	params->netifName = netifName;
	params->netifNameOffset = netifNameOffset;
	params->netifNameLength = netifNameLength;
	params->address = address;
	params->addressOffset = addressOffset;
	params->addressLength = addressLength;
	params->index = index;

	MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_async_exec(&ecom_network_worker, job, action, on_done);
	if (status == MICROEJ_ASYNC_WORKER_OK) {
		// Wait for the action to be done
		return 0; //returned value not used
	}

	// Else an error occurred and MICROEJ_ASYNC_WORKER_async_exec has thrown a SNI exception
	MICROEJ_ASYNC_WORKER_free_job(&ecom_network_worker, job);
	return J_EUNKNOWN; //unused value
}

static int32_t LLECOM_NETWORK_async_exec_dns_context_on_done(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t* address, int32_t addressOffset, int32_t addressLength, int32_t index) {

	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_get_job_done();
	ECOM_NETWORK_dns_context_t* params = (ECOM_NETWORK_dns_context_t*)job->params;

	int32_t result = params->result;
	if(result < 0){
		// Exception
		SNI_throwNativeIOException(params->error_code, params->error_message);
	}
	MICROEJ_ASYNC_WORKER_free_job(&ecom_network_worker, job);

	return result;
}

static int32_t LLECOM_NETWORK_async_exec_ip_context(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t* address, int32_t addressOffset, int32_t addressLength, SNI_callback retry_function, MICROEJ_ASYNC_WORKER_action_t action, SNI_callback on_done) {

	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_allocate_job(&ecom_network_worker, retry_function);
	if (job == NULL) {
		// No job available, either:
		// - wait for a job to be available and this function to be executed again,
		// - or an exception is pending
		return J_EUNKNOWN; //unused value
	}

	ECOM_NETWORK_ip_context_t* params = (ECOM_NETWORK_ip_context_t*)job->params;
	params->netifName = netifName;
	params->netifNameOffset = netifNameOffset;
	params->netifNameLength = netifNameLength;
	params->address = address;
	params->addressOffset = addressOffset;
	params->addressLength = addressLength;

	MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_async_exec(&ecom_network_worker, job, action, on_done);
	if (status == MICROEJ_ASYNC_WORKER_OK) {
		// Wait for the action to be done
		return 0; //returned value not used
	}

	// Else an error occurred and MICROEJ_ASYNC_WORKER_async_exec has thrown a SNI exception
	MICROEJ_ASYNC_WORKER_free_job(&ecom_network_worker, job);
	return J_EUNKNOWN; //unused value
}

static int32_t LLECOM_NETWORK_async_exec_ip_context_on_done(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t* address, int32_t addressOffset, int32_t addressLength) {

	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_get_job_done();
	ECOM_NETWORK_ip_context_t* params = (ECOM_NETWORK_ip_context_t*)job->params;

	int32_t result = params->result;
	if(result < 0){
		// Exception
		SNI_throwNativeIOException(params->error_code, params->error_message);
	}
	MICROEJ_ASYNC_WORKER_free_job(&ecom_network_worker, job);

	return result;
}

static int32_t LLECOM_NETWORK_async_exec_ip_config(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t use, SNI_callback retry_function, MICROEJ_ASYNC_WORKER_action_t action, SNI_callback on_done) {

	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_allocate_job(&ecom_network_worker, retry_function);
	if (job == NULL) {
		// No job available, either:
		// - wait for a job to be available and this function to be executed again,
		// - or an exception is pending
		return J_EUNKNOWN; //unused value
	}

	ECOM_NETWORK_ip_config_t* params = (ECOM_NETWORK_ip_config_t*)job->params;
	params->netifName = netifName;
	params->netifNameOffset = netifNameOffset;
	params->netifNameLength = netifNameLength;
	params->use = use;

	MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_async_exec(&ecom_network_worker, job, action, on_done);
	if (status == MICROEJ_ASYNC_WORKER_OK) {
		// Wait for the action to be done
		return 0; //returned value not used
	}

	// Else an error occurred and MICROEJ_ASYNC_WORKER_async_exec has thrown a SNI exception
	MICROEJ_ASYNC_WORKER_free_job(&ecom_network_worker, job);
	return J_EUNKNOWN; //unused value
}

static int32_t LLECOM_NETWORK_async_exec_ip_config_on_done(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t use) {

	MICROEJ_ASYNC_WORKER_job_t* job = MICROEJ_ASYNC_WORKER_get_job_done();
	ECOM_NETWORK_ip_config_t* params = (ECOM_NETWORK_ip_config_t*)job->params;

	int32_t result = params->result;
	if(result < 0){
		// Exception
		SNI_throwNativeIOException(params->error_code, params->error_message);
	}
	MICROEJ_ASYNC_WORKER_free_job(&ecom_network_worker, job);

	return result;
}

void LLECOM_NETWORK_initialize(void) {

	MICROEJ_ASYNC_WORKER_status_t res = MICROEJ_ASYNC_WORKER_initialize(&ecom_network_worker, (uint8_t *)"MicroEJ ECOM_NETWORK", ecom_network_worker_stack, ECOM_NETWORK_WORKER_PRIORITY);

	if (res == MICROEJ_ASYNC_WORKER_INVALID_ARGS) {
		SNI_throwNativeException(res, "Invalid argument for ECOM_NETWORK async worker");
	} else if (res == MICROEJ_ASYNC_WORKER_ERROR) {
		SNI_throwNativeException(res, "Error while initializing ECOM_NETWORK async worker");
	}
	// else OK
}

int32_t LLECOM_NETWORK_IMPL_disable(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t getResult) {

	return LLECOM_NETWORK_async_exec_netif_context(netifName, netifNameOffset, netifNameLength, (SNI_callback)LLECOM_NETWORK_IMPL_disable, LLECOM_NETWORK_IMPL_disable_action, (SNI_callback)LLECOM_NETWORK_async_exec_netif_context_on_done);
}

int32_t LLECOM_NETWORK_IMPL_enable(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t getResult) {

	return LLECOM_NETWORK_async_exec_netif_context(netifName, netifNameOffset, netifNameLength, (SNI_callback)LLECOM_NETWORK_IMPL_enable, LLECOM_NETWORK_IMPL_enable_action, (SNI_callback)LLECOM_NETWORK_async_exec_netif_context_on_done);
}

int32_t LLECOM_NETWORK_IMPL_isEnabled(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t getResult) {

	return LLECOM_NETWORK_async_exec_netif_context(netifName, netifNameOffset, netifNameLength, (SNI_callback)LLECOM_NETWORK_IMPL_isEnabled, LLECOM_NETWORK_IMPL_isEnabled_action, (SNI_callback)LLECOM_NETWORK_async_exec_netif_context_on_done);
}

int32_t LLECOM_NETWORK_IMPL_start(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t getResult) {

	return LLECOM_NETWORK_async_exec_netif_context(netifName, netifNameOffset, netifNameLength, (SNI_callback)LLECOM_NETWORK_IMPL_start, LLECOM_NETWORK_IMPL_start_action, (SNI_callback)LLECOM_NETWORK_async_exec_netif_context_on_done);
}

int32_t LLECOM_NETWORK_IMPL_stop(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t getResult) {

	return LLECOM_NETWORK_async_exec_netif_context(netifName, netifNameOffset, netifNameLength, (SNI_callback)LLECOM_NETWORK_IMPL_stop, LLECOM_NETWORK_IMPL_stop_action, (SNI_callback)LLECOM_NETWORK_async_exec_netif_context_on_done);
}

int32_t LLECOM_NETWORK_IMPL_isStarted(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t getResult) {

	return LLECOM_NETWORK_async_exec_netif_context(netifName, netifNameOffset, netifNameLength, (SNI_callback)LLECOM_NETWORK_IMPL_isStarted, LLECOM_NETWORK_IMPL_isStarted_action, (SNI_callback)LLECOM_NETWORK_async_exec_netif_context_on_done);
}

int32_t LLECOM_NETWORK_IMPL_getDNSCount(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t getResult) {

	return LLECOM_NETWORK_async_exec_netif_context(netifName, netifNameOffset, netifNameLength, (SNI_callback)LLECOM_NETWORK_IMPL_getDNSCount, LLECOM_NETWORK_IMPL_getDNSCount_action, (SNI_callback)LLECOM_NETWORK_async_exec_netif_context_on_done);
}

int32_t LLECOM_NETWORK_IMPL_getDNS(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t* address, int32_t addressOffset, int32_t addressLength, int32_t index, int8_t getResult) {

	return LLECOM_NETWORK_async_exec_dns_context(netifName, netifNameOffset, netifNameLength, address, addressOffset, addressLength, index, (SNI_callback)LLECOM_NETWORK_IMPL_getDNS, LLECOM_NETWORK_IMPL_getDNS_action, (SNI_callback)LLECOM_NETWORK_async_exec_dns_context_on_done);
}

int32_t LLECOM_NETWORK_IMPL_getGateway(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t* address, int32_t addressOffset, int32_t addressLength, int8_t getResult) {

	return LLECOM_NETWORK_async_exec_ip_context(netifName, netifNameOffset, netifNameLength, address, addressOffset, addressLength, (SNI_callback)LLECOM_NETWORK_IMPL_getGateway, LLECOM_NETWORK_IMPL_getGateway_action, (SNI_callback)LLECOM_NETWORK_async_exec_ip_context_on_done);
}

int32_t LLECOM_NETWORK_IMPL_getIP(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t* address, int32_t addressOffset, int32_t addressLength, int8_t getResult) {

	return LLECOM_NETWORK_async_exec_ip_context(netifName, netifNameOffset, netifNameLength, address, addressOffset, addressLength, (SNI_callback)LLECOM_NETWORK_IMPL_getIP, LLECOM_NETWORK_IMPL_getIP_action, (SNI_callback)LLECOM_NETWORK_async_exec_ip_context_on_done);
}

int32_t LLECOM_NETWORK_IMPL_getNetmask(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t* address, int32_t addressOffset, int32_t addressLength, int8_t getResult) {

	return LLECOM_NETWORK_async_exec_ip_context(netifName, netifNameOffset, netifNameLength, address, addressOffset, addressLength, (SNI_callback)LLECOM_NETWORK_IMPL_getNetmask, LLECOM_NETWORK_IMPL_getNetmask_action, (SNI_callback)LLECOM_NETWORK_async_exec_ip_context_on_done);
}

int32_t LLECOM_NETWORK_IMPL_isDNSStatic(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t getResult) {

	return LLECOM_NETWORK_async_exec_netif_context(netifName, netifNameOffset, netifNameLength, (SNI_callback)LLECOM_NETWORK_IMPL_isDNSStatic, LLECOM_NETWORK_IMPL_isDNSStatic_action, (SNI_callback)LLECOM_NETWORK_async_exec_netif_context_on_done);
}

int32_t LLECOM_NETWORK_IMPL_isStatic(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t getResult) {

	return LLECOM_NETWORK_async_exec_netif_context(netifName, netifNameOffset, netifNameLength, (SNI_callback)LLECOM_NETWORK_IMPL_isStatic, LLECOM_NETWORK_IMPL_isStatic_action, (SNI_callback)LLECOM_NETWORK_async_exec_netif_context_on_done);
}

int32_t LLECOM_NETWORK_IMPL_endConfiguration(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t getResult) {

	return LLECOM_NETWORK_async_exec_netif_context(netifName, netifNameOffset, netifNameLength, (SNI_callback)LLECOM_NETWORK_IMPL_endConfiguration, LLECOM_NETWORK_IMPL_endConfiguration_action, (SNI_callback)LLECOM_NETWORK_async_exec_netif_context_on_done);
}

int32_t LLECOM_NETWORK_IMPL_startConfiguration(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t getResult) {

	return LLECOM_NETWORK_async_exec_netif_context(netifName, netifNameOffset, netifNameLength, (SNI_callback)LLECOM_NETWORK_IMPL_startConfiguration, LLECOM_NETWORK_IMPL_startConfiguration_action, (SNI_callback)LLECOM_NETWORK_async_exec_netif_context_on_done);
}

int32_t LLECOM_NETWORK_IMPL_setDNS(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t* address, int32_t addressOffset, int32_t addressLength, int32_t index, int8_t getResult) {

	return LLECOM_NETWORK_async_exec_dns_context(netifName, netifNameOffset, netifNameLength, address, addressOffset, addressLength, index, (SNI_callback)LLECOM_NETWORK_IMPL_setDNS, LLECOM_NETWORK_IMPL_setDNS_action, (SNI_callback)LLECOM_NETWORK_async_exec_dns_context_on_done);
}

int32_t LLECOM_NETWORK_IMPL_setGateway(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t* address, int32_t addressOffset, int32_t addressLength, int8_t getResult) {

	return LLECOM_NETWORK_async_exec_ip_context(netifName, netifNameOffset, netifNameLength, address, addressOffset, addressLength, (SNI_callback)LLECOM_NETWORK_IMPL_setGateway, LLECOM_NETWORK_IMPL_setGateway_action, (SNI_callback)LLECOM_NETWORK_async_exec_ip_context_on_done);
}

int32_t LLECOM_NETWORK_IMPL_setIP(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t* address, int32_t addressOffset, int32_t addressLength, int8_t getResult) {

	return LLECOM_NETWORK_async_exec_ip_context(netifName, netifNameOffset, netifNameLength, address, addressOffset, addressLength, (SNI_callback)LLECOM_NETWORK_IMPL_setIP, LLECOM_NETWORK_IMPL_setIP_action, (SNI_callback)LLECOM_NETWORK_async_exec_ip_context_on_done);
}

int32_t LLECOM_NETWORK_IMPL_setNetmask(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t* address, int32_t addressOffset, int32_t addressLength, int8_t getResult) {

	return LLECOM_NETWORK_async_exec_ip_context(netifName, netifNameOffset, netifNameLength, address, addressOffset, addressLength, (SNI_callback)LLECOM_NETWORK_IMPL_setNetmask, LLECOM_NETWORK_IMPL_setNetmask_action, (SNI_callback)LLECOM_NETWORK_async_exec_ip_context_on_done);
}

int32_t LLECOM_NETWORK_IMPL_useDHCP(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t use, int8_t getResult) {

	return LLECOM_NETWORK_async_exec_ip_config(netifName, netifNameOffset, netifNameLength, use, (SNI_callback)LLECOM_NETWORK_IMPL_useDHCP, LLECOM_NETWORK_IMPL_useDHCP_action, (SNI_callback)LLECOM_NETWORK_async_exec_ip_config_on_done);
}

int32_t LLECOM_NETWORK_IMPL_useStaticDNS(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t use, int8_t getResult) {

	return LLECOM_NETWORK_async_exec_ip_config(netifName, netifNameOffset, netifNameLength, use, (SNI_callback)LLECOM_NETWORK_IMPL_useStaticDNS, LLECOM_NETWORK_IMPL_useStaticDNS_action, (SNI_callback)LLECOM_NETWORK_async_exec_ip_config_on_done);
}
