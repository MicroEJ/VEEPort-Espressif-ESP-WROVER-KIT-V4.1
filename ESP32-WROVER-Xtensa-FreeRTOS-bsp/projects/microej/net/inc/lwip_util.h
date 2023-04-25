/*
 * C
 *
 * Copyright 2017-2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#ifndef __LWIP_UTIL_H
#define __LWIP_UTIL_H

/**
 * @file
 * @brief LLNET utility functions for LWIP.
 * @author MicroEJ Developer Team
 * @version 0.2.2
 * @date 27 November 2020
 */

#include <stdint.h>
#include <lwip/netif.h>

/**
 * Network initialization. Start network interfaces and configure it.
 *
 * @return 0 if no error occurred, error code otherwise.
 */
int32_t llnet_lwip_init(void);

/**
 * Sets the lwIP network interface name in the stored list.
 *
 * @param id the id of the network interface to be stored.
 * @param netif_name the lwIP network interface name to be stored in the list.
 */
void set_lwip_netif_name(int32_t id, char *netif_name);

/**
 * Gets the lwIP network interface name from the stored list.
 *
 * @param id the id of the network interface to be stored.
 *
 * @return the lwIP network interface name from the stored list.
 */
char *get_lwip_netif_name(int32_t id);

/**
 * Gets the lwIP network interface address.
 *
 * @param name the buffer that store the interface name.
 *
 * @return the lwIP network interface address or null if no interface with the specified name.
 */
struct netif* getNetworkInterface(int8_t* name);

#endif // __LWIP_UTIL_H
