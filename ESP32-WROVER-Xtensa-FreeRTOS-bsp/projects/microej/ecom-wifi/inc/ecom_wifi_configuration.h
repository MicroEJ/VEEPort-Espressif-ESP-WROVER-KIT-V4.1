/*
 * C
 *
 * Copyright 2020-2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#ifndef  ECOM_WIFI_CONFIGURATION_H
#define  ECOM_WIFI_CONFIGURATION_H

/**
 * @file
 * @brief LLECOM_WIFI configuration.
 * @author MicroEJ Developer Team
 * @version 2.3.0
 * @date 27 February 2023
 */

#include "microej_async_worker.h"
#include "WIFI_ESP32_driver.h"

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
#define ECOM_WIFI_CONFIGURATION_VERSION (2)

/**
 * @brief Use this macro to define the initialization function of the wifi stack.
 * Called from LLECOM_WIFI_IMPL_initialize().
 * By default this macro does nothing.
 */
#define llecom_wifi_init() (WIFI_ESP32_initialize_f())

/**
 * @brief Set this define to use a custom worker to handle ECOM-WIFI asynchronous jobs.
 */
//#define ECOM_WIFI_CUSTOM_WORKER

/**
 * @brief Define ECOM-WIFI custom or module specific worker.
 */
#ifdef ECOM_WIFI_CUSTOM_WORKER
extern MICROEJ_ASYNC_WORKER_handle_t my_custom_worker;
#define ecom_wifi_worker my_custom_worker
#else
extern MICROEJ_ASYNC_WORKER_handle_t ecom_wifi_worker;
#endif

/**
 * @brief Number of workers dedicated to the ECOM-WIFI in async_worker.
 */
#define ECOM_WIFI_WORKER_JOB_COUNT (4)

/**
 * @brief Size of the waiting list for ECOM-WIFI jobs in async_worker.
 */
#define ECOM_WIFI_WAITING_LIST_SIZE (16)

/**
 * @brief Size of the ECOM-WIFI stack in bytes.
 */
#define ECOM_WIFI_WORKER_STACK_SIZE (1024*3)

/**
 * @brief ECOM-WIFI worker stack size must be calibrated, unless using a custom worker defined in another module.
 */
#if !defined(ECOM_WIFI_CUSTOM_WORKER) && !defined(ECOM_WIFI_WORKER_STACK_SIZE)
#error "ECOM_WIFI_WORKER_STACK_SIZE not declared. Please uncomment the line above to enable macro declaration and put the right value according to the stack size calibration done for your environment"
#endif // ECOM_WIFI_WORKER_STACK_SIZE

/**
 * @brief Priority of the ECOM-WIFI workers.
 */
#define ECOM_WIFI_WORKER_PRIORITY (6)

/**
 * @brief Generic error returned by native functions.
 */
#define ECOM_WIFI_ERROR (-1)

/**
 * @brief Size of a ssid in bytes (32 bytes).
 */
#define MAX_SSID_SIZE (32)

/**
 * @brief Size of a passphrase in bytes (64 bytes).
 */
#define MAX_PASSPHRASE_SIZE  (64)

/**
 * @brief Size of a SoftAP name in bytes (64 bytes).
 */
#define MAX_NAME_SIZE (64)

/**
 * @brief Set this define to print debug traces.
 */
//#define LLECOM_WIFI_DEBUG

#ifdef __cplusplus
	}
#endif

#endif // ECOM_WIFI_CONFIGURATION_H
