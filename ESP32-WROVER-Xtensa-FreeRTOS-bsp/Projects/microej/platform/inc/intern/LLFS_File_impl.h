/*
 * C
 *
 * Copyright 2016-2017 IS2T. All rights reserved.
 * IS2T PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */

/**
 * @file
 * @brief MicroEJ FS low level API
 * @author MicroEJ Developer Team
 * @version 1.0.2
 * @date 1 June 2017
 */

#define LLFS_File_IMPL_open Java_com_is2t_java_io_DefaultFileChannel_openNative
#define LLFS_File_IMPL_write Java_com_is2t_java_io_DefaultFileChannel_writeNative
#define LLFS_File_IMPL_write_byte Java_com_is2t_java_io_DefaultFileChannel_writeByteNative
#define LLFS_File_IMPL_read Java_com_is2t_java_io_DefaultFileChannel_readNative
#define LLFS_File_IMPL_read_byte Java_com_is2t_java_io_DefaultFileChannel_readByteNative
#define LLFS_File_IMPL_close Java_com_is2t_java_io_DefaultFileChannel_closeNative
#define LLFS_File_IMPL_skip Java_com_is2t_java_io_DefaultFileChannel_skipNative
#define LLFS_File_IMPL_available Java_com_is2t_java_io_DefaultFileChannel_availableNative
