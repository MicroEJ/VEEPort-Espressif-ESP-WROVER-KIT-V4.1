
/*
 * C
 *
 * Copyright 2014-2022 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#ifndef FS_ADAPT_DELEGATE_TASK_H
#define FS_ADAPT_DELEGATE_TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "ff.h"
#include "fs_helper.h"

#ifdef __cplusplus
	extern "C" {
#endif

/** @brief define the amount of file in private pool module */
#define FS_MAX_NUMBER_OF_FILE_IN_POOL  (FF_FS_LOCK)

/** @brief define the amount of dir in private pool module */
#define FS_MAX_NUMBER_OF_DIR_IN_POOL  (FF_FS_LOCK)

/** @brief list of module constant */
typedef enum
{
	FS_ADAPT_NO_ERROR,
	FS_ADAPT_ERROR,
}FS_ADAPT_status_t;


#define FS_ADAPT_YIELD(tick)                                           /*taskYIELD()*/
#define FS_ADAPT_MAILBOX_DECLARE(name)                                 QueueHandle_t (name)
#define FS_ADAPT_FIL                                                   FIL
#define FS_ADAPT_DIR                                                   DIR

#define FS_ADAPT_SUSPEND_CALLER_THREAD()                   
#define FS_ADAPT_INFITIVE_TIMEOUT                                      (portMAX_DELAY)

#define FS_ADAPT_INIT_MAILBOX(mailbox,size_of_msg,number_of_msg)       mailbox = xQueueCreate((number_of_msg),(size_of_msg))              

// Mapping for different compilers.
#define __MODULE__ __FILE__

/**
 * @brief function to initialise the FS delegate module adaptation
 *
 * @return @see FS_ADAPT_status_t
 */
FS_ADAPT_status_t FS_ADAPT_init_f(void);

/**
 * @brief function to initialise the SD card
 *
 * @return @see FS_ADAPT_status_t
 */
FS_ADAPT_status_t FS_ADAPT_init_media(BYTE pdrv);

/**
 * @brief function to start the FS delegate thread
 *
 * @return @see FS_ADAPT_status_t
 */
FS_ADAPT_status_t FS_ADAPT_start_f(void);

/**
 * @brief function to get the caller java thread id
 *
 * @param[in,out] _pul_java_thread pointer on java thread
 *
 * @return @see FS_ADAPT_status_t
 */
FS_ADAPT_status_t FS_ADAPT_get_java_thread(unsigned long * _pul_java_thread);

/**
 * @brief function to suspend the current java thread 
 *
 * @return @see FS_ADAPT_status_t
 */
FS_ADAPT_status_t FS_ADAPT_suspend_java_thread(void);

/**
 * @brief function to resume a java thread
 *
 * @param[in] _ul_java_thread java thread id
 *
 * @return @see FS_ADAPT_status_t
 */
FS_ADAPT_status_t FS_ADAPT_resume_java_thread(unsigned long const _ul_java_thread);

/**
 * @brief this function is call to post event action
 *
 * @param[in,out] _pv_mailbox pointer on mailbox
 * @param[in]     _pv_event event to post
 * @param[in]     _ul_timeout timeout to wait to post event
 *
 * @return @see FS_ADAPT_status_t
 */
FS_ADAPT_status_t FS_ADAPT_post_event_action_f(void * _pv_mailbox, void* _pv_event, unsigned long _ul_timeout);

/**
 * @brief this function is call to wait event action
 *
 * this function must idle the task until an event is present in mailbox
 *
 * @param[in,out] _pv_mailbox pointer on mailbox
 * @param[out]    _ppv_event event to post
 * @param[in]     _ul_timeout timeout to wait to post event
 *
 * @return @see FS_ADAPT_status_t
 */
FS_ADAPT_status_t FS_ADAPT_wait_event(void * _pv_mailbox, void** _ppv_event, unsigned long _ul_timeout);

