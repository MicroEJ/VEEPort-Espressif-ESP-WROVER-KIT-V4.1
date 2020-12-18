/*
 * C
 *
 * Copyright 2018-2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */

#include <LLNET_DNS_impl.h>

#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include "LLNET_CONSTANTS.h"
#include "LLNET_ERRORS.h"
#include "lwip/dns.h"

extern uint8_t dns_servers_list_updated;

uint8_t LLNET_DNS_IMPL_dnsServersListHasChanged()
{
	uint8_t hasChanged = dns_servers_list_updated;

	if(hasChanged)
	{
		dns_servers_list_updated = 0;
	}

	return hasChanged;
}

int32_t LLNET_DNS_IMPL_getIPv4DNSServersList(int8_t* serversAddr, int32_t offset, int32_t length, uint8_t retry)
{
	int32_t offsetIndex = offset;
	int32_t i = -1;
	int32_t count = 0;

	for(i = -1; ++i < DNS_MAX_SERVERS;)
	{
		const ip_addr_t* dns_addr = dns_getserver(i);
		// check if the DNS server IP address is right
		if(ip_2_ip4(dns_addr)->addr != IPADDR_ANY)
		{
			// copy DNS server IP address
			memcpy(serversAddr + offsetIndex, ip_2_ip4(dns_addr), sizeof(ip4_addr_t));
			offsetIndex += sizeof(ip4_addr_t);
			++count;
		}
	}

	return count;
}
