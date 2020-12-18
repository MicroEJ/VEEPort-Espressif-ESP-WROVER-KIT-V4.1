/* 
 * C
 * Copyright 2018-2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */

/* Includes ------------------------------------------------------------------*/

#include "LLFS_impl.h"
#include "sni.h"
#include "fs_helper.h"

#ifdef __cplusplus
	extern "C" {
#endif

/* Private functions ---------------------------------------------------------*/

/** @brief Returns the FS_access_t that corresponds to the given LLFS_ACCESS_* */
FS_access_t getFSAccessFromJAccess(int32_t jaccess){
	switch(jaccess){
	case LLFS_ACCESS_READ:
		return FS_ACCESS_READ;
	case LLFS_ACCESS_WRITE:
		return FS_ACCESS_WRITE;
	default:
		return FS_ACCESS_EXECUTE;
	}
}

/* Public API ----------------------------------------------------------------*/

int32_t LLFS_IMPL_exist(uint8_t* path, int32_t action) 
{
	FS_event_t * pst_event;
	int32_t e_return;

	if (action == LLFS_EVENT_POST)
	{
		if (FS_get_event(&pst_event) != FS_NO_ERROR)
		{
			e_return = LLFS_NOK;
		}
		else
		{
			pst_event->e_id_event = FS_EXIST_FILE_EVENT;
			pst_event->u_ctx_event.st_exist_file_ctx.pc_path_in =(char *)path;

			if (FS_post_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				FS_release_event(pst_event);
				e_return = LLFS_NOK;
			}
			else
			{
				e_return = LLFS_OK;
			}
		}
	}
	else /* else it's second execution the thread has just been resumed by end of delegate process */
	{
		/* retrieve event in pool according to java thread id */
		if (FS_retrieve_event(&pst_event, SNI_getCurrentJavaThreadID()) != FS_NO_ERROR)
		{
			/* an error occurred */
			e_return = LLFS_NOK;
		}
		else
		{
			/* return value from context */
			if (pst_event->u_ctx_event.st_exist_file_ctx.uc_bool_out != 0)
			{
				e_return = LLFS_NOK;
			}
			else
			{
				e_return = LLFS_OK;
			}

			/* release pool event */
			if (FS_release_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				e_return = LLFS_NOK;
			}		
		}
	}

	return (e_return);
}

int64_t LLFS_IMPL_get_free_space(uint8_t* path, int32_t action)
{
	FS_event_t * pst_event;
	int64_t l_return;

	if (action == LLFS_EVENT_POST)
	{
		if (FS_get_event(&pst_event) != FS_NO_ERROR)
		{
			l_return = LLFS_NOK;
		}
		else
		{
			pst_event->e_id_event = FS_GET_FREE_SPACE_EVENT;
			pst_event->u_ctx_event.st_get_free_space_ctx.pc_path_in = (char *)path;

			if (FS_post_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				FS_release_event(pst_event);
				l_return =LLFS_NOK;
			}
			else
			{
				l_return = LLFS_OK;
			}
		}
	}
	else /* else it's second execution the thread has just been resumed by end of delegate process */
	{
		/* retrieve event in pool according to java thread id */
		if (FS_retrieve_event(&pst_event, SNI_getCurrentJavaThreadID()) != FS_NO_ERROR)
		{
			/* an error occurred */
			l_return = LLFS_NOK;
		}
		else
		{
			/* return value from context */
			l_return = pst_event->u_ctx_event.st_get_free_space_ctx.ul_size_out;

			/* release pool event */
			if (FS_release_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				l_return = LLFS_NOK;
			}	
		}
	}

	return (l_return);
}

int64_t LLFS_IMPL_get_total_space(uint8_t* path, int32_t action)
{
	FS_event_t * pst_event;
	int64_t l_return;

	if (action == LLFS_EVENT_POST)
	{
		if (FS_get_event(&pst_event) != FS_NO_ERROR)
		{
			l_return = LLFS_NOK;
		}
		else
		{
			pst_event->e_id_event = FS_GET_TOTAL_SPACE_EVENT;
			pst_event->u_ctx_event.st_get_total_space_ctx.pc_path_in = (char *)path;

			if (FS_post_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				FS_release_event(pst_event);
				l_return = LLFS_NOK;
			}
			else
			{
				l_return = LLFS_OK;
			}
		}
	}
	else /* else it's second execution the thread has just been resumed by end of delegate process */
	{
		/* retrieve event in pool according to java thread id */
		if (FS_retrieve_event(&pst_event, SNI_getCurrentJavaThreadID()) != FS_NO_ERROR)
		{
			/* an error occurred */
			l_return = LLFS_NOK;
		}
		else
		{
			/* return value from context */
			l_return = pst_event->u_ctx_event.st_get_total_space_ctx.ul_size_out;

			/* release pool event */
			if (FS_release_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				l_return = LLFS_NOK;
			}
		}
	}

	return (l_return);
	
}

int64_t LLFS_IMPL_get_usable_space(uint8_t* path, int32_t action)
{
	FS_event_t * pst_event;
	int64_t l_return;

	if (action == LLFS_EVENT_POST)
	{
		if (FS_get_event(&pst_event) != FS_NO_ERROR)
		{
			l_return = LLFS_NOK;
		}
		else
		{
			pst_event->e_id_event = FS_GET_USABLE_SPACE_EVENT;
			pst_event->u_ctx_event.st_get_usable_space_ctx.pc_path_in = (char *)path;

			if (FS_post_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				FS_release_event(pst_event);
				l_return = LLFS_NOK;
			}
			else
			{
				l_return = LLFS_OK;
			}
		}
	}
	else /* else it's second execution the thread has just been resumed by end of delegate process */
	{
		/* retrieve event in pool according to java thread id */
		if (FS_retrieve_event(&pst_event, SNI_getCurrentJavaThreadID()) != FS_NO_ERROR)
		{
			/* an error occurred */
			l_return = LLFS_NOK;
		}
		else
		{
			/* return value from context */
			l_return = pst_event->u_ctx_event.st_get_usable_space_ctx.ul_size_out;

			/* release pool event */
			if (FS_release_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				l_return = LLFS_NOK;
			}
		}
	}

	return (l_return);
}

int32_t LLFS_IMPL_initialize(void)
{
	if(FS_init_f() != FS_NO_ERROR)
	{
		// Initialization failed!
		return LLFS_NOK;
	}
	
	if(FS_start_f() != FS_NO_ERROR)
	{
		// Thread not started!
		return LLFS_NOK;
	}
	
	// So far so good
	return LLFS_OK;
}

int32_t LLFS_IMPL_is_directory(uint8_t* path, int32_t action)
{
	FS_event_t * pst_event;
	int32_t e_return;

	if (action == LLFS_EVENT_POST)
	{
		if (FS_get_event(&pst_event) != FS_NO_ERROR)
		{
			e_return = LLFS_NOK;
		}
		else
		{
			pst_event->e_id_event = FS_IS_DIRECTORY_EVENT;
			pst_event->u_ctx_event.st_is_directory_ctx.pc_path_in = (char *)path;

			if (FS_post_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				FS_release_event(pst_event);
				e_return = LLFS_NOK;
			}
			else
			{
				e_return = LLFS_OK;
			}
		}
	}
	else /* else it's second execution the thread has just been resumed by end of delegate process */
	{
		/* retrieve event in pool according to java thread id */
		if (FS_retrieve_event(&pst_event, SNI_getCurrentJavaThreadID()) != FS_NO_ERROR)
		{
			/* an error occurred */
			e_return = LLFS_NOK;
		}
		else
		{
			/* return value from context */
			if (pst_event->u_ctx_event.st_is_directory_ctx.uc_bool_out != 0)
			{
				e_return = LLFS_NOK;
			}
			else
			{
				e_return = LLFS_OK;
			}

			/* release pool event */
			if (FS_release_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				e_return = LLFS_NOK;
			}
		}
	}

	return (e_return);
}

int32_t LLFS_IMPL_is_file(uint8_t* path, int32_t action)
{
	FS_event_t * pst_event;
	int32_t e_return;

	if (action == LLFS_EVENT_POST)
	{
		if (FS_get_event(&pst_event) != FS_NO_ERROR)
		{
			e_return = LLFS_NOK;
		}
		else
		{
			pst_event->e_id_event = FS_IS_FILE_EVENT;
			pst_event->u_ctx_event.st_is_file_ctx.pc_path_in = (char *)path;

			if (FS_post_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				FS_release_event(pst_event);
				e_return = LLFS_NOK;
			}
			else
			{
				e_return = LLFS_OK;
			}
		}
	}
	else /* else it's second execution the thread has just been resumed by end of delegate process */
	{
		/* retrieve event in pool according to java thread id */
		if (FS_retrieve_event(&pst_event, SNI_getCurrentJavaThreadID()) != FS_NO_ERROR)
		{
			/* an error occurred */
			e_return = LLFS_NOK;
		}
		else
		{
			/* return value from context */
			if (pst_event->u_ctx_event.st_is_file_ctx.uc_bool_out != 0)
			{
				e_return = LLFS_NOK;
			}
			else
			{
				e_return = LLFS_OK;
			}

			/* release pool event */
			if (FS_release_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				e_return = LLFS_NOK;
			}
		}
	}

	return (e_return);
}

int32_t LLFS_IMPL_is_hidden(uint8_t* path, int32_t action)
{
	FS_event_t * pst_event;
	int32_t e_return;

	if (action == LLFS_EVENT_POST)
	{
		if (FS_get_event(&pst_event) != FS_NO_ERROR)
		{
			e_return = LLFS_NOK;
		}
		else
		{
			pst_event->e_id_event = FS_IS_HIDDEN_EVENT;
			pst_event->u_ctx_event.st_is_hidden_ctx.pc_path_in = (char *)path;

			if (FS_post_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				FS_release_event(pst_event);
				e_return = LLFS_NOK;
			}
			else
			{
				e_return = LLFS_OK;
			}
		}
	}
	else /* else it's second execution the thread has just been resumed by end of delegate process */
	{
		/* retrieve event in pool according to java thread id */
		if (FS_retrieve_event(&pst_event, SNI_getCurrentJavaThreadID()) != FS_NO_ERROR)
		{
			/* an error occurred */
			e_return = LLFS_NOK;
		}
		else
		{
			/* return value from context */
			if (pst_event->u_ctx_event.st_is_hidden_ctx.uc_bool_out != 0)
			{
				e_return = LLFS_NOK;
			}
			else
			{
				e_return = LLFS_OK;
			}

			/* release pool event */
			if (FS_release_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				e_return = LLFS_NOK;
			}
		}
	}

	return (e_return);
}

int32_t LLFS_IMPL_get_last_modified(uint8_t* path, int32_t* date, int32_t action) 
{
	FS_event_t * pst_event;
	int32_t e_return;

	if (action == LLFS_EVENT_POST)
	{
		if (FS_get_event(&pst_event) != FS_NO_ERROR)
		{
			e_return = LLFS_NOK;
		}
		else
		{
			pst_event->e_id_event = FS_GET_LAST_MODIFIED_EVENT;
			pst_event->u_ctx_event.st_get_last_modified_ctx.pc_path_in = (char *)path;
			pst_event->u_ctx_event.st_get_last_modified_ctx.pi_data_out = (int*)date;
			
			if (FS_post_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				FS_release_event(pst_event);
				e_return = LLFS_NOK;
			}
			else
			{
				e_return = LLFS_OK;
			}
		}
	}
	else /* else it's second execution the thread has just been resumed by end of delegate process */
	{
		/* retrieve event in pool according to java thread id */
		if (FS_retrieve_event(&pst_event, SNI_getCurrentJavaThreadID()) != FS_NO_ERROR)
		{
			/* an error occurred */
			e_return = LLFS_NOK;
		}
		else
		{
			/* return value from context */
			if (pst_event->u_ctx_event.st_get_last_modified_ctx.uc_bool_out != 0)
			{
				e_return = LLFS_NOK;
			}
			else
			{
				/* nothing to do the immortal space has been normaly updated */
				e_return = LLFS_OK;
			}

			/* release pool event */
			if (FS_release_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				e_return = LLFS_NOK;
			}
		}
	}

	return (e_return);
}

int64_t LLFS_IMPL_get_length(uint8_t* path, int32_t action) 
{
	FS_event_t * pst_event;
	int64_t j_return;

	if (action == LLFS_EVENT_POST)
	{
		if (FS_get_event(&pst_event) != FS_NO_ERROR)
		{
			j_return = LLFS_NOK;
		}
		else
		{
			pst_event->e_id_event = FS_GET_LENGTH;
			pst_event->u_ctx_event.st_get_length_ctx.pc_path_in = (char*)path;

			if (FS_post_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				FS_release_event(pst_event);
				j_return = LLFS_NOK;
			}
			else
			{
				j_return = LLFS_OK;
			}

		}
	}
	else /* else it's second execution the thread has just been resumed by end of delegate process */
	{
		/* retrieve event in pool according to java thread id */
		if (FS_retrieve_event(&pst_event, SNI_getCurrentJavaThreadID()) != FS_NO_ERROR)
		{
				/* an error occurred */
				j_return = LLFS_NOK;
		}
		else
		{
			/* return value from context */
			j_return = (int32_t)pst_event->u_ctx_event.st_get_length_ctx.ul_size_out;

			/* release pool event */
			if (FS_release_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				j_return = LLFS_NOK;
			}
		}
	}

	return (j_return);
}

int32_t LLFS_IMPL_make_directory(uint8_t* path, int32_t action)
{
	FS_event_t * pst_event;
	int32_t e_return;

	if (action == LLFS_EVENT_POST)
	{
		if (FS_get_event(&pst_event) != FS_NO_ERROR)
		{
			e_return = LLFS_NOK;
		}
		else
		{
			pst_event->e_id_event = FS_MK_DIR_EVENT;
			pst_event->u_ctx_event.st_mk_dir_ctx.pc_path_in = (char *)path;
			if (FS_post_event(pst_event) != FS_NO_ERROR) 
			{
				/* an error occurred */
				FS_release_event(pst_event);
				e_return = LLFS_NOK;
			}
			else
			{
				e_return = LLFS_OK;
			}
		}
	}
	else /* else it's second execution the thread has just been resumed by end of delegate process */
	{
		/* retrieve event in pool according to java thread id */
		if (FS_retrieve_event(&pst_event, SNI_getCurrentJavaThreadID()) != FS_NO_ERROR)
		{
			/* an error occurred */
			e_return = LLFS_NOK;
		}
		else
		{
			/* return value from context */
			if (pst_event->u_ctx_event.st_mk_dir_ctx.uc_bool_out != 0)
			{
				e_return = LLFS_NOK;
			}
			else
			{
				e_return = LLFS_OK;
			}

			/* release pool event */
			if (FS_release_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				e_return = LLFS_NOK;
			}
		}
	}

	return (e_return);
}

int32_t LLFS_IMPL_open_directory(uint8_t* path, int32_t action)
{
	FS_event_t * pst_event;
	int32_t j_return;

	if (action == LLFS_EVENT_POST)
	{
		if (FS_get_event(&pst_event) != FS_NO_ERROR)
		{
			j_return = LLFS_NOK;
		}
		else
		{
			pst_event->e_id_event = FS_OPEN_DIR_EVENT;
			pst_event->u_ctx_event.st_open_dir_ctx.pc_path_in = (char*)path;

			if (FS_post_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				FS_release_event(pst_event);
				j_return = LLFS_NOK;
			}
			else
			{
				j_return = LLFS_OK;
			}
		}
	}
	else /* else it's second execution the thread has just been resumed by end of delegate process */
	{
		/* retrieve event in pool according to java thread id */
		if (FS_retrieve_event(&pst_event, SNI_getCurrentJavaThreadID()) != FS_NO_ERROR)
		{
			/* an error occurred */
			j_return = LLFS_NOK;
		}
		else
		{
			/* return value from context */
			j_return = (int32_t)pst_event->u_ctx_event.st_open_dir_ctx.i_fd_out;

			/* release pool event */
			if (FS_release_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				j_return = LLFS_NOK;
			}
		}
	}

	return (j_return);
}

int32_t LLFS_IMPL_close_directory(int32_t directory_ID, int32_t action)
{
	FS_event_t * pst_event;
	int32_t e_return;

	if (action == LLFS_EVENT_POST)
	{
		if (FS_get_event(&pst_event) != FS_NO_ERROR)
		{
			e_return = LLFS_NOK;
		}
		else
		{
			pst_event->e_id_event = FS_CLOSE_DIR_EVENT;
			pst_event->u_ctx_event.st_close_dir_ctx.ui_fd_in = (unsigned int)directory_ID;

			if (FS_post_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				FS_release_event(pst_event);
				e_return = LLFS_NOK;
			}
			else
			{
				e_return = LLFS_OK;
			}
		}
	}
	else /* else it's second execution the thread has just been resumed by end of delegate process */
	{
		/* retrieve event in pool according to java thread id */
		if (FS_retrieve_event(&pst_event, SNI_getCurrentJavaThreadID()) != FS_NO_ERROR)
		{
			/* an error occurred */
			e_return = LLFS_NOK;
		}
		else
		{
			/* return value from context */
			if (pst_event->u_ctx_event.st_close_dir_ctx.uc_bool_out != 0)
			{
				e_return = LLFS_NOK;
			}
			else
			{
				e_return = LLFS_OK;
			}

			/* release pool event */
			if (FS_release_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				e_return = LLFS_NOK;
			}
		}
	}

	return (e_return);
}

int32_t LLFS_IMPL_read_directory(int32_t directory_ID, uint8_t* path, int32_t action)
{
	FS_event_t * pst_event;
	int32_t e_return;

	if (action == LLFS_EVENT_POST)
	{
		if (FS_get_event(&pst_event) != FS_NO_ERROR)
		{
			e_return = LLFS_NOK;
		}
		else
		{
			pst_event->e_id_event = FS_READ_DIR_EVENT;
			pst_event->u_ctx_event.st_read_dir_ctx.ui_fd_in = (unsigned int)directory_ID;
			pst_event->u_ctx_event.st_read_dir_ctx.pc_filename_out = (char*)path;
			
			if (FS_post_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				FS_release_event(pst_event);
				e_return = LLFS_NOK;
			}
			else
			{
				e_return = LLFS_OK;
			}
		}
	}
	else /* else it's second execution the thread has just been resumed by end of delegate process */
	{
		/* retrieve event in pool according to java thread id */
		if (FS_retrieve_event(&pst_event, SNI_getCurrentJavaThreadID()) != FS_NO_ERROR)
		{
			/* an error LLFS_NOK */
			e_return = LLFS_NOK;
		}
		else
		{
			/* return value from context */
			if (pst_event->u_ctx_event.st_read_dir_ctx.uc_bool_out != 0)
			{
				e_return = LLFS_NOK;
			}
			else
			{
				/* nothing to do */
				e_return = LLFS_OK;
			}

			/* release pool event */
			if (FS_release_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				e_return = LLFS_NOK;
			}
		}
	}

	return (e_return);
}

int32_t LLFS_IMPL_rename_to(uint8_t* path, uint8_t* new_path, int32_t action)
{
	FS_event_t * pst_event;
	int32_t e_return;

	if (action == LLFS_EVENT_POST)
	{
		if (FS_get_event(&pst_event) != FS_NO_ERROR)
		{
			e_return = LLFS_NOK;
		}
		else
		{
			pst_event->e_id_event = FS_RENAME_EVENT;
			pst_event->u_ctx_event.st_rename_ctx.pc_old_name_in = (char *)path;
			pst_event->u_ctx_event.st_rename_ctx.pc_new_name_in = (char *)new_path;

			if (FS_post_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				FS_release_event(pst_event);
				e_return = LLFS_NOK;
			}
			else
			{
				e_return = LLFS_OK;
			}
		}
	}
	else /* else it's second execution the thread has just been resumed by end of delegate process */
	{
		/* retrieve event in pool according to java thread id */
		if (FS_retrieve_event(&pst_event, SNI_getCurrentJavaThreadID()) != FS_NO_ERROR)
		{
			/* an error occurred */
			e_return = LLFS_NOK;
		}
		else
		{
			/* return value from context */
			if (pst_event->u_ctx_event.st_rename_ctx.uc_bool_out != 0)
			{
				e_return = LLFS_NOK;
			}
			else
			{
				e_return = LLFS_OK;
			}

			/* release pool event */
			if (FS_release_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				e_return = LLFS_NOK;
			}
		}
	}

	return (e_return);
}

int32_t LLFS_IMPL_set_last_modified(uint8_t* path, int32_t year, int32_t month, int32_t day, int32_t hour, int32_t minute, int32_t second, int32_t action)
{
	FS_event_t * pst_event;
	int32_t e_return;

	if (action == LLFS_EVENT_POST)
	{
		if (FS_get_event(&pst_event) != FS_NO_ERROR)
		{
			e_return = LLFS_NOK;
		}
		else
		{
			pst_event->e_id_event = FS_SET_LAST_MODIFIED_EVENT;
			pst_event->u_ctx_event.st_set_last_modified_ctx.pc_path_in = (char *)path;
			pst_event->u_ctx_event.st_set_last_modified_ctx.i_years_in = (int)year;
			pst_event->u_ctx_event.st_set_last_modified_ctx.i_month_in = (int)month;
			pst_event->u_ctx_event.st_set_last_modified_ctx.i_day_in = (int)day;
			pst_event->u_ctx_event.st_set_last_modified_ctx.i_hour_in = (int)hour;
			pst_event->u_ctx_event.st_set_last_modified_ctx.i_minute_in = (int)minute;
			pst_event->u_ctx_event.st_set_last_modified_ctx.i_second_in = (int)second;


			if (FS_post_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				FS_release_event(pst_event);
				e_return = LLFS_NOK;
			}
			else
			{
				e_return = LLFS_OK;
			}
		}
	}
	else /* else it's second execution the thread has just been resumed by end of delegate process */
	{
		/* retrieve event in pool according to java thread id */
		if (FS_retrieve_event(&pst_event, SNI_getCurrentJavaThreadID()) != FS_NO_ERROR)
		{
			/* an error occurred */
			e_return = LLFS_NOK;
		}
		else
		{
			/* return value from context */
			if (pst_event->u_ctx_event.st_set_last_modified_ctx.uc_bool_out != 0)
			{
				e_return = LLFS_NOK;
			}
			else
			{
				e_return = LLFS_OK;
			}

			/* release pool event */
			if (FS_release_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				e_return = LLFS_NOK;
			}
		}
	}

	return (e_return);
}

int32_t LLFS_IMPL_set_read_only(uint8_t* path, int32_t action) 
{
	FS_event_t * pst_event;
	int32_t e_return;

	if (action == LLFS_EVENT_POST)
	{
		if (FS_get_event(&pst_event) != FS_NO_ERROR)
		{
			e_return = LLFS_NOK;
		}
		else
		{
			pst_event->e_id_event = FS_SET_READ_ONLY_EVENT;
			pst_event->u_ctx_event.st_set_read_only_ctx.pc_path_in = (char *)path;

			if (FS_post_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				FS_release_event(pst_event);
				e_return = LLFS_NOK;
			}
			else
			{
				e_return = LLFS_OK;
			}
		}
	}
	else /* else it's second execution the thread has just been resumed by end of delegate process */
	{
		/* retrieve event in pool according to java thread id */
		if (FS_retrieve_event(&pst_event, SNI_getCurrentJavaThreadID()) != FS_NO_ERROR)
		{
			/* an error occurred */
			e_return = LLFS_NOK;
		}
		else
		{
			/* return value from context */
			if (pst_event->u_ctx_event.st_set_read_only_ctx.uc_bool_out != 0)
			{
				e_return = LLFS_NOK;
			}
			else
			{
				e_return = LLFS_OK;
			}

			/* release pool event */
			if (FS_release_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				e_return = LLFS_NOK;
			}
		}
	}

	return (e_return);
}

int32_t LLFS_IMPL_create(uint8_t* path, int32_t action)
{
	FS_event_t * pst_event;
	int32_t e_return;

	if (action == LLFS_EVENT_POST)
	{
		if (FS_get_event(&pst_event) != FS_NO_ERROR)
		{
			e_return = LLFS_NOK;
		}
		else
		{
			pst_event->e_id_event = FS_CREATE_EVENT;
			pst_event->u_ctx_event.st_create_ctx.pc_path_in = (char *)path;

			if (FS_post_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				FS_release_event(pst_event);
				e_return = LLFS_NOK;
			}
			else
			{
				e_return = LLFS_OK;
			}
		}
	}
	else /* else it's second execution the thread has just been resumed by end of delegate process */
	{
		/* retrieve event in pool according to java thread id */
		if (FS_retrieve_event(&pst_event, SNI_getCurrentJavaThreadID()) != FS_NO_ERROR)
		{
			/* an error occurred */
			e_return = LLFS_NOK;
		}
		else
		{
			/* return value from context */
			FS_status_t res = pst_event->u_ctx_event.st_create_ctx.status_error_out;
			if(res == FS_NO_ERROR){
				e_return = LLFS_OK;
			}
			else if(res == FS_FILE_NOT_CREATED){
				e_return = LLFS_NOT_CREATED;
			}
			else {
				e_return = LLFS_NOK;
			}

			/* release pool event */
			if (FS_release_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				e_return = LLFS_NOK;
			}
		}
	}

	return (e_return);
}

int32_t LLFS_IMPL_delete(uint8_t* path, int32_t action)
{
	FS_event_t * pst_event;
	int32_t e_return;

	if (action == LLFS_EVENT_POST)
	{
		if (FS_get_event(&pst_event) != FS_NO_ERROR)
		{
			e_return = LLFS_NOK;
		}
		else
		{
			pst_event->e_id_event = FS_DELETE_EVENT;
			pst_event->u_ctx_event.st_delete_ctx.pc_path_in = (char *)path;

			if (FS_post_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				FS_release_event(pst_event);
				e_return = LLFS_NOK;
			}
			else
			{
				e_return = LLFS_OK;
			}
		}
	}
	else /* else it's second execution the thread has just been resumed by end of delegate process */
	{
		/* retrieve event in pool according to java thread id */
		if (FS_retrieve_event(&pst_event, SNI_getCurrentJavaThreadID()) != FS_NO_ERROR)
		{
			/* an error occurred */
			e_return = LLFS_NOK;
		}
		else
		{
			/* return value from context */
			if (pst_event->u_ctx_event.st_delete_ctx.uc_bool_out != 0)
			{
				e_return = LLFS_NOK;
			}
			else
			{
				e_return = LLFS_OK;
			}

			/* release pool event */
			if (FS_release_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				e_return = LLFS_NOK;
			}
		}
	}

	return (e_return);
}

int32_t LLFS_IMPL_is_accessible(uint8_t* path, int32_t access, int32_t action)
{
	FS_event_t * pst_event;
	int32_t e_return;

	if (action == LLFS_EVENT_POST)
	{
		if (FS_get_event(&pst_event) != FS_NO_ERROR)
		{
			e_return = LLFS_NOK;
		}
		else
		{
			pst_event->e_id_event = FS_CHECK_ACCESS;
			pst_event->u_ctx_event.st_check_access_ctx.pc_path_in = (char *)path;
			pst_event->u_ctx_event.st_check_access_ctx.access_in = getFSAccessFromJAccess(access);

			if (FS_post_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				FS_release_event(pst_event);
				e_return = LLFS_NOK;
			}
			else
			{
				e_return = LLFS_OK;
			}
		}
	}
	else /* else it's second execution the thread has just been resumed by end of delegate process */
	{
		/* retrieve event in pool according to java thread id */
		if (FS_retrieve_event(&pst_event, SNI_getCurrentJavaThreadID()) != FS_NO_ERROR)
		{
			/* an error occurred */
			e_return = LLFS_NOK;
		}
		else
		{
			/* return value from context */
			if (pst_event->u_ctx_event.st_check_access_ctx.uc_bool_out != 0)
			{
				e_return = LLFS_NOK;
			}
			else
			{
				e_return = LLFS_OK;
			}

			/* release pool event */
			if (FS_release_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				e_return = LLFS_NOK;
			}
		}
	}

	return (e_return);
}

int32_t LLFS_IMPL_set_permission(uint8_t* path, int32_t access, int32_t enable, int32_t owner, int32_t action)
{
	FS_event_t * pst_event;
	int32_t e_return;

	if (action == LLFS_EVENT_POST)
	{
		if (FS_get_event(&pst_event) != FS_NO_ERROR)
		{
			e_return = LLFS_NOK;
		}
		else
		{
			pst_event->e_id_event = FS_SET_PERMISSION;
			pst_event->u_ctx_event.st_set_permission_ctx.pc_path_in = (char *)path;
			pst_event->u_ctx_event.st_set_permission_ctx.access_in = getFSAccessFromJAccess(access);
			pst_event->u_ctx_event.st_set_permission_ctx.uc_enable_in = enable == LLFS_PERMISSION_ENABLE ? 1 : 0;
			pst_event->u_ctx_event.st_set_permission_ctx.uc_owner_only_in = owner == LLFS_PERMISSION_OWNER_ONLY ? 1 : 0;

			if (FS_post_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				FS_release_event(pst_event);
				e_return = LLFS_NOK;
			}
			else
			{
				e_return = LLFS_OK;
			}
		}
	}
	else /* else it's second execution the thread has just been resumed by end of delegate process */
	{
		/* retrieve event in pool according to java thread id */
		if (FS_retrieve_event(&pst_event, SNI_getCurrentJavaThreadID()) != FS_NO_ERROR)
		{
			/* an error occurred */
			e_return = LLFS_NOK;
		}
		else
		{
			/* return value from context */
			if (pst_event->u_ctx_event.st_set_permission_ctx.uc_bool_out != 0)
			{
				e_return = LLFS_NOK;
			}
			else
			{
				e_return = LLFS_OK;
			}

			/* release pool event */
			if (FS_release_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				e_return = LLFS_NOK;
			}
		}
	}

	return (e_return);
}

#ifdef __cplusplus
	}
#endif

