/*
 * C
 *
 * Copyright 2014-2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */

/**
 * @file
 * @brief LLNET_MULTICASTSOCKETCHANNEL 2.1.0 implementation over BSD-like API.
 * @author MicroEJ Developer Team
 * @version 0.2.0
 * @date 21 December 2017
 */

#include <LLNET_MULTICASTSOCKETCHANNEL_impl.h>

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "LLNET_CONSTANTS.h"
#include "LLNET_ERRORS.h"
#include "LLNET_Common.h"
#include "LLNET_CHANNEL_impl.h"

#ifdef __cplusplus
	extern "C" {
#endif

int32_t LLNET_MULTICASTSOCKETCHANNEL_IMPL_getTimeToLive(int32_t fd, uint8_t retry)
{
	LLNET_DEBUG_TRACE("%s(fd=0x%X, ..., retry=%d)\n", __func__, fd, retry);
	return LLNET_CHANNEL_IMPL_getOption(fd, IP_MULTICAST_TTL, 0);
}

int32_t LLNET_MULTICASTSOCKETCHANNEL_IMPL_setTimeToLive(int32_t fd, int32_t ttl, uint8_t retry)
{
	LLNET_DEBUG_TRACE("%s(fd=0x%X, ..., retry=%d)\n", __func__, fd, retry);
	return LLNET_CHANNEL_IMPL_setOption(fd, IP_MULTICAST_TTL, ttl, 0);
}

int32_t LLNET_MULTICASTSOCKETCHANNEL_IMPL_joinOrLeave(int32_t fd, uint8_t join, int8_t* mcastAddr, int32_t mcastAddrLength, int8_t* netIfAddr, int32_t netIfAddrLength, uint8_t retry)
{
	LLNET_DEBUG_TRACE("%s(fd=0x%X, ..., retry=%d)\n", __func__, fd, retry);
#if ADDRESS_FAMILY == AF_INET
	struct ip_mreq optval;
	*((int32_t*)(&optval.imr_multiaddr.s_addr)) = *((int32_t*)(mcastAddr));
	*((int32_t*)(&optval.imr_interface.s_addr)) = *((int32_t*)(netIfAddr));
	if (llnet_setsockopt(fd, IPPROTO_IP, join ? IP_ADD_MEMBERSHIP : IP_DROP_MEMBERSHIP, &optval, sizeof(optval)) == -1)
	{
		LLNET_DEBUG_TRACE("%s(%d): errno=%d\n", __func__, __LINE__, llnet_errno(fd));
		return map_to_java_exception(llnet_errno(fd));
	}
	return 0;
#else
	return J_NET_NATIVE_NOT_IMPLEMENTED;
#endif //ADDRESS_FAMILY == AF_INET
}
#ifdef __cplusplus
	}
#endif
