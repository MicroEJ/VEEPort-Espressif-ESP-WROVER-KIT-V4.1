/*
 * C
 *
 * Copyright 2016-2017 IS2T. All rights reserved.
 * IS2T PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */
#ifndef LLFS_IMPL
#define LLFS_IMPL

/**
 * @file
 * @brief MicroEJ FS low level API
 * @author MicroEJ Developer Team
 * @version 1.0.2
 * @date 1 June 2017
 */

#include <stdint.h>
#include <intern/LLFS_impl.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Return code when an action succeeds
 */
#define LLFS_OK (0)

/*
 * Return code for End Of File (EOF)
 */
#define LLFS_EOF (-1)

/*
 * Return code when an error occurs
 */
#define LLFS_NOK (-2)

/*
 * Return code when a file is not created
 * @see LLFS_IMPL_create
 */
#define LLFS_NOT_CREATED (-3)

/*
 * Action to post an event in delegate task
 */
#define LLFS_EVENT_POST (0)

/*
 * Action to retrieve an event previously posted in delegate task
 */
#define LLFS_EVENT_RETRIEVE (1)

/*
 * Permission on a file: owner only
 * @see LLFS_IMPL_set_permission
 */
#define LLFS_PERMISSION_OWNER_ONLY  (0)

/*
 * Permission on a file: not owner only
 * @see LLFS_IMPL_set_permission
 */
#define LLFS_PERMISSION_ALL  (1)

/*
 * Enable a permission on a file
 * @see LLFS_IMPL_set_permission
 */
#define LLFS_PERMISSION_ENABLE  (0)

/*
 * Disable a permission on a file
 * @see LLFS_IMPL_set_permission
 */
#define LLFS_PERMISSION_DISABLE  (1)

/*
 * Read access mode
 * @see LLFS_IMPL_set_permission
 * @see LLFS_IMPL_is_accessible
 */
#define LLFS_ACCESS_READ 	(0x04)

/*
 * Write access mode
 * @see LLFS_IMPL_set_permission
 * @see LLFS_IMPL_is_accessible
 */
#define LLFS_ACCESS_WRITE 	(0x02)

/*
 * Execute access mode
 * @see LLFS_IMPL_set_permission
 * @see LLFS_IMPL_is_accessible
 */
#define LLFS_ACCESS_EXECUTE 	(0x01)

/*
 * Position of 'year' element in array 'date'
 * @see LLFS_IMPL_get_last_modified
 */
#define LLFS_POSITION_YEAR (0)

/*
 * Position of 'month' element in array 'date'
 * @see LLFS_IMPL_get_last_modified
 */
#define LLFS_POSITION_MONTH (1)

/*
 * Position of 'day of month' element in array 'date'
 * @see LLFS_IMPL_get_last_modified
 */
#define LLFS_POSITION_DAY (2)

/*
 * Position of 'hour' element in array 'date'
 * @see LLFS_IMPL_get_last_modified
 */
#define LLFS_POSITION_HOUR (3)

/*
 * Position of 'minute' element in array 'date'
 * @see LLFS_IMPL_get_last_modified
 */
#define LLFS_POSITION_MINUTE (4)

/*
 * Position of 'second' element in array 'date'
 * @see LLFS_IMPL_get_last_modified
 */
#define LLFS_POSITION_SECOND (5)

/*
 * Initializes the C part of the FS library such as the file system
 * middleware, the management tasks and the drivers.
 *
 * @return {@link #LLFS_OK} (no error) or {@link #LLFS_NOK}
 */
int32_t LLFS_IMPL_initialize(void);

