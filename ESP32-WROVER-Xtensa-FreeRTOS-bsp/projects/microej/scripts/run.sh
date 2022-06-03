#!/bin/bash
#
# BASH
#
# Copyright 2020-2022 MicroEJ Corp. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be found with this software.

# 'run.sh' implementation for Espressif IDF.

# 'run.sh' is responsible for flashing the executable file on the target device 
# then resetting target device

# Keep current directory.
CURRENT_DIRECTORY=$(pwd)

# Set environment variables.
cd $(dirname $0)
. set_project_env.sh

if [ -z "$ESPPORT" ]; then
	echo "Please set the environment variable 'ESPPORT' to the COM port used to flash your ESP32"
	exit -1
fi

# Export IDF and flash device.
. $IDF_PATH/export.sh
idf.py -B $ESP_BUILD_DIR flash -p ESPPORT

# Restore current directory.
cd $CURRENT_DIRECTORY
