
/*
 * C
 *
 * Copyright 2014-2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */

#ifndef FS_DELEGATE_TASK_H
#define FS_DELEGATE_TASK_H

#ifdef __cplusplus
	extern "C" {
#endif

/** @brief define the amount of message in private mailbox of module */
#define FS_MAX_NUMBER_OF_EVENT_IN_MAILBOX  (10)

/** @brief list of module constant */
typedef enum
{
	FS_NO_ERROR,
	FS_ADAPTATION_ERROR_INIT,
	FS_MODULE_NOT_INITIALIZED,
	FS_PARAMETER_ERROR,
	FS_UNEXPECTED_ERROR_IN_RECEIVED,
	FS_UNKNOWN_EVENT_RECEIVED,
	FS_ERROR_DURING_FREEING_POOL,
	FS_NO_SPACE_AVAILABLE,
	FS_CANNOT_SUSPEND_JVM_THREAD,
	FS_CANNOT_RESUME_JVM_THREAD,
	FS_CANNOT_GET_JVM_THREAD,
	FS_EVENT_NOT_FOUND,
	FS_FILE_NOT_CREATED,
	FS_IO_ERROR
}FS_status_t;

typedef enum
{
	FS_ACCESS_READ,
	FS_ACCESS_WRITE,
	FS_ACCESS_EXECUTE,
}FS_access_t;

/** @brief list of event module's */
typedef enum
{
	/* file management */
	FS_OPEN_FILE_EVENT,
	FS_CLOSE_FILE_EVENT,
	FS_READ_FILE_EVENT,
	FS_WRITE_FILE_EVENT,
	FS_EXIST_FILE_EVENT,
	FS_WRITE_BYTE_FILE_EVENT,
	FS_READ_BYTE_FILE_EVENT,
	FS_SKIP_FILE_EVENT,
	FS_AVAILABLE_FILE_EVENT,

	/* space volume management */
	FS_GET_FREE_SPACE_EVENT,
	FS_GET_TOTAL_SPACE_EVENT,
	FS_GET_USABLE_SPACE_EVENT,

	/* properties item management */
	FS_IS_DIRECTORY_EVENT,
	FS_IS_FILE_EVENT,
	FS_IS_HIDDEN_EVENT,
	FS_GET_LAST_MODIFIED_EVENT,
	FS_SET_LAST_MODIFIED_EVENT,
	FS_SET_READ_ONLY_EVENT,
	FS_GET_LENGTH,

	/* directory management */
	FS_MK_DIR_EVENT,
	FS_OPEN_DIR_EVENT,
	FS_READ_DIR_EVENT,
	FS_CLOSE_DIR_EVENT,

	/* system management */
	FS_RENAME_EVENT,
	FS_CREATE_EVENT,
	FS_DELETE_EVENT,

	/* permission management */
	FS_CHECK_ACCESS,
	FS_SET_PERMISSION,

	FS_MAX_EVENT,  /* no insertion after this line */
}FS_id_event_t;

/** @brief define argument for open file event */
typedef struct
{
	char * pc_path_in;          /**< in path of file to open */
	unsigned char uc_mode_in;   /**< in open mode (A,R,RW) */
	int i_fd_out;               /**< out result */
}FS_open_file_ctx_t;

/** @brief define argument for close file event */
typedef struct
{
	unsigned int ui_fd_in;      /**< in file descriptor to close */
	unsigned char uc_bool_out;  /**< out result */
}FS_close_file_ctx_t;

/** @brief define argument for read file event */
typedef struct
{
	unsigned int ui_fd_in;              /**< in file descriptor to close */
	char* pc_output_buffer_in;          /**< in pointer on output buffer */
	unsigned int ui_data_length_in;     /**< in size of output buffer */
	int i_error_out;                    /**< out result */
}FS_read_file_ctx_t;

/** @brief define argument for write file event */
typedef struct
{
	unsigned int ui_fd_in;              /**< in file descriptor to close */
	char* pc_intput_buffer_in;          /**< in pointer on input buffer */
	unsigned int ui_data_length_in;     /**< in size of input buffer */
	int i_error_out;                    /**< out result */
}FS_write_file_ctx_t;

/** @brief define argument for exist event */
typedef struct
{
	char * pc_path_in;          /**< in path of file to test */
	unsigned char uc_bool_out;  /**< out result */
}FS_exist_file_ctx_t;

/** @brief define argument for write byte file event */
typedef struct
{
	unsigned int ui_fd_in;              /**< in file descriptor to close */
	int i_data_in;          			/**< in the byte to write */
	int i_error_out;                    /**< out result */
}FS_write_byte_file_ctx_t;

/** @brief define argument for read byte file event */
typedef struct
{
	unsigned int ui_fd_in;              /**< in file descriptor to close */
	int i_error_out;                    /**< out result */
}FS_read_byte_file_ctx_t;

/** @brief define argument for skip file event */
typedef struct
{
	unsigned int ui_fd_in;          /**< in file descriptor to close */
	long long int l_n_in;          	/**< in the number of bytes to skip */
	long long int l_error_out;      /**< out result */
}FS_skip_file_ctx_t;

/** @brief define argument for available file event */
typedef struct
{
	unsigned int ui_fd_in;          /**< in file descriptor to close */
	int i_error_out;                /**< out result */
}FS_available_file_ctx_t;

/** @brief define argument for get free space event */
typedef struct
{
	char * pc_path_in;          /**< in path of file/directory */
	unsigned long ul_size_out;  /**< out result */
}FS_get_free_space_ctx_t;

/** @brief define argument for get total space event */
typedef struct
{
	char * pc_path_in;          /**< in path of file/directory */
	unsigned long ul_size_out;  /**< out result */
}FS_get_total_space_ctx_t;

/** @brief define argument for get usable space event */
typedef struct
{
	char * pc_path_in;          /**< in path of file/directory */
	unsigned long ul_size_out;  /**< out result */
}FS_get_usable_space_ctx_t;

/** @brief define argument for is directory event */
typedef struct
{
	char * pc_path_in;          /**< in path of file/directory */
	unsigned char uc_bool_out;  /**< out result */
}FS_is_directory_ctx_t;

/** @brief define argument for is file event */
typedef struct
{
	char * pc_path_in;          /**< in path of file/directory */
	unsigned char uc_bool_out;  /**< out result */
}FS_is_file_ctx_t;

/** @brief define argument for is hidden event */
typedef struct
{
	char * pc_path_in;          /**< in path of file/directory */
	unsigned char uc_bool_out;  /**< out result */
}FS_is_hidden_ctx_t;

/** @brief define argument for get last modified event */
typedef struct
{
	char * pc_path_in;          /**< in path of file/directory */
	int* pi_data_out;           /**< out pointer on array to stored date and time */
	unsigned char uc_bool_out;  /**< out result */
}FS_get_last_modified_ctx_t;

/** @brief define argument for mk dir event */
typedef struct
{
	char * pc_path_in;           /**< in path of directory */
	unsigned char uc_bool_out;  /**< out result */
}FS_mk_dir_ctx_t;

/** @brief define argument for open dir event */
typedef struct
{
	char * pc_path_in;           /**< in path of directory */
	int i_fd_out;               /**< out result */
}FS_open_dir_ctx_t;

/** @brief define argument for read dir event */
typedef struct
{
	unsigned int ui_fd_in;      /**< in file descriptor to close */
	char* pc_filename_out;      /**< out pointer on filename read */
	unsigned char uc_bool_out;  /**< out result */
}FS_read_dir_ctx_t;

/** @brief define argument for close dir event */
typedef struct
{
	unsigned int ui_fd_in;      /**< in file descriptor to close */
	unsigned char uc_bool_out;  /**< out result */
}FS_close_dir_ctx_t;

/** @brief define argument for rename event */
typedef struct
{
	char* pc_old_name_in;        /**< in name of old item */
	char* pc_new_name_in;        /**< in name of new item */
	unsigned char uc_bool_out;   /**< out result */
}FS_rename_ctx_t;

/** @brief define argument for set last modified event */
typedef struct
{
	char * pc_path_in;           /**< in path of directory */
	int i_years_in;             /**< in years */
	int i_month_in;             /**< in month */
	int i_day_in;               /**< in day */
	int i_hour_in;              /**< in hour */
	int i_minute_in;            /**< in minute */
	int i_second_in;            /**< in second */
	unsigned char uc_bool_out;  /**< out result */
}FS_set_last_modified_ctx_t;

/** @brief define argument for set read only event */
typedef struct
{
	char * pc_path_in;           /**< in path of file/directory */
	unsigned char uc_bool_out;  /**< out result */
}FS_set_read_only_ctx_t;

/** @brief define argument for get length event */
typedef struct
{
	char * pc_path_in;           /**< in path of file/directory */
	unsigned long ul_size_out;  /**< out result */
}FS_get_length_ctx_t;

/** @brief define argument for create event */
typedef struct
{
	char * pc_path_in;           /**< in path of file/directory */
	FS_status_t status_error_out;  /**< out result */
}FS_create_ctx_t;

/** @brief define argument for delete event */
typedef struct
{
	char * pc_path_in;           /**< in path of file/directory */
	unsigned char uc_bool_out;  /**< out result */
}FS_delete_ctx_t;

/** @brief define argument for can write event */
typedef struct
{
	char * pc_path_in;          /**< in path of file/directory */
	FS_access_t  access_in;     /**< in access type  */
	unsigned char uc_bool_out;  /**< out result */
}FS_check_access_ctx_t;

/** @brief define argument for set executable event */
typedef struct
{
	char * pc_path_in;               /**< in path of file/directory */
	FS_access_t access_in;           /**< in access mode */
	unsigned char uc_enable_in;      /**< in enable permission or not */
	unsigned char uc_owner_only_in;  /**< in owner only or not */
	unsigned char uc_bool_out;       /**< out result */
}FS_set_permission_ctx_t;


/** @brief union with all parameter available */
typedef union{
	FS_open_file_ctx_t st_open_file_ctx;
	FS_close_file_ctx_t st_close_file_ctx;
	FS_read_file_ctx_t st_read_file_ctx;
	FS_write_file_ctx_t st_write_file_ctx;
	FS_exist_file_ctx_t st_exist_file_ctx;
	FS_write_byte_file_ctx_t st_write_byte_file_ctx;
	FS_read_byte_file_ctx_t st_read_byte_file_ctx;
	FS_skip_file_ctx_t st_skip_file_ctx;
	FS_available_file_ctx_t st_available_file_ctx;
	FS_get_free_space_ctx_t st_get_free_space_ctx;
	FS_get_total_space_ctx_t st_get_total_space_ctx;
	FS_get_usable_space_ctx_t st_get_usable_space_ctx;
	FS_is_directory_ctx_t st_is_directory_ctx;
	FS_is_file_ctx_t st_is_file_ctx;
	FS_is_hidden_ctx_t st_is_hidden_ctx;
	FS_get_last_modified_ctx_t st_get_last_modified_ctx;
	FS_mk_dir_ctx_t st_mk_dir_ctx;
	FS_open_dir_ctx_t st_open_dir_ctx;
	FS_read_dir_ctx_t st_read_dir_ctx;
	FS_close_dir_ctx_t st_close_dir_ctx;
	FS_rename_ctx_t st_rename_ctx;
	FS_set_last_modified_ctx_t st_set_last_modified_ctx;
	FS_set_read_only_ctx_t st_set_read_only_ctx;
	FS_get_length_ctx_t st_get_length_ctx;
	FS_create_ctx_t st_create_ctx;
	FS_delete_ctx_t st_delete_ctx;
	FS_check_access_ctx_t st_check_access_ctx;
	FS_set_permission_ctx_t st_set_permission_ctx;
}FS_generic_ctx_t;

/** @brief define an event */
typedef struct
{
	FS_id_event_t e_id_event;               /**< id event */
	FS_generic_ctx_t u_ctx_event;           /**< event context */
	unsigned long ul_id_java_thread;           /**< id original java thread */
}FS_event_t;

/**
 * @brief function to initialise the FS delegate module
 *
 * @return @see FS_status_t
 */
FS_status_t FS_init_f(void);

/**
 * @brief function to start the FS delegate thread
 *
 * @return @see FS_ADAPT_status_t
 */
FS_status_t FS_start_f(void);

/**
 * @brief function to get an event of private FS delegate module pool
 *
 * @param[out] _pst_event pointer of pointer of event
 *
 * @return @see FS_status_t
 */
FS_status_t FS_get_event(FS_event_t ** _pst_event);

/**
 * @brief function to release an event of private FS delegate module pool
 *
 * @param[in] _pst_event pointer of event to release
 *
 * @return @see FS_status_t
 */
FS_status_t FS_release_event(FS_event_t * _pst_event);

/**
 * @brief function to post event on FS delegate module
 *
 * This function suspend the JVM thread caller if no error occurred.
 *
 * @param[in,out] _pst_event pointer of event
 *
 * @return @see FS_status_t
 */
FS_status_t FS_post_event(FS_event_t * _pst_event);

/**
 * @brief function to retrieve the event on FS delegate module, coresponfing with java thread id
 *
 * @param[out] _pst_event pointer of pointer of event
 * @param[in] _ul_id_java_thread java thread id 
 *
 * @return @see FS_status_t
 */
FS_status_t FS_retrieve_event(FS_event_t ** _pst_event, unsigned long _ul_id_java_thread);


/**
 * @brief main function of FS delegate module
 *
 * @todo the function shall notice that error occurred but how ? (log, callback, api status function ...)
 */
void FS_run_f(void);

#ifdef __cplusplus
	}
#endif


#endif /* FATS_DELEGATE_TASK_H */