/*
 * Get last modified date of the file. The date must be stored in given
 * array using the position constants {@link #LLFS_POSITION_YEAR},
 * {@link #LLFS_POSITION_MONTH}, {@link #LLFS_POSITION_DAY},
 * {@link #LLFS_POSITION_HOUR}, {@link #LLFS_POSITION_MINUTE} and
 * {@link #LLFS_POSITION_SECOND}.
 *
 * @param path
 *            absolute path of file
 * @param date
 *            the array where store the date
 * @param action
 *            the event action, {@link #LLFS_EVENT_POST} |
 *            {@link #LLFS_EVENT_RETRIEVE}
 * @return {@link #LLFS_OK} (no error) or {@link #LLFS_NOK}
 */
int32_t LLFS_IMPL_get_last_modified(uint8_t* path, int32_t* date, int32_t action);

/*
 * Update file as read only.
 *
 * @param path
 *            absolute path of file
 * @param action
 *            the event action, {@link #LLFS_EVENT_POST} |
 *            {@link #LLFS_EVENT_RETRIEVE}
 * @return {@link #LLFS_OK} (no error) or {@link #LLFS_NOK}
 */
int32_t LLFS_IMPL_set_read_only(uint8_t* path, int32_t action);

/*
 * Create a new empty file with the given pathname.
 * <p>
 * When action is {@link #LLFS_EVENT_RETRIEVE}, return {@link #LLFS_OK} if
 * the file was created, {@link #LLFS_NOT_CREATED} if a file or directory
 * with the given pathname already exists, {@link #LLFS_NOK} if an I/O
 * error occurs.
 *
 * @param path
 *            absolute path of file
 * @param action
 *            the event action, {@link #LLFS_EVENT_POST} |
 *            {@link #LLFS_EVENT_RETRIEVE}
 * @return {@link #LLFS_OK} (no error), {@link #LLFS_NOK} or
 *         {@link #LLFS_NOT_CREATED}
 *
 */
int32_t LLFS_IMPL_create(uint8_t* path, int32_t action);

/*
 * Opens a directory in order to read from it later. Framework ensures only
 * one directory is opened at the same time closing current directory
 * without opening a new one. The returned value is an positive integer used
 * as ID to identify the opened directory. This ID will be used later by
 * read and close methods.
 *
 * @param path
 *            absolute path of directory
 * @param action
 *            the event action, {@link #LLFS_EVENT_POST} |
 *            {@link #LLFS_EVENT_RETRIEVE}
 * @return an positive ID or {@link #LLFS_NOK}
 */
int32_t LLFS_IMPL_open_directory(uint8_t* path, int32_t action);

/*
 * Specify the next available file in directory. Returns {@link #LLFS_NOK}
 * when no more file is available.
 *
 * @param dirID
 *            the directory ID
 * @param path
 *            path where store the next available file
 * @param action
 *            the event action, {@link #LLFS_EVENT_POST} |
 *            {@link #LLFS_EVENT_RETRIEVE}
 * @return {@link #LLFS_OK} (no error) or {@link #LLFS_NOK}
 */
int32_t LLFS_IMPL_read_directory(int32_t directory_ID, uint8_t* path, int32_t action);

/*
 * Close the directory.
 *
 * @param dirID
 *            the directory ID
 * @param action
 *            the event action, {@link #LLFS_EVENT_POST} |
 *            {@link #LLFS_EVENT_RETRIEVE}
 * @return {@link #LLFS_OK} (no error) or {@link #LLFS_NOK}
 */
int32_t LLFS_IMPL_close_directory(int32_t directory_ID, int32_t action);

/*
 * Rename a file.
 *
 * @param path
 *            absolute path of file to modify
 * @param newPath
 *            new absolute path of file
 * @param action
 *            the event action, {@link #LLFS_EVENT_POST} |
 *            {@link #LLFS_EVENT_RETRIEVE}
 * @return {@link #LLFS_OK} (no error) or {@link #LLFS_NOK}
 */
int32_t LLFS_IMPL_rename_to(uint8_t* path, uint8_t* new_path, int32_t action);

