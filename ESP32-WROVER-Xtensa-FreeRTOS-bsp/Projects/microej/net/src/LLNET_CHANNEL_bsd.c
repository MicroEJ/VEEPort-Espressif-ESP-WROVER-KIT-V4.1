/*
 * C
 *
 * Copyright 2014-2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */

/**
 * @file
 * @brief LLNET_CHANNEL 2.1.0 implementation over BSD-like API.
 * @author MicroEJ Developer Team
 * @version 0.2.0
 * @date 21 December 2017
 */

#include <LLNET_CHANNEL_impl.h>

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "LLNET_CONSTANTS.h"
#include "async_select.h"
#include "async_select_cache.h"
#include "LLNET_ERRORS.h"
#include "LLNET_Common.h"

#include "LLECOM_NETWORK.h"
#include "LLECOM_WIFI.h"

#ifdef __cplusplus
	extern "C" {
#endif

int32_t LLNET_CHANNEL_IMPL_close(int32_t fd, uint8_t retry)
{
	LLNET_DEBUG_TRACE("%s[thread %d](fd=0x%X)\n", __func__, SNI_getCurrentJavaThreadID(), fd);

    if(llnet_is_ready() == false){
        return J_NETWORK_NOT_INITIALIZED;
    }

	if(llnet_close(fd) == -1){
		return map_to_java_exception(llnet_errno(fd));
	}

	async_select_remove_socket_timeout_from_cache(fd);

	return 0;
}

int32_t LLNET_CHANNEL_IMPL_initialize(void)
{
	LLNET_DEBUG_TRACE("%s\n", __func__);
	int32_t res;

	LLECOM_NETWORK_initialize();
	LLECOM_WIFI_initialize();

	async_select_init_socket_timeout_cache();

	res = async_select_init();
	if(res != 0){
		return J_EUNKNOWN;
	}

	return 0;
}

int32_t LLNET_CHANNEL_IMPL_setBlocking(int32_t fd, uint8_t blocking, uint8_t retry)
{
	LLNET_DEBUG_TRACE("%s[thread %d](fd=0x%X, blocking=%d)\n", __func__, SNI_getCurrentJavaThreadID(), fd, blocking);

    if(llnet_is_ready() == false){
        return J_NETWORK_NOT_INITIALIZED;
    }

	int32_t res = set_socket_non_blocking(fd, blocking==0 ? true : false);
	if(res == 0){
		return 0;
	}
	return map_to_java_exception(llnet_errno(fd));
}

int32_t LLNET_CHANNEL_IMPL_shutdown(int32_t fd, uint8_t retry)
{
	LLNET_DEBUG_TRACE("%s[thread %d](fd=0x%X)\n", __func__, SNI_getCurrentJavaThreadID(), fd);
	int32_t ret = llnet_shutdown(fd, SHUT_RD); //shutdown input stream
	LLNET_DEBUG_TRACE("%s[thread %d](fd=0x%X) ret=%d errno=%d\n", __func__, SNI_getCurrentJavaThreadID(), fd, ret, llnet_errno(fd));
	(void)ret;
	return 0;
}

int32_t LLNET_CHANNEL_IMPL_bind(int32_t fd, int8_t* addr, int32_t length, int32_t port, uint8_t retry)
{
	LLNET_DEBUG_TRACE("%s[thread %d](fd=0x%X, port=%d, addrLength=%d,...)\n", __func__, SNI_getCurrentJavaThreadID(), fd, port, length);

    if(llnet_is_ready() == false){
        return J_NETWORK_NOT_INITIALIZED;
    }

#if ADDRESS_FAMILY == AF_INET
	struct sockaddr_in sockaddr = {0};

	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = llnet_htons(port);
	if(length < sizeof(in_addr_t)){
		return J_EINVAL;
	}
	sockaddr.sin_addr.s_addr = *(in_addr_t*)addr;
#else
	struct sockaddr_in6 sockaddr = {0};

	sockaddr.sin6_family = AF_INET6;
	sockaddr.sin6_port = llnet_htons(port);
	if(length < sizeof(struct in6_addr)){
		return J_EINVAL;
	}
    memcpy(&sockaddr.sin6_addr, addr, sizeof(struct in6_addr));
#endif // ADDRESS_FAMILY == AF_INET

	int32_t ret = llnet_bind(fd, (struct sockaddr*)&sockaddr, sizeof(sockaddr));
	LLNET_DEBUG_TRACE("%s[thread %d](fd=0x%X, ret=%d, errno=%d, ...)\n", __func__, SNI_getCurrentJavaThreadID(), fd, ret, llnet_errno(fd));
	if(ret == -1){
		return map_to_java_exception(llnet_errno(fd));
	}
	return 0;
}

int32_t LLNET_CHANNEL_IMPL_getOption(int32_t fd, int32_t option, uint8_t retry)
{
	LLNET_DEBUG_TRACE("%s[thread %d](fd=0x%X, option=%d)\n", __func__, SNI_getCurrentJavaThreadID(), fd, option);

    if(llnet_is_ready() == false){
        return J_NETWORK_NOT_INITIALIZED;
    }

	int32_t optname = -1;
	int32_t level = SOL_SOCKET;
	int32_t optlen = sizeof(int32_t);
	int32_t optVal = 0;
	struct timeval sock_timeout;
	void* p_optVal = &optVal;

	if (option == CPNET_SO_TIMEOUT) {
		optVal = async_select_get_socket_timeout_from_cache(fd);
		if (optVal != -1) {
			return optVal;
		}
	}

	switch (option) {
		case CPNET_SO_KEEPALIVE:
			optname = SO_KEEPALIVE;
			break;
		case CPNET_SO_BROADCAST:
			optname = SO_BROADCAST;
			break;
		case CPNET_SO_TIMEOUT :
			p_optVal = &sock_timeout;
			optlen = sizeof(sock_timeout);
			optname = SO_RCVTIMEO;
			break;
		case CPNET_SO_SNDBUF :
			optname = SO_SNDBUF;
			break;
		case CPNET_SO_RCVBUF :
			optname = SO_RCVBUF;
			break;
		case CPNET_TCP_NODELAY:
			level = IPPROTO_TCP;
			optname = TCP_NODELAY;
			break;
		case CPNET_IP_TOS:
			level = IPPROTO_IP;
			optname = IP_TOS;
			break;
		case CPNET_SO_REUSEADDR:
			optname = SO_REUSEADDR;
			break;
		case CPNET_IP_MULTICAST_LOOP:
			level = IPPROTO_IP;
			optname = IP_MULTICAST_LOOP;
			break;
		case CPNET_IP_TTL:
			level = IPPROTO_IP;
			optname = IP_MULTICAST_TTL;
			break;
		case CPNET_SO_OOBINLINE:	// no urgent data option for this implementation
		default:
			//option not available
			return J_ENOPROTOOPT;
	}

	int32_t res = llnet_getsockopt(fd, level, optname, p_optVal, (socklen_t *)&optlen);
	LLNET_DEBUG_TRACE("%s[thread %d](fd=0x%X, option=%d) res=%d errno=%d\n", __func__, SNI_getCurrentJavaThreadID(), fd, option, res, llnet_errno(fd));
	if(res == -1){
		return map_to_java_exception(llnet_errno(fd));
	}

	if (option == CPNET_SO_TIMEOUT) {
		async_select_set_socket_timeout_in_cache(fd, (sock_timeout.tv_usec / 1000) + (sock_timeout.tv_sec * 1000));
	}

	if(option == CPNET_SO_TIMEOUT){
		return (sock_timeout.tv_usec / 1000) + (sock_timeout.tv_sec * 1000);
	}
	return optVal;
}

int32_t LLNET_CHANNEL_IMPL_setOption(int32_t fd, int32_t option, int32_t value, uint8_t retry)
{
	LLNET_DEBUG_TRACE("%s[thread %d](fd=0x%X, option=%d, value=%d)\n", __func__, SNI_getCurrentJavaThreadID(), fd, option, value);

    if(llnet_is_ready() == false){
        return J_NETWORK_NOT_INITIALIZED;
    }

	int32_t optname = -1;
	int32_t level = SOL_SOCKET;
	struct timeval sock_timeout;

	int optlen = sizeof(int32_t);

	void* p_optval = &value;

	switch (option) {
		case CPNET_SO_KEEPALIVE:
			optname = SO_KEEPALIVE;
			break;
		case CPNET_SO_BROADCAST:
			optname = SO_BROADCAST;
			break;
		case CPNET_SO_TIMEOUT:
			if(value > 1000){
				sock_timeout.tv_sec = value / 1000;
				sock_timeout.tv_usec = (value % 1000) * 1000;
			}else{
				sock_timeout.tv_sec = 0;
				sock_timeout.tv_usec = value * 1000;
			}
			p_optval = &sock_timeout;
			optlen = sizeof(sock_timeout);
			optname = SO_RCVTIMEO;
			break;
		case CPNET_SO_SNDBUF:
			optname = SO_SNDBUF;
			break;
		case CPNET_SO_RCVBUF:
			optname = SO_RCVBUF;
			break;
		case CPNET_TCP_NODELAY:
			level = IPPROTO_TCP;
			optname = TCP_NODELAY;
			break;
		case CPNET_IP_TOS:
			level = IPPROTO_IP;
			optname = IP_TOS;
			break;
		case CPNET_SO_REUSEADDR :	// no reuse option for this implementation
			optname = SO_REUSEADDR;
			break;
		case CPNET_IP_MULTICAST_LOOP:
			level = IPPROTO_IP;
			optname = IP_MULTICAST_LOOP;
			break;
		case CPNET_IP_TTL:
			level = IPPROTO_IP;
			optname = IP_MULTICAST_TTL;
			break;
		case CPNET_SO_OOBINLINE:	// no urgent data option for this implementation
		default:
			/* option not available */
			return J_ENOPROTOOPT;
	}
	int32_t res = llnet_setsockopt (fd, level, optname, p_optval, optlen);
	LLNET_DEBUG_TRACE("%s[thread %d](fd=0x%X, option=%d, value=%d), res=%d, errno=%d\n", __func__, SNI_getCurrentJavaThreadID(), fd, option, value, res, llnet_errno(fd));
	if (res == -1) {
		return map_to_java_exception(llnet_errno(fd));
	}

	if (option == CPNET_SO_TIMEOUT) {
		async_select_set_socket_timeout_in_cache(fd, value);
	}

	return 0;
}

int32_t LLNET_CHANNEL_IMPL_getOptionAsByteArray(int32_t fd, int32_t option, int8_t* dst, int32_t dstLength, uint8_t retry)
{
	/* remove unused retry parameter warning*/
	(void)(retry);

	LLNET_DEBUG_TRACE("%s[thread %d](fd=0x%X, option=%d, dstLength=%d)\n", __func__, SNI_getCurrentJavaThreadID(), fd, option, dstLength);

	switch (option) {
		case CPNET_IP_MULTICAST_IF:
		case CPNET_IP_MULTICAST_IF2:
			{
				socklen_t option_length;
#if ADDRESS_FAMILY == AF_INET
				option_length = sizeof(struct in_addr);
#else
				option_length = sizeof(struct in6_addr);
#endif	// ADDRESS_FAMILY == AF_INET
				if(dstLength < option_length){
					return J_EINVAL;
				}

				int32_t res = llnet_getsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, dst, &option_length);
				LLNET_DEBUG_TRACE("%s[thread %d](fd=0x%X, option=%d) res=%d errno=%d\n", __func__, SNI_getCurrentJavaThreadID(), fd, option, res, llnet_errno(fd));
				if(res == -1){
					return map_to_java_exception(llnet_errno(fd));
				}
			}
			break;
		default:
			//option not available
			return J_ENOPROTOOPT;
	}
	return 0;
}

