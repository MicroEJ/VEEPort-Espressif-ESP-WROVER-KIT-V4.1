/*
 * C
 *
 *  Copyright 2020-2023 MicroEJ Corp. All rights reserved.
 *  Use of this source code is governed by a BSD-style license that can be found with this software.
 *
 */

/**
 * @file
 * @brief MicroEJ startup.
 * @author MicroEJ Developer Team
 * @version 2.1.0
 * @date 17 June 2022
 */

#ifndef MICROEJ_MAIN_H_
#define MICROEJ_MAIN_H_

#ifdef __cplusplus
    extern "C" {
#endif

/**
 * @brief Creates and starts a MicroEJ instance. This function returns when the MicroEJ execution ends.
 * @param argc arguments count
 * @param argv arguments vector
 */
void microej_main(int argc, char **argv);

#ifdef __cplusplus
    }
#endif

#endif /* MICROEJ_MAIN_H_ */
