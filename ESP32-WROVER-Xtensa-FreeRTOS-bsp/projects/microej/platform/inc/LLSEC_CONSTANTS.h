/*
 * C
 *
 * Copyright 2014-2021 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */
#ifndef __LLSEC_CONSTANTS__
#define __LLSEC_CONSTANTS__

/**
 * @file
 * @brief MicroEJ Security low level API
 * @author MicroEJ Developer Team
 * @version 2.0.1
 * @date 20 December 2021
 */

#include <sni.h>

/**
 * PEM Certificate encoded format
 */
#define CERT_PEM_FORMAT (0)

/**
 * DER (ASN1) Certificate encoded format
 */
#define CERT_DER_FORMAT (1)

// comment this define to remove debug trace
//#define LLSEC_DEBUG


#ifdef LLSEC_DEBUG
#include <stdio.h>
#define LLSEC_DEBUG_DEBUG_TRACE 	printf
#define LLSEC_DEBUG_DEBUG_ASSERT(...) 	printf(__VA_ARGS__); while(1);
#else
#define LLSEC_DEBUG_DEBUG_TRACE(...) ((void) 0)
#define LLSEC_DEBUG_DEBUG_ASSERT(...) ((void) 0)
#endif

#endif //__LLSEC_CONSTANTS__
