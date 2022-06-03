/*
 * C
 *
 * Copyright 2014-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief LLNET_STREAMSOCKETCHANNEL 2.1.0 implementation over BSD-like API.
 * @author MicroEJ Developer Team
 * @version 1.3.1
 * @date 20 April 2021
 */

#include <LLNET_STREAMSOCKETCHANNEL_impl.h>

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include "LLNET_CONSTANTS.h"
#include <LLNET_CHANNEL_impl.h>
#include "LLNET_ERRORS.h"
#include "LLNET_Common.h"

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef USE_MSG_PEEK_FOR_AVAILABLE
// Define the buffers used for LLNET_STREAMSOCKETCHANNEL_IMPL_available() function.
// The LLNET_configuration.h must define the size of this buffer: NET_EMBEDDED_AVAILABLE_BUFFER_SIZE.
uint8_t NET_EMBEDDED_AVAILABLE_BUFFER[NET_EMBEDDED_AVAILABLE_BUFFER_SIZE];
#endif

int32_t StreamSocketChannel_accept(int32_t fd)
{
	LLNET_DEBUG_TRACE("%s[thread %d](fd=0x%X)\n", __func__, SNI_getCurrentJavaThreadID(), fd);
	int32_t ret;
	ret = llnet_accept(fd, NULL, NULL);
	if(ret == -1){
		LLNET_DEBUG_TRACE("Error on accept (fd=0x%X)\n", fd);
		return map_to_java_exception(llnet_errno(fd));
	}

	LLNET_DEBUG_TRACE("Successful accept OK fd=0x%X\n", ret);
	return ret;
}

int32_t StreamSocketChannel_writeByteBufferNative(int32_t fd, int8_t* src, int32_t offset, int32_t length)
{
	LLNET_DEBUG_TRACE("%s[thread %d]\n", __func__, SNI_getCurrentJavaThreadID());
	int32_t ret;
	ret = llnet_send(fd, src+offset, length, 0);
	LLNET_DEBUG_TRACE("sent bytes size = %d (length=%d)\n", ret, length);
	if(ret == -1){
		return map_to_java_exception(llnet_errno(fd));
	}
	return ret;
}

int32_t StreamSocketChannel_readByteBufferNative(int32_t fd, int8_t* dst, int32_t offset, int32_t length)
{
	LLNET_DEBUG_TRACE("%s[thread %d]\n", __func__, SNI_getCurrentJavaThreadID());
	int32_t ret;
	ret = llnet_recv(fd, dst+offset, length, 0);
	LLNET_DEBUG_TRACE("nb received data : %d errno=%d\n", ret, llnet_errno(fd));
	if(ret == -1){
		return map_to_java_exception(llnet_errno(fd));
	}

	if (0 == ret) {
		return -1; //EOF
	}
	return ret;
}

int32_t LLNET_STREAMSOCKETCHANNEL_IMPL_readByteBufferNative(int32_t fd, int32_t kind, int8_t* dst, int32_t offset, int32_t length, uint8_t retry)
{
	LLNET_DEBUG_TRACE("%s[thread %d](fd=0x%X, kind=%d, retry=%d)\n", __func__, SNI_getCurrentJavaThreadID(), fd, kind, retry);

	(void)kind;

    if(llnet_is_ready() == false){
        return J_NETWORK_NOT_INITIALIZED;
    }

	int32_t selectRes = non_blocking_select(fd, SELECT_READ);

	if(selectRes == 0){
		return asyncOperation(fd, SELECT_READ, retry);
	}else{
		return StreamSocketChannel_readByteBufferNative(fd, dst, offset, length);
	}
}

int32_t LLNET_STREAMSOCKETCHANNEL_IMPL_writeByteBufferNative(int32_t fd, int32_t kind, int8_t* src, int32_t offset, int32_t length, uint8_t retry)
{
	LLNET_DEBUG_TRACE("%s[thread %d]\n", __func__, SNI_getCurrentJavaThreadID());

	(void)kind;

    if(llnet_is_ready() == false){
        return J_NETWORK_NOT_INITIALIZED;
    }

	int32_t selectRes = non_blocking_select(fd, SELECT_WRITE);

	if(selectRes == 0){
		return asyncOperation(fd, SELECT_WRITE, retry);
	}else{
		return StreamSocketChannel_writeByteBufferNative(fd, src, offset, length);
	}
}

int32_t LLNET_STREAMSOCKETCHANNEL_IMPL_available(int32_t fd, uint8_t retry)
{
	LLNET_DEBUG_TRACE("%s[thread %d](fd=0x%X)\n", __func__, SNI_getCurrentJavaThreadID(), fd);

	(void)retry;

    if(llnet_is_ready() == false){
        return J_NETWORK_NOT_INITIALIZED;
    }

#if !defined(USE_MSG_PEEK_FOR_AVAILABLE) && !defined(USE_SOCK_OPTION_FOR_AVAILABLE)
	int32_t size;
	int32_t res = llnet_ioctl(fd, FIONREAD, &size);
	LLNET_DEBUG_TRACE("%s[thread %d](fd=0x%X) size=%d errno=%d res=%d\n", __func__, SNI_getCurrentJavaThreadID(), fd, size, llnet_errno(fd), res);
	if(res == 0){
		return size;
	}
	// error on ioctl
	return map_to_java_exception(llnet_errno(fd));
#elif defined(USE_MSG_PEEK_FOR_AVAILABLE)
	int32_t size = llnet_recv(fd, NET_EMBEDDED_AVAILABLE_BUFFER, NET_EMBEDDED_AVAILABLE_BUFFER_SIZE, MSG_PEEK | MSG_DONTWAIT);
	LLNET_DEBUG_TRACE("%s[thread %d](fd=0x%X) size=%d errno=%d\n", __func__, SNI_getCurrentJavaThreadID(), fd, size, llnet_errno(fd));
	if(size >= 0){
		return size;
	}
	if(llnet_errno(fd) == EAGAIN){
		return 0;
	}
	return map_to_java_exception(llnet_errno(fd));
#else
	int32_t size;
	int32_t optlen = sizeof(int32_t);
	int32_t res = llnet_getsockopt (fd, SOL_SOCKET, SO_RXDATA, &size, (socklen_t *)&optlen);
	LLNET_DEBUG_TRACE("%s[thread %d](fd=0x%X) size=%d errno=%d res=%d\n", __func__, SNI_getCurrentJavaThreadID(), fd, size, llnet_errno(fd), res);
	if(res == 0){
		return size;
	}
	return map_to_java_exception(llnet_errno(fd));
#endif
}

int32_t LLNET_STREAMSOCKETCHANNEL_IMPL_accept(int32_t fd, uint8_t retry)
{
	LLNET_DEBUG_TRACE("%s[thread %d](fd=0x%X, retry=%d)\n", __func__, SNI_getCurrentJavaThreadID(), fd, retry);

    if(llnet_is_ready() == false){
        return J_NETWORK_NOT_INITIALIZED;
    }

	int32_t selectRes = non_blocking_select(fd, SELECT_READ);

	if(selectRes == 0){
		return asyncOperation(fd, SELECT_READ, retry);
	}else{
		return StreamSocketChannel_accept(fd);
	}
}

#ifdef __cplusplus
	}
#endif
