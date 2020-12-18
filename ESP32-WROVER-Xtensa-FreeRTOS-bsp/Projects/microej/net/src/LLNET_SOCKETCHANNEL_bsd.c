/*
 * C
 *
 * Copyright 2014-2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */

/**
 * @file
 * @brief LLNET_SOCKETCHANNEL 2.1.0 implementation over BSD-like API.
 * @author MicroEJ Developer Team
 * @version 0.2.0
 * @date 21 December 2017
 */

#include <LLNET_SOCKETCHANNEL_impl.h>

#include <stdio.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include "LLNET_CONSTANTS.h"
#include "LLNET_NETWORKADDRESS_impl.h"
#include "LLNET_ERRORS.h"
#include "LLNET_Common.h"

#ifdef __cplusplus
	extern "C" {
#endif

// external function used to retrieve currentTime (same as MicroJvm)
static int32_t SocketChanel_Address(int32_t fd, int8_t* name, int32_t nameLength, uint8_t localAddress);

static int32_t SocketChanel_Port(int32_t fd, uint8_t localPort);

int32_t LLNET_SOCKETCHANNEL_IMPL_connect(int32_t fd, int8_t* addr, int32_t length, int32_t port, int32_t timeout, uint8_t retry)
{
	LLNET_DEBUG_TRACE("%s[thread %d](fd=0x%X, port=%d, timeout=%d)\n", __func__, SNI_getCurrentJavaThreadID(),fd, port, timeout);

    if(llnet_is_ready() == false){
        return J_NETWORK_NOT_INITIALIZED;
    }

	int32_t res;

	if(retry){
		//check if the socket is connected
#if ADDRESS_FAMILY == AF_INET
		struct sockaddr_in sockaddr = {0};
#else
		struct sockaddr_in6 sockaddr = {0};
#endif
		uint32_t addrlen = sizeof(sockaddr);
		int32_t selectRes = non_blocking_select(fd, SELECT_WRITE);
		int32_t peerRes = llnet_getpeername(fd, (struct sockaddr *)&sockaddr, (socklen_t*)&addrlen);
		int32_t socket_port;
#if ADDRESS_FAMILY == AF_INET
		socket_port = llnet_ntohs(sockaddr.sin_port);
#else
		socket_port = llnet_ntohs(sockaddr.sin6_port);
#endif
		if(selectRes > 0 && (peerRes == 0 && socket_port > 0)){
			//the socket is connected to a remote host
			return 1; //success
		}
		if(selectRes == 0 || (peerRes < 0 && llnet_errno(fd) == ENOTCONN)){
			//the socket is not connected or
			//a timeout expires during the first connect
			return J_ENOTCONN;
		}
		//other error
		return map_to_java_exception(llnet_errno(fd));
	}
	else { // retry == false

#if ADDRESS_FAMILY == AF_INET
		struct sockaddr_in addr_remote = {0};
		if(length < sizeof(in_addr_t)){
			return J_EINVAL;
		}
		addr_remote.sin_family = AF_INET;
		addr_remote.sin_port = llnet_htons(port);
		addr_remote.sin_addr.s_addr = *((in_addr_t*)addr);
#else
		struct sockaddr_in6 addr_remote = {0};
		if(length < sizeof(struct in6_addr)){
			return J_EINVAL;
		}
		addr_remote.sin6_family = AF_INET6;
		addr_remote.sin6_port = llnet_htons(port);
		memcpy((void*)&addr_remote.sin6_addr, addr, sizeof(struct in6_addr));
#endif

		// Set nonblocking mode.
		bool was_non_blocking = is_socket_non_blocking(fd);
		if(was_non_blocking == false){
			res = set_socket_non_blocking(fd, true);
			if (res != 0) {
				return map_to_java_exception(llnet_errno(fd));
			}
		}
	
		int32_t connectRes = llnet_connect(fd, (struct sockaddr*)&addr_remote, sizeof(addr_remote));
		int32_t connectErrno = llnet_errno(fd);

		// Set blocking mode.
		if(was_non_blocking == false){
			// The socket was blocking initially.
			res = set_socket_non_blocking(fd, false);
			if (res != 0) {
				return map_to_java_exception(llnet_errno(fd));
			}
		}

		if (connectRes == -1) {
			if(connectErrno == EINPROGRESS) {
				//The connection can not be completed immediately.

				if(was_non_blocking == true){
					// the socket is in non-blocking mode and the connection did not immediately succeed
					return 0;
				}

				// A blocking request on write operation is added in the dispatch event queue for completion.
				int32_t asyncSelectRes = async_select(fd, SELECT_WRITE, timeout, NULL);
				if(asyncSelectRes == 0){
					// request added in the queue
					LLNET_DEBUG_TRACE("Connect has blocked without result\n");
					return J_NET_NATIVE_CODE_BLOCKED_WITHOUT_RESULT;
				}
				// requests queue limit reached
				LLNET_DEBUG_TRACE("Blocking Request Queue Limit Reached\n");
				return J_ASYNC_BLOCKING_REQUEST_QUEUE_LIMIT_REACHED;
			}
			else {
				LLNET_DEBUG_TRACE("[ERROR] Connection failed (retry=%d, errno=%d)\n", retry, connectErrno);
				return map_to_java_exception(connectErrno);
			}
		}
		else {
			return 1;// connected
		}
	}
}

int32_t LLNET_SOCKETCHANNEL_IMPL_getLocalPort(int32_t fd, uint8_t retry)
{
	LLNET_DEBUG_TRACE("%s[thread %d](fd=0x%X)\n", __func__, SNI_getCurrentJavaThreadID(), fd);

    if(llnet_is_ready() == false){
        return J_NETWORK_NOT_INITIALIZED;
    }

	return SocketChanel_Port(fd, JTRUE);
}

int32_t LLNET_SOCKETCHANNEL_IMPL_getLocalAddress(int32_t fd, int8_t* name, int32_t length, uint8_t retry)
{
	LLNET_DEBUG_TRACE("%s[thread %d](fd=0x%X)\n", __func__, SNI_getCurrentJavaThreadID(), fd);

    if(llnet_is_ready() == false){
        return J_NETWORK_NOT_INITIALIZED;
    }

	return SocketChanel_Address(fd, name, length, JTRUE);
}

int32_t LLNET_SOCKETCHANNEL_IMPL_getPeerPort(int32_t fd, uint8_t retry)
{
	LLNET_DEBUG_TRACE("%s[thread %d](fd=0x%X)\n", __func__, SNI_getCurrentJavaThreadID(), fd);

    if(llnet_is_ready() == false){
        return J_NETWORK_NOT_INITIALIZED;
    }

	return SocketChanel_Port(fd, JFALSE);
}

int32_t LLNET_SOCKETCHANNEL_IMPL_getPeerAddress(int32_t fd, int8_t* name, int32_t length, uint8_t retry)
{
	LLNET_DEBUG_TRACE("%s[thread %d](fd=0x%X)\n", __func__, SNI_getCurrentJavaThreadID(), fd);

    if(llnet_is_ready() == false){
        return J_NETWORK_NOT_INITIALIZED;
    }

	return SocketChanel_Address(fd, name, length, JFALSE);
}


static int32_t SocketChanel_Port(int32_t fd, uint8_t localPort){
#if ADDRESS_FAMILY == AF_INET
	struct sockaddr_in sockaddr = {0};
#else
	struct sockaddr_in6 sockaddr = {0};
#endif
	uint32_t addrlen = sizeof(sockaddr);
	int32_t ret = -1;
	if(localPort){
		ret = llnet_getsockname(fd, (struct sockaddr*)&sockaddr, (socklen_t*)&addrlen);
	}else{
		ret = llnet_getpeername(fd, (struct sockaddr*)&sockaddr, (socklen_t*)&addrlen);
	}
	if (ret == 0){
#if ADDRESS_FAMILY == AF_INET
		if(sockaddr.sin_family == AF_INET) {
			return llnet_ntohs(((struct sockaddr_in *)&sockaddr)->sin_port);
		}
#else
		if(sockaddr.sin6_family == AF_INET6) {
			return llnet_ntohs(((struct sockaddr_in6 *)&sockaddr)->sin6_port);
		}
#endif
		return J_EUNKNOWN;
	}

	return map_to_java_exception(llnet_errno(fd));
}

static int32_t SocketChanel_Address(int32_t fd, int8_t* name, int32_t nameLength, uint8_t localAddress){

#if ADDRESS_FAMILY == AF_INET
	struct sockaddr_in sockaddr = {0};
#else
	struct sockaddr_in6 sockaddr = {0};
#endif
	uint32_t addrlen = sizeof(sockaddr);
	int32_t ret = -1;
	if(localAddress){
		ret = llnet_getsockname(fd, (struct sockaddr *)&sockaddr, (socklen_t*)&addrlen);
		LLNET_DEBUG_TRACE("getsockname(fd=0x%X) ret=%d errno=%d\n", fd,ret,llnet_errno(fd));
	}else{
		ret = llnet_getpeername(fd, (struct sockaddr *)&sockaddr, (socklen_t*)&addrlen);
		LLNET_DEBUG_TRACE("getpeername(fd=0x%X) ret=%d errno=%d\n", fd,ret,llnet_errno(fd));
	}
	if (ret == 0){
#if ADDRESS_FAMILY == AF_INET
		if(sockaddr.sin_family == AF_INET) {
			if(!localAddress && (sockaddr.sin_addr.s_addr == 0 || sockaddr.sin_port == llnet_htons(0))){
				//not connected
				return J_ENOTCONN;
			}
			if(nameLength < sizeof(in_addr_t)){
				return J_EINVAL;
			}
			*(in_addr_t*)name = sockaddr.sin_addr.s_addr;
			return sizeof(in_addr_t);
		}
#else // ADDRESS_FAMILY == AF_INET6
		if(sockaddr.sin6_family == AF_INET6) {
			if(!localAddress && (sockaddr.sin6_addr.s6_addr == 0 || sockaddr.sin6_port == llnet_htons(0))){
				//not connected
				return J_ENOTCONN;
			}
			if(nameLength < sizeof(struct in6_addr)){
				return J_EINVAL;
			}
			memcpy(name, (void *)&(sockaddr.sin6_addr), sizeof(struct in6_addr));
			return sizeof(struct in6_addr);
		}
#endif // ADDRESS_FAMILY == AF_INET
		return J_EUNKNOWN;
	}
	return map_to_java_exception(llnet_errno(fd));
}

int32_t LLNET_SOCKETCHANNEL_IMPL_socket(uint8_t stream, uint8_t retry)
{
	LLNET_DEBUG_TRACE("%s[thread %d](stream=%d) ", __func__, SNI_getCurrentJavaThreadID(), stream);

    if(llnet_is_ready() == false){
        return J_NETWORK_NOT_INITIALIZED;
    }

	int32_t ret;
	int32_t sock_protocol = 0; //default value
	ret = llnet_socket(ADDRESS_FAMILY, stream ? SOCK_STREAM : SOCK_DGRAM, sock_protocol);
	if (ret == -1) {
		int32_t err = map_to_java_exception(llnet_errno(-1));
		LLNET_DEBUG_TRACE("err=%d\n", err);
		return err;
	}
	LLNET_DEBUG_TRACE("fd=0x%X\n", ret);
	return ret;
}

int32_t LLNET_SOCKETCHANNEL_IMPL_serverSocket(uint8_t retry){
	return LLNET_SOCKETCHANNEL_IMPL_socket(JTRUE, retry);
}

int32_t LLNET_SOCKETCHANNEL_IMPL_multicastSocket(uint8_t retry){
	return LLNET_SOCKETCHANNEL_IMPL_socket(JFALSE, retry);
}

#ifdef __cplusplus
	}
#endif