/**
 * @brief this specific function is call to close file
 *
 * @param[in] _ui_fd_in file descriptor of file
 * @param[out] _puc_bool_out pointer on boolean value (1 error / 0 ok)
 *
 */
void FS_ADAPT_close_file_f(unsigned int _ui_fd_in,
		                       unsigned char * _puc_bool_out);

/**
 * @brief this specific function is call to open file
 *
 * @param[in] _pc_path_in pointer on path file
 * @param[in] _uc_mode_in open mode (a/R/RW)
 * @param[out] _pi_fd_out pointer on file descriptor open (or -2 if error)
 *
 */
void FS_ADAPT_open_file_f(char* _pc_path_in,
		                      unsigned char _uc_mode_in,
		                      int * _pi_fd_out);

/**
 * @brief this specific function is call to read file
 *
 * @param[in] _ui_fd_in file descriptor of file
 * @param[out] _pc_output_buffer_in pointer on read buffer
 * @param[in] _ui_data_length_in data length to read
 * @param[out] _pi_error_out pointer on error value (-2 error / positive number of read bytes)
 *
 */													
void FS_ADAPT_read_file_f(unsigned int _ui_fd_in,
						              char* _pc_output_buffer_in,
						              unsigned int _ui_data_length_in,
						              int * _pi_error_out);
													

/**
 * @brief this specific function is call to write file
 *
 * @param[in] _ui_fd_in file descriptor of file
 * @param[out] _pc_intput_buffer_in pointer on write buffer
 * @param[in] _ui_data_length_in data length to read
 * @param[out] _pi_error_out pointer on error value (-2 error / positive number of read bytes)
 *
 */	
void FS_ADAPT_write_file_f(unsigned int _ui_fd_in,
						               char* _pc_intput_buffer_in,
						               unsigned int _ui_data_length_in,
						               int * _pi_error_out);

/**
 * @brief this specific function is call to check existance of file
 *
 * @param[in] _pc_path_in pointer on path file
 * @param[out] _puc_bool_out pointer on boolean value (1 error / 0 ok)
 *
 */
void FS_ADAPT_exist_file_f(char* _pc_path_in, unsigned char * _puc_bool_out);

/**
 * @brief this specific function is call to write a byte in a file
 *
 * @param[in] _ui_fd_in file descriptor of file
 * @param[in] _i_data_in byte to write
 * @param[out] _pi_error_out pointer on error value (-1 error / 0 on success)
 *
 */
void FS_ADAPT_write_byte_file_f(unsigned int _ui_fd_in,
										int _i_data_in,
										int * _pi_error_out);

/**
 * @brief this specific function is call to read a byte from a file
 *
 * @param[in] _ui_fd_in file descriptor of file
 * @param[out] _pi_error_out pointer on read value (on success the unsigned byte value between 0 and 255, -1 on End Of File, -2 on error)
 *
 */
void FS_ADAPT_read_byte_file_f(unsigned int _ui_fd_in,
										int * _pi_error_out);

/**
 * @brief this specific function is call to skip n bytes in a file
 * The method may, for a variety of reasons, end up skipping over some smaller
 * number of bytes, possibly 0. If _l_n_in is negative, the method will try to
 * skip backwards. In case the backing file does not support backward skip at
 * its current position, -1 is stored in _pl_error_out. The actual number of
 * bytes skipped is stored in _pl_error_out. If it skips forwards, it stores a
 * positive value. If it skips backwards, it stores a negative value.
 *
 * @param[in] _ui_fd_in file descriptor of file
 * @param[in] _l_n_in number of bytes to skip
 * @param[out] _pl_error_out pointer on error value (-1 on error, the actual number of bytes skipped on success)
 *
 */
void FS_ADAPT_skip_file_f(unsigned int _ui_fd_in,
										long long int _l_n_in,
										long long int * _pl_error_out);

