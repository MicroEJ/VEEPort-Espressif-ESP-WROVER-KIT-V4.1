/*
 * C
 *
 * Copyright 2020-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#ifndef LLECOM_WIFI_H
#define LLECOM_WIFI_H

/**
 * @file
 * @brief ECOM-WIFI exported interface.
 * @author MicroEJ Developer Team
 * @version 2.1.0
 * @date 17 June 2021
 */

#ifdef __cplusplus
	extern "C" {
#endif

/**
 * @brief High level interface used for generic ecom-wifi module initialization.
 * It initializes internal components, like the worker and the helper.
 */
void LLECOM_WIFI_initialize(void);

#ifdef __cplusplus
	}
#endif

#endif /* LLECOM_WIFI_H */
