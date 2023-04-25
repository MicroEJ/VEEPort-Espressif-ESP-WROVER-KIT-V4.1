/* 
 * C
 * 
 * Copyright 2014-2016 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */
/* 
 * This file is auto-generated - DO NOT EDIT IT
 */
/* 
 * IceTea to C mapping support. Shall not be directly included by clients.
 */
typedef struct LLKERNEL LLKERNEL;
#include <stdint.h>
#define LLKERNEL_CONCAT0(p,s) p ## s
#define LLKERNEL_CONCAT(p,s) LLKERNEL_CONCAT0(p,s)
#define LLKERNEL_IMPL_allocate LLKERNEL_CONCAT(LLKERNEL_IMPL,_allocate__I)
#define LLKERNEL_IMPL_free LLKERNEL_CONCAT(LLKERNEL_IMPL,_free__LiceTea_lang_Ram_2)
struct LLKERNEL
{
	int32_t __icetea__id__;
};
LLKERNEL LLKERNEL_getSingleton_instance;

/* 
 * Get the unique instance of this type within the system.
 */
LLKERNEL* LLKERNEL_IMPL_getSingleton(void)
{
	return &LLKERNEL_getSingleton_instance;
}
