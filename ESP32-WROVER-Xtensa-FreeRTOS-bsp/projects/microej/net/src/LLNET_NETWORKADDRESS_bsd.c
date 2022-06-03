/*
 * C
 *
 * Copyright 2014-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief LLNET_NETWORKADDRESS 2.1.0 implementation over BSD-like API.
 * @author MicroEJ Developer Team
 * @version 1.3.1
 * @date 20 April 2021
 */

#include <LLNET_NETWORKADDRESS_impl.h>

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
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

	(void)retry;

	jint ret;
	ret = llnet_gethostname(result, length);

	if(ret == 0){
		// From gethostname() man:
		// * If the null-terminated hostname is too large to fit, then the name is truncated, and no  error
		// * is  returned  (but  see  NOTES  below).  POSIX.1-2001 says that if such
		// * truncation occurs, then it is unspecified whether the  returned  buffer
		// * includes a terminating null byte.
		//
		// To avoid any issue, we set to 0 the last char.
		result[length-1] = 0;
	}else{
		strncpy((char*)result, LOCALHOST_NAME, length);

		// From strncpy() man:
		// * Warning: If there is no null byte among the first n bytes
	    // * of src, the string placed in dest will not be null terminated.
		//
		// To avoid any issue, we set to 0 the last char.
		result[length-1] = 0;
	}
	return 0;
}

int32_t LLNET_NETWORKADDRESS_IMPL_lookupInaddrAny(int8_t* result, int32_t length, uint8_t retry)
{
	LLNET_DEBUG_TRACE("%s\n", __func__);

	(void)retry;

// If IPv6 or IPv4+IPv6 configuration, then use IPv6. Otherwise (only IPv4 configuration) use IPv4.
#if LLNET_AF & LLNET_AF_IPV6
	if((uint32_t)length >= sizeof(struct in6_addr)){
		*((struct in6_addr*)result) = in6addr_any;
		return sizeof(struct in6_addr);
	}
#else // only IPv4
	if((uint32_t)length >= sizeof(struct in_addr)){
		*((in_addr_t*)result) = llnet_htonl(INADDR_ANY);
		return sizeof(in_addr_t);
	}
#endif

	return J_EINVAL;
}

int32_t LLNET_NETWORKADDRESS_IMPL_loopbackAddress(int8_t* result, int32_t length, uint8_t retry){
	LLNET_DEBUG_TRACE("%s\n", __func__);

	(void)retry;

// If IPv6 or IPv4+IPv6 configuration, then use IPv6. Otherwise (only IPv4 configuration) use IPv4.
#if LLNET_AF & LLNET_AF_IPV6
	if((uint32_t)length >= sizeof(struct in6_addr)){
		*((struct in6_addr*)result) = in6addr_loopback;
		return sizeof(struct in6_addr);
	}
#else // only IPv4
	if((uint32_t)length >= sizeof(struct in_addr)){
		*((in_addr_t*)result) = llnet_htonl(INADDR_LOOPBACK);
		return sizeof(in_addr_t);
	}
#endif

	return J_EINVAL;
}

#ifdef __cplusplus
	}
#endif