/*
 * Get the length, in bytes, of the file denoted by path, or 0L if the file
 * does not exist. Some operating systems may return 0L for pathnames
 * denoting system-dependent entities such as devices or pipes
 *
 * @param path
 *            absolute path of file to test
 * @param action
 *            the event action, {@link #LLFS_EVENT_POST} |
 *            {@link #LLFS_EVENT_RETRIEVE}
 * @return The length of the file or {@link #LLFS_NOK} on error.
 */
int64_t LLFS_IMPL_get_length(uint8_t* path, int32_t action);

/*
 * Returns {@link #LLFS_OK} if and only if the file or directory denoted by
 * this abstract pathname exists, {@link #LLFS_NOK} otherwise.
 *
 * @param path
 *            absolute path of file to test
 * @param action
 *            the event action, {@link #LLFS_EVENT_POST} |
 *            {@link #LLFS_EVENT_RETRIEVE}
 * @return {@link #LLFS_OK} when file exists, {@link #LLFS_NOK} otherwise.
 */
int32_t LLFS_IMPL_exist(uint8_t* path, int32_t action);

/*
 * Returns the number of unallocated bytes on the partition or 0L if the
 * path does not name a partition.
 *
 * @param path
 *            absolute path of file in the require partition
 * @param action
 *            the event action, {@link #LLFS_EVENT_POST} |
 *            {@link #LLFS_EVENT_RETRIEVE}
 * @return The number of unallocated bytes or {@link #LLFS_NOK} on error.
 */
int64_t LLFS_IMPL_get_free_space(uint8_t* path, int32_t action);

/*
 * Create the directory.
 *
 * @param path
 *            absolute path of directory with the last entity to create
 * @param action
 *            the event action, {@link #LLFS_EVENT_POST} |
 *            {@link #LLFS_EVENT_RETRIEVE}
 * @return {@link #LLFS_OK} (no error) or {@link #LLFS_NOK}
 */
int32_t LLFS_IMPL_make_directory(uint8_t* path, int32_t action);

/*
 * Tests whether the file named by this abstract pathname is a hidden file.
 * The exact definition of hidden is system-dependent. On UNIX systems, a
 * file is considered to be hidden if its name begins with a period
 * character ('.'). On Microsoft Windows systems, a file is considered to be
 * hidden if it has been marked as such in the filesystem.
 *
 * @param path
 *            absolute path of file to test.
 * @param action
 *            the event action, {@link #LLFS_EVENT_POST} |
 *            {@link #LLFS_EVENT_RETRIEVE}
 * @return {@link #LLFS_OK} if and only if the file denoted by this path is
 *         hidden according to the conventions of the underlying platform,
 *         {@link #LLFS_NOK} otherwise
 */
int32_t LLFS_IMPL_is_hidden(uint8_t* path, int32_t action);

/*
 * Check if the file denoted by this abstract pathname exists and is a
 * directory
 *
 * @param path
 *            absolute path of file to test
 * @param action
 *            the event action, {@link #LLFS_EVENT_POST} |
 *            {@link #LLFS_EVENT_RETRIEVE}
 * @return {@link #LLFS_OK} (file is a directory) or {@link #LLFS_NOK}
 */
int32_t LLFS_IMPL_is_directory(uint8_t* path, int32_t action);

/*
 * Check if the file denoted by this abstract pathname exists and is a file
 *
 * @param path
 *            absolute path of file to test
 * @param action
 *            the event action, {@link #LLFS_EVENT_POST} |
 *            {@link #LLFS_EVENT_RETRIEVE}
 * @return {@link #LLFS_OK} (file is not a directory) or {@link #LLFS_NOK}
 */
int32_t LLFS_IMPL_is_file(uint8_t* path, int32_t action);

/*
 * Get the number of available bytes on the partition or 0L if the path does
 * not name a partition. On systems where this information is not available,
 * this method will be equivalent to a call to getFreeSpace()
 *
 * @param path
 *            absolute path of file in the require partition
 * @param action
 *            the event action, {@link #LLFS_EVENT_POST} |
 *            {@link #LLFS_EVENT_RETRIEVE}
 * @return The number of available bytes on the partition or
 *         {@link #LLFS_NOK} on error.
 */
