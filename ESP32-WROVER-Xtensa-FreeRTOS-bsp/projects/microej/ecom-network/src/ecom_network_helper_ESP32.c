/*
 * C
 *
 * Copyright 2020-2023 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 *
 */

#include <stdint.h>
#include <string.h>
#include "WIFI_ESP32_driver.h"
#include "ecom_network_helper.h"
#include "microej_async_worker.h"
#include "LLNET_ERRORS.h"
#include "LLECOM_NETWORK_impl.h"
#include "LLNET_configuration.h"
#include <lwip/netif.h>
#include <lwip/ip.h>
#include "lwip/dns.h"
#include "lwip_util.h"

#ifdef LLECOM_NETWORK_DEBUG
#include <stdio.h>
#endif

#ifdef LLECOM_NETWORK_DEBUG
#define LLECOM_NETWORK_DEBUG_TRACE printf("[DEBUG] ");printf
#else
#define LLECOM_NETWORK_DEBUG_TRACE(...) ((void) 0)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* global to know if configuration is static or dhcp */
static int8_t ecom_network_is_static;

void LLECOM_NETWORK_IMPL_enable_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_NETWORK_netif_context_t* param = (ECOM_NETWORK_netif_context_t*) job->params;

	if (param->netifName == NULL) {
		param->result = J_EINVAL;
		if (param->error_message != NULL) {
			param->error_message = "null pointer";
		}
	} else {
		struct netif* interface;

		interface = getNetworkInterface(param->netifName + param->netifNameOffset);

		if (interface != NULL) {
			netif_set_up(interface);
			param->result = 0;
		} else {
			param->result = J_EUNKNOWN;
			if (param->error_message != NULL) {
				param->error_message = "interface not found";
			}
		}
	}

	LLECOM_NETWORK_DEBUG_TRACE("[%s:%u] interface enabled with result : %d (err %d)\n", __func__, __LINE__, param->result, param->error_code);
}

void LLECOM_NETWORK_IMPL_disable_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_NETWORK_netif_context_t* param = (ECOM_NETWORK_netif_context_t*) job->params;

	if (param->netifName == NULL) {
		param->result = J_EINVAL;
		if (param->error_message != NULL) {
			param->error_message = "null pointer";
		}
	} else {
		struct netif* interface;

		interface = getNetworkInterface(param->netifName + param->netifNameOffset);

		if (interface != NULL) {
			netif_set_down(interface);
			param->result = 0;
		} else {
			param->result = J_EUNKNOWN;
			if (param->error_message != NULL) {
				param->error_message = "interface not found";
			}
		}
	}

	LLECOM_NETWORK_DEBUG_TRACE("[%s:%u] interface disabled with result : %d (err %d)\n", __func__, __LINE__, param->result, param->error_code);
}

void LLECOM_NETWORK_IMPL_isEnabled_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_NETWORK_netif_context_t* param = (ECOM_NETWORK_netif_context_t*) job->params;

	if (param->netifName == NULL) {
		param->result = J_EINVAL;
		if (param->error_message != NULL) {
			param->error_message = "null pointer";
		}
	} else {
		struct netif* interface;

		interface = getNetworkInterface(param->netifName + param->netifNameOffset);

		if (interface != NULL) {
			param->result = netif_is_up(interface) ? IF_STATE_ENABLED : IF_STATE_NOT_ENABLED;
		} else {
			param->result = J_EUNKNOWN;
			if (param->error_message != NULL) {
				param->error_message = "interface not found";
			}
		}
	}

	LLECOM_NETWORK_DEBUG_TRACE("[%s:%u] result : %d (err %d)\n", __func__, __LINE__, param->result, param->error_code);
}

