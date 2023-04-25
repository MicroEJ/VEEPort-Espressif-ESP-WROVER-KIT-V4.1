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
 * IceTea to C mapping support. Shall not be directly included by clients.
 */
#include <stdint.h>
#define LLCOMM_CONCAT0(p,s) p ## s
#define LLCOMM_CONCAT(p,s) LLCOMM_CONCAT0(p,s)
#define LLCOMM_IMPL_initialize LLCOMM_CONCAT(LLCOMM_IMPL,_initialize)
#define LLCOMM_IMPL_syncConnectionsEnter LLCOMM_CONCAT(LLCOMM_IMPL,_syncConnectionsEnter)
#define LLCOMM_IMPL_syncConnectionsExit LLCOMM_CONCAT(LLCOMM_IMPL,_syncConnectionsExit)
LLCOMM LLCOMM_getSingleton_instance;

/* 
 * Get the unique instance of this type within the system.
 */
LLCOMM* LLCOMM_IMPL_getSingleton(void)
{
	return &LLCOMM_getSingleton_instance;
}
