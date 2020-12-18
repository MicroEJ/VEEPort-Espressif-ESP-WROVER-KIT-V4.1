
/*
 * C
 * Copyright 2014-2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */

#include <string.h>
#include "pool.h"
#include "fs_helper_os.h"
#include "fs_helper.h"

#ifdef __cplusplus
	extern "C" {
#endif

//#define FS_DEBUG
#define FS_ERROR

#ifdef FS_ERROR
#include <stdio.h>
#endif

/** @brief private status for initialisation */
enum
{
	NO_INIT,
	INIT_PASSED,
};

/** @brief private init toggle */
static unsigned char guc_init_passed = NO_INIT;
static unsigned char media_init_passed = NO_INIT;

/** @brief private mailbox */
static FS_ADAPT_MAILBOX_DECLARE(gpu32_mailbox);

/** @brief private pool event */
static FS_event_t gpst_pool_event[FS_MAX_NUMBER_OF_EVENT_IN_MAILBOX];
static POOL_item_status_t gpst_pool_event_item_status[FS_MAX_NUMBER_OF_EVENT_IN_MAILBOX];
static POOL_ctx_t gst_pool_event_ctx =
{
	gpst_pool_event,
	gpst_pool_event_item_status,
	sizeof(FS_event_t),
	sizeof(gpst_pool_event)/sizeof(FS_event_t)
};

FS_status_t FS_init_f(void)
{
	FS_status_t e_return;
	FS_ADAPT_status_t e_adapt_return;

	/* Initialise the private mailbox */
	FS_ADAPT_INIT_MAILBOX(gpu32_mailbox, sizeof(FS_event_t*), FS_MAX_NUMBER_OF_EVENT_IN_MAILBOX);
	
	/* Initialise the private pool */
	memset(gpst_pool_event, 0, sizeof(gpst_pool_event));
	memset(gpst_pool_event_item_status, POOL_FREE, sizeof(gpst_pool_event_item_status));

	/* init adaptation layer */
	e_adapt_return = FS_ADAPT_init_f();
	
	if (e_adapt_return == FS_ADAPT_NO_ERROR)
	{
		/* init passed */
		guc_init_passed = INIT_PASSED;
		
		e_return = FS_NO_ERROR;
	}
	else
	{		
		e_return = FS_ADAPTATION_ERROR_INIT;
	}
	
	return (e_return);
}

FS_status_t FS_start_f(void)
{
	// Starting a thread depends on the underlying OS, hence we ask the ADAPT layer to do that
	FS_ADAPT_status_t e_adapt_return = FS_ADAPT_start_f();
	
	return e_adapt_return == FS_ADAPT_NO_ERROR ? FS_NO_ERROR : FS_ADAPTATION_ERROR_INIT;
}

FS_status_t FS_get_event(FS_event_t ** _pst_event)
{
	FS_status_t e_return;
	POOL_status_t e_pool_return;

	/* test init */
	if (guc_init_passed != NO_INIT)
	{
		/* test entry parameter */
		if (_pst_event != NULL)
		{
			/* allocate in pool */
			e_pool_return = POOL_reserve_f(&gst_pool_event_ctx, (void**)_pst_event);

			/* test if alloc is ok */
			if (e_pool_return == POOL_NO_ERROR)
			{
				e_return = FS_NO_ERROR;
			}
			else
			{
				#ifdef FS_ERROR
				printf("FS_ERROR [%s:%u] : get event no space availabe\n", __MODULE__, __LINE__);
				#endif
				e_return = FS_NO_SPACE_AVAILABLE;
			}
		}
		else
		{
				#ifdef FS_ERROR
				printf("FS_ERROR [%s:%u] : get event parameter error\n", __MODULE__, __LINE__);
				#endif
			e_return = FS_PARAMETER_ERROR;
		}
	}
	else
	{
		#ifdef FS_ERROR
		printf("FS_ERROR [%s:%u] : get event module not initialized\n", __MODULE__, __LINE__);
		#endif
		e_return = FS_MODULE_NOT_INITIALIZED;
	}

	return (e_return);
}

FS_status_t FS_release_event(FS_event_t * _pst_event)
{
	FS_status_t e_return;

	/* test init */
	if (guc_init_passed != NO_INIT)
	{
		/* test entry parameter */
		if (_pst_event != NULL)
		{
			/* release in pool */
			if (POOL_free_f(&gst_pool_event_ctx, (void*)_pst_event) != POOL_NO_ERROR)
			{
				#ifdef FS_ERROR
				printf("FS_ERROR [%s:%u] : release event error during freeing\n", __MODULE__, __LINE__);
				#endif
				e_return = FS_ERROR_DURING_FREEING_POOL;
			}
			else
			{
				e_return = FS_NO_ERROR;
			}
		}
		else
		{
			#ifdef FS_ERROR
			printf("FS_ERROR [%s:%u] : release event parameter error\n", __MODULE__, __LINE__);
			#endif
			e_return = FS_PARAMETER_ERROR;
		}
	}
	else
	{
		#ifdef FS_ERROR
		printf("FS_ERROR [%s:%u] : release event module not initialized\n", __MODULE__, __LINE__);
		#endif
		e_return = FS_MODULE_NOT_INITIALIZED;
	}

	return (e_return);
}

FS_status_t FS_post_event(FS_event_t * _pst_event)
{
	FS_status_t e_return;
	FS_ADAPT_status_t e_result;

	if (guc_init_passed != NO_INIT)
	{
		if (_pst_event != NULL)
		{
			/* update java thread into the event */
			e_result = FS_ADAPT_get_java_thread(&_pst_event->ul_id_java_thread);
			
			if (e_result == FS_ADAPT_NO_ERROR)
			{
				/* suspend java thread caller */
				if (FS_ADAPT_suspend_java_thread() == FS_ADAPT_NO_ERROR)
				{
					#ifdef FS_DEBUG
						printf("FS_DEBUG [%s:%u] : Java thread %lu post event %d \n", __MODULE__, __LINE__, _pst_event->ul_id_java_thread, _pst_event->e_id_event);
					#endif
					e_result = FS_ADAPT_post_event_action_f(&gpu32_mailbox, _pst_event, 0);

					if (e_result == FS_ADAPT_NO_ERROR)
					{					
						e_return = FS_NO_ERROR;
					}
					else
					{
						e_return = FS_NO_SPACE_AVAILABLE;					
					}		
				}
				else
				{
					e_return = FS_CANNOT_SUSPEND_JVM_THREAD;					
				}	
			}
			else
			{
				e_return = FS_CANNOT_GET_JVM_THREAD;
			}
		}
		else
		{
			#ifdef FS_ERROR
				printf("FS_ERROR [%s:%u] : post event error parameter\n", __MODULE__, __LINE__);
			#endif
			e_return = FS_PARAMETER_ERROR;
		}
	}
	else
	{
		#ifdef FS_ERROR
		printf("FS_ERROR [%s:%u] : post event module not initialized\n", __MODULE__, __LINE__);
		#endif
		e_return = FS_MODULE_NOT_INITIALIZED;
	}

	return (e_return);
}

FS_status_t FS_retrieve_event(FS_event_t ** _pst_event, unsigned long _ul_id_java_thread)
{
	FS_status_t e_return;
	unsigned char uc_found = 0;
	int i;

	/* test init */
	if (guc_init_passed != NO_INIT)
	{
		/* test entry parameter */
		if (_pst_event != NULL)
		{
			/* search the java id in all pool event */
			for (i=0;(i<sizeof(gpst_pool_event)/sizeof(FS_event_t)) && (uc_found != 1);i++)
			{
				if (gpst_pool_event[i].ul_id_java_thread == _ul_id_java_thread)
				{
					/* event found */
					uc_found = 1;
				}
			}
			
			if (uc_found != 0)
			{
				/* return the address of event found
         * due to the loop for i exit with i + 1 value */ 
				* _pst_event = &gpst_pool_event[i-1];
				e_return = FS_NO_ERROR;
			}
			else
			{
				/* error event not found */
				e_return = FS_EVENT_NOT_FOUND;
				#ifdef FS_ERROR
				printf("FS_ERROR [%s:%u] : retrieve event not found\n", __MODULE__, __LINE__);
				#endif
			}
		}
		else
		{
				#ifdef FS_ERROR
				printf("FS_ERROR [%s:%u] : retrieve event parameter error\n", __MODULE__, __LINE__);
				#endif
			e_return = FS_PARAMETER_ERROR;
		}
	}
	else
	{
		#ifdef FS_ERROR
		printf("FS_ERROR [%s:%u] : get retrieve module not initialized\n", __MODULE__, __LINE__);
		#endif
		e_return = FS_MODULE_NOT_INITIALIZED;
	}

	return (e_return);	
}

void FS_run_f(void)
{
	FS_event_t * pst_event;
	FS_ADAPT_status_t e_result;
	
	if (guc_init_passed != NO_INIT)
	{
		/* forever */
		for(;;)
		{		
			/* wait a message (indefinite timeout) */
			e_result = FS_ADAPT_wait_event(&gpu32_mailbox, (void**)&pst_event, FS_ADAPT_INFITIVE_TIMEOUT);

			#ifdef FS_DEBUG
				printf("FS_DEBUG [%s:%u] : event %d received !!!\n", __MODULE__, __LINE__, pst_event->e_id_event);
			#endif
			
			/* test received function */
			if (e_result != FS_ADAPT_NO_ERROR)
			{
				/* log error FS_UNEXPECTED_ERROR_IN_RECEIVED; */
				#ifdef FS_ERROR
					printf("FS_ERROR [%s:%u] : run unexpected error in received\n", __MODULE__, __LINE__);
				#endif
			}
			else
			{
				if (media_init_passed == NO_INIT)
				{
					BYTE pdrv = 0xFF;
					FS_ADAPT_status_t err = FS_ADAPT_init_media(pdrv);
					if (err != FS_ADAPT_NO_ERROR)
					{
						#ifdef FS_ERROR
							printf("FS_ERROR [%s:%u] : sdmmc initialization failed (error %d)\n", __MODULE__, __LINE__, err);
						#endif
					}
					else
					{
						media_init_passed = INIT_PASSED;
					}

				}

				/* process event */
				switch (pst_event->e_id_event)
				{
					case (FS_OPEN_FILE_EVENT) :
						FS_ADAPT_open_file_f(pst_event->u_ctx_event.st_open_file_ctx.pc_path_in,
						                     pst_event->u_ctx_event.st_open_file_ctx.uc_mode_in,
						                     &pst_event->u_ctx_event.st_open_file_ctx.i_fd_out);
						break;
					case (FS_CLOSE_FILE_EVENT) :
						FS_ADAPT_close_file_f(pst_event->u_ctx_event.st_close_file_ctx.ui_fd_in,
						                      &pst_event->u_ctx_event.st_close_file_ctx.uc_bool_out);
						break;
					case (FS_READ_FILE_EVENT) :
						FS_ADAPT_read_file_f(pst_event->u_ctx_event.st_read_file_ctx.ui_fd_in,
						                     pst_event->u_ctx_event.st_read_file_ctx.pc_output_buffer_in,
						                     pst_event->u_ctx_event.st_read_file_ctx.ui_data_length_in,
						                     &pst_event->u_ctx_event.st_read_file_ctx.i_error_out);
						break;
					case (FS_WRITE_FILE_EVENT) :
						FS_ADAPT_write_file_f(pst_event->u_ctx_event.st_write_file_ctx.ui_fd_in,
						                      pst_event->u_ctx_event.st_write_file_ctx.pc_intput_buffer_in,
						                      pst_event->u_ctx_event.st_write_file_ctx.ui_data_length_in,
						                      &pst_event->u_ctx_event.st_write_file_ctx.i_error_out);
						break;
					case (FS_EXIST_FILE_EVENT) :
						FS_ADAPT_exist_file_f(pst_event->u_ctx_event.st_exist_file_ctx.pc_path_in,
						                      &pst_event->u_ctx_event.st_exist_file_ctx.uc_bool_out);
						break;
					case (FS_WRITE_BYTE_FILE_EVENT) :
						FS_ADAPT_write_byte_file_f(pst_event->u_ctx_event.st_write_byte_file_ctx.ui_fd_in,
						                      pst_event->u_ctx_event.st_write_byte_file_ctx.i_data_in,
						                      &pst_event->u_ctx_event.st_write_byte_file_ctx.i_error_out);
						break;
					case (FS_READ_BYTE_FILE_EVENT) :
						FS_ADAPT_read_byte_file_f(pst_event->u_ctx_event.st_read_byte_file_ctx.ui_fd_in,
						                      &pst_event->u_ctx_event.st_read_byte_file_ctx.i_error_out);
						break;
					case (FS_SKIP_FILE_EVENT) :
						FS_ADAPT_skip_file_f(pst_event->u_ctx_event.st_skip_file_ctx.ui_fd_in,
											  pst_event->u_ctx_event.st_skip_file_ctx.l_n_in,
								              &pst_event->u_ctx_event.st_skip_file_ctx.l_error_out);
						break;
					case (FS_AVAILABLE_FILE_EVENT) :
						FS_ADAPT_available_file_f(pst_event->u_ctx_event.st_available_file_ctx.ui_fd_in,
								              &pst_event->u_ctx_event.st_available_file_ctx.i_error_out);
						break;
					case (FS_GET_FREE_SPACE_EVENT) :
						FS_ADAPT_get_free_space_f(pst_event->u_ctx_event.st_get_free_space_ctx.pc_path_in,
			                                &pst_event->u_ctx_event.st_get_free_space_ctx.ul_size_out);
						break;
					case (FS_GET_TOTAL_SPACE_EVENT) :
						FS_ADAPT_get_total_space_f(pst_event->u_ctx_event.st_get_total_space_ctx.pc_path_in,
						                           &pst_event->u_ctx_event.st_get_total_space_ctx.ul_size_out);
						break;
					case (FS_GET_USABLE_SPACE_EVENT) :
						FS_ADAPT_get_usable_space_f(pst_event->u_ctx_event.st_get_usable_space_ctx.pc_path_in,
						                            &pst_event->u_ctx_event.st_get_usable_space_ctx.ul_size_out);
						break;
					case (FS_IS_DIRECTORY_EVENT) :
						FS_ADAPT_is_directory_f(pst_event->u_ctx_event.st_is_directory_ctx.pc_path_in,
						                        &pst_event->u_ctx_event.st_is_directory_ctx.uc_bool_out);
						break;
					case (FS_IS_FILE_EVENT) :
						FS_ADAPT_is_file_f(pst_event->u_ctx_event.st_is_file_ctx.pc_path_in,
						                   &pst_event->u_ctx_event.st_is_file_ctx.uc_bool_out);
						break;
					case (FS_IS_HIDDEN_EVENT) :
						FS_ADAPT_is_hidden_f(pst_event->u_ctx_event.st_is_hidden_ctx.pc_path_in,
						                     &pst_event->u_ctx_event.st_is_hidden_ctx.uc_bool_out);
						break;
					case (FS_GET_LAST_MODIFIED_EVENT) :
						FS_ADAPT_get_last_modified_f(pst_event->u_ctx_event.st_get_last_modified_ctx.pc_path_in,
						                             pst_event->u_ctx_event.st_get_last_modified_ctx.pi_data_out,
						                             &pst_event->u_ctx_event.st_get_last_modified_ctx.uc_bool_out);
						break;
					case (FS_SET_LAST_MODIFIED_EVENT) :
						FS_ADAPT_set_last_modified_f(pst_event->u_ctx_event.st_set_last_modified_ctx.pc_path_in,
						                             pst_event->u_ctx_event.st_set_last_modified_ctx.i_years_in,
						                             pst_event->u_ctx_event.st_set_last_modified_ctx.i_month_in,
						                             pst_event->u_ctx_event.st_set_last_modified_ctx.i_day_in,
						                             pst_event->u_ctx_event.st_set_last_modified_ctx.i_hour_in,
						                             pst_event->u_ctx_event.st_set_last_modified_ctx.i_minute_in,
						                             pst_event->u_ctx_event.st_set_last_modified_ctx.i_second_in,
						                             &pst_event->u_ctx_event.st_set_last_modified_ctx.uc_bool_out);
						break;
					case (FS_SET_READ_ONLY_EVENT) :
						FS_ADAPT_set_read_only_f(pst_event->u_ctx_event.st_set_read_only_ctx.pc_path_in,
						                         &pst_event->u_ctx_event.st_set_read_only_ctx.uc_bool_out);
						break;
					case (FS_GET_LENGTH) :
						FS_ADAPT_get_length_f(pst_event->u_ctx_event.st_get_length_ctx.pc_path_in,
						                      &pst_event->u_ctx_event.st_get_length_ctx.ul_size_out);
						break;
					case (FS_MK_DIR_EVENT) :
						FS_ADAPT_mk_dir_f(pst_event->u_ctx_event.st_mk_dir_ctx.pc_path_in,
						                  &(pst_event->u_ctx_event.st_mk_dir_ctx.uc_bool_out));
						break;
					case (FS_OPEN_DIR_EVENT) :
						FS_ADAPT_open_dir_f(pst_event->u_ctx_event.st_open_dir_ctx.pc_path_in,
						                    &pst_event->u_ctx_event.st_open_dir_ctx.i_fd_out);
						break;
					case (FS_READ_DIR_EVENT) :
						FS_ADAPT_read_dir_f(pst_event->u_ctx_event.st_read_dir_ctx.ui_fd_in,
						                    pst_event->u_ctx_event.st_read_dir_ctx.pc_filename_out,
						                    &pst_event->u_ctx_event.st_read_dir_ctx.uc_bool_out);
						break;
					case (FS_CLOSE_DIR_EVENT) :
						FS_ADAPT_close_dir_f(pst_event->u_ctx_event.st_close_dir_ctx.ui_fd_in,
						                     &pst_event->u_ctx_event.st_close_dir_ctx.uc_bool_out);
						break;
					case (FS_RENAME_EVENT) :
						FS_ADAPT_rename_f(pst_event->u_ctx_event.st_rename_ctx.pc_old_name_in,
						                  pst_event->u_ctx_event.st_rename_ctx.pc_new_name_in,
						                  &pst_event->u_ctx_event.st_rename_ctx.uc_bool_out);
						break;
					case (FS_CREATE_EVENT) :
						FS_ADAPT_create_f(pst_event->u_ctx_event.st_create_ctx.pc_path_in,
						                  &pst_event->u_ctx_event.st_create_ctx.status_error_out);
						break;
					case (FS_DELETE_EVENT) :
						FS_ADAPT_delete_f(pst_event->u_ctx_event.st_delete_ctx.pc_path_in,
						                  &pst_event->u_ctx_event.st_delete_ctx.uc_bool_out);
						break;
					case (FS_CHECK_ACCESS) :
						FS_ADAPT_check_access_f(pst_event->u_ctx_event.st_check_access_ctx.pc_path_in,
											 pst_event->u_ctx_event.st_check_access_ctx.access_in,
						                     &pst_event->u_ctx_event.st_check_access_ctx.uc_bool_out);
						break;
					case (FS_SET_PERMISSION) :
						FS_ADAPT_set_permission_f(pst_event->u_ctx_event.st_set_permission_ctx.pc_path_in,
						                          pst_event->u_ctx_event.st_set_permission_ctx.access_in,
						                          pst_event->u_ctx_event.st_set_permission_ctx.uc_enable_in,
						                          pst_event->u_ctx_event.st_set_permission_ctx.uc_owner_only_in,
						                          &pst_event->u_ctx_event.st_set_permission_ctx.uc_bool_out);
						break;
					default :
						;				
						#ifdef FS_ERROR
						printf("FS_ERROR [%s:%u] : run unknown event received\n", __MODULE__, __LINE__);
						#endif
						/* theoretically impossible */

				}
				
				#ifdef FS_DEBUG
					printf("FS_DEBUG [%s:%u] : event %d processed !!!\n", __MODULE__, __LINE__, pst_event->e_id_event);
				#endif

				/* resume JVM thread caller */
				if (FS_ADAPT_resume_java_thread(pst_event->ul_id_java_thread) != FS_ADAPT_NO_ERROR)
				{
						#ifdef FS_ERROR
							printf("FS_ERROR [%s:%u] : cannot resume java thread\n", __MODULE__, __LINE__);
						#endif
				}
				else
				{
					pst_event = NULL;
					
					/* yield cpu if needed */
					FS_ADAPT_YIELD(10);
				}
			}
		}
	}
	else
	{
		#ifdef FS_ERROR
		printf("FS_ERROR [%s:%u] : run module not initialized\n", __MODULE__, __LINE__);
		#endif
	}
}

#ifdef __cplusplus
	}
#endif