void LLECOM_NETWORK_IMPL_start_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_NETWORK_netif_context_t* param = (ECOM_NETWORK_netif_context_t*) job->params;

	if (param->netifName == NULL) {
		param->result = J_EINVAL;
		if (param->error_message != NULL) {
			param->error_message = "null pointer";
		}
	} else {
		if (WIFI_ESP32_sta_start_f() == true) {
			param->result = 0;
		} else {
			param->result = J_EUNKNOWN;
			if (param->error_message != NULL) {
				param->error_message = "wifi esp32 driver error";
			}
		}
	}

	LLECOM_NETWORK_DEBUG_TRACE("[%s:%u] interface started with result : %d (err %d)\n", __func__, __LINE__, param->result, param->error_code);
}

void LLECOM_NETWORK_IMPL_stop_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_NETWORK_netif_context_t* param = (ECOM_NETWORK_netif_context_t*) job->params;

	if (param->netifName == NULL) {
		param->result = J_EINVAL;
		if (param->error_message != NULL) {
			param->error_message = "null pointer";
		}
	} else {
		if (WIFI_ESP32_sta_stop_f() == true) {
			param->result = 0;
		} else {
			param->result = J_EUNKNOWN;
			if (param->error_message != NULL) {
				param->error_message = "wifi esp32 driver error";
			}
		}
	}

	LLECOM_NETWORK_DEBUG_TRACE("[%s:%u] interface stopped with result : %d (err %d)\n", __func__, __LINE__, param->result, param->error_code);
}

void LLECOM_NETWORK_IMPL_isStarted_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_NETWORK_netif_context_t* param = (ECOM_NETWORK_netif_context_t*) job->params;

	if (param->netifName == NULL) {
		param->result = J_EINVAL;
		if (param->error_message != NULL) {
			param->error_message = "null pointer";
		}
	} else {
		unsigned char netif_started;

		if (WIFI_ESP32_sta_netif_is_started_f(&netif_started) == true) {
			param->result = (netif_started == 1) ? IF_STATE_STARTED : IF_STATE_NOT_STARTED;
		} else {
			param->result = J_EUNKNOWN;
			if (param->error_message != NULL) {
				param->error_message = "wifi esp32 driver error";
			}
		}
	}

	LLECOM_NETWORK_DEBUG_TRACE("[%s:%u] result : %d (err %d)\n", __func__, __LINE__, param->result, param->error_code);
}

void LLECOM_NETWORK_IMPL_getDNSCount_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_NETWORK_netif_context_t* param = (ECOM_NETWORK_netif_context_t*) job->params;

	if (param->netifName == NULL) {
		param->result = J_EINVAL;
		if (param->error_message != NULL) {
			param->error_message = "null pointer";
		}
	} else {
		int32_t count = 0;

		for (int32_t i = 0; i < DNS_MAX_SERVERS; i++) {
			const ip_addr_t* dns_addr = dns_getserver(i);

			// check if the DNS server IP address is right
			if (!ip_addr_isany(dns_addr)) {
				count++;
			} else {
				break;
			}
		}

		param->result = count;
	}

	LLECOM_NETWORK_DEBUG_TRACE("[%s:%u] DNS counted : %d (err %d)\n", __func__, __LINE__, param->result, param->error_code);
}

void LLECOM_NETWORK_IMPL_getDNS_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_NETWORK_dns_context_t* param = (ECOM_NETWORK_dns_context_t*) job->params;

	if ((param->netifName == NULL) || (param->address == NULL) || (param->index < 0) || (param->index >= DNS_MAX_SERVERS)) {
		param->result = J_EINVAL;
		if (param->error_message != NULL) {
			param->error_message = "null pointer or wrong index";
		}
	} else {
		const ip_addr_t* dns_addr = dns_getserver(param->index);

		// check if the DNS server IP address is right
		if (!ip_addr_isany(dns_addr)) {
			memcpy(param->address + param->addressOffset, ip_2_ip4(dns_addr), sizeof(ip4_addr_t));
			param->result = 0;
		} else {
			param->result = J_EUNKNOWN;
			if (param->error_message != NULL) {
				param->error_message = "DNS server IP address is not valid";
			}
		}
	}

	LLECOM_NETWORK_DEBUG_TRACE("[%s:%u] get DNS result : %d (err %d)\n", __func__, __LINE__, param->result, param->error_code);
}

