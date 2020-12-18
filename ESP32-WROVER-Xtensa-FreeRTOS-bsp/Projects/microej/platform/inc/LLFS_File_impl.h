/*
 * C
 *
 * Copyright 2016-2017 IS2T. All rights reserved.
 * IS2T PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */
#ifndef LLFS_FILE_IMPL
#define LLFS_FILE_IMPL

/**
 * @file
 * @brief MicroEJ FS low level API
 * @author MicroEJ Developer Team
 * @version 1.0.2
 * @date 1 June 2017
 */

#include <stdint.h>
#include <LLFS_impl.h>
#include <intern/LLFS_File_impl.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * File mode for 'READ'
 */
#define LLFS_FILE_MODE_READ ('R')

/*
 * File mode for 'WRITE'
 */
#define LLFS_FILE_MODE_WRITE ('W')

/*
 * File mode for 'APPEND'
 */
#define LLFS_FILE_MODE_APPEND ('A')

/*
 * Open a file according given mode. The returned value is a positive
 * integer used to identify the file later.
 *
 * @param path
 *            absolute path of file to open
 * @param mode
 *            opening mode : {@link #LLFS_FILE_MODE_READ},
 *            {@link #LLFS_FILE_MODE_WRITE} and/or
 *            {@link #LLFS_FILE_MODE_APPEND}
 * @param action
 *            the event action, {@link #LLFS_EVENT_POST} |
 *            {@link #LLFS_EVENT_RETRIEVE}
 *
 * @return an unique ID or {@link #LLFS_NOK}
 */
int32_t LLFS_File_IMPL_open(uint8_t* path, uint8_t mode, int32_t action);

/*
 * Writes data to the channel.
 *
 * @param fileID
 *            file identifier
 * @param data
 *            pointer to byte array
 * @param length
 *            number of data to write
 * @param action
 *            the event action, {@link #LLFS_EVENT_POST} |
 *            {@link #LLFS_EVENT_RETRIEVE}
 * @return The number of bytes written. Possibly zero or {@link #LLFS_NOK}
 *         on error.
 */
int32_t LLFS_File_IMPL_write(int32_t file_id, uint8_t* data, int32_t length, int32_t action);

/*
 * Writes the given byte in file.
 *
 * @param fileID
 *            file identifier
 * @param data
 *            the byte to write
 * @param action
 *            the event action, {@link #LLFS_EVENT_POST} |
 *            {@link #LLFS_EVENT_RETRIEVE}
 * @return {@link #LLFS_OK} (no error) or {@link #LLFS_NOK}
 */
int32_t LLFS_File_IMPL_write_byte(int32_t file_id, int32_t data, int32_t action);
/*
 * Reads data from the channel.
 *
 * @param fileID
 *            file identifier
 * @param data
 *            pointer to byte array
 * @param length
 *            number of data to read
 * @param action
 *            the event action, {@link #LLFS_EVENT_POST} |
 *            {@link #LLFS_EVENT_RETRIEVE}
 * @return number of bytes read, {@link #LLFS_EOF} on EOF, {@link #LLFS_NOK}
 *         on error.
 */
int32_t LLFS_File_IMPL_read(int32_t file_id, uint8_t* data, int32_t length, int32_t action);

/*
 * Read one byte from the file.
 *
 * @param fileID
 *            file identifier
 * @param action
 *            the event action, {@link #LLFS_EVENT_POST} |
 *            {@link #LLFS_EVENT_RETRIEVE}
 * @return number of bytes read, {@link #LLFS_EOF} on EOF, {@link #LLFS_NOK}
 *         on error.
 */
int32_t LLFS_File_IMPL_read_byte(int32_t file_id, int32_t action);

/*
 * Close the file.
 *
 * @param fileID
 *            file identifier
 * @param action
 *            the event action, {@link #LLFS_EVENT_POST} |
 *            {@link #LLFS_EVENT_RETRIEVE}
 * @return {@link #LLFS_OK} (no error) or {@link #LLFS_NOK}
 */
int32_t LLFS_File_IMPL_close(int32_t file_id, int32_t action);


/*
 * Skips over and discards <code>n</code> bytes of data from the input
 * stream.
 *
 * <p>
 * The <code>skip</code> method may, for a variety of reasons, end up
 * skipping over some smaller number of bytes, possibly <code>0</code>. If
 * <code>n</code> is negative, the method will try to skip backwards. In
 * case the backing file does not support backward skip at its current
 * position, {@link #LLFS_NOK} is returned. The actual number of bytes
 * skipped is returned. If it skips forwards, it returns a positive value.
 * If it skips backwards, it returns {@link #LLFS_NOK}.
 *
 * <p>
 * This method may skip more bytes than what are remaining in the backing
 * file. This produces no error and the number of bytes skipped may include
 * some number of bytes that were beyond the EOF of the backing file.
 * Attempting to read from the stream after skipping past the end will
 * result in {@link #LLFS_EOF} indicating the end of the file.
 *
 * @param fileID
 *            file identifier
 * @param n
 *            the number of bytes to be skipped.
 * @param action
 *            the event action, {@link #LLFS_EVENT_POST} |
 *            {@link #LLFS_EVENT_RETRIEVE}
 * @return the actual number of bytes skipped or {@link #LLFS_EOF} or
 *         {@link #LLFS_NOK}.
 */
int64_t LLFS_File_IMPL_skip(int32_t file_id, int64_t n, int32_t action);

/*
 * Returns an estimate of the number of remaining bytes that can be read (or
 * skipped over) from this channel without blocking by the next invocation
 * of a method for this channel. Returns 0 when the file position is beyond
 * EOF.
 *
 * @param fileID
 *            file identifier
 * @param action
 *            the event action, {@link #LLFS_EVENT_POST} |
 *            {@link #LLFS_EVENT_RETRIEVE}
 * @return an estimate of the number of remaining bytes that can be read (or
 *         skipped over) from this channel without blocking or
 *         {@link #LLFS_NOK} on error.
 */
int32_t LLFS_File_IMPL_available(int32_t file_id, int32_t action);

#ifdef __cplusplus
}
#endif
#endif
