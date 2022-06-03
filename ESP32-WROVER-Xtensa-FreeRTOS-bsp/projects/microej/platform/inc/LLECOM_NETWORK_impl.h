/*
 * C
 *
 * Copyright 2016-2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */

/**
 * @file
 * @brief MicroEJ ECOM-NETWORK low level API
 * @author MicroEJ Developer Team
 * @version 1.1.0
 * @date 23 November 2020
 */

#ifndef _LLECOM_NETWORK_IMPL
#define _LLECOM_NETWORK_IMPL

#include <stdint.h>
#include <intern/LLECOM_NETWORK_impl.h>

#ifdef __cplusplus
extern "C" {
#endif


/** @brief list of Network Interface states constants */
#define IF_STATE_NOT_ENABLED  		(0)
#define IF_STATE_ENABLED		 	(1)
#define IF_STATE_NOT_STARTED		(2)
#define IF_STATE_STARTED			(3)

/** @brief list of Network Interface DNS configuration constants */
#define IF_DNS_NOT_STATIC			(0)
#define IF_DNS_STATIC				(1)

/** @brief list of Network Interface IP configuration constants */
#define IF_IP_STATIC				(0)
#define IF_IP_DYNAMIC				(1)


/**
 * Disable an interface.
 *
 * @param netifName
 *            the buffer that store interface name
 * @param netifNameOffset
 *            the offset where the interface name is stored
 * @param netifNameLength
 *            the interface name buffer length
 * @param getResult
 *            false to post the request, true to get the result
 * @return 0 on success or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_NETWORK_IMPL_disable(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t getResult);

/**
 * Enable an interface.
 *
 * @param netifName
 *            the buffer that store interface name
 * @param netifNameOffset
 *            the offset where the interface name is stored
 * @param netifNameLength
 *            the interface name buffer length
 * @param getResult
 *            false to post the request, true to get the result
 * @return 0 on success or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_NETWORK_IMPL_enable(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t getResult);

/**
 * Tell whether an interface is enabled or not.
 *
 * @param netifName
 *            the buffer that store interface name
 * @param netifNameOffset
 *            the offset where the interface name is stored
 * @param netifNameLength
 *            the interface name buffer length
 * @param getResult
 *            false to post the request, true to get the result
 * @return {@link #IF_STATE_ENABLED} if enabled, {@link #IF_STATE_NOT_ENABLED} if not, or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_NETWORK_IMPL_isEnabled(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t getResult);

/**
 * Start an interface.
 *
 * @param netifName
 *            the buffer that store interface name
 * @param netifNameOffset
 *            the offset where the interface name is stored
 * @param netifNameLength
 *            the interface name buffer length
 * @param getResult
 *            false to post the request, true to get the result
 * @return 0 on success or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_NETWORK_IMPL_start(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t getResult);

/**
 * Stop an interface.
 *
 * @param netifName
 *            the buffer that store interface name
 * @param netifNameOffset
 *            the offset where the interface name is stored
 * @param netifNameLength
 *            the interface name buffer length
 * @param getResult
 *            false to post the request, true to get the result
 * @return 0 on success or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_NETWORK_IMPL_stop(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t getResult);

/**
 * Tell whether an interface is started or not.
 *
 * @param netifName
 *            the buffer that store interface name
 * @param netifNameOffset
 *            the offset where the interface name is stored
 * @param netifNameLength
 *            the interface name buffer length
 * @param getResult
 *            false to post the request, true to get the result
 * @return {@link #IF_STATE_STARTED} if started, {@link #IF_STATE_NOT_STARTED}, or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_NETWORK_IMPL_isStarted(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t getResult);

/**
 * Get the number of available DNS servers.
 *
 * @param netifName
 *            the buffer that store interface name
 * @param netifNameOffset
 *            the offset where the interface name is stored
 * @param netifNameLength
 *            the interface name buffer length
 * @param getResult
 *            false to post the request, true to get the result
 * @return a positive integer representing the number of DNS or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_NETWORK_IMPL_getDNSCount(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t getResult);

/**
 * Get the address of the n-th DNS server.
 *
 * @param netifName
 *            the buffer that store interface name
 * @param netifNameOffset
 *            the offset where the interface name is stored
 * @param netifNameLength
 *            the interface name buffer length
 * @param address
 *            the buffer where the address will be stored
 * @param addressOffset
 *            the offset where the address will be stored
 * @param addressLength
 *            the address buffer length
 * @param index
 *            the index of the DNS server (from 0 to {@link #getDNSCount()} -1)
 * @param getResult
 *            false to post the request, true to get the result
 * @return 0 on success or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_NETWORK_IMPL_getDNS(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t* address, int32_t addressOffset, int32_t addressLength, int32_t index, int8_t getResult);

/**
 * Get the gateway.
 *
 * @param netifName
 *            the buffer that store interface name
 * @param netifNameOffset
 *            the offset where the interface name is stored
 * @param netifNameLength
 *            the interface name buffer length
 * @param address
 *            the buffer where the address will be stored
 * @param addressOffset
 *            the offset where the address will be stored
 * @param addressLength
 *            the address buffer length
 * @param getResult
 *            false to post the request, true to get the result
 * @return 0 on success or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_NETWORK_IMPL_getGateway(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t* address, int32_t addressOffset, int32_t addressLength, int8_t getResult);

/**
 * Get the netmask.
 *
 * @param netifName
 *            the buffer that store interface name
 * @param netifNameOffset
 *            the offset where the interface name is stored
 * @param netifNameLength
 *            the interface name buffer length
 * @param address
 *            the buffer where the address will be stored
 * @param addressOffset
 *            the offset where the address will be stored
 * @param addressLength
 *            the address buffer length
 * @param getResult
 *            false to post the request, true to get the result
 * @return 0 on success or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_NETWORK_IMPL_getNetmask(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t* address, int32_t addressOffset, int32_t addressLength, int8_t getResult);

/**
 * Get whether DNS is statically configured or given by DHCP.
 *
 * @param netifName
 *            the buffer that store interface name
 * @param netifNameOffset
 *            the offset where the interface name is stored
 * @param netifNameLength
 *            the interface name buffer length
 * @param getResult
 *            false to post the request, true to get the result
 * @return {@link #IF_DNS_STATIC} if true, {@link #IF_DNS_NOT_STATIC} if false, or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_NETWORK_IMPL_isDNSStatic(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t getResult);

/**
 * Get whether IP configuration is static or not.
 *
 * @param netifName
 *            the buffer that store interface name
 * @param netifNameOffset
 *            the offset where the interface name is stored
 * @param netifNameLength
 *            the interface name buffer length
 * @param getResult
 *            false to post the request, true to get the result
 * @return {@link #IF_IP_STATIC} if true, {@link #IF_IP_DYNAMIC} if provided by DHCP or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_NETWORK_IMPL_isStatic(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t getResult);

/**
 * In order to avoid a lot of immortal buffer, parameters are sent one by one to the native world. This method is
 * here to apply the previously set parameters. Some modules need to reboot to apply the IP configuration; some
 * others can apply on the fly and won't do anything in this method.
 *
 * @param netifName
 *            the buffer that store interface name
 * @param netifNameOffset
 *            the offset where the interface name is stored
 * @param netifNameLength
 *            the interface name buffer length
 * @param getResult
 *            false to post the request, true to get the result
 * @return 0 on success or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_NETWORK_IMPL_endConfiguration(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t getResult);

/**
 * In order to avoid a lot of immortal buffer, parameters are sent one by one to the native world. This method is
 * here to notify the underlying module that the configuration process will begin. The module may then enter
 * configuration mode.
 *
 * @param netifName
 *            the buffer that store interface name
 * @param netifNameOffset
 *            the offset where the interface name is stored
 * @param netifNameLength
 *            the interface name buffer length
 * @param getResult
 *            false to post the request, true to get the result
 * @return 0 on success or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_NETWORK_IMPL_startConfiguration(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t getResult);

/**
 * Set a DNS server. Since an {@link IPConfiguration} contains a list of DNS servers, we need to set them one by one
 * so that we have only one immortal buffer. Several calls to this native method are made with a index (from 0 to
 * n-1) to identify the DNS server in the list.
 *
 * @param netifName
 *            the buffer that store interface name
 * @param netifNameOffset
 *            the offset where the interface name is stored
 * @param netifNameLength
 *            the interface name buffer length
 * @param address
 *            the buffer that store IP address
 * @param addressOffset
 *            the offset where the IP address is stored
 * @param addressLength
 *            the IP address buffer length
 * @param index
 *            the index of the DNS
 * @param getResult
 *            false to post the request, true to get the result
 * @return 0 on success or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_NETWORK_IMPL_setDNS(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t* address, int32_t addressOffset, int32_t addressLength, int32_t index, int8_t getResult);

/**
 * Set the gateway.
 *
 * @param netifName
 *            the buffer that store interface name
 * @param netifNameOffset
 *            the offset where the interface name is stored
 * @param netifNameLength
 *            the interface name buffer length
 * @param address
 *            the buffer that store IP address
 * @param addressOffset
 *            the offset where the IP address will be stored
 * @param addressLength
 *            the IP address buffer length
 * @param getResult
 *            false to post the request, true to get the result
 * @return 0 on success or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_NETWORK_IMPL_setGateway(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t* address, int32_t addressOffset, int32_t addressLength, int8_t getResult);

/**
 * Set the IP.
 *
 * @param netifName
 *            the buffer that store interface name
 * @param netifNameOffset
 *            the offset where the interface name is stored
 * @param netifNameLength
 *            the interface name buffer length
 * @param address
 *            the buffer that store IP address
 * @param addressOffset
 *            the offset where the IP address will be stored
 * @param addressLength
 *            the IP address buffer length
 * @param getResult
 *            false to post the request, true to get the result
 * @return 0 on success or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_NETWORK_IMPL_setIP(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t* address, int32_t addressOffset, int32_t addressLength, int8_t getResult);

/**
 * Set the netmask.
 *
 * @param netifName
 *            the buffer that store interface name
 * @param netifNameOffset
 *            the offset where the interface name is stored
 * @param netifNameLength
 *            the interface name buffer length
 * @param address
 *            the buffer that store IP address
 * @param addressOffset
 *            the offset where the IP address will be stored
 * @param addressLength
 *            the IP address buffer length
 * @param getResult
 *            false to post the request, true to get the result
 * @return 0 on success or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_NETWORK_IMPL_setNetmask(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t* address, int32_t addressOffset, int32_t addressLength, int8_t getResult);

/**
 * Ask native to use DHCP.
 *
 * @param netifName
 *            the buffer that store interface name
 * @param netifNameOffset
 *            the offset where the interface name is stored
 * @param netifNameLength
 *            the interface name buffer length
 * @param use
 *            true to use DHCP, false otherwise
 * @param getResult
 *            false to post the request, true to get the result
 * @return 0 on success or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_NETWORK_IMPL_useDHCP(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t use, int8_t getResult);

/**
 * Ask native to use static DNS.
 *
 * @param netifName
 *            the buffer that store interface name
 * @param netifNameOffset
 *            the offset where the interface name is stored
 * @param netifNameLength
 *            the interface name buffer length
 * @param use
 *            true to use static DNS, false otherwise
 * @param getResult
 *            false to post the request, true to get the result
 * @return 0 on success or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_NETWORK_IMPL_useStaticDNS(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t use, int8_t getResult);

/**
 * Set the default network interface.
 *
 * @param netifName
 *            the buffer that store interface name
 * @param netifNameOffset
 *            the offset where the interface name is stored
 * @param netifNameLength
 *            the interface name buffer length
 * @param getResult
 *            false to post the request, true to get the result
 * @return 0 on success or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLECOM_NETWORK_IMPL_setDefault(int8_t* netifName, int32_t netifNameOffset, int32_t netifNameLength, int8_t getResult);

#ifdef __cplusplus
}
#endif
#endif // _LLECOM_NETWORK_IMPL