void LLECOM_NETWORK_IMPL_getGateway_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_NETWORK_ip_context_t* param = (ECOM_NETWORK_ip_context_t*) job->params;

	if ((param->netifName == NULL) || (param->address == NULL)) {
		param->result = J_EINVAL;
		if (param->error_message != NULL) {
			param->error_message = "null pointer";
		}
	} else {
		struct netif* interface;

		interface = getNetworkInterface(param->netifName + param->netifNameOffset);

		if (interface != NULL) {
			ip4_addr_set_u32((ip4_addr_t*)(param->address + param->addressOffset), *(u32_t*)netif_ip4_gw(interface));
			param->result = 0;
		} else {
			param->result = J_EUNKNOWN;
			if (param->error_message != NULL) {
				param->error_message = "interface not found";
			}
		}
	}

	LLECOM_NETWORK_DEBUG_TRACE("[%s:%u] get gateway result : %d (err %d)\n", __func__, __LINE__, param->result, param->error_code);
}

void LLECOM_NETWORK_IMPL_getNetmask_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_NETWORK_ip_context_t* param = (ECOM_NETWORK_ip_context_t*) job->params;

	if ((param->netifName == NULL) || (param->address == NULL)) {
		param->result = J_EINVAL;
		if (param->error_message != NULL) {
			param->error_message = "null pointer";
		}
	} else {
		struct netif* interface;

		interface = getNetworkInterface(param->netifName + param->netifNameOffset);

		if (interface != NULL) {
			ip4_addr_set_u32((ip4_addr_t*)(param->address + param->addressOffset), *(u32_t*)netif_ip4_netmask(interface));
			param->result = 0;
		} else {
			param->result = J_EUNKNOWN;
			if (param->error_message != NULL) {
				param->error_message = "interface not found";
			}
		}
	}

	LLECOM_NETWORK_DEBUG_TRACE("[%s:%u] get netmask result : %d (err %d)\n", __func__, __LINE__, param->result, param->error_code);
}

void LLECOM_NETWORK_IMPL_getIP_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_NETWORK_ip_context_t* param = (ECOM_NETWORK_ip_context_t*) job->params;

	if ((param->netifName == NULL) || (param->address == NULL)) {
		param->result = J_EINVAL;
		if (param->error_message != NULL) {
			param->error_message = "null pointer";
		}
	} else {
		struct netif* interface;

		interface = getNetworkInterface(param->netifName + param->netifNameOffset);

		if (interface != NULL) {
			ip4_addr_set_u32((ip4_addr_t*)(param->address + param->addressOffset), *(u32_t*)netif_ip4_addr(interface));
			param->result = 0;
		} else {
			param->result = J_EUNKNOWN;
			if (param->error_message != NULL) {
				param->error_message = "interface not found";
			}
		}
	}

	LLECOM_NETWORK_DEBUG_TRACE("[%s:%u] get ip address result : %d (err %d)\n", __func__, __LINE__, param->result, param->error_code);
}

void LLECOM_NETWORK_IMPL_isDNSStatic_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_NETWORK_netif_context_t* param = (ECOM_NETWORK_netif_context_t*) job->params;

	if (param->netifName == NULL) {
		param->result = J_EINVAL;
		if (param->error_message != NULL) {
			param->error_message = "null pointer";
		}
	} else {
		param->result = (ecom_network_is_static == 0) ? IF_DNS_NOT_STATIC : IF_DNS_STATIC;
	}

	LLECOM_NETWORK_DEBUG_TRACE("[%s:%u] is static DNS result : %d (err %d)\n", __func__, __LINE__, param->result, param->error_code);
}

