/*
 * C
 *
 * Copyright 2021-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */
 
/**
 * @file
 * @brief Security natives configuration.
 * @author MicroEJ Developer Team
 * @version 1.0.0
 */

#include "microej_allocator.h"

#define LLSEC_calloc microej_calloc
#define LLSEC_free microej_free

/*
* Used for private and public key generation
*/
#define LLSEC_PRIVATE_KEY_LOCAL_BUFFER_SIZE 3072
#define LLSEC_PUBLIC_KEY_LOCAL_BUFFER_SIZE  3072
