/* 
 * IceTea
 * 
 * Copyright 2014 IS2T. All rights reserved.
 * Modification and distribution is permitted under certain conditions.
 * IS2T PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */
/* 
 * This file is auto-generated - DO NOT EDIT IT
 */
/**
 * ECOM-COMM connections initialization
 */
#ifndef _LLCOMM
#define _LLCOMM 
#include <intern/LLCOMM.h>
#include <stdint.h>
#include <LLCOMM_CONNECTION.h>
#ifdef __cplusplus
	extern "C" {
#endif
// --------------------------------------------------------------------------------
// -                      Functions provided by the library                       -
// --------------------------------------------------------------------------------

/**
 * Add a new connection to the pool of connections.
 * @return 1 if the connection has been added, 0 if the maximum number of connections is reached or the connection already exists
 */
uint8_t LLCOMM_addConnection(LLCOMM_CONNECTION* connection);

/**
 * Remove a connection that was previously added to the pool of connections using {@link LLCOMM_addConnection}.
 * The connection will be effectively released (and thus eligible to be reused) once {@link LLCOMM_CONNECTION_released} is called.
 * @return 0 if the connection is unknown or was not dynamically added, 1 otherwise
 */
uint8_t LLCOMM_removeConnection(LLCOMM_CONNECTION* connection);

/**
 * This function is the first function called.
 * Some connections can be added using {@link LLCOMM_addConnection}. 
 * These connections are considered to be static connections and cannot be removed.
 */
void LLCOMM_initialize(void);

/**
 * This function is called before accessing the pool of connections, from initialize/add/remove/open/close operations.
 * If the implementation dynamically adds or removes connections, it must implement a reentrant synchronization mechanism to avoid execution of parallel operations.
 * If the implementation does not dynamically add or remove connections, it can do nothing.
 */
void LLCOMM_syncConnectionsEnter(void);

/**
 * This function is called after modifying the array of connections.
 * If the implementation dynamically adds or removes connections, it must implement a reentrant synchronization mechanism to avoid execution of parallel operations.
 * If the implementation does not dynamically add or remove connections, it can do nothing.
 */
void LLCOMM_syncConnectionsExit(void);

#ifdef __cplusplus
	}
#endif
#endif
