/*
 * C
 *
 * Copyright 2018-2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */

#include "microjvm_main.h"

#include <stdio.h>
#include "LLMJVM.h"
#include "sni.h"

/*
 * Generic VM main function
 */
void microjvm_main(void)
{
	void* vm;
	int32_t err;
	int32_t exitcode;
	
	// create VM
	vm = SNI_createVM();

	if(vm == NULL)
	{
		printf("VM initialization error.\n");
	}
	else
	{
		printf("VM START\n");
		err = SNI_startVM(vm, 0, NULL);

		if(err < 0)
		{
			// Error occurred
			if(err == LLMJVM_E_EVAL_LIMIT)
			{
				printf("Evaluation limits reached.\n");
			}
			else
			{
				printf("VM execution error (err = %d).\n", (int)err);
			}
		}
		else
		{
			// VM execution ends normally
			exitcode = SNI_getExitCode(vm);
			printf("VM END (exit code = %d)\n", (int)exitcode);
		}

		// delete VM
		SNI_destroyVM(vm);
	}
}