/**
 * @brief this specific function is call to retrieve the number of available byte in a file
 *
 * @param[in] _ui_fd_in file descriptor of file
 * @param[out] _pi_error_out pointer on result value (-1 on error, an estimate of the number of remaining bytes that can be read on success)
 *
 */
void FS_ADAPT_available_file_f(unsigned int _ui_fd_in,
										int * _pi_error_out);

/**
 * @brief this specific function is call to get free space on volume
 *
 * @param[in] _pc_path_in pointer on path file
 * @param[out] _pul_size_out pointer on size value
 *
 */
void FS_ADAPT_get_free_space_f(char* _pc_path_in, unsigned long * _pul_size_out);
													 
/**
 * @brief this specific function is call to get total space on volume
 *
 * @param[in] _pc_path_in pointer on path file
 * @param[out] _pul_size_out pointer on size value
 *
 */
void FS_ADAPT_get_total_space_f(char* _pc_path_in, unsigned long * _pul_size_out);

/**
 * @brief this specific function is call to get usable space on volume
 *
 * @param[in] _pc_path_in pointer on path file
 * @param[out] _pul_size_out pointer on size value
 *
 */
void FS_ADAPT_get_usable_space_f(char* _pc_path_in, unsigned long * _pul_size_out);
													 
/**
 * @brief this specific function is call to check if item is directory
 *
 * @param[in] _pc_path_in pointer on path file
 * @param[out] _puc_bool_out pointer on boolean value (1 error / 0 ok)
 *
 */
void FS_ADAPT_is_directory_f(char* _pc_path_in, unsigned char * _puc_bool_out);

/**
 * @brief this specific function is call to check if item is file
 *
 * @param[in] _pc_path_in pointer on path file
 * @param[out] _puc_bool_out pointer on boolean value (1 error / 0 ok)
 *
 */
void FS_ADAPT_is_file_f(char* _pc_path_in, unsigned char * _puc_bool_out);

/**
 * @brief this specific function is call to check if item is hidden
 *
 * @param[in] _pc_path_in pointer on path file
 * @param[out] _puc_bool_out pointer on boolean value (1 error / 0 ok)
 *
 */
void FS_ADAPT_is_hidden_f(char* _pc_path_in, unsigned char * _puc_bool_out);

/**
 * @brief this specific function is call to get last modified date of item
 *
 * @param[in] _pc_path_in pointer on path file
 * @param[out] _pi_data_out pointer on date buffer read
 * @param[out] _puc_bool_out pointer on boolean value (1 error / 0 ok)
 *
 */
void FS_ADAPT_get_last_modified_f(char* _pc_path_in, int * _pi_data_out, unsigned char * _puc_bool_out);

/**
 * @brief this specific function is call to create directory
 *
 * @param[in] _pc_path_in pointer on path file
 * @param[out] _puc_bool_out pointer on boolean value (1 error / 0 ok)
 *
 */
void FS_ADAPT_mk_dir_f(const char* _pc_path_in, unsigned char * _puc_bool_out);

/**
 * @brief this specific function is call to open dir
 *
 * @param[in] _pc_path_in pointer on path dir
 * @param[out] _pi_fd_out pointer on file descriptor open (or -2 if error)
 *
 */
void FS_ADAPT_open_dir_f(char* _pc_path_in, int * _pi_fd_out);
													 
/**
 * @brief this specific function is call to close dir
 *
 * @param[in] ui_fd_in file descriptor of dir
 * @param[out] puc_bool_out pointer on boolean value (1 error / 0 ok)
 *
 */
void FS_ADAPT_close_dir_f(unsigned int _ui_fd_in, unsigned char * _puc_bool_out);

/**
 * @brief this specific function is call to read directory
 *
 * to scan dir, this function must be call in loop until _pc_filename_in is null.
 *
 * @param[in] ui_fd_in file descriptor of dir
 * @param[in] _pc_filename_in pointer on first name item read in dir
 * @param[out] _puc_bool_out pointer on boolean value (1 error / 0 ok)
 *
 */