int64_t LLFS_IMPL_get_usable_space(uint8_t* path, int32_t action);

/*
 * Get the size, in bytes, of the partition or 0L if this path does not name
 * a partition.
 *
 * @param path
 *            absolute path of file in the require partition
 * @param action
 *            the event action, {@link #LLFS_EVENT_POST} |
 *            {@link #LLFS_EVENT_RETRIEVE}
 * @return The size of the partition or {@link #LLFS_NOK} on error.
 */
int64_t LLFS_IMPL_get_total_space(uint8_t* path, int32_t action);

/*
 * Change the last modified date of file.
 *
 * @param path
 *            absolute path of file to modified
 * @param year
 *            the new year
 * @param month
 *            the new month
 * @param day
 *            the new day of month
 * @param hour
 *            the new hour
 * @param minute
 *            the new minute
 * @param second
 *            the new second
 * @param action
 *            the event action, {@link #LLFS_EVENT_POST} |
 *            {@link #LLFS_EVENT_RETRIEVE}
 * @return {@link #LLFS_OK} (no error) or {@link #LLFS_NOK}
 */
int32_t LLFS_IMPL_set_last_modified(uint8_t* path, int32_t year, int32_t month, int32_t day, int32_t hour, int32_t minute, int32_t second, int32_t action);

/*
 * Delete the file or directory denoted by the given absolute pathname.
 *
 * @param path
 *            absolute path of file to modified
 * @param action
 *            the event action, {@link #LLFS_EVENT_POST} |
 *            {@link #LLFS_EVENT_RETRIEVE}
 *
 * @return {@link #LLFS_OK} (no error) or {@link #LLFS_NOK}
 */
int32_t LLFS_IMPL_delete(uint8_t* path, int32_t action);

/*
 * Check whether the file or directory denoted by the given abstract
 * pathname may be accessed by this process. The second argument specifies
 * which access, {@link #ACCESS_READ}, {@link #ACCESS_WRITE} or
 * {@link #ACCESS_EXECUTE}, to check.
 *
 * @param path
 *            absolute path of file to modified
 * @param access
 *            {@link #ACCESS_READ}, {@link #ACCESS_WRITE} or
 *            {@link #ACCESS_EXECUTE}
 * @param action
 *            the event action, {@link #LLFS_EVENT_POST} |
 *            {@link #LLFS_EVENT_RETRIEVE}
 *
 * @return {@link #LLFS_OK} (no error) or {@link #LLFS_NOK}
 */
int32_t LLFS_IMPL_is_accessible(uint8_t* path, int32_t access, int32_t action);

/*
 * Set on or off the access permission (to owner only or to all) to the file
 * or directory denoted by the given abstract pathname.
 *
 * @param path
 *            absolute path of file to modified
 * @param access
 *            {@link #ACCESS_READ}, {@link #ACCESS_WRITE} or
 *            {@link #ACCESS_EXECUTE}
 * @param enable
 *            {@link #LLFS_PERMISSION_ENABLE} or
 *            {@link #LLFS_PERMISSION_DISABLE}
 * @param owner
 *            {@link #LLFS_PERMISSION_OWNER_ONLY} or
 *            {@link #LLFS_PERMISSION_ALL}
 * @param action
 *            the event action, {@link #LLFS_EVENT_POST} |
 *            {@link #LLFS_EVENT_RETRIEVE}
 *
 * @return {@link #LLFS_OK} (no error) or {@link #LLFS_NOK}
 */
int32_t LLFS_IMPL_set_permission(uint8_t* path, int32_t access, int32_t enable, int32_t owner, int32_t action);

#ifdef __cplusplus
}
#endif
#endif
