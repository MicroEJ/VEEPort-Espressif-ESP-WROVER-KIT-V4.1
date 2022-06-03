#
# Makefile
#
# Copyright 2020-2022 MicroEJ Corp. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be found with this software.
#

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/config/inc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ssl/inc)

if (CMAKE_HOST_WIN32)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMBEDTLS_USER_CONFIG_FILE=<microej_mbedtls_config.h>")
else()
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMBEDTLS_USER_CONFIG_FILE='<microej_mbedtls_config.h>'")
endif()