void FS_ADAPT_read_dir_f(unsigned int _ui_fd_in,
						             char* _pc_filename_in,
						             unsigned char * _puc_bool_out);
												 
/**
 * @brief this specific function is call to rename item
 *
 * @param[in] _pc_old_name_in pointer on old name item
 * @param[in] _pc_new_name_in pointer on new name item
 * @param[out] _puc_bool_out pointer on boolean value (1 error / 0 ok)
 *
 */												 
void FS_ADAPT_rename_f(char* _pc_old_name_in,
					             char* _pc_new_name_in,
					             unsigned char * _puc_bool_out);
											 
/**
 * @brief this specific function is call to set last modified date on item
 *
 * @param[in] _pc_path_in pointer on path file
 * @param[in] _i_years_in years of last modified date.	
 * @param[in] _i_month_in month of last modified date.
 * @param[in] _i_day_in day of last modified date.
 * @param[in] _i_hour_in hour of last modified date.
 * @param[in] _i_minute_in minute of last modified date.
 * @param[in] _i_second_in second of last modified date.											 
 * @param[out] _puc_bool_out pointer on boolean value (1 error / 0 ok)
 *
 */
void FS_ADAPT_set_last_modified_f(char* _pc_path_in,
		                              int _i_years_in,
		                              int _i_month_in,
		                              int _i_day_in,
		                              int _i_hour_in,
		                              int _i_minute_in,
		                              int _i_second_in,
		                              unsigned char * _puc_bool_out);
																	
/**
 * @brief this specific function is call to set read only property on item
 *
 * @param[in] _pc_path_in pointer on path file
 * @param[out] _puc_bool_out pointer on boolean value (1 error / 0 ok)
 *
 */
void FS_ADAPT_set_read_only_f(char* _pc_path_in, unsigned char * _puc_bool_out);
																	
/**
 * @brief this specific function is call to get size of item
 *
 * @param[in] _pc_path_in pointer on path file
 * @param[out] _pul_size_out pointer on size value
 *
 */
void FS_ADAPT_get_length_f(char* _pc_path_in, unsigned long * _pul_size_out);
																	
/**
 * @brief this specific function is call to create item
 *
 * @param[in] _pc_path_in pointer on path file
 * @param[out] _pstatus_error_out pointer on status value (FS_NO_ERROR ok / FS_FILE_NOT_CREATED file already exists / FS_IO_ERROR error)
 *
 */
void FS_ADAPT_create_f(char* _pc_path_in, FS_status_t * _pstatus_error_out);
																	
/**
 * @brief this specific function is call to delete item
 *
 * @param[in] _pc_path_in pointer on path file
 * @param[out] _puc_bool_out pointer on boolean value (1 error / 0 ok)
 *
 */
void FS_ADAPT_delete_f(char* _pc_path_in, unsigned char * _puc_bool_out);

/**
 * @brief this specific function is call to get can write permission
 *
 * @param[in] _pc_path_in pointer on path file
 * @param[in] _access_in access mode
 * @param[out] _puc_bool_out pointer on boolean value (1 error / 0 ok)
 *
 */
void FS_ADAPT_check_access_f(char* _pc_path_in, FS_access_t _access_in, unsigned char * _puc_bool_out);

/**
 * @brief this specific function is call to set can execute permission
 *
 * @param[in] _pc_path_in pointer on path file
 * @param[in] _access_in access mode
 * @param[in] _uc_enable enable permission or not
 * @param[in] _uc_owner_only by owner only or not
 * @param[out] _puc_bool_out pointer on boolean value (1 error / 0 ok)
 *
 */
void FS_ADAPT_set_permission_f(char* _pc_path_in,
							   FS_access_t _access_in,
		                       unsigned char _uc_enable,
		                       unsigned char _uc_owner_only,
		                       unsigned char * _puc_bool_out);

#ifdef __cplusplus
	}
#endif

#endif /* FS_ADAPT_DELEGATE_TASK_H */
