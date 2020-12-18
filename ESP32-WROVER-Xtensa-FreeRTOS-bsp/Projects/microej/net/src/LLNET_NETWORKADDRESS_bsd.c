/*
 * C
 *
 * Copyright 2014-2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */

/**
 * @file
 * @brief LLNET_NETWORKADDRESS 2.1.0 implementation over BSD-like API.
 * @author MicroEJ Developer Team
 * @version 0.2.0
 * @date 21 December 2017
 */

#include <LLNET_NETWORKADDRESS_impl.h>

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "LLNET_CONSTANTS.h"
#include "LLNET_ERRORS.h"
#include "LLNET_Common.h"

#ifdef __cplusplus
	extern "C" {
#endif

#define LOCALHOST_NAME ("localhost")

int32_t LLNET_NETWORKADDRESS_IMPL_getLocalHostnameNative(int8_t* result, int32_t length, uint8_t retry)
{
	LLNET_DEBUG_TRACE("%s\n", __func__);

	strlcpy((char*)result, LOCALHOST_NAME, length);

	return 0;
}

int32_t LLNET_NETWORKADDRESS_IMPL_lookupInaddrAny(int8_t* result, int32_t length, uint8_t retry)
{
	LLNET_DEBUG_TRACE("%s\n", __func__);
#if ADDRESS_FAMILY == AF_INET
	if(length < sizeof(struct in_addr)){
		return J_EINVAL;
	}
	*((in_addr_t*)result) = llnet_htonl(INADDR_ANY);

	return sizeof(in_addr_t);

#else  // ADDRESS_FAMILY == AF_INET6
	if(length < sizeof(struct in6_addr)){
		return J_EINVAL;
	}
	*((struct in6_addr*)result) = in6addr_any;

	return sizeof(struct in6_addr);
#endif // ADDRESS_FAMILY == AF_INET
}

int32_t LLNET_NETWORKADDRESS_IMPL_loopbackAddress(int8_t* result, int32_t length, uint8_t retry){
	LLNET_DEBUG_TRACE("%s\n", __func__);
#if ADDRESS_FAMILY == AF_INET

	if(length < sizeof(struct in_addr)){
		return J_EINVAL;
	}
	*((in_addr_t*)result) = llnet_htonl(INADDR_LOOPBACK);

	return sizeof(in_addr_t);

#else  // ADDRESS_FAMILY == AF_INET6
	if(length < sizeof(struct in6_addr)){
		return J_EINVAL;
	}
	*((struct in6_addr*)result) = in6addr_loopback;

	return sizeof(struct in6_addr);
#endif // ADDRESS_FAMILY == AF_INET
}

#ifdef __cplusplus
	}
#endif
