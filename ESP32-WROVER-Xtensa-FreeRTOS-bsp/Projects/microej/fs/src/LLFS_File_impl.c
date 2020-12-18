/* 
 * C
 * Copyright 2018-2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */

/* Includes ------------------------------------------------------------------*/

#include "LLFS_File_impl.h"
#include "sni.h"
#include "fs_helper.h"

#ifdef __cplusplus
	extern "C" {
#endif

/* Public API ----------------------------------------------------------------*/

int32_t LLFS_File_IMPL_close(int32_t file_id, int32_t action)
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
			pst_event->e_id_event = FS_CLOSE_FILE_EVENT;
			pst_event->u_ctx_event.st_close_file_ctx.ui_fd_in = (unsigned int)file_id;

			if (FS_post_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
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
		if (FS_retrieve_event(&pst_event, (unsigned long)SNI_getCurrentJavaThreadID()) != FS_NO_ERROR)
		{
			/* an error occurred */
			e_return = LLFS_NOK;
		}
		else
		{
			/* return value from context */
			if (pst_event->u_ctx_event.st_close_file_ctx.uc_bool_out != 0)
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

int32_t LLFS_File_IMPL_open(uint8_t* path, uint8_t mode, int32_t action)
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
			pst_event->e_id_event = FS_OPEN_FILE_EVENT;
			pst_event->u_ctx_event.st_open_file_ctx.pc_path_in = (char*)path;
			pst_event->u_ctx_event.st_open_file_ctx.uc_mode_in = (unsigned char)mode;

			if (FS_post_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				j_return = LLFS_NOK;
			}
			else
			{
				j_return = LLFS_OK;  /* not an error but must return thing */
			}
		}
	}
	else /* else it's second execution the thread has just been resumed by end of delegate process */
	{
		/* retrieve event in pool according to java thread id */
		if (FS_retrieve_event(&pst_event, (unsigned long)SNI_getCurrentJavaThreadID()) != FS_NO_ERROR)
		{
				/* an error occurred */
				j_return = LLFS_NOK;
		}
		else
		{
			/* return value from context */
			j_return = (int32_t)pst_event->u_ctx_event.st_open_file_ctx.i_fd_out;

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

int32_t LLFS_File_IMPL_read(int32_t file_id, uint8_t* data, int32_t length, int32_t action)
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
			pst_event->e_id_event = FS_READ_FILE_EVENT;
			pst_event->u_ctx_event.st_read_file_ctx.ui_fd_in = (unsigned int)file_id;
			pst_event->u_ctx_event.st_read_file_ctx.pc_output_buffer_in = (char*)data;
			pst_event->u_ctx_event.st_read_file_ctx.ui_data_length_in = (unsigned int)length;

			if (FS_post_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				j_return = LLFS_NOK;
			}
			else
			{
				j_return = LLFS_OK;  /* not an error but must return thing */
			}
		}
	}
	else /* else it's second execution the thread has just been resumed by end of delegate process */
	{
		/* retrieve event in pool according to java thread id */
		if (FS_retrieve_event(&pst_event, (unsigned long)SNI_getCurrentJavaThreadID()) != FS_NO_ERROR)
		{
				/* an error occurred */
				j_return = LLFS_NOK;
		}
		else
		{
			/* return value from context */
			j_return = (int32_t)pst_event->u_ctx_event.st_read_file_ctx.i_error_out;

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

int32_t LLFS_File_IMPL_write(int32_t file_id, uint8_t* data, int32_t length, int32_t action)
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
			pst_event->e_id_event = FS_WRITE_FILE_EVENT;
			pst_event->u_ctx_event.st_write_file_ctx.ui_fd_in = (unsigned int)file_id;
			pst_event->u_ctx_event.st_write_file_ctx.pc_intput_buffer_in = (char*)data;
			pst_event->u_ctx_event.st_write_file_ctx.ui_data_length_in = (unsigned int)length;

			if (FS_post_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				j_return = LLFS_NOK;
			}
			else
			{
				j_return = LLFS_OK;  /* not an error but must return thing */
			}
		}
	}
	else /* else it's second execution the thread has just been resumed by end of delegate process */
	{
		/* retrieve event in pool according to java thread id */
		if (FS_retrieve_event(&pst_event, (unsigned long)SNI_getCurrentJavaThreadID()) != FS_NO_ERROR)
		{
				/* an error occurred */
				j_return = LLFS_NOK;
		}
		else
		{
			/* return value from context */
			j_return = (int32_t)pst_event->u_ctx_event.st_write_file_ctx.i_error_out;

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

int32_t LLFS_File_IMPL_write_byte(int32_t file_id, int32_t data, int32_t action)
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
			pst_event->e_id_event = FS_WRITE_BYTE_FILE_EVENT;
			pst_event->u_ctx_event.st_write_byte_file_ctx.ui_fd_in = (unsigned int)file_id;
			pst_event->u_ctx_event.st_write_byte_file_ctx.i_data_in = (int)data;

			if (FS_post_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				j_return = LLFS_NOK;
			}
			else
			{
				j_return = LLFS_OK;  /* not an error but must return thing */
			}
		}
	}
	else /* else it's second execution the thread has just been resumed by end of delegate process */
	{
		/* retrieve event in pool according to java thread id */
		if (FS_retrieve_event(&pst_event, (unsigned long)SNI_getCurrentJavaThreadID()) != FS_NO_ERROR)
		{
				/* an error occurred */
				j_return = LLFS_NOK;
		}
		else
		{
			/* return value from context */
			j_return = (int32_t)pst_event->u_ctx_event.st_write_byte_file_ctx.i_error_out;

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

int32_t LLFS_File_IMPL_read_byte(int32_t file_id, int32_t action)
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
			pst_event->e_id_event = FS_READ_BYTE_FILE_EVENT;
			pst_event->u_ctx_event.st_read_byte_file_ctx.ui_fd_in = (unsigned int)file_id;

			if (FS_post_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				j_return = LLFS_NOK;
			}
			else
			{
				j_return = LLFS_OK;  /* not an error but must return thing */
			}
		}
	}
	else /* else it's second execution the thread has just been resumed by end of delegate process */
	{
		/* retrieve event in pool according to java thread id */
		if (FS_retrieve_event(&pst_event, (unsigned long)SNI_getCurrentJavaThreadID()) != FS_NO_ERROR)
		{
				/* an error occurred */
				j_return = LLFS_NOK;
		}
		else
		{
			/* return value from context */
			j_return = (int32_t)pst_event->u_ctx_event.st_read_byte_file_ctx.i_error_out;

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

int64_t LLFS_File_IMPL_skip(int32_t file_id, int64_t n, int32_t action)
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
			pst_event->e_id_event = FS_SKIP_FILE_EVENT;
			pst_event->u_ctx_event.st_skip_file_ctx.ui_fd_in = (unsigned int)file_id;
			pst_event->u_ctx_event.st_skip_file_ctx.l_n_in = (long long int)n;

			if (FS_post_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				j_return = LLFS_NOK;
			}
			else
			{
				j_return = LLFS_OK;  /* not an error but must return thing */
			}
		}
	}
	else /* else it's second execution the thread has just been resumed by end of delegate process */
	{
		/* retrieve event in pool according to java thread id */
		if (FS_retrieve_event(&pst_event, (unsigned long)SNI_getCurrentJavaThreadID()) != FS_NO_ERROR)
		{
				/* an error occurred */
				j_return = LLFS_NOK;
		}
		else
		{
			/* return value from context */
			j_return = (jlong)pst_event->u_ctx_event.st_skip_file_ctx.l_error_out;

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

int32_t LLFS_File_IMPL_available(int32_t file_id, int32_t action)
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
			pst_event->e_id_event = FS_AVAILABLE_FILE_EVENT;
			pst_event->u_ctx_event.st_available_file_ctx.ui_fd_in = (unsigned int)file_id;

			if (FS_post_event(pst_event) != FS_NO_ERROR)
			{
				/* an error occurred */
				j_return = LLFS_NOK;
			}
			else
			{
				j_return = LLFS_OK;  /* not an error but must return thing */
			}
		}
	}
	else /* else it's second execution the thread has just been resumed by end of delegate process */
	{
		/* retrieve event in pool according to java thread id */
		if (FS_retrieve_event(&pst_event, (unsigned long)SNI_getCurrentJavaThreadID()) != FS_NO_ERROR)
		{
				/* an error occurred */
				j_return = LLFS_NOK;
		}
		else
		{
			/* return value from context */
			j_return = (int32_t)pst_event->u_ctx_event.st_available_file_ctx.i_error_out;

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

#ifdef __cplusplus
	}
#endif