int32_t LLNET_CHANNEL_IMPL_setOptionAsByteArray(int32_t fd, int32_t option, int8_t* value, int32_t valueLength, uint8_t retry)
{
	/* remove unused retry parameter warning*/
	(void)(retry);

	LLNET_DEBUG_TRACE("%s[thread %d](fd=0x%X, option=%d, valueLength=%d)\n", __func__, SNI_getCurrentJavaThreadID(), fd, option, valueLength);
	switch (option) {
		case CPNET_IP_MULTICAST_IF:
		case CPNET_IP_MULTICAST_IF2:
			{
				socklen_t option_length;
#if ADDRESS_FAMILY == AF_INET
				option_length = sizeof(in_addr_t);
#else
				option_length = sizeof(struct in6_addr);
#endif // ADDRESS_FAMILY == AF_INET
				if(valueLength < option_length){
					return J_EINVAL;
				}
				int32_t res;
				res = llnet_setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, value, option_length);
				LLNET_DEBUG_TRACE("%s[thread %d](fd=0x%X, option=%d) res=%d errno=%d\n", __func__, SNI_getCurrentJavaThreadID(), fd, option, res, llnet_errno(fd));
				if(res == -1){
					return map_to_java_exception(llnet_errno(fd));
				}
				return 0;
			}
		default:
			/* option not available */
			return J_ENOPROTOOPT;
	}
}

int32_t LLNET_CHANNEL_IMPL_listen(int32_t fd, int32_t backlog, uint8_t retry)
{
	/* remove unused retry parameter warning*/
	(void)(retry);

	LLNET_DEBUG_TRACE("%s[thread %d](fd=0x%X, backlog=%d)\n", __func__, SNI_getCurrentJavaThreadID(), fd, backlog);

    if(llnet_is_ready() == false){
        return J_NETWORK_NOT_INITIALIZED;
    }

	int32_t res = llnet_listen(fd, backlog);
	if(res == -1){
		return map_to_java_exception(llnet_errno(fd));
	}
	return 0;
}

#ifdef __cplusplus
	}
#endif
