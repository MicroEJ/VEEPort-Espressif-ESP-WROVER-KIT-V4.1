/*
 * C
 *
 * Copyright 2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 *
 */

#ifndef  ECOM_NETWORK_CONFIGURATION_H
#define  ECOM_NETWORK_CONFIGURATION_H

/**
 * @file
 * @brief LLECOM_NETWORK configuration.
 * @author MicroEJ Developer Team
 * @version 1.0.0
 * @date 14 August 2020
 */

#ifdef __cplusplus
	extern "C" {
#endif


/**
 * @brief Compatibility sanity check value.
 * This define value is checked in the implementation to validate that the version of this configuration
 * is compatible with the implementation.
 *
 * This value must not be changed by the user of the CCO.
 * This value must be incremented by the implementor of the CCO when a configuration define is added, deleted or modified.
 */
#define ECOM_NETWORK_CONFIGURATION_VERSION (1)

	/**
	 * @brief Number of workers dedicated to the ECOM-NETWORK in async_worker.
	 */
#define ECOM_NETWORK_WORKER_JOB_COUNT (4)

	/**
	 * @brief Size of the waiting list for ECOM-NETWORK jobs in async_worker.
	 */
#define ECOM_NETWORK_WAITING_LIST_SIZE (16)

	/**
	 * @brief Size of the ECOM-NETWORK stack in bytes.
	 */
#if defined(__QNXNTO__)
	#define ECOM_NETWORK_WORKER_STACK_SIZE (1024*16) // stack requires more space on QNX
#else
	#define ECOM_NETWORK_WORKER_STACK_SIZE (1024*2)
#endif

	/**
	 * @brief Priority of the ECOM-NETWORK workers.
	 */
#define ECOM_NETWORK_WORKER_PRIORITY (6)

	/**
	 * @brief Set this define to print debug traces.
	 */
//#define	LLECOM_NETWORK_DEBUG
#ifdef __cplusplus
	}
#endif

#endif // ECOM_NETWORK_CONFIGURATION_H
