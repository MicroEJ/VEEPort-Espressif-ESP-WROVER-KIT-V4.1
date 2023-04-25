/*
 * C
 *
 * Copyright 2015-2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief lwip_util implementation over LWIP and FreeRTOS.
 * @author MicroEJ Developer Team
 * @version 0.2.2
 * @date 27 November 2020
 */

#include <string.h>
#include "lwip_util.h"
#include "LLNET_Common.h"
#include "LLNET_CONF.h"
#include "WIFI_ESP32_driver.h"
#include "lwip/dns.h"

/** @brief Array holding all the network interface names at LwIP level. */
static char lwip_netif[NUMB_OF_NETIF_TO_STORE][MAX_SIZE_OF_NETIF_NAME];

int32_t llnet_lwip_init(void)
{
	/* Initialize the LwIP TCP/IP stack */
	if (!WIFI_ESP32_initialize_f()) {
		return -1;
	}

	/* Retrieve the first non-loopback network interface as this will be the WiFi station. */
	struct netif *interface = netif_list;
#if (defined(LWIP_NETIF_LOOPBACK) && (LWIP_NETIF_LOOPBACK != 0))
	interface = interface->next;
#endif
	if (interface == NULL) {
		return -1;
	}

	/* Retrieve the IP and DNS configuration (static or via DHCP). */
	bool ipDhcpEnabled = LLNET_CONF_is_ip_dhcp_enabled() == LLNET_CONF_TRUE;
	bool dnsDhcpEnabled = LLNET_CONF_is_dns_dhcp_enabled() == LLNET_CONF_TRUE;

	/* Print warnings in case of not suitable configuration. */
	if (!dnsDhcpEnabled && ipDhcpEnabled) {
		puts("WARNING: a statically set DNS address will be overridden by DHCP when joining a network");
	}

	/* Retrieve static IP configuration. */
	if (!ipDhcpEnabled) {
		int32_t err = 0;
		ip4_addr_t ip = {0};
		ip4_addr_t gw = {0};
		ip4_addr_t netmask = {0};

		err += lwip_inet_pton(AF_INET, LLNET_CONF_get_device_ip_address(), &ip);
		err += lwip_inet_pton(AF_INET, LLNET_CONF_get_netmask(), &netmask);
		err += lwip_inet_pton(AF_INET, LLNET_CONF_get_gateway_ip_address(), &gw);
		if (err != 3) {
			return -1;
		}

		/* Set the static IP configuration. */
		netif_set_ipaddr(interface, &ip);
		netif_set_netmask(interface, &netmask);
		netif_set_gw(interface, &gw);
	}

	/* Retrieve static DNS configuration. */
	if (!dnsDhcpEnabled || !ipDhcpEnabled) {
		int32_t err = 0;
		ip_addr_t dns = {0};

		err += lwip_inet_pton(AF_INET, LLNET_CONF_get_dns_ip_address(), &dns);
		if (err != 1) {
			return -1;
		}

		/* Set the static DNS configuration. */
		dns_setserver(0, &dns);
	}

	/* Apply configuration to the WiFi station. */
	if (WIFI_ESP32_sta_apply_configuration_f(!ipDhcpEnabled, *netif_ip4_addr(interface), *netif_ip4_netmask(interface), *netif_ip4_gw(interface)) == false) {
		return -1;
	}

	return 0;
}

void set_lwip_netif_name(int32_t id, char *netif_name)
{
	strcpy(lwip_netif[id], netif_name);
}

char *get_lwip_netif_name(int32_t id)
{
	return lwip_netif[id];
}

struct netif* getNetworkInterface(int8_t* name)
{
	char *llnet_netif[] = LLNET_NETIF_NAMES;
	for (int32_t netif_idx = 0; netif_idx < (sizeof(llnet_netif) / sizeof(llnet_netif[0])); netif_idx++) {
		if (strcmp((char *)name, llnet_netif[netif_idx]) == 0) {
			// the same interface name at LLNET level can have different names at platform level
			// so, keep trying to find the lwip level interface until one is found or until we exhaust the list
			struct netif *ret = netif_find(lwip_netif[netif_idx]);
			if (ret) {
				return ret;
			}
		}
	}
	return NULL;
}
