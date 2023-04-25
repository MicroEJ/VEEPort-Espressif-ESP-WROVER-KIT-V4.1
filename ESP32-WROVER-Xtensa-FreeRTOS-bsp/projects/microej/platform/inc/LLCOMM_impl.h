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
/* 
 * Implementation header file. Shall only be included by client implementation C files.
 */
#include <LLCOMM.h>
#include <stdint.h>
#include <intern/LLCOMM_impl.h>
// --------------------------------------------------------------------------------
// -                      Functions that must be implemented                      -
// --------------------------------------------------------------------------------

/**
 * This function is the first function called.
 * Some connections can be added using {@link LLCOMM_addConnection}. 
 * These connections are considered to be static connections and cannot be removed.
 */
void LLCOMM_IMPL_initialize(void);

/**
 * This function is called before accessing the pool of connections, from initialize/add/remove/open/close operations.
 * If the implementation dynamically adds or removes connections, it must implement a reentrant synchronization mechanism to avoid execution of parallel operations.
 * If the implementation does not dynamically add or remove connections, it can do nothing.
 */
void LLCOMM_IMPL_syncConnectionsEnter(void);

/**
 * This function is called after modifying the array of connections.
 * If the implementation dynamically adds or removes connections, it must implement a reentrant synchronization mechanism to avoid execution of parallel operations.
 * If the implementation does not dynamically add or remove connections, it can do nothing.
 */
void LLCOMM_IMPL_syncConnectionsExit(void);