void LLECOM_NETWORK_IMPL_isStatic_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_NETWORK_netif_context_t* param = (ECOM_NETWORK_netif_context_t*) job->params;

	if (param->netifName == NULL) {
		param->result = J_EINVAL;
		if (param->error_message != NULL) {
			param->error_message = "null pointer";
		}
	} else {
		param->result = (ecom_network_is_static == 0) ? IF_IP_DYNAMIC : IF_IP_STATIC;
	}

	LLECOM_NETWORK_DEBUG_TRACE("[%s:%u] is static IP result : %d (err %d)\n", __func__, __LINE__, param->result, param->error_code);
}

void LLECOM_NETWORK_IMPL_startConfiguration_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_NETWORK_netif_context_t* param = (ECOM_NETWORK_netif_context_t*) job->params;

	if (param->netifName == NULL) {
		param->result = J_EINVAL;
		if (param->error_message != NULL) {
			param->error_message = "null pointer";
		}
	} else {
		param->result = 0;
	}

	LLECOM_NETWORK_DEBUG_TRACE("[%s:%u] configuration started with result : %d (err %d)\n", __func__, __LINE__, param->result, param->error_code);
}

void LLECOM_NETWORK_IMPL_endConfiguration_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_NETWORK_netif_context_t* param = (ECOM_NETWORK_netif_context_t*) job->params;

	if (param->netifName == NULL) {
		param->result = J_EINVAL;
		if (param->error_message != NULL) {
			param->error_message = "null pointer";
		}
	} else {
		struct netif* interface;

		interface = getNetworkInterface(param->netifName + param->netifNameOffset);

		if (interface != NULL) {
			if (WIFI_ESP32_sta_apply_configuration_f(ecom_network_is_static, *netif_ip4_addr(interface), *netif_ip4_netmask(interface), *netif_ip4_gw(interface)) == true) {
				param->result = 0;
			} else {
				param->result = J_EUNKNOWN;
				if (param->error_message != NULL) {
					param->error_message = "wifi esp32 driver error";
				}
			}
		} else {
			param->result = J_EUNKNOWN;
			if (param->error_message != NULL) {
				param->error_message = "interface not found";
			}
		}
	}

	LLECOM_NETWORK_DEBUG_TRACE("[%s:%u] configuration ended with result : %d (err %d)\n", __func__, __LINE__, param->result, param->error_code);
}

void LLECOM_NETWORK_IMPL_setDNS_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_NETWORK_dns_context_t* param = (ECOM_NETWORK_dns_context_t*) job->params;

	if ((param->netifName == NULL) || (param->address == NULL) || (param->index < 0) || (param->index >= DNS_MAX_SERVERS)) {
		param->result = J_EINVAL;
		if (param->error_message != NULL) {
			param->error_message = "null pointer or wrong index";
		}
	} else {
		dns_setserver((u8_t)param->index, (ip_addr_t*)(param->address + param->addressOffset));
		param->result = 0;
	}

	LLECOM_NETWORK_DEBUG_TRACE("[%s:%u] set DNS result : %d (err %d)\n", __func__, __LINE__, param->result, param->error_code);
}

void LLECOM_NETWORK_IMPL_setGateway_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_NETWORK_ip_context_t* param = (ECOM_NETWORK_ip_context_t*) job->params;

	if ((param->netifName == NULL) || (param->address == NULL)) {
		param->result = J_EINVAL;
		if (param->error_message != NULL) {
			param->error_message = "null pointer";
		}
	} else {
		struct netif* interface;

		interface = getNetworkInterface(param->netifName + param->netifNameOffset);

		if (interface != NULL) {
			netif_set_gw(interface, (ip4_addr_t*)(param->address + param->addressOffset));
			param->result = 0;
		} else {
			param->result = J_EUNKNOWN;
			if (param->error_message != NULL) {
				param->error_message = "interface not found";
			}
		}
	}

	LLECOM_NETWORK_DEBUG_TRACE("[%s:%u] set gateway result : %d (err %d)\n", __func__, __LINE__, param->result, param->error_code);
}

