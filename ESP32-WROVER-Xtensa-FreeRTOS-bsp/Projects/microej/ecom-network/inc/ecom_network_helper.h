/*
 * C
 *
 * Copyright 2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 *
 */

#ifndef ECOM_NETWORK_HELPER_H
#define ECOM_NETWORK_HELPER_H

#include "ecom_network_configuration.h"
#include "microej_async_worker.h"

#ifdef __cplusplus
	extern "C" {
#endif

/*
 * IMPORTANT NOTICES
 *
 * This file lists the data structures used by all the LLECOM_NETWORK_* functions.
 * If you add a new function and its corresponding data structure, do not forget to add
 * the data structure to the ECOM_NETWORK_worker_param_t union.
 *
 * Some structures duplicate the definition of fields specified in generic structures instead of
 * referencing the generic structures (e.g. the <code>path</code> and <code>result</code> fields).
 * This decision has been made to simplify usage and readability at the expense of maintainability.
 * Be careful when modifying the fields of the structures or when defining new structures.
 * See the comment of each structure to identify the ones that are affected by these constraints.
 */

/**
 * @brief Data structure for enable/disable and start/stop operations.
 *
 * This structure is used by <code>LLECOM_NETWORK_IMPL_enable</code>, <code>LLECOM_NETWORK_IMPL_disable</code>,
 * <code>LLECOM_NETWORK_IMPL_isEnabled</code>, <code>LLECOM_NETWORK_IMPL_start</code>,
 * <code>LLECOM_NETWORK_IMPL_stop</code>, <code>LLECOM_NETWORK_IMPL_isStarted</code>,
 * <code>LLECOM_NETWORK_IMPL_getDNSCount</code>, <code>LLECOM_NETWORK_IMPL_isDNSStatic</code>,
 * <code>LLECOM_NETWORK_IMPL_isStatic</code>, <code>LLECOM_NETWORK_IMPL_startConfiguration</code>
 * and <code>LLECOM_NETWORK_IMPL_endConfiguration</code>.
 *
 * Fields defined in this structure correspond to the parameters of these functions and
 * <code>result</code> field corresponds to the value returned by them.
 */
typedef struct {
	int8_t* netifName; /*!< [IN] The buffer that store the interface name. */
	int32_t netifNameOffset; /*!< [IN] The offset where the interface name is stored. */
	int32_t netifNameLength; /*!< [IN] The interface name buffer length. */
	int32_t result; /*!< [OUT] Result of the operation. */
	int32_t error_code; /*!< [OUT] Error code returned in case of error. */
	char* error_message; /*!< [OUT] Error message related to the error code. */
} ECOM_NETWORK_netif_context_t;

/**
 * @brief Data structure for set/get DNS operations.
 *
 * This structure is used by <code>LLECOM_NETWORK_IMPL_setDNS</code>
 * and <code>LLECOM_NETWORK_IMPL_getDNS</code>.
 *
 * Fields defined in this structure correspond to the parameters of these functions and
 * <code>result</code> field corresponds to the value returned by them.
 */
typedef struct {
	int8_t* netifName; /*!< [IN] The buffer that store the interface name. */
	int32_t netifNameOffset; /*!< [IN] The offset where the interface name is stored. */
	int32_t netifNameLength; /*!< [IN] The interface name buffer length. */
	int8_t* address; /*!< [IN/OUT] The buffer that store IP address to be set (input) or the buffer where the address will be get (output). */
	int32_t addressOffset; /*!< [IN/OUT] The offset where the IP address to be set is stored (input) or the offset where the address will be get (output). */
	int32_t addressLength; /*!< [IN/OUT] The IP address buffer length (input) or the address buffer length (output). */
	int32_t index; /*!< [IN/OUT] The index of the DNS (input) or the index of the DNS server (from 0 to {@link #getDNSCount()} -1) (output). */
	int32_t result; /*!< [OUT] Result of the operation. */
	int32_t error_code; /*!< [OUT] Error code returned in case of error. */
	char* error_message; /*!< [OUT] Error message related to the error code. */
} ECOM_NETWORK_dns_context_t;

/**
 * @brief Data structure for get Gateway or Netmask operations.
 *
 * This structure is used by <code>LLECOM_NETWORK_IMPL_getGateway</code>,
 * <code>LLECOM_NETWORK_IMPL_setGateway</code>, <code>LLECOM_NETWORK_IMPL_getIP</code>,
 * <code>LLECOM_NETWORK_IMPL_setIP</code>, <code>LLECOM_NETWORK_IMPL_getNetmask</code>
 * and <code>LLECOM_NETWORK_IMPL_setNetmask</code>.
 *
 * Fields defined in this structure correspond to the parameters of these functions and
 * <code>result</code> field corresponds to the value returned by them.
 */
typedef struct {
	int8_t* netifName; /*!< [IN] The buffer that store the interface name. */
	int32_t netifNameOffset; /*!< [IN] The offset where the interface name is stored. */
	int32_t netifNameLength; /*!< [IN] The interface name buffer length. */
	int8_t* address; /*!< [IN/OUT] The buffer that store IP address to be set (input) or the buffer where the address will be get (output). */
	int32_t addressOffset; /*!< [IN/OUT] The offset where the IP address to be set is stored (input) or the offset where the address will be get (output). */
	int32_t addressLength; /*!< [IN/OUT] The IP address buffer length (input) or the address buffer length (output). */
	int32_t result; /*!< [OUT] Result of the operation. */
	int32_t error_code; /*!< [OUT] Error code returned in case of error. */
	char* error_message; /*!< [OUT] Error message related to the error code. */
} ECOM_NETWORK_ip_context_t;

/**
 * @brief Data structure for use DHCP and static DNS operations.
 *
 * This structure is used by <code>LLECOM_NETWORK_IMPL_useDHCP</code>
 * and <code>LLECOM_NETWORK_IMPL_useStaticDNS</code>.
 *
 * Fields defined in this structure correspond to the parameters of these functions and
 * <code>result</code> field corresponds to the value returned by them.
 */
typedef struct {
	int8_t* netifName; /*!< [IN] The buffer that store the interface name. */
	int32_t netifNameOffset; /*!< [IN] The offset where the interface name is stored. */
	int32_t netifNameLength; /*!< [IN] The interface name buffer length. */
	int8_t use; /*!< [IN] True to use DHCP / static DNS, false otherwise. */
	int32_t result; /*!< [OUT] Result of the operation. */
	int32_t error_code; /*!< [OUT] Error code returned in case of error. */
	char* error_message; /*!< [OUT] Error message related to the error code. */
} ECOM_NETWORK_ip_config_t;

/**
 * @union ECOM_NETWORK_worker_param_t
 */
typedef union {
	ECOM_NETWORK_netif_context_t start_context;
	ECOM_NETWORK_dns_context_t set_get_DNS;
	ECOM_NETWORK_ip_context_t ip_context;
	ECOM_NETWORK_ip_config_t use_dhcp_dns;
} ECOM_NETWORK_worker_param_t;

void LLECOM_NETWORK_IMPL_enable_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_NETWORK_IMPL_disable_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_NETWORK_IMPL_isEnabled_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_NETWORK_IMPL_start_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_NETWORK_IMPL_stop_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_NETWORK_IMPL_isStarted_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_NETWORK_IMPL_getDNSCount_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_NETWORK_IMPL_getDNS_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_NETWORK_IMPL_getGateway_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_NETWORK_IMPL_getNetmask_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_NETWORK_IMPL_getIP_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_NETWORK_IMPL_isDNSStatic_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_NETWORK_IMPL_isStatic_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_NETWORK_IMPL_startConfiguration_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_NETWORK_IMPL_endConfiguration_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_NETWORK_IMPL_setDNS_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_NETWORK_IMPL_setGateway_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_NETWORK_IMPL_setIP_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_NETWORK_IMPL_setNetmask_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_NETWORK_IMPL_useDHCP_action(MICROEJ_ASYNC_WORKER_job_t* job);
void LLECOM_NETWORK_IMPL_useStaticDNS_action(MICROEJ_ASYNC_WORKER_job_t* job);

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

#endif /* ECOM_NETWORK_HELPER_H */
