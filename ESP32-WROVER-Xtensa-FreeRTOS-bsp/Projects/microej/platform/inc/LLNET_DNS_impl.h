/*
 * C
 *
 * Copyright 2016 IS2T. All rights reserved.
 * For demonstration purpose only.
 * IS2T PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */
#ifndef LLNET_DNS_IMPL
#define LLNET_DNS_IMPL

#include <sni.h>
#include <intern/LLNET_DNS_impl.h>
#include <LLNET_ERRORS.h>

#ifdef __cplusplus
	extern "C" {
#endif


/**
 * Return 1 if the DNS servers list changed in the native part, 0 otherwise.
 * @return 1 if the DNS servers list changed in the native part, 0 otherwise.
 */
uint8_t LLNET_DNS_IMPL_dnsServersListHasChanged(void);

/**
 * Gets the DNS servers IP addresses list.
 * <p>When this method returns, the associated DNS servers IP are stored in the input buffer {@code host}.
 * @param serversAddr the input buffer in which DNS server IP addresses will be stored
 * @param offset the offset of the input buffer
 * @param length the input buffer length
 * @param retry true when the previous call returned {@link J_NATIVE_CODE_BLOCKED_WITHOUT_RESULT}
 * and the calling process repeats the call to this operation for its completion
 * @return the number of DNS server IP address stored in the input buffer or a negative error code
 * @see {@link LLNET_ERRORS} header file for error codes
 */
int32_t LLNET_DNS_IMPL_getIPv4DNSServersList(int8_t* serversAddr, int32_t offset, int32_t length, uint8_t retry);

#ifdef __cplusplus
	}
#endif
#endif