void LLECOM_NETWORK_IMPL_setIP_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_NETWORK_ip_context_t* param = (ECOM_NETWORK_ip_context_t*) job->params;

	if ((param->netifName == NULL) || (param->address == NULL)) {
		param->result = J_EINVAL;
		if (param->error_message != NULL) {
			param->error_message = "null pointer";
		}
	} else {
		struct netif* interface;

		interface = getNetworkInterface(param->netifName + param->netifNameOffset);

		if (interface != NULL) {
			netif_set_ipaddr(interface, (ip4_addr_t*)(param->address + param->addressOffset));
			param->result = 0;
		} else {
			param->result = J_EUNKNOWN;
			if (param->error_message != NULL) {
				param->error_message = "interface not found";
			}
		}
	}

	LLECOM_NETWORK_DEBUG_TRACE("[%s:%u] set ip address result : %d (err %d)\n", __func__, __LINE__, param->result, param->error_code);
}

void LLECOM_NETWORK_IMPL_setNetmask_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_NETWORK_ip_context_t* param = (ECOM_NETWORK_ip_context_t*) job->params;

	if ((param->netifName == NULL) || (param->address == NULL)) {
		param->result = J_EINVAL;
		if (param->error_message != NULL) {
			param->error_message = "null pointer";
		}
	} else {
		struct netif* interface;

		interface = getNetworkInterface(param->netifName + param->netifNameOffset);

		if (interface != NULL) {
			netif_set_netmask(interface, (ip4_addr_t*)(param->address + param->addressOffset));
			param->result = 0;
		} else {
			param->result = J_EUNKNOWN;
			if (param->error_message != NULL) {
				param->error_message = "interface not found";
			}
		}
	}

	LLECOM_NETWORK_DEBUG_TRACE("[%s:%u] set netmask result : %d (err %d)\n", __func__, __LINE__, param->result, param->error_code);
}

void LLECOM_NETWORK_IMPL_useDHCP_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_NETWORK_ip_config_t* param = (ECOM_NETWORK_ip_config_t*) job->params;

	if (param->netifName == NULL) {
		param->result = J_EINVAL;
		if (param->error_message != NULL) {
			param->error_message = "null pointer";
		}
	} else {
		ecom_network_is_static = (param->use == JTRUE) ? 0 : 1;
		param->result = 0;
	}

	LLECOM_NETWORK_DEBUG_TRACE("[%s:%u] use DHCP result : %d (err %d)\n", __func__, __LINE__, param->result, param->error_code);
}

void LLECOM_NETWORK_IMPL_useStaticDNS_action(MICROEJ_ASYNC_WORKER_job_t* job) {

	ECOM_NETWORK_ip_config_t* param = (ECOM_NETWORK_ip_config_t*) job->params;

	if (param->netifName == NULL) {
		param->result = J_EINVAL;
		if (param->error_message != NULL) {
			param->error_message = "null pointer";
		}
	} else {
		if ((param->use == JTRUE) && !ecom_network_is_static) {
			puts("WARNING: a statically set DNS address will be overridden by DHCP when joining a network");
		}
		if ((param->use == JFALSE) && ecom_network_is_static) {
			puts("WARNING: a dynamically set DNS address will have no effect until enabling the DHCP");
		}
		param->result = 0;
	}

	LLECOM_NETWORK_DEBUG_TRACE("[%s:%u] use static DNS result : %d (err %d)\n", __func__, __LINE__, param->result, param->error_code);
}

void LLECOM_NETWORK_IMPL_setDefault_action(MICROEJ_ASYNC_WORKER_job_t* job) {
	/* Platform do not contain multiple network stacks, so this function does not need to be implemented */
}

#ifdef __cplusplus
}
#endif
