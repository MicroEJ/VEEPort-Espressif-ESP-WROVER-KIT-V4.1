/*
 * C
 *
 * Copyright 2014-2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */

/**
 * @file
 * @brief LLNET_DATAGRAMSOCKETCHANNEL 2.1.0 implementation over BSD-like API.
 * @author MicroEJ Developer Team
 * @version 0.2.0
 * @date 21 December 2017
 */

#include <LLNET_DATAGRAMSOCKETCHANNEL_impl.h>

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "LLNET_CONSTANTS.h"
#include "LLNET_ERRORS.h"
#include "LLNET_Common.h"

#ifdef __cplusplus
	extern "C" {
#endif

int64_t DatagramSocketChannel_recvfrom(int32_t fd, int8_t* dst, int32_t dstOffset, int32_t dstLength, int8_t* hostPort, int32_t hostPortLength){
	LLNET_DEBUG_TRACE("%s(fd=0x%X, dstLength=%d, hostPortLength=%d)\n", __func__, fd, dstLength, hostPortLength);
#if ADDRESS_FAMILY == AF_INET
	struct sockaddr_in sockaddr = {0};
#else
	struct sockaddr_in6 sockaddr = {0};
#endif
	int32_t addrLen = sizeof(sockaddr);
	int32_t flags = MSG_WAITALL;
	int32_t ret = llnet_recvfrom(fd, dst+dstOffset, dstLength, flags, (struct sockaddr*)&sockaddr, (socklen_t *)&addrLen);

	if (ret == -1) {
		return map_to_java_exception(llnet_errno(fd));
	}
#if ADDRESS_FAMILY == AF_INET
	if (sockaddr.sin_family == AF_INET) {
		if(hostPortLength < (sizeof(in_addr_t)+sizeof(int32_t))){
			return J_EINVAL;
		}
		// push host address in result bufffer
		*((in_addr_t*)hostPort) = sockaddr.sin_addr.s_addr;
		hostPort += sizeof(in_addr_t);
		// push host port in result buffer
		*((int32_t*)hostPort) = llnet_ntohs(sockaddr.sin_port);
		// add data receive length in return value
		int64_t retValue = (((int64_t)ret) << 32l);
		// add host length in return value
		retValue |= sizeof(in_addr_t);
		return retValue;
	}
#else // ADDRESS_FAMILY == AF_INET6
	if (sockaddr.sin6_family == AF_INET6) {
		if(hostPortLength < (sizeof(struct in6_addr)+sizeof(int32_t))){
			return J_EINVAL;
		}
		// push host address in result bufffer
		memcpy(hostPort, (void*)&sockaddr.sin6_addr, sizeof(struct in6_addr));
		// push host port in result buffer
		hostPort +=  sizeof(struct in6_addr);
		*((int32_t*)hostPort) = llnet_ntohs(sockaddr.sin6_port);
		// add data receive length in return value
		int64_t retValue = (((int64_t)ret) << 32l);
		// add host length in return value
		retValue |= sizeof(struct in6_addr);
		return retValue;
	}
#endif // ADDRESS_FAMILY == AF_INET
	if(ret == 0){
		return 0;
	}
	//unsupported address family
	return J_EAFNOSUPPORT;
}

int32_t DatagramSocketChannel_sendto(int32_t fd, int8_t* src, int32_t srcoffset, int32_t srclength, int8_t* addr, int32_t addrlength, int32_t port){
	LLNET_DEBUG_TRACE("%s(fd=0x%X)\n", __func__, fd);
#if ADDRESS_FAMILY == AF_INET
	struct sockaddr_in sockaddr = {0};
	if(addrlength < sizeof(in_addr_t)){
		return J_EINVAL;
	}
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = *((in_addr_t*)addr);
	sockaddr.sin_port = llnet_htons(port);
#else
	struct sockaddr_in6 sockaddr = {0};
	if(addrlength < sizeof(struct in6_addr)){
		return J_EINVAL;
	}
	sockaddr.sin6_family = AF_INET6;
	memcpy((void*)&sockaddr.sin6_addr, addr, sizeof(struct in6_addr));
	sockaddr.sin6_port = llnet_htons(port);
#endif //ADDRESS_FAMILY == AF_INET
	int32_t flags = 0;
	int32_t ret = llnet_sendto(fd, src+srcoffset, srclength, flags,(struct sockaddr*)&sockaddr, sizeof(sockaddr));

	LLNET_DEBUG_TRACE("%s(fd=0x%X) sendto result=%d errno=%d\n", __func__, fd, ret, llnet_errno(fd));

	if(ret == -1){
		if(llnet_errno(fd) == EAGAIN || llnet_errno(fd) == EWOULDBLOCK){
			return 0;
		}
		return map_to_java_exception(llnet_errno(fd));
	}
	return ret;
}

int64_t LLNET_DATAGRAMSOCKETCHANNEL_IMPL_receive(int32_t fd, int8_t* dst, int32_t dstOffset, int32_t dstLength, int8_t* hostPort, int32_t hostPortLength, uint8_t retry)
{
	LLNET_DEBUG_TRACE("%s(fd=0x%X, retry=%d)\n", __func__, fd, retry);

    if(llnet_is_ready() == false){
        return J_NETWORK_NOT_INITIALIZED;
    }

	// first, a select with zero timeout is done to check if the socket is readable.
	// The select call is non blocking when the given timeout is equals to zero.
	// On success, the socket is ready to read data. A blocking call to recvfrom function
	// can be done. It will not block the socket.
	int32_t selectRes = non_blocking_select(fd, SELECT_READ);

	if(selectRes == 0){
		return asyncOperation(fd, SELECT_READ, retry);
	}else{
		return DatagramSocketChannel_recvfrom(fd, dst, dstOffset, dstLength, hostPort, hostPortLength);
	}

}

int32_t LLNET_DATAGRAMSOCKETCHANNEL_IMPL_send(int32_t fd, int8_t* src, int32_t srcoffset, int32_t srclength, int8_t* addr, int32_t addrlength, int32_t port, uint8_t retry)
{
	LLNET_DEBUG_TRACE("%s(fd=0x%X, retry=%d)\n", __func__, fd, retry);

    if(llnet_is_ready() == false){
        return J_NETWORK_NOT_INITIALIZED;
    }

	// first, a select with zero timeout is done to check if the socket is writable.
	// The select call is non blocking when the given timeout is equals to zero.
	// On success, the socket is ready to write data. A blocking call to sendto function
	// can be done. It will not block the socket.
	int32_t selectRes = non_blocking_select(fd, SELECT_WRITE);

	if(selectRes == 0){
		return asyncOperation(fd, SELECT_WRITE, retry);
	}else{
		return DatagramSocketChannel_sendto(fd, src, srcoffset, srclength, addr, addrlength, port);
	}

}

int32_t LLNET_DATAGRAMSOCKETCHANNEL_IMPL_disconnect(int32_t fd, uint8_t retry)
{
	LLNET_DEBUG_TRACE("%s(fd=0x%X)\n ", __func__, fd);

    if(llnet_is_ready() == false){
        return J_NETWORK_NOT_INITIALIZED;
    }

	struct sockaddr sockaddr = {0};
	sockaddr.sa_family = AF_UNSPEC;
	if(llnet_connect(fd, &sockaddr, sizeof(struct sockaddr)) == -1) {
		return map_to_java_exception(llnet_errno(fd));
	}
	return 0; //success
}

#ifdef __cplusplus
	}
#endif
