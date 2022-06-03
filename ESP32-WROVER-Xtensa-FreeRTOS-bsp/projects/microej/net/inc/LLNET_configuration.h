/*
 * C
 *
 * Copyright 2017-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief Platform implementation specific macro.
 * @author MicroEJ Developer Team
 * @version 0.2.0
 * @date 21 December 2017
 */

#ifndef  LLNET_CONFIGURATION_H
#define  LLNET_CONFIGURATION_H

#include <lwip/sockets.h>

/**
 * @brief Compatibility sanity check value.
 * This define value is checked in the implementation to validate that the version of this configuration
 * is compatible with the implementation.
 *
 * This value must not be changed by the user of the CCO.
 * This value must be incremented by the implementor of the CCO when a configuration define is added, deleted or modified.
 */
#define LLNET_CONFIGURATION_VERSION (2)

/**
 * All the llnet_* functions are mapped on custom functions from the async_select component.
 */
#define llnet_accept		accept
#define llnet_bind			bind
#define llnet_close			close
#define llnet_connect		connect
#define llnet_fcntl			fcntl
#define llnet_getpeername	getpeername
#define llnet_getsockname	getsockname
#define llnet_getsockopt	getsockopt
#define llnet_htonl			lwip_htonl
#define llnet_htons			lwip_htons
#define llnet_ioctl			ioctl
#define llnet_listen		listen
#define llnet_ntohs			lwip_ntohs
#define llnet_recv			recv
#define llnet_recvfrom		recvfrom
#define llnet_send			send
#define llnet_sendto		sendto
#define llnet_setsockopt	setsockopt
#define llnet_socket		socket
#define llnet_shutdown		shutdown
#define llnet_resolvename	dns_gethostbyname_addrtype
#define llnet_gethostname(a, b)	(1)

/**
 * Don't modify the LLNET_AF_* constants.
 */
#define LLNET_AF_IPV4	(0x1)
#define LLNET_AF_IPV6	(0x2)
#define LLNET_AF_DUAL	((LLNET_AF_IPV4) | (LLNET_AF_IPV6))

/**
 *  Address family:
 *  - LLNET_AF_IPV4 for only IPv4
 *  - LLNET_AF_IPV6 for only IPv6
 *  - LLNET_AF_DUAL for both IPv4 and IPv6
 */
#define LLNET_AF (LLNET_AF_IPV4)

/**
 * Define the maximum number of sockets that can be handled by the net module
 */
#define LLNET_MAX_SOCKETS MEMP_NUM_NETCONN

/**
 * Define the logical FD starting value
 */
#define LLNET_SOCKFD_START_IDX LWIP_SOCKET_OFFSET

/**
 * Returns the errno value for the given file descriptor.
 * Given file descriptor may be -1 if no file descriptor is defined.
 */
#include <errno.h>
#define llnet_errno(fd) 	errno

/*
 * Initialize the network stack.
 * Returns 0 on success, -1 on error.
 * This method may block and must be called in another RTOS task.
 * See async_select task.
 * By default this macro does nothing.
 */
#include "lwip_util.h"
#define llnet_init		llnet_lwip_init

/*
 * Returns true (bool) if we can call the services of the network stack without jeopardizing the system,
 * otherwise returns false (bool).
 * Returning true does not mean that a network interface is up.
 * By default this macro always return true.
 */
#define llnet_is_ready()	(true)

/**
 * Enable network debug trace
 */
//#define LLNET_DEBUG

/**
 * If ioctl() cannot be used to implement the LLNET_STREAMSOCKETCHANNEL_IMPL_available() function,
 * define USE_MSG_PEEK_FOR_AVAILABLE and NET_EMBEDDED_AVAILABLE_BUFFER_SIZE.
 */
//#define USE_MSG_PEEK_FOR_AVAILABLE
#define NET_EMBEDDED_AVAILABLE_BUFFER_SIZE (CONFIG_TCP_WND_DEFAULT)

/**
 * Define USE_IOCTL_FOR_BLOCKING_OPTION to use ioctl() instead of fcntl() to configure socket
 * blocking or non-blocking mode.
 */
#define USE_IOCTL_FOR_BLOCKING_OPTION

/**
 * Define the mapping between the LLNET provided network interface names
 * and the platform dependent network interface names
 * - at platform level, the netif names are created by LwIP
 * - the loopback netif is always the first one created and maintained forever
 * - the station and soft ap netifs can change order, depending on how the application will start/stop them
 */
#if (defined(LWIP_NETIF_LOOPBACK) && (LWIP_NETIF_LOOPBACK != 0))
#define LLNET_NETIF_NAMES    {"loopback_interface", "st1", "ap2"}
#define PLATFORM_NETIF_NAMES {"lo"                , "st"            , "ap"            }
/*number of netifs stored*/
#define NUMB_OF_NETIF_TO_STORE 3
#else
#define LLNET_NETIF_NAMES    {"st1", "ap2"}
#define PLATFORM_NETIF_NAMES {"st"            , "ap"            }
/*number of netifs stored*/
#define NUMB_OF_NETIF_TO_STORE 2
#endif

/*max length of netif name from lwip*/
#define MAX_SIZE_OF_NETIF_NAME 6

#endif // LLNET_CONFIGURATION_H
