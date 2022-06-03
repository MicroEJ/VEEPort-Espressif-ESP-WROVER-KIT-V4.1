/*
 * C
 *
 * Copyright 2020-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 *
 */

/**
 * @file
 * @brief MicroEJ startup.
 * @author MicroEJ Developer Team
 * @version 2.0.0
 * @date 15 September 2020
 */

#include <stdio.h>
#include "microej_main.h"
#include "LLMJVM.h"
#include "sni.h"

#ifdef __cplusplus
    extern "C" {
#endif

/**
 * @brief Creates and starts a MicroEJ instance. This function returns when the MicroEJ execution ends.
 */
void microej_main(int argc, char **argv) {
	void* vm;
	int32_t err;
	int32_t exitcode;

	// create VM
	vm = SNI_createVM();

	if (vm == NULL) {
		printf("MicroEJ initialization error.\n");
	} else {
		printf("MicroEJ START\n");

		// Error codes documentation is available in LLMJVM.h
		err = SNI_startVM(vm, argc, argv);

		if (err < 0) {
			// Error occurred
			if (err == LLMJVM_E_EVAL_LIMIT) {
				printf("Evaluation limits reached.\n");
			} else {
				printf("MicroEJ execution error (err = %d).\n", (int) err);
			}
		} else {
			// VM execution ends normally
			exitcode = SNI_getExitCode(vm);
			printf("MicroEJ END (exit code = %d)\n", (int) exitcode);
		}

		// delete VM
		SNI_destroyVM(vm);
	}
}

#ifdef __cplusplus
    }
#endif
