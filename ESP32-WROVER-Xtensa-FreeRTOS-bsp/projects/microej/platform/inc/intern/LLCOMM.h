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
#ifndef _LLCOMM_intern
#define _LLCOMM_intern 
typedef struct LLCOMM LLCOMM;
#include <stdint.h>
#ifdef __cplusplus
	extern "C" {
#endif
#define LLCOMM_addConnection __icetea__virtual__com_is2t_comm_CommInterface___addConnection__Lcom_is2t_comm_StaticCommStream_2
#define LLCOMM_removeConnection __icetea__virtual__com_is2t_comm_CommInterface___removeConnection__Lcom_is2t_comm_StaticCommStream_2
#define LLCOMM_initialize __icetea__virtual__com_is2t_comm_CommInterface___initialize
#define LLCOMM_syncConnectionsEnter __icetea__virtual__com_is2t_comm_CommInterface___syncConnectionsEnter
#define LLCOMM_syncConnectionsExit __icetea__virtual__com_is2t_comm_CommInterface___syncConnectionsExit
struct LLCOMM
{
	int32_t __icetea__id__;
};
#ifdef __cplusplus
	}
#endif
#endif
